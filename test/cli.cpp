#include "../src/cli/parser.hpp"
#include "../src/commands/create_table.hpp"
#include "../src/commands/list_tables.hpp"
#include "../src/commands/table_info.hpp"
#include "../src/commands/insert.hpp"
#include "../src/commands/select.hpp"
#include "../src/statement.hpp"

#include <cstdlib>

#include <iostream>
#include <string>
#include <string_view>


#define GREEN "\x1b[32m"
#define RED "\x1b[31m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"


int total = 0;
int passed = 0;
int failed = 0;

enum class Condition {
	SHOULD_PASS,
	SHOULD_FAIL
};

/**
 * @brief Tests whether the syntax of the given command is correct.
 *        Default condition is SHOULD_PASS.
 */
void test_command(std::string_view line, Condition condition = Condition::SHOULD_PASS) {
	std::cout << "Testing command:\n\"" << line <<  "\"\n";
	fmisql::Statement statement = fmisql::parse_line(line);
	if (statement.type != fmisql::Statement::Type::INVALID) {
		switch (condition) {
		case Condition::SHOULD_PASS:
			passed++;
			std::cout << GREEN "  OK\n" RESET;
			break;
		case Condition::SHOULD_FAIL:
			failed++;
			std::cout << RED "  FAILED\n" RESET;
			break;
		}
	} else {
		switch (condition) {
		case Condition::SHOULD_PASS:
			failed++;			
			std::cout << RED "  FAILED\n" RESET;
			break;
		case Condition::SHOULD_FAIL:
			passed++;
			std::cout << CYAN "  failed... OK\n" RESET;
		}
	}
	total++;
	std::cout << '\n';
}


int main() {

	/* CreateTable command */ {

		// by requirement
		test_command("CreateTable Sample (ID:Int, Name:String, Value:Int)");

		// correct commands

		// with one column
		test_command("CreateTable Sample (id:Int)");
		test_command("CreateTable Sample (id: Int)");
		test_command("  CreateTable  Sample  (  id  :  Int  )  ");
		// with multiple columns
		test_command("CreateTable Sample (id:Int, name:String)");
		test_command("CreateTable Sample (id:Int,name:String)");
		test_command("CreateTable Sample (id:Int ,name:String)");
		test_command("CreateTable Sample (id: Int, name: String)");
		test_command("CreateTable Sample (id :Int, name :String)");
		test_command("  CreateTable  Sample  (  id  :  Int  ,  name  :  String  )  ");

		// wrong commands

		// with one column
		test_command("CreateTable Sample id:Int)", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample  id  :  Int  )  ", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (id:Int", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample  (  id  :  Int  ", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample id:Int", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample  id  :  Int  ", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (idInt)", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (id Int)", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (id, )", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (, Int)", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (,)", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample ()", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (  )", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample ("	, Condition::SHOULD_FAIL);
		test_command("CreateTable Sample )"	, Condition::SHOULD_FAIL);
		// with multiple columns
		test_command("CreateTable Sample id:Int, name:String)", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample  id  :  Int  ,  name  :  String  )  ", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (id:Int, name:String", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample  (  id : Int, name:String ", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample id:Int, name:String", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample  id : Int , name : String ", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (idInt, name:String)", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (id:Int, nameString)", Condition::SHOULD_FAIL);
	}

	/* DropTable command */ {

		// correct commands

		test_command("DropTable Sample");
		test_command("  DropTable  Sample  ");

		// wrong commands

		test_command("DropTable", Condition::SHOULD_FAIL);
		test_command("  DropTable  ", Condition::SHOULD_FAIL);
	}

	/* ListTables command */ {

		// correct commands

		test_command("ListTables");
		test_command("  ListTables  ");

		// wrong commands

		// can't mess it up :)
	}

	/* TableInfo command */ {
		
		// correct commands

		test_command("TableInfo Sample");
		test_command("  TableInfo  Sample  ");

		// wrong commands

		test_command("TableInfo", Condition::SHOULD_FAIL);
		test_command("  TableInfo  ", Condition::SHOULD_FAIL);
	}

	// /* Select command */ {

	// 	// by requirement
	// 	test_command("Select Name FROM Sample WHERE ID != 5 AND Value < 50");
	// 	test_command("Select * FROM Sample WHERE ID != 5 AND Name > “Baba” ORDER BY Name");
		
	// 	// correct commands

	// 	test_command("Select Sample");

	// 	// wrong commands


	// }

	std::cout << "Results:\n"
		<< total << " total, "
		<< passed << GREEN " passed, " RESET
		<< failed << RED " failed" RESET;

	return 0;
}
