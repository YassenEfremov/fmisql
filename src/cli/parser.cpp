#include "parser.hpp"

#include "../data_types.hpp"
#include "../statement.hpp"

#include <cstddef>
#include <cstdint>

#include <algorithm>
#include <charconv>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <iostream>
#include <unordered_set>


namespace fmisql {

void trim(std::string_view &str) {
	str.remove_prefix(std::min(str.find_first_not_of(" "), str.size()));
	str.remove_suffix(str.size() - std::min(str.find_last_not_of(" ") + 1, str.size()));
}

/**
 * @brief Parses a name starting at the given pos. pos is updated to point one
 *        character past the end of the name.
 * 
 * A name is any string that:
 * 
 * - contains only lowercase/uppercase latin letters, the digits 0 to 9 and an
 *   underscore
 * 
 * - does not start with a digit
 * 
 * - is separated from other text by any characters not listed in the first
 *   constraint
 */
static std::string_view parse_name(std::string_view line, std::size_t &pos) {
	if (line.at(pos) >= '0' && line.at(pos) <= '9') {
		throw std::runtime_error("names cannot start with a digit");
	}
	std::size_t next_size = line.find_first_not_of(
		"_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ", pos
	) - pos;
	std::string_view name = line.substr(pos, next_size);
	if (name.empty())
		throw std::runtime_error("empty name");
	pos += next_size;
	return name;
}

/**
 * @brief Skips any number of spaces starting at pos. pos is updated to point to
 *        the first non-space character.
 */
static void skip_spaces(std::string_view line, std::size_t &pos) {
	pos = line.find_first_not_of(" ", pos);
}

/**
 * @brief Consumes the given string starting at pos. pos is updated to point one
 *        character past the end of the string.
 */
static void consume_string(std::string_view line, std::size_t &pos,
                           std::string_view str) {
	for (int i = 0; i < str.size(); i++, pos++) {
		if (line.at(pos) != str[i])
			throw std::runtime_error("expected " + std::string(str));
	}
	if (pos >= line.size()) pos = std::string_view::npos;
}

/**
 * @brief Parses a value staring at pos. pos is updated to point one character
 *        past the end of the value.
 * 
 * Possible values are:
 * 
 * - Int: any integer number
 * 
 * - String: any text surrounded by double quotes (e.g. "Some text")
 */
static sql_types::Value parse_value(std::string_view line, std::size_t &pos) {

	if (line.at(pos) >= '0' && line.at(pos) <= '9') {
		std::size_t next_size = line.find_first_not_of("0123456789", pos) - pos;
		sql_types::Int integer;
		std::string_view value_str = line.substr(pos, next_size);
		std::from_chars(value_str.data(), value_str.data() + value_str.size(), integer);
		pos += next_size;
		return sql_types::Value(integer);

	} else if (line.at(pos) == '"') {
		consume_string(line, pos, "\"");
		std::size_t string_size = line.find_first_of("\"", pos) - pos;
		sql_types::String string = line.substr(pos, string_size);
		pos += string_size;
		consume_string(line, pos, "\"");
		return sql_types::Value(string);

	} else {
		throw std::runtime_error("invalid value");
	}
}

/**
 * @brief Parses an operator staring at pos. pos is updated to point one
 *        character past the end of the operator.
 * 
 * Possible operators are:
 * 
 * - = equals
 * 
 * - != not equals
 * 
 * - < less than
 * 
 * - > greater than
 * 
 * - <= less than or equals
 * 
 * - >= greater than or equals
 */
static void parse_operator(std::string_view line, std::size_t &pos) {
	char first = line.at(pos);
	char second = line.at(pos + 1);

	if (first == '=') {
		consume_string(line, pos, "=");
		
	} else if (first == '!') {
		consume_string(line, pos, "!=");

	} else if (first == '<') {
		if (second == ' ') {
			consume_string(line, pos, "<");

		} else if (second == '=') {
			consume_string(line, pos, "<=");

		} else {
			throw std::runtime_error("invalid logic operation in select condition");
		}
	} else if (first == '>') {
		if (second == ' ') {
			consume_string(line, pos, ">");

		} else if (second == '=') {
			consume_string(line, pos, ">=");

		} else {
			throw std::runtime_error("invalid logic operation in select condition");
		}
	} else {
		throw std::runtime_error("invalid logic operation in select condition");
	}
}

/**
 * @brief Parses table columns in the format of the CreateTable command staring
 *        at pos. pos is updated to point one character past the closing
 *        parenthesis.
 */
static std::vector<sql_types::Column> parse_create_columns(std::string_view line,
                                                std::size_t &pos) {
	/**
	 *  (<name>:<type>, ...)
	 *  |
	 * pos
	 */
	consume_string(line, pos, "(");

	std::vector<sql_types::Column> table_columns;
	std::unordered_set<std::string> unique_columns;

	while (line.at(pos) != ')') {
		/**
		 *      (<name>:<type>, ...)
		 *      \/
		 * spaces allowed
		 */
		skip_spaces(line, pos);

		/**
		 * (<column name>:<type>, ...)
		 *  |^^^^^^^^^^^\ /
		 *  |      spaces allowed
		 *  |
		 * pos + next_size
		 */
		std::string_view column_name = parse_name(line, pos);

		/**
		 * (<column name>:<type>, ...)
		 *                |^^^^\/
		 *                | spaces allowed
		 *                |
		 *               pos + next_size
		 */
		skip_spaces(line, pos);
		consume_string(line, pos, ":");
		skip_spaces(line, pos);
		std::string_view column_type_str = parse_name(line, pos);
		skip_spaces(line, pos);
		if (line.at(pos) == ',')
			consume_string(line, pos, ",");
		skip_spaces(line, pos);

		sql_types::Id column_type_id;
		if (column_type_str == "Int") {
			column_type_id = sql_types::Id::INT;
		} else if (column_type_str == "String") {
			column_type_id = sql_types::Id::STRING;
		} else if (column_type_str == "Date") {
			column_type_id = sql_types::Id::DATE;
		} else {
			throw std::runtime_error(
				"unknown data type: " + std::string(column_type_str) + '\n'
			);
		}

		if (table_columns.size() > sql_types::max_string_size) {
			throw std::runtime_error("maximum column count is " + std::to_string(sql_types::max_string_size));
		} else {
			std::string column_str = std::string(column_name) + ':' + std::string(column_type_str);
			if (unique_columns.find(column_str) == unique_columns.end()) {
				unique_columns.insert(column_str);
				table_columns.push_back(sql_types::Column{column_name, column_type_id});
			} else {
				throw std::runtime_error("duplicate columns");
			}
		}
	}
	consume_string(line, pos, ")");

	if (table_columns.empty())
		throw std::runtime_error("expected column names and types");

	return table_columns;
}

/**
 * @brief Parses rows in the format of the Insert command staring at pos. pos is
 *        updated to point one character past the closing curly brace.
 */
static std::vector<std::vector<sql_types::Value>> parse_inserts(std::string_view line,
                                                        std::size_t &pos) {

	/**
	 *  {(<value>, ...), ...}
	 *  |
	 * pos
	 */
	consume_string(line, pos, "{");

	std::vector<std::vector<sql_types::Value>> rows;

	while (line.at(pos) != '}') {
		/**
		 * {(<value>, ...), ...}
		 *  |
		 * pos
		 */
		skip_spaces(line, pos);
		consume_string(line, pos, "(");

		std::vector<sql_types::Value> row;

		while (line[pos] != ')') {
			/**
			 *     {(<value>, ...), ...}
			 *       ^^^^^^^
			 *
			 */
			skip_spaces(line, pos);
			row.push_back(parse_value(line, pos));
			skip_spaces(line, pos);

			if (line.at(pos) != ')')
				consume_string(line, pos, ",");
		}

		if (row.empty())
			throw std::runtime_error("empty value");

		consume_string(line, pos, ")");
		rows.push_back(row);
		skip_spaces(line, pos);
		if (line.at(pos) != '}')
			consume_string(line, pos, ",");
	}
	consume_string(line, pos, "}");

	return rows;
}

/**
 * @brief Parses table columns in the format of the Select command staring at
 *        pos. pos is updated to point one character past the last column.
 */
static std::vector<std::string_view> parse_select_columns(std::string_view line,
                                                          std::size_t &pos) {

	if (line.at(pos) == '*') {
		// an empty vector means "all columns", because we can't know
		// what columns the table has without checking the schema
		pos++;
		return {};
	}

	std::vector<std::string_view> columns;

	while (true) {

		/**
		 *  <column>,...
		 *  |^^^^^^^^
		 * pos + next_size
		 */
		columns.push_back(parse_name(line, pos));
		skip_spaces(line, pos);
		if (line.at(pos) == ',') {
			consume_string(line, pos, ",");
			skip_spaces(line, pos);
		} else {
			break;
		}
	}

	return columns;
}

/**
 * @brief Parses conditions in the format of the Select and Remove commands
 *        starting at pos.
 */
static void parse_conditions(std::string_view line, std::size_t &pos) {
	while (true) {
		/**
		 *  <column> <operator> <value> <logic operation> ...
		 *  |^^^^^^^
		 * pos
		 */
		std::string_view column_name = parse_name(line, pos);

		/**
		 *  <column> <operator> <value> <logic operation> ...
		 *           |^^^^^^^^^
		 *          pos
		 */
		skip_spaces(line, pos);
		/*std::string_view operator_str =*/ parse_operator(line, pos);

		/**
		 *  <column> <operator> <value> <logic operation> ...
		 *                      |^^^^^^
		 *                     pos
		 */
		skip_spaces(line, pos);
		sql_types::Value value = parse_value(line, pos);

		if (pos == std::string_view::npos)
			break;

		/**
		 *  <column> <operator> <value> <logic operation> ...
		 *                              |^^^^^^^^^^^^^^^^
		 *                             pos
		 */
		skip_spaces(line, pos);
		std::string_view logic_operation_str = parse_name(line, pos);
		pos -= logic_operation_str.size(); // reset because of consume later
		if (logic_operation_str != "AND" &&
		    logic_operation_str != "OR" &&
		    logic_operation_str != "NOT") {
			break;
		}

		consume_string(line, pos, logic_operation_str);
		skip_spaces(line, pos);
	}

	// return ?
}

/**
 * @brief Parses table columns in the format of the Select command starting at
 *        pos.
 */
static void parse_order_by_columns(std::string_view line, std::size_t &pos) {
	// TODO
}

Statement parse_line(std::string_view line) {

	// TODO: more error handling

	trim(line);

	/**
	 *    <command> ...
	 *    |^^^^^^^^^
	 *   pos + next_size
	 */
	std::size_t pos = 0;
	std::size_t next_size = 0;
	std::string_view command_name = parse_name(line, pos);

	if (command_name == "CreateTable") {
		if (line.size() > sql_types::max_string_size) {
			std::cerr << "command too long\n";
			return Statement(Statement::Type::INVALID);
		}

		std::string_view table_name;
		std::vector<sql_types::Column> table_columns;
		try {
			/**
			 * CreateTable <name> (<name>:<type>, ...)
			 *             |^^^^^^
			 *            pos + next_size
			 */
			skip_spaces(line, pos);
			table_name = parse_name(line, pos);

			/**
			 * CreateTable <name> (<name>:<type>, ...)
			 *                    |^^^^^^^^^^^^^^^^^^^^
			 *                   pos + next_size
			 */
			skip_spaces(line, pos);
			table_columns = parse_create_columns(line, pos);

		} catch (const std::out_of_range &e) {
			std::cout << "not enough arguments given\n";
			return Statement(Statement::Type::INVALID);
		} catch (const std::exception &e) {
			std::cout << e.what() << '\n';
			return Statement(Statement::Type::INVALID);
		}

		return Statement(Statement::Type::CREATE_TABLE, table_name, table_columns);

	} else if (command_name == "DropTable") {
		std::string_view table_name;
		try {
			/**
			 * DropTable <table>
			 *           |^^^^^^|
			 *          pos    npos
			 */
			skip_spaces(line, pos);
			table_name = parse_name(line, pos);

		} catch (const std::out_of_range &e) {
			std::cout << "not enough arguments given\n";
			return Statement(Statement::Type::INVALID);
		} catch (const std::exception &e) {
			std::cout << e.what() << '\n';
			return Statement(Statement::Type::INVALID);
		}

		return Statement(Statement::Type::DROP_TABLE, table_name);

	} else if (command_name == "ListTables") {
		try {
			/**
			 * ListTables <anything>
			 *            ^^^^^^^^^^
			 */
			if (pos != std::string_view::npos && pos < line.size())
				throw std::runtime_error("too many arguments given");

		} catch (const std::runtime_error &e) {
			std::cout << e.what() << '\n';
			return Statement(Statement::Type::INVALID);
		} catch (const std::exception &e) {
			std::cout << e.what() << '\n';
			return Statement(Statement::Type::INVALID);
		}

		return Statement(Statement::Type::LIST_TABLES);

	} else if (command_name == "TableInfo") {
		std::string_view table_name;
		try {
			/**
			 * TableInfo <table>
			 *           |^^^^^^|
			 *          pos    npos
			 */
			skip_spaces(line, pos);
			table_name = parse_name(line, pos);

		} catch (const std::out_of_range &e) {
			std::cout << "not enough arguments given\n";
			return Statement(Statement::Type::INVALID);
		} catch (const std::exception &e) {
			std::cout << e.what() << '\n';
			return Statement(Statement::Type::INVALID);
		}

		return Statement(Statement::Type::TABLE_INFO, table_name);

	} else if (command_name == "Select") {
		std::string_view table_name;
		std::vector<std::string_view> column_names;
		try {
			/**
			 * Select <columns> FROM <table>
			 *        |^^^^^^^^^
			 *       pos + next_size
			 */
			skip_spaces(line, pos);
			column_names = parse_select_columns(line, pos);

			/**
			 * Select <columns> FROM <table>
			 *                  |^^^^
			 *                 pos + next_size
			 */
			skip_spaces(line, pos);
			consume_string(line, pos, "FROM");

			/**
			 * Select <columns> FROM <table>
			 *                       |^^^^^^^
			 *                      pos + next_size
			 */
			skip_spaces(line, pos);
			table_name = parse_name(line, pos);

			if (pos == std::string_view::npos)
				return Statement(Statement::Type::SELECT, table_name, {}, column_names);

			/**
			 * Select <columns> FROM <table> WHERE <cond>
			 *                               |^^^^^
			 *                              pos + next_size
			 */
			skip_spaces(line, pos);
			consume_string(line, pos, "WHERE");

			/**
			 * Select <columns> FROM <table> WHERE <cond>
			 *                                     |^^^^^^
			 *                                    pos + next_size
			 */
			skip_spaces(line, pos);
			/*conditions =*/ parse_conditions(line, pos);

			if (pos == std::string_view::npos)
				return Statement(Statement::Type::SELECT, table_name, {}, column_names);

			/**
			 * Select <columns> FROM <table> WHERE <cond> ORDER BY <columns>
			 *                                            |^^^^^^^^
			 *                                           pos + next_size
			 */
			skip_spaces(line, pos);
			consume_string(line, pos, "ORDER BY");

			/**
			 * Select <columns> FROM <table> WHERE <cond> ORDER BY <columns>
			 *                                                     |^^^^^^^^^
			 *                                                    pos + next_size
			 */
			skip_spaces(line, pos);
			/*columns =*/ parse_order_by_columns(line, pos);

		} catch (const std::out_of_range &e) {
			std::cout << "not enough arguments given\n";
			return Statement(Statement::Type::INVALID);
		} catch (const std::exception &e) {
			std::cout << e.what() << '\n';
			return Statement(Statement::Type::INVALID);
		}

		return Statement(Statement::Type::SELECT, table_name, {}, column_names);

	} else if (command_name == "Remove") {
		std::string_view table_name;
		try {
			/**
			 * Remove FROM <table>
			 *        |^^^^
			 *       pos + next_size
			 */
			skip_spaces(line, pos);
			consume_string(line, pos, "FROM");

			/**
			 * Remove FROM <table>
			 *             |^^^^^^^
			 *            pos + next_size
			 */
			skip_spaces(line, pos);
			table_name = parse_name(line, pos);

			if (pos == std::string_view::npos)
				return Statement(Statement::Type::REMOVE, table_name);

			/**
			 * REMOVE FROM <table> WHERE <cond>
			 *                     |^^^^^
			 *                    pos + next_size
			 */
			skip_spaces(line, pos);
			consume_string(line, pos, "WHERE");

			/**
			 * Remove FROM <table> WHERE <cond>
			 *                           |^^^^^^
			 *                          pos + next_size
			 */
			skip_spaces(line, pos);
			/*conditions =*/ parse_conditions(line, pos);

			if (pos == std::string_view::npos)
				return Statement(Statement::Type::REMOVE, table_name);
			
			// ORDER BY

		} catch (const std::out_of_range &e) {
			std::cout << "not enough arguments given\n";
			return Statement(Statement::Type::INVALID);
		} catch (const std::exception &e) {
			std::cout << e.what() << '\n';
			return Statement(Statement::Type::INVALID);
		}

		return Statement(Statement::Type::REMOVE, table_name);

	} else if (command_name == "Insert") {
		std::string_view table_name;
		std::vector<std::vector<sql_types::Value>> rows;
		try {
			/**
			 * Insert INTO <table> {(<value>, ...), ...}
			 *        |^^^^
			 *       pos + next_size
			 */
			skip_spaces(line, pos);
			consume_string(line, pos, "INTO");

			/**
			 * Insert INTO <table> {(<value>, ...), ...}
			 *             |^^^^^^^
			 *            pos + next_size
			 */
			skip_spaces(line, pos);
			table_name = parse_name(line, pos);

			/**
			 * Insert INTO <table> {(<value>, ...), ...}
			 *                     |^^^^^^^^^^^^^^^^^^^^^
			 *                    pos + next_size
			 */
			skip_spaces(line, pos);
			rows = parse_inserts(line, pos);

		} catch (const std::out_of_range &e) {
			std::cout << "not enough arguments given\n";
			return Statement(Statement::Type::INVALID);
		} catch (const std::exception &e) {
			std::cout << e.what() << '\n';
			return Statement(Statement::Type::INVALID);
		}

		return Statement(Statement::Type::INSERT, table_name, {}, {}, rows);

	} else {
		std::cout << command_name << ": unknown command\n";
		return Statement(Statement::Type::INVALID);
	}
}

} // namespace fmisql
