#include "parser.hpp"

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

/**
 * @brief Removes leading and trailing spaces from the given string
 */
static void trim(std::string_view &str) {
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
	if (line[0] >= '0' && line[0] <= '9') {
		throw std::runtime_error("names cannot start with a digit");
	}
	std::size_t next_pos = line.find_first_not_of(
		"_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ", pos
	);
	if (next_pos == std::string_view::npos) {
		next_pos = line.size();
	}
	int next_size = next_pos - pos;
	std::string_view name = line.substr(pos, next_size);
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
}

/**
 * @brief Parses table columns in the format of the CreateTable command staring
 *        at pos. pos is updated to point one character past the closing
 *        parenthesis.
 */
static std::vector<Column> parse_create_columns(std::string_view line,
                                                std::size_t &pos) {
	/**
	 *  (<name>:<type>, ...)
	 *  |
	 * pos
	 */
	consume_string(line, pos, "(");

	std::vector<Column> table_columns;

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

		table_columns.push_back(Column{column_name, column_type_id});
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
static std::vector<sql_types::ExampleRow> parse_inserts(std::string_view line,
                                                        std::size_t &pos) {

	/**
	 *  {(<value>, ...), ...}
	 *  |
	 * pos
	 */
	if (line[pos] != '{') {
		throw std::runtime_error("expected '{'\n");
	}
	pos++;

	std::vector<sql_types::ExampleRow> table_inserts;

	while (line[pos] != '}') {
		pos = line.find_first_of("(", pos);
		/**
		 * {(<value>, ...), ...}
		 *  |
		 * pos
		 */
		if (line[pos] != '(') {
			throw std::runtime_error("expected '('\n");
		}
		pos++;

		sql_types::ExampleRow row;

		while (line[pos] != ')') {
			/**
			 *     {(<value>, ...), ...}
			 *      ^^
			 * spaces allowed
			 */
			pos = line.find_first_not_of(" ,", pos);

			// TEMPORARY
			/**
			 * {(<ID>, <Name>, <Value>), ...}
			 *   |^^^^
			 *  pos + next_size
			 */
			std::size_t next_size = line.find_first_of(",", pos) - pos;
			// std::cout << inserts_str.substr(pos, next_size) << '\n';
			row.ID = std::stoi(line.substr(pos, next_size).data());
			pos = pos + next_size;
			pos = line.find_first_not_of(" ,\"", pos);
			/**
			 * {(<ID>, "<Name>", <Value>), ...}
			 *          |^^^^^^
			 *         pos + next_size
			 */
			next_size = line.find_first_of(",\"", pos) - pos;
			// std::cout << inserts_str.substr(pos, next_size) << '\n';
			line.copy(row.Name, 256, pos);
			// row.Name[next_size] = '\0';
			pos = pos + next_size + 1;
			pos = line.find_first_not_of(" ,", pos);
			/**
			 * {(<ID>, <Name>, <Value>), ...}
			 *                 |^^^^^^^
			 *                pos + next_size
			 */
			next_size = line.find_first_of(")", pos) - pos;
			// std::cout << inserts_str.substr(pos, next_size) << '\n';
			row.Value = std::stoi(line.substr(pos, next_size).data());
			pos = pos + next_size;
			// pos = inserts_str.find_first_not_of(" ,", pos);
		}
		pos++;

		// std::cout << "debug: parsed row: " <<
		// 	"  " << row.ID <<
		// 	"  " << row.Name <<
		// 	"  " << row.Value << '\n';
		
		table_inserts.push_back(row);
	}

	return table_inserts;
}

/**
 * @brief Parses table columns in the format of the Select command staring at
 *        pos. pos is updated to point one character past the last column.
 */
static std::vector<std::string_view> parse_select_columns(std::string_view line,
                                                          std::size_t &pos) {

	if (line[pos] == '*') {
		// an empty vector means "all columns", because we can't know
		// what columns the table has without checking the schema/pager
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
		if (line[pos] == ',')
			consume_string(line, pos, ",");
		else
			break;
	}

	return columns;
}

Statement parse_line(std::string_view line) {

	// TODO: error handling

	trim(line);

	/**
	 *    <command>
	 *    |^^^^^^^^^
	 *   pos + next_size
	 */
	std::size_t pos = 0;
	std::size_t next_size = 0;
	std::string_view command_name = parse_name(line, pos);

	if (command_name == "CreateTable") {
		std::string_view table_name;
		std::vector<Column> table_columns;
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
		} catch (const std::runtime_error &e) {
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
		}

		return Statement(Statement::Type::TABLE_INFO, table_name);

	} else if (command_name == "Select") {
		std::vector<std::string_view> column_names;
		std::string_view table_name;
		try {
			/**
			 * Select <columns> FROM <table> WHERE <cond> ORDER BY <column>
			 *        |^^^^^^^^^
			 *       pos + next_size
			 */
			skip_spaces(line, pos);
			column_names = parse_select_columns(line, pos);

			/**
			 * Select <columns> FROM <table> WHERE <conditions> ORDER BY <column>
			 *                  |^^^^
			 *                 pos + next_size
			 */
			skip_spaces(line, pos);
			consume_string(line, pos, "FROM");

			/**
			 * Select <columns> FROM <table> WHERE <cond> ORDER BY <column>
			 *                       |^^^^^^^
			 *                      pos + next_size
			 */
			skip_spaces(line, pos);
			table_name = parse_name(line, pos);

			if (pos == std::string_view::npos)
				return Statement(Statement::Type::SELECT, table_name, {}, column_names);

			/**
			 * Select <columns> FROM <table> WHERE <cond> ORDER BY <column>
			 *                               |^^^^^
			 *                              pos + next_size
			 */
			skip_spaces(line, pos);
			consume_string(line, pos, "WHERE");

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

		return Statement(Statement::Type::SELECT, table_name, {}, column_names);

	} else if (command_name == "Remove") {
		
		// TODO

		return Statement(Statement::Type::REMOVE);

	} else if (command_name == "Insert") {
		std::string_view table_name;
		std::vector<sql_types::ExampleRow> rows;
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
		}

		return Statement(Statement::Type::INSERT, table_name, {}, {}, rows);

	} else {
		std::cout << command_name << ": unknown command\n";
		return Statement(Statement::Type::INVALID);
	}
}

} // namespace fmisql
