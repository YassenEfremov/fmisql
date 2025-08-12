#include "cmd_seq.hpp"

#include "../include/fmisql.hpp"
#include "../src/cli/parser.hpp"
#include "../src/commands/create_table.hpp"
#include "../src/commands/drop_table.hpp"
#include "../src/commands/list_tables.hpp"
#include "../src/commands/table_info.hpp"
#include "../src/commands/select.hpp"
#include "../src/commands/remove.hpp"
#include "../src/commands/insert.hpp"
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
		drop_table(statement.table_name);
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
		remove(statement.table_name, statement.condition);
		break;

	case Statement::Type::INSERT:
		insert(statement.table_name, statement.insert_rows);
		break;

	default:
		break;
	}
}

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

void test_command_setup_repeat_end(
	std::vector<std::string_view> setup_lines,
    std::string_view repeat_line,
    std::size_t n,
    std::vector<std::string_view> end_lines,
	Condition condition
) {
	std::cout << "Testing command sequence:\n  Setup:\n";
	for (std::string_view line : setup_lines) {
		std::cout << "    " << line << '\n';
	}
	std::cout << "  Repeating x" << n << " times:\n    " << repeat_line << '\n';

	std::remove("fmisql.db");
	init();
	// suppress cout?
	try {
		for (std::string_view line : setup_lines) {
			execute_command(line);
		}
		for (int i = 0; i < n; i++) {
			execute_command(repeat_line);
		}
		for (std::string_view line : end_lines) {
			execute_command(line);
		}

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
	// for (std::string_view line : setup_lines) {
	// 	try {
	// 		execute_command(line);
	// 	} catch (const std::runtime_error &e) {
	// 		std::cout << e.what() << '\n';
	// 		if (condition == Condition::SHOULD_FAIL) {
	// 			passed++;
	// 			std::cout << CYAN "  failed... OK\n" RESET;
	// 		} else {
	// 			failed++;
	// 			std::cout << RED "  FAILED\n" RESET;
	// 		}
			
	// 		total++;
	// 		deinit();
	// 		std::remove("fmisql.db");
	// 		std::cout << '\n';
	// 		return;
	// 	}
	// }
	// for (int i = 0; i < n; i++) {
	// 	try {
	// 		execute_command(repeat_line);
	// 	} catch (const std::runtime_error &e) {
	// 		std::cout << e.what() << '\n';
	// 		if (condition == Condition::SHOULD_FAIL) {
	// 			passed++;
	// 			std::cout << CYAN "  failed... OK\n" RESET;
	// 		} else {
	// 			failed++;
	// 			std::cout << RED "  FAILED\n" RESET;
	// 		}
			
	// 		total++;
	// 		deinit();
	// 		std::remove("fmisql.db");
	// 		std::cout << '\n';
	// 		return;
	// 	}
	// }
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

	/* Creating a lot of tables in order to cause leaf node splits */ {

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

		// I haven't tested internal node splitting using the CreateTable command
		// because that would require creating something like 2040 tables.
		// It could be automated, but I'm also lazy.
	}

	/* Inserting a lot of rows in order to cause interior node splits */ {

		// The page is 4096 bytes and excluding the header, the space for rows is
		//   4096 - 9 = 4087 bytes
		// Therefore the maximum size for a row that contains only strings is
		//   (4087 - 4) / 256 = ~15.9
		// So a table with 15 string columns would require one row to fill an entire page.
		// 512 rows in such a table would cause an interior node split.
		test_command_setup_repeat_end(
			{ "CreateTable Sample (A:String, B:String, C:String, D:String, E:String,"
			                      "F:String, G:String, H:String, I:String, J:String,"
			                      "K:String, L:String, M:String, N:String, O:String)" },
			"Insert INTO Sample {(\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\")}",
			511,
			{ "TableInfo Sample" }
		);
		test_command_setup_repeat_end(
			{ "CreateTable Sample (A:String, B:String, C:String, D:String, E:String,"
			                      "F:String, G:String, H:String, I:String, J:String,"
			                      "K:String, L:String, M:String, N:String, O:String)" },
			"Insert INTO Sample {(\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\")}",
			512,
			{ "TableInfo Sample" }
		);
		test_command_setup_repeat_end(
			{ "CreateTable Sample (A:String, B:String, C:String, D:String, E:String,"
			                      "F:String, G:String, H:String, I:String, J:String,"
			                      "K:String, L:String, M:String, N:String, O:String)" },
			"Insert INTO Sample {(\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\")}",
			600,
			{ "TableInfo Sample" }
		);

		// 256 more rows will cause another interior node split, so 768 total
		// which will require an insertion into the root note
		test_command_setup_repeat_end(
			{ "CreateTable Sample (A:String, B:String, C:String, D:String, E:String,"
			                      "F:String, G:String, H:String, I:String, J:String,"
			                      "K:String, L:String, M:String, N:String, O:String)" },
			"Insert INTO Sample {(\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\")}",
			768,
			{ "TableInfo Sample" }
		);

		// and just an arbitrary big number of rows
		test_command_setup_repeat_end(
			{ "CreateTable Sample (A:String, B:String, C:String, D:String, E:String,"
			                      "F:String, G:String, H:String, I:String, J:String,"
			                      "K:String, L:String, M:String, N:String, O:String)" },
			"Insert INTO Sample {(\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\")}",
			1500,
			{ "TableInfo Sample" }
		);
	}

	/* Dropping some tables */ {

		// correct commands
		test_command_sequence({
			"CreateTable Sample1 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample2 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample3 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample4 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample5 (ID:Int, Name:String, Value:Int)",
			"CreateTable Sample6 (ID:Int, Name:String, Value:Int)",
			"ListTables",
			"DropTable Sample6",
			"DropTable Sample1",
			"DropTable Sample3",
			"ListTables"
		});

		// wrong commands
		test_command_sequence({
			"DropTable SomeTable"
		}, Condition::SHOULD_FAIL);
	}

	/* Removing from tables */ {

		test_command_sequence({
			"CreateTable Sample (ID:Int, Name:String, Value:Int)",
			"Insert INTO Sample {(1, \"test\", 12), (2, \"hello\", 10), (3, \"bye\", 4)}",
			"Remove FROM Sample WHERE ID = 1",
			"Select * FROM Sample"
		});
		test_command_sequence({
			"CreateTable Sample (ID:Int, Name:String, Value:Int)",
			"Insert INTO Sample {(1, \"test\", 12), (2, \"hello\", 10), (3, \"bye\", 4)}",
			"Remove FROM Sample WHERE ID != 1",
			"Select * FROM Sample"
		});
		test_command_sequence({
			"CreateTable Sample (ID:Int, Name:String, Value:Int)",
			"Insert INTO Sample {(1, \"test\", 12), (2, \"hello\", 10), (3, \"bye\", 4)}",
			"Remove FROM Sample WHERE Name = \"hello\"",
			"Select * FROM Sample"
		});
	}

	/* Dropping tables in order to cause leaf node merges */ {

		// refer to btree.hpp for explanation of the 5 possible cases that
		// can occur when removing cells from leaf nodes

		// case 1 - tested in the previous section (Dropping some tables)

		// case 2
		// dropping tables Sample1 and Sample2 requires taking cells from
		// the right sibling node
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
			"ListTables",
			"DropTable Sample1",
			"DropTable Sample2",
			"ListTables"
		});

		// case 3
		// dropping tables Sample2 and Sample3 requires merging two of the leaf
		// nodes into one
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
			"CreateTable Sample12 (ID:Int, Name:String, Value:Int)",
			"ListTables",
			"DropTable Sample2",
			"ListTables"
		});
		// there is one more subcase here, which is when we merge the last two
		// leafs, resulting in a new right child
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
			"CreateTable Sample12 (ID:Int, Name:String, Value:Int)",
			"ListTables",
			"DropTable Sample7",
			"ListTables"
		});

		// case 4 - this case cannot occur in the current implementation of the
		// database because of the constantly incrementing key values

		// case 5
		// dropping table Sample12 requires merging the last two leaf nodes into
		// one
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
			"CreateTable Sample12 (ID:Int, Name:String, Value:Int)",
			"ListTables",
			"DropTable Sample12",
			"ListTables"
		});
	}

	/* Removing a lot of rows in order to cause interior node merges */ {

		// test_command_setup_repeat_end(
		// 	{ "CreateTable Sample (A:String, B:String, C:String, D:String, E:String,"
		// 	                      "F:String, G:String, H:String, I:String, J:String,"
		// 	                      "K:String, L:String, M:String, N:String, O:String)" },
		// 	"Insert INTO Sample {(\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\")}",
		// 	511,
		// 	{
		// 		"Insert INTO Sample {(\"a\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\",\"s\")}",
		// 		"TableInfo Sample",
		// 		"Remove FROM Sample WHERE A == \"a\""
		// 	}
		// );
	}
}

} // namespace fmisql::test
