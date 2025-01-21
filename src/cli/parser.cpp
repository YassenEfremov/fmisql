#include "parser.hpp"

#include "../data_types.hpp"

#include <cstring>

#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <iostream>


// std::vector<std::string> parse_line(std::string &line) {

//     std::vector<std::string> tokens;

// 	// std::string token;
// 	// std::istringstream line_stream(line);
// 	// while (line_stream >> token) {
// 	// 	tokens.push_back(token);
// 	// }

// 	char *token = std::strtok(line.data(), " ,()");
// 	while (token) {
// 		tokens.push_back(std::string(token));
// 		token = std::strtok(nullptr, " ,()");
// 	}

// 	return tokens;
// }

void interpret(std::string_view line) {

	// TODO: error handling

	line.remove_prefix(std::min(line.find_first_not_of(" "), line.size()));
	line.remove_suffix(line.size() - std::min(line.find_last_not_of(" ") + 1, line.size()));

	/**
	 *    <command>
	 *    |^^^^^^^^^
	 *   pos + next_size
	 */
	int pos = 0;
	std::size_t next_size = line.find_first_of(" ", pos) - pos;
	std::string_view command = line.substr(pos, next_size);
	pos = line.find_first_not_of(" ", pos + next_size);

	if (command == "CreateTable") {
		try {
			/**
			 * CreateTable <table name> (<column name>:<type>, ...)
			 *             |^^^^^^^^^^^^
			 *            pos    + next_size
			 */
			next_size = line.find_first_of(" ", pos) - pos;
			std::string_view table_name = line.substr(pos, next_size);
			pos = line.find_first_not_of(" ", pos + next_size);

			/**
			 * CreateTable <table name> (<column name>:<type>, ...)
			 *                          |
			 *                         pos
			 */
			if (line[pos] != '(') {
				std::cout << "expected '('\n";
				return;
			}
			pos++;

			std::unordered_map<std::string_view, DataType> table_columns;

			while (line[pos] != ')') {
				/**
				 * CreateTable <table name> (<column name>:<type>, ...)
				 *                          ^^
				 *                     spaces allowed
				 */
				pos = line.find_first_not_of(" ,", pos);

				/**
				 * CreateTable <table name> (<column name>:<type>, ...)
				 *                           |^^^^^^^^^^^^^
				 *                          pos     + next_size
				 */
				next_size = line.find_first_of(":", pos) - pos;
				std::string_view column_name = line.substr(pos, next_size);
				pos = pos + next_size + 1;

				/**
				 * CreateTable <table name> (<column name>:<type>, ...)
				 *                                         |^^^^^^
				 *                                        pos + next_size
				 */
				next_size = line.find_first_of(",)", pos) - pos;
				std::string_view column_type_str = line.substr(pos, next_size);
				pos = pos + next_size;

				DataType column_type;
				if (column_type_str == "Int") {
					column_type = DataType::INT;
				} else if (column_type_str == "String") {
					column_type = DataType::STRING;
				} else if (column_type_str == "Date") {
					column_type = DataType::DATE;
				} else {
					std::cout << "unknown data type: " << column_type_str << '\n';
					return;
				}

				table_columns.insert({column_name, column_type});
			}

			// for (auto tc : table_columns) {
			// 	std::cout << tc.first << " - ";
			// 	switch (tc.second) {
			// 		case DataType::INT: std::cout << "INT\n"; break;
			// 		case DataType::STRING: std::cout << "STRING\n"; break;
			// 		case DataType::DATE: std::cout << "DATE\n"; break;
			// 	}
			// }

		} catch (const std::out_of_range &e) {
			std::cout << "not enough arguments given\n";
		}

		// TODO:
		// std::unordered_map<std::string, DataType>
		// 	table_columns = parse_columns(tokens[2]);
		// create_table(table_name, table_columns);

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
		}

		// TODO:
		// drop the actual table

	} else if (command == "ListTables") {
		// TODO:
		// list the actual tables

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
		}

		// TODO:
		// show info about the actual table

	} else if (command == "Select") {


	} else if (command == "Remove") {

	/**
	 * Insert INTO <table name> {(<column 1>, <column 2>, ...), ...}
	 * ^^^^^^
	 */
	} else if (command == "Insert") {
		if (true) {
			std::cout << "not enough arguments given\n";
		} else {
			/**
			 * Insert INTO <table name> {(<column 1>, <column 2>, ...), ...}
			 *        ^^^^
			 */
			// std::string into = tokens[1];

			/**
			 * Insert INTO <table name> {(<column 1>, <column 2>, ...), ...}
			 *             ^^^^^^^^^^^^
			 */
			// std::string table_name = tokens[2];

			/**
			 * Insert INTO <table name> {(<column 1>, <column 2>, ...), ...}
			 *                          ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
			 */
			// std::vector<std::string> inserts = parse_inserts(tokens[3]);
		}

	} else {
		std::cout << command << ": unknown command\n";
	}
}
