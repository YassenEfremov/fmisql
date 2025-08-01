#include "cmd_seq.hpp"

#include "../src/cli/parser.hpp"
#include "../src/commands/create_table.hpp"
#include "../src/commands/list_tables.hpp"
#include "../src/commands/table_info.hpp"
#include "../src/commands/insert.hpp"
#include "../src/commands/select.hpp"
#include "../src/statement.hpp"

#include "util.hpp"

#include <cstdlib>

#include <iostream>
#include <string>
#include <string_view>
#include <vector>


namespace fmisql::test {

/**
 * @brief Executes the given command
 */
static bool execute_command(std::string_view line) {
	Statement statement = parse_line(line);

	switch (statement.type) {
	case Statement::Type::CREATE_TABLE:
		create_table(statement.table_name, statement.create_columns,
			line);
		break;

	case Statement::Type::DROP_TABLE:
		// TODO
		break;

	case Statement::Type::LIST_TABLES:
		list_tables();
		break;

	case Statement::Type::TABLE_INFO:
		table_info(statement.table_name);
		break;

	case Statement::Type::SELECT:
		select(statement.select_columns, statement.table_name);
		break;

	case Statement::Type::REMOVE:
		// TODO
		break;

	case Statement::Type::INSERT:
		insert(statement.table_name, statement.insert_rows);
		break;

	default:
		return false;
	}
	return true;
}

/**
 * @brief Tests whether the given sequence of commands works.
 *        Default condition is SHOULD_PASS.
 */
void test_command_sequence(std::vector<std::string_view> lines, Condition condition) {
	std::cout << "Testing command sequence:\n";
	for (std::string_view line : lines) {
		std::cout << "  " << line << '\n';
	}

	for (std::string_view line : lines) {
		bool result = execute_command(line);
		if (result) {
			if (condition == Condition::SHOULD_FAIL) {
				total++;
				failed++;
				std::cout << RED "  FAILED\n" RESET;
				std::cout << '\n';
				return;
			}
		} else {
			if (condition == Condition::SHOULD_PASS) {
				total++;
				failed++;			
				std::cout << RED "  FAILED\n" RESET;
				std::cout << '\n';
				return;
			}
		}
	}
	switch (condition) {
	case Condition::SHOULD_PASS:
		std::cout << GREEN "  OK\n" RESET;
		break;
	case Condition::SHOULD_FAIL:
		std::cout << CYAN "  failed... OK\n" RESET;
		break;
	}
	passed++;
	total++;
	std::cout << '\n';
}
	
/**
 * @brief Tests a lot of possible commands sequences
 */	
void test_full() {
	
	/* Just creating simple tables and showing data about them */ {

		test_command_sequence({
			"CreateTable Sample (ID:Int, Name:String, Value:Int)",
			"ListTables"
		});
		test_command_sequence({
			"CreateTable Sample (ID:Int, Name:String, Value:Int)",
			"TableInfo Sample"
		});
	}

}

} // namespace fmisql::test
