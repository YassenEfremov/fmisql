#include "parser.hpp"

#include "../classes.hpp"
#include "../schema.hpp"
#include "../data_types.hpp"
#include "../commands/create_table.hpp"
#include "../pager.hpp"

#include <cstddef>
#include <cstdint>

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>
#include <iostream>


std::vector<Column> parse_columns(std::string_view columns_str) {

	/**
	 *  (<column name>:<type>, ...)
	 *  |
	 * pos
	 */
	std::size_t pos = 0;
	if (columns_str[pos] != '(') {
		throw std::runtime_error("expected '('\n");
	}
	pos++;

	std::vector<Column> table_columns;

	while (columns_str[pos] != ')') {
		/**
		 *      (<column name>:<type>, ...)
		 *      ^^
		 * spaces allowed
		 */
		pos = columns_str.find_first_not_of(" ,", pos);

		/**
		 * (<column name>:<type>, ...)
		 *  |^^^^^^^^^^^^^
		 * pos + next_size
		 */
		std::size_t next_size = columns_str.find_first_of(":", pos) - pos;
		std::string_view column_name = columns_str.substr(pos, next_size);
		pos = pos + next_size + 1;

		/**
		 * (<column name>:<type>, ...)
		 *                |^^^^^^
		 *               pos + next_size
		 */
		next_size = columns_str.find_first_of(",)", pos) - pos;
		std::string_view column_type_str = columns_str.substr(pos, next_size);
		pos = pos + next_size;

		DataType column_type;
		if (column_type_str == "Int") {
			column_type = DataType::INT;
		} else if (column_type_str == "String") {
			column_type = DataType::STRING;
		} else if (column_type_str == "Date") {
			column_type = DataType::DATE;
		} else {
			throw std::runtime_error(
				"unknown data type: " + std::string(column_type_str) + '\n'
			);
		}

		table_columns.push_back(Column(column_name, column_type));
	}

	return table_columns;
}

std::vector<ExampleRow> parse_inserts(std::string_view inserts_str) {

	/**
	 *  {(<value>, ...), ...}
	 *  |
	 * pos
	 */
	std::size_t pos = 0;
	if (inserts_str[pos] != '{') {
		throw std::runtime_error("expected '{'\n");
	}
	pos++;

	std::vector<ExampleRow> table_inserts;

	while (inserts_str[pos] != '}') {
		pos = inserts_str.find_first_of("(", pos);
		/**
		 * {(<value>, ...), ...}
		 *  |
		 * pos
		 */
		if (inserts_str[pos] != '(') {
			throw std::runtime_error("expected '('\n");
		}
		pos++;

		ExampleRow row;

		while (inserts_str[pos] != ')') {
			/**
			 *     {(<value>, ...), ...}
			 *      ^^
			 * spaces allowed
			 */
			pos = inserts_str.find_first_not_of(" ,", pos);

			// TEMPORARY
			/**
			 * {(<ID>, <Name>, <Value>), ...}
			 *   |^^^^
			 *  pos + next_size
			 */
			std::size_t next_size = inserts_str.find_first_of(",", pos) - pos;
			// std::cout << inserts_str.substr(pos, next_size) << '\n';
			row.ID = std::stoi(inserts_str.substr(pos, next_size).data());
			pos = pos + next_size;
			pos = inserts_str.find_first_not_of(" ,\"", pos);
			/**
			 * {(<ID>, "<Name>", <Value>), ...}
			 *         |^^^^^^^
			 *        pos + next_size
			 */
			next_size = inserts_str.find_first_of(",\"", pos) - pos;
			// std::cout << inserts_str.substr(pos, next_size) << '\n';
			inserts_str.copy(row.Name, next_size, pos);
			pos = pos + next_size + 1;
			pos = inserts_str.find_first_not_of(" ,", pos);
			/**
			 * {(<ID>, <Name>, <Value>), ...}
			 *                 |^^^^^^^
			 *                pos + next_size
			 */
			next_size = inserts_str.find_first_of(")", pos) - pos;
			// std::cout << inserts_str.substr(pos, next_size) << '\n';
			row.Value = std::stoi(inserts_str.substr(pos, next_size).data());
			pos = pos + next_size;
			// pos = inserts_str.find_first_not_of(" ,", pos);
		}
		pos++;

		std::cout << "row:\n" <<
			"  " << row.ID <<
			"  " << row.Name <<
			"  " << row.Value << '\n';
		
		table_inserts.push_back(row);
	}

	return table_inserts;
}

void parse_line(std::string_view line) {

	// TODO: error handling

	line.remove_prefix(std::min(line.find_first_not_of(" "), line.size()));
	line.remove_suffix(line.size() - std::min(line.find_last_not_of(" ") + 1, line.size()));

	/**
	 *    <command>
	 *    |^^^^^^^^^
	 *   pos + next_size
	 */
	std::size_t pos = 0;
	std::size_t next_size = line.find_first_of(" ", pos) - pos;
	std::string_view command = line.substr(pos, next_size);
	pos = line.find_first_not_of(" ", pos + next_size);

	if (command == "CreateTable") {
		std::string_view table_name;
		std::vector<Column> table_columns;
		try {
			/**
			 * CreateTable <table name> (<column name>:<type>, ...)
			 *             |^^^^^^^^^^^^
			 *            pos + next_size
			 */
			next_size = line.find_first_of(" ", pos) - pos;
			table_name = line.substr(pos, next_size);
			pos = line.find_first_not_of(" ", pos + next_size);

			/**
			 * CreateTable <table name> (<column name>:<type>, ...)
			 *                          |^^^^^^^^^^^^^^^^^^^^^^^^^^^
			 *                         pos + next_size
			 */
			table_columns = parse_columns(line.substr(pos));

		} catch (const std::out_of_range &e) {
			std::cout << "not enough arguments given\n";
			return;
		} catch (const std::runtime_error &e) {
			std::cout << e.what();
			return;
		}

		// for (auto tc : table_columns) {
		// 	std::cout << tc.name << " - ";
		// 	switch (tc.type) {
		// 		case DataType::INT: std::cout << "INT\n"; break;
		// 		case DataType::STRING: std::cout << "STRING\n"; break;
		// 		case DataType::DATE: std::cout << "DATE\n"; break;
		// 	}
		// }

		// create_table(table_name, table_columns);
		// Table table(table_columns);
		Schema::add(table_name);
		std::cout << "Table " << table_name << " created!\n";

	} else if (command == "DropTable") {
		try {
			/**
			 * DropTable <table name>
			 *           |^^^^^^^^^^^|
			 *          pos         npos
			 */
			std::string_view table_name = line.substr(pos);

		} catch (const std::out_of_range &e) {
			std::cout << "not enough arguments given\n";
			return;
		}

		// TODO:
		// drop the actual table

	} else if (command == "ListTables") {
		// TODO:
		std::cout << "There are " << Schema::rows_count << " table/s in the database:\n";
		Schema::list();

	} else if (command == "TableInfo") {
		try {
			/**
			 * TableInfo <table name>
			 *           |^^^^^^^^^^^|
			 *          pos         npos
			 */
			std::string_view table_name = line.substr(pos);

		} catch (const std::out_of_range &e) {
			std::cout << "not enough arguments given\n";
			return;
		}

		// TODO:
		// show info about the actual table

	} else if (command == "Select") {
		// std::vector<ExampleRow> selected_rows;
		// std::cout << ""

		std::cout << "--------------------\n";
		
		// magic
		ExampleRow row;
		for (int i = 0; i < Pager::rows_count; i++) {
			row.deserialize(Pager::row_slot(i));
			std::cout << "| " << row.ID << " | " << row.Name << " | " << row.Value << " |\n";
		}

	} else if (command == "Remove") {
		// TODO

	} else if (command == "Insert") {
		std::vector<ExampleRow> table_inserts;
		try {
			/**
			 * Insert INTO <table name> {(<value>, ...), ...}
			 *        |^^^^
			 *       pos + next_size
			 */
			next_size = line.find_first_of(" ", pos) - pos;
			std::string_view kw_into = line.substr(pos, next_size);
			if (kw_into != "INTO") {
				std::cout << "expected keyword INTO\n";
				return;
			}
			pos = line.find_first_not_of(" ", pos + next_size);

			/**
			 * Insert INTO <table name> {(<value>, ...), ...}
			 *             |^^^^^^^^^^^^
			 *            pos + next_size
			 */
			next_size = line.find_first_of(" ", pos) - pos;
			std::string_view table_name = line.substr(pos, next_size);
			pos = line.find_first_not_of(" ", pos + next_size);

			/**
			 * Insert INTO <table name> {(<value>, ...), ...}
			 *                          |^^^^^^^^^^^^^^^^^^^^^
			 *                         pos + next_size
			 */
			table_inserts = parse_inserts(line.substr(pos));

		} catch (const std::out_of_range &e) {
			std::cout << "not enough arguments given\n";
		}

		// magic
		for (auto row : table_inserts) {
			row.serialize(Pager::row_slot(Pager::rows_count));
			Pager::rows_count++;
		}

	} else {
		std::cout << command << ": unknown command\n";
	}
}
