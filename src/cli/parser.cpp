#include "parser.hpp"

#include "../classes.hpp"
#include "../data_types.hpp"
#include "../statement.hpp"

#include <cstddef>
#include <cstdint>

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>
#include <iostream>


namespace fmisql {

std::vector<Column> parse_create_columns(std::string_view columns_str) {

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

		sql_types::Id column_type;
		if (column_type_str == "Int") {
			column_type = sql_types::Id::INT;
		} else if (column_type_str == "String") {
			column_type = sql_types::Id::STRING;
		} else if (column_type_str == "Date") {
			column_type = sql_types::Id::DATE;
		} else {
			throw std::runtime_error(
				"unknown data type: " + std::string(column_type_str) + '\n'
			);
		}

		table_columns.push_back(Column(column_name, column_type));
	}

	return table_columns;
}

std::vector<sql_types::ExampleRow> parse_inserts(std::string_view inserts_str) {

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

	std::vector<sql_types::ExampleRow> table_inserts;

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

		sql_types::ExampleRow row;

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
			 *          |^^^^^^
			 *         pos + next_size
			 */
			next_size = inserts_str.find_first_of(",\"", pos) - pos;
			// std::cout << inserts_str.substr(pos, next_size) << '\n';
			inserts_str.copy(row.Name, next_size, pos);
			row.Name[next_size] = '\0';
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

std::vector<std::string_view> parse_select_columns(std::string_view columns_str) {

	if (columns_str == "*") {
		// an empty vector means "all columns", because we can't know
		// what columns the table has without checking the schema/pager
		return {};
	}

	std::vector<std::string_view> columns;

	std::size_t pos = 0;
	while (pos != std::string_view::npos) {

		/**
		 *  <column>,...
		 *  |^^^^^^^^
		 * pos + next_size
		 */
		std::size_t next_size = columns_str.find_first_of(",", pos) - pos;
		// std::cout << inserts_str.substr(pos, next_size) << '\n';
		columns.push_back(columns_str.substr(pos, next_size));
		pos = pos + next_size;
		pos = columns_str.find_first_not_of(",", pos);
	}

	return columns;
}

Statement parse_line(std::string_view line) {

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
			table_columns = parse_create_columns(line.substr(pos));

		} catch (const std::out_of_range &e) {
			std::cout << "not enough arguments given\n";
			return Statement(Statement::Type::INVALID);
		} catch (const std::runtime_error &e) {
			std::cout << e.what();
			return Statement(Statement::Type::INVALID);
		}

		return Statement(Statement::Type::CREATE_TABLE, table_name);

	} else if (command == "DropTable") {
		std::string_view table_name;
		try {
			/**
			 * DropTable <table>
			 *           |^^^^^^|
			 *          pos    npos
			 */
			table_name = line.substr(pos);

		} catch (const std::out_of_range &e) {
			std::cout << "not enough arguments given\n";
			return Statement(Statement::Type::INVALID);
		}

		return Statement(Statement::Type::DROP_TABLE, table_name);

	} else if (command == "ListTables") {

		return Statement(Statement::Type::LIST_TABLES);

	} else if (command == "TableInfo") {
		std::string_view table_name;
		try {
			/**
			 * TableInfo <table>
			 *           |^^^^^^|
			 *          pos    npos
			 */
			table_name = line.substr(pos);

		} catch (const std::out_of_range &e) {
			std::cout << "not enough arguments given\n";
			return Statement(Statement::Type::INVALID);
		}

		return Statement(Statement::Type::TABLE_INFO, table_name);

	} else if (command == "Select") {
		std::vector<std::string_view> column_names;
		std::string_view table_name;
		try {
			/**
			 * Select <columns> FROM <table> WHERE <cond> ORDER BY <column>
			 *        |^^^^^^^^^
			 *       pos + next_size
			 */
			next_size = line.find_first_of(" ", pos) - pos;
			std::string_view column_names_str = line.substr(pos, next_size);
			pos = line.find_first_not_of(" ", pos + next_size);
			column_names = parse_select_columns(column_names_str);

			/**
			 * Select <columns> FROM <table> WHERE <conditions> ORDER BY <column>
			 *                  |^^^^
			 *                 pos + next_size
			 */
			next_size = line.find_first_of(" ", pos) - pos;
			std::string_view kw_from = line.substr(pos, next_size);
			if (kw_from != "FROM") {
				std::cout << "expected keyword FROM\n";
				return Statement(Statement::Type::INVALID);
			}
			pos = line.find_first_not_of(" ", pos + next_size);

			/**
			 * Select <columns> FROM <table> WHERE <cond> ORDER BY <column>
			 *                       |^^^^^^^
			 *                      pos + next_size
			 */
			next_size = line.find_first_of(" ", pos) - pos;
			table_name = line.substr(pos, next_size);
			pos = line.find_first_not_of(" ", pos + next_size);

			if (pos == std::string_view::npos)
				return Statement(Statement::Type::SELECT, table_name, column_names);

			/**
			 * Select <columns> FROM <table> WHERE <cond> ORDER BY <column>
			 *                               |^^^^^
			 *                              pos + next_size
			 */
			next_size = line.find_first_of(" ", pos) - pos;
			std::string_view kw_where = line.substr(pos, next_size);
			if (kw_from != "WHERE") {
				std::cout << "expected keyword WHERE\n";
				return Statement(Statement::Type::INVALID);
			}
			pos = line.find_first_not_of(" ", pos + next_size);

			/**
			 * Select <columns> FROM <table> WHERE <cond> ORDER BY <column>
			 *                                     |^^^^^^
			 *                                    pos + next_size
			 */
			// conditions = parse_conditions(line.substr(pos));

		} catch (const std::out_of_range &e) {
			std::cout << "not enough arguments given\n";
			return Statement(Statement::Type::INVALID);
		}

		return Statement(Statement::Type::SELECT, table_name, column_names);

	} else if (command == "Remove") {
		
		// TODO

		return Statement(Statement::Type::REMOVE);

	} else if (command == "Insert") {
		std::string_view table_name;
		std::vector<sql_types::ExampleRow> rows;
		try {
			/**
			 * Insert INTO <table> {(<value>, ...), ...}
			 *        |^^^^
			 *       pos + next_size
			 */
			next_size = line.find_first_of(" ", pos) - pos;
			std::string_view kw_into = line.substr(pos, next_size);
			if (kw_into != "INTO") {
				std::cout << "expected keyword INTO\n";
				return Statement(Statement::Type::INVALID);
			}
			pos = line.find_first_not_of(" ", pos + next_size);

			/**
			 * Insert INTO <table> {(<value>, ...), ...}
			 *             |^^^^^^^
			 *            pos + next_size
			 */
			next_size = line.find_first_of(" ", pos) - pos;
			table_name = line.substr(pos, next_size);
			pos = line.find_first_not_of(" ", pos + next_size);

			/**
			 * Insert INTO <table> {(<value>, ...), ...}
			 *                     |^^^^^^^^^^^^^^^^^^^^^
			 *                    pos + next_size
			 */
			rows = parse_inserts(line.substr(pos));

		} catch (const std::out_of_range &e) {
			std::cout << "not enough arguments given\n";
			return Statement(Statement::Type::INVALID);
		}

		return Statement(Statement::Type::INSERT, table_name, {}, rows);

	} else {
		std::cout << command << ": unknown command\n";
		return Statement(Statement::Type::INVALID);
	}
}

} // namespace fmisql
