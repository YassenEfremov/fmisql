#include "cmd_seq.hpp"

#include "../include/fmisql.hpp"
#include "../src/cli/parser.hpp"
#include "../src/commands/create_table.hpp"
#include "../src/commands/list_tables.hpp"
#include "../src/commands/table_info.hpp"
#include "../src/commands/insert.hpp"
#include "../src/commands/select.hpp"
#include "../src/statement.hpp"

#include "util.hpp"

#include <cstdlib>
#include <cstdio>

#include <iostream>
#include <string>
#include <string_view>
#include <vector>


namespace fmisql::test {

/**
 * @brief Executes the given command
 */
static void execute_command(std::string_view line) {
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
		break;
	}
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

	std::remove("fmisql.db");
	init();
	for (std::string_view line : lines) {
		try {
			execute_command(line);
		} catch (const std::runtime_error &e) {
			std::cout << e.what() << '\n';
			if (condition == Condition::SHOULD_FAIL) {
				passed++;
				std::cout << CYAN "  failed... OK\n" RESET;
			} else {
				failed++;
				std::cout << RED "  FAILED\n" RESET;
			}
			
			total++;
			deinit();
			std::remove("fmisql.db");
			std::cout << '\n';
			return;
		}
	}
	if (condition == Condition::SHOULD_FAIL) {
		failed++;
		std::cout << RED "  FAILED\n" RESET;
	} else {
		passed++;
		std::cout << GREEN "  OK\n" RESET;
	}
	
	total++;
	deinit();
	std::remove("fmisql.db");
	std::cout << '\n';
}

/**
 * @brief Executes `setup_lines` once and `repeat_line` `n` times.
 *        Default condition is SHOULD_PASS.
 */
void test_command_setup_and_repeat(std::vector<std::string_view> setup_lines,
                                   std::string_view repeat_line,
                                   std::size_t n,
	                               Condition condition) {
	std::cout << "Testing command sequence:\n  Setup:\n";
	for (std::string_view line : setup_lines) {
		std::cout << "    " << line << '\n';
	}
	std::cout << "  Repeating x" << n << " times:\n    " << repeat_line << '\n';

	std::remove("fmisql.db");
	init();
	for (std::string_view line : setup_lines) {
		try {
			execute_command(line);
		} catch (const std::runtime_error &e) {
			std::cout << e.what() << '\n';
			if (condition == Condition::SHOULD_FAIL) {
				passed++;
				std::cout << CYAN "  failed... OK\n" RESET;
			} else {
				failed++;
				std::cout << RED "  FAILED\n" RESET;
			}
			
			total++;
			deinit();
			std::remove("fmisql.db");
			std::cout << '\n';
			return;
		}
	}
	for (int i = 0; i < n; i++) {
		try {
			execute_command(repeat_line);
		} catch (const std::runtime_error &e) {
			std::cout << e.what() << '\n';
			if (condition == Condition::SHOULD_FAIL) {
				passed++;
				std::cout << CYAN "  failed... OK\n" RESET;
			} else {
				failed++;
				std::cout << RED "  FAILED\n" RESET;
			}
			
			total++;
			deinit();
			std::remove("fmisql.db");
			std::cout << '\n';
			return;
		}
	}
	if (condition == Condition::SHOULD_FAIL) {
		failed++;
		std::cout << RED "  FAILED\n" RESET;
	} else {
		passed++;
		std::cout << GREEN "  OK\n" RESET;
	}
	
	total++;
	deinit();
	std::remove("fmisql.db");
	std::cout << '\n';
}
	
/**
 * @brief Tests a lot of possible commands sequences
 */	
void test_command_sequences() {

	/* by requirement */ {

		test_command_sequence({
			"CreateTable Sample (ID:Int, Name:String, Value:Int)",
			"ListTables",
			"TableInfo Simple"
		}, Condition::SHOULD_FAIL);
		test_command_sequence({
			"CreateTable Sample (ID:Int, Name:String, Value:Int)",
			"ListTables",
			"TableInfo Sample",
			"Insert INTO Sample {(1, \"Test\", 1), (2, \"something else\", 100)}",
			"Select Name FROM Sample WHERE ID != 5 AND Value < 50",
			"Select * FROM Sample WHERE ID != 5 AND Name > \"Baba\" ORDER BY Name"
		});
	}
	
	/* Creating simple tables and showing info about them */ {

		// correct commands
		test_command_sequence({
			"CreateTable Sample (ID:Int, Name:String, Value:Int)",
			"ListTables"
		});
		test_command_sequence({
			"CreateTable Sample (ID:Int, Name:String, Value:Int)",
			"ListTables"
		});
		test_command_sequence({
			"CreateTable Sample (ID:Int, Name:String, Value:Int)",
			"TableInfo Sample"
		});

		// wrong commands
		test_command_sequence({
			"CreateTable Sample (Value:Int)",
			"CreateTable Sample (Value:Int)"
		}, Condition::SHOULD_FAIL);
		test_command_sequence({
			"TableInfo Sample"
		}, Condition::SHOULD_FAIL);
	}

	/* Text big row size */ {
	
		// temporary while we implement overflow pages
		test_command_sequence({
			"CreateTable Sample (A:String, B:String, C:String, D:String, E:String,"
			                    "F:String, G:String, H:String, I:String, J:String,"
			                    "K:String, L:String, M:String, N:String, O:String)"
		});
		test_command_sequence({
			"CreateTable Sample (A:String, B:String, C:String, D:String, E:String,"
			                    "F:String, G:String, H:String, I:String, J:String,"
			                    "K:String, L:String, M:String, N:String, O:String,"
								"P:String)"
		}, Condition::SHOULD_FAIL);
	}

	/* Inserting into tables */ {

		// correct commands
		test_command_sequence({
			"CreateTable Sample (ID:Int, Name:String, Value:Int)",
			"Insert INTO Sample {(15, \"Test message\", 120)}",
			"TableInfo Sample"
		});
		test_command_sequence({
			"CreateTable Sample (ID:Int, Name:String, Value:Int)",
			"Insert INTO Sample {(1, \"asd\", 12), (2, \"asd\", 12), (3, \"asd\", 12), (4, \"asd\", 12), (5, \"ddd\", 5)}",
			"TableInfo Sample"
		});
		test_command_sequence({
			"CreateTable Sample (ID:Int, Name:String, Value:Int)",
			"Insert INTO Sample {(15, \"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			                           "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
									   "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
									   "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\", 120)}"
		});

		// wrong commands
		test_command_sequence({
			"CreateTable Sample (ID:Int, Name:String, Value:Int)",
			"Insert INTO Sample {(15, \"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
			                           "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
									   "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
									   "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\", 120)}"
		}, Condition::SHOULD_FAIL);
		test_command_sequence({
			"CreateTable Sample (ID:Int, Name:String)",
			"Insert INTO Sample {(15, \"some text\", 120)}"
		}, Condition::SHOULD_FAIL);
		test_command_sequence({
			"CreateTable Sample (ID:Int, Name:String)",
			"Insert INTO Sample {(12, 1000)}"
		}, Condition::SHOULD_FAIL);
		test_command_sequence({
			"CreateTable Sample (ID:Int, Name:String)",
			"Insert INTO Sample {(\"some more text\", 1000)}"
		}, Condition::SHOULD_FAIL);
		test_command_sequence({
			"CreateTable Sample (ID:Int, n:Int)",
			"Insert INTO Sample {(123, 1000), (234, \"asd\")}"
		}, Condition::SHOULD_FAIL);
	}

	/* Creating a lot of tables in order to cause node splits */ {

		// creating 8 tables causes 1 leaf split
		test_command_sequence({
			"CreateTable Sample1 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample2 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample3 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample4 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample5 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample6 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample7 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample8 (ID:Int, Name:String, Value:Int)"
		});
		// creating 12 tables causes 2 leaf splits
		test_command_sequence({
			"CreateTable Sample1 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample2 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample3 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample4 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample5 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample6 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample7 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample8 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample9 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample10 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample11 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample12 (ID:Int, Name:String, Value:Int)"
		});

		// etc... every 4 new tables cause a leaf split

		// I haven't tested internal node splits using the CreateTable command
		// because that would require creating like 2040 tables.
		// It could be automated, but I'm also lazy.
	}

	/* Inserting a lot of rows in order to cause node splits */ {

		// The page is 4096 bytes and excluding the header, the space for rows is
		//   4096 - 9 = 4087 bytes
		// Therefore the maximum size for a row that contains only strings is
		//   4087 / (4 + 256) = ~15.7
		// So a table with 15 string columns would require one row to fill an entire page.
		// 512 rows in such a table would cause an interior node split.
		test_command_setup_and_repeat(
			{
			"CreateTable Sample (A:String, B:String, C:String, D:String, E:String,"
			                    "F:String, G:String, H:String, I:String, J:String,"
			                    "K:String, L:String, M:String, N:String, O:String)"
			},
			"Insert INTO Sample {(\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\")}",
			511
		);
		test_command_setup_and_repeat(
			{
			"CreateTable Sample (A:String, B:String, C:String, D:String, E:String,"
			                    "F:String, G:String, H:String, I:String, J:String,"
			                    "K:String, L:String, M:String, N:String, O:String)"
			},
			"Insert INTO Sample {(\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\")}",
			512
		);
	}
}

} // namespace fmisql::test
