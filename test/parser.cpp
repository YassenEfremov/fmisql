#include "parser.hpp"

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


namespace fmisql::test {

void test_command(std::string_view line, Condition condition) {
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
			break;
		}
	}
	total++;
	std::cout << '\n';
}

void test_parser() {

	/* CreateTable command */ {

		// by requirement
		test_command("CreateTable Sample (ID:Int, Name:String, Value:Int)");

		// correct commands
		test_command("CreateTable Sample (id:Int)");
		test_command("CreateTable S1mple (id:Int)");
		test_command("CreateTable Sample(id: Int)");
		test_command("  CreateTable  Sample  (  id  :  Int  )  ");

		test_command("CreateTable Sample (id:Int, name:String)");
		test_command("CreateTable Sample (id:Int,name:String)");
		test_command("CreateTable Sample (id:Int ,name:String)");
		test_command("CreateTable Sample (id: Int, name: String)");
		test_command("CreateTable Sample (id :Int, name :String)");
		test_command("  CreateTable  Sample  (  id  :  Int  ,  name  :  String  )  ");

		// wrong commands
		test_command("CreateTable 5ample (id:Int)", Condition::SHOULD_FAIL);
		test_command("CreateTable Samp!e (id:Int)", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample id:Int)", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample  id  :  Int  )  ", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (id:Int", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample  (  id  :  Int  ", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample id:Int", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample  id  :  Int  ", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (idInt)", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (id Int)", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (id:)", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (:String)", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (id::Int)", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (id, )", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (, Int)", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (,)", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample ()", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (  )", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample ("	, Condition::SHOULD_FAIL);
		test_command("CreateTable Sample )"	, Condition::SHOULD_FAIL);
		test_command("CreateTable Sample"	, Condition::SHOULD_FAIL);
	
		test_command("CreateTable Sample id:Int, name:String)", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample  id  :  Int  ,  name  :  String  )  ", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (id:Int, name:String", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample  (  id : Int, name:String ", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample id:Int, name:String", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample  id : Int , name : String ", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (idInt, name:String)", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (id:Int, nameString)", Condition::SHOULD_FAIL);

		// logically wrong commands
		test_command("CreateTable Sample (name:Strin)", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (ID:Int, ID:String)");
		test_command("CreateTable Sample (ID:Int, ID:String, ID:Date)");
		test_command("CreateTable Sample (ID:Int, ID:Int)", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (ID:Int, ID:String, ID:Int)", Condition::SHOULD_FAIL);
		test_command("CreateTable Sample (ID:Int, ID:String, ID:String)", Condition::SHOULD_FAIL);

		// command limits
		test_command(
			"CreateTable                        Sample               (                        ID         :          Int                     ,         Name         :             String                  ,              Value             :   Int                           )"
		);
		test_command(
			"CreateTable                         Sample               (                        ID         :          Int                     ,         Name         :             String                  ,              Value             :   Int                           )"
		, Condition::SHOULD_FAIL);
		test_command(
			"                   CreateTable                        Sample               (                        ID         :          Int                     ,         Name         :             String                  ,              Value             :   Int                           )                   "
		);
		test_command(
			"                   CreateTable                         Sample               (                        ID         :          Int                     ,         Name         :             String                  ,              Value             :   Int                           )                   "
		, Condition::SHOULD_FAIL);
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
		test_command("ListTables"); // by requirement
		test_command("  ListTables  ");

		// wrong commands
		// can't mess it up :)
	}

	/* TableInfo command */ {
		
		// correct commands
		test_command("TableInfo Sample"); // by requirement
		test_command("  TableInfo  Sample  ");

		// wrong commands
		test_command("TableInfo", Condition::SHOULD_FAIL);
		test_command("  TableInfo  ", Condition::SHOULD_FAIL);
	}

	/* Select command */ {

		// by requirement
		// test_command("Select Name FROM Sample WHERE ID != 5 AND Value < 50");
		// test_command("Select * FROM Sample WHERE ID != 5 AND Name > \"Baba\" ORDER BY Name");
		
		// correct commands
		test_command("Select Name FROM Sample");
		test_command("  Select  Name  FROM  Sample  ");
		test_command("Select Id, Name FROM Sample");
		test_command("  Select  Id  ,  Name  FROM  Sample  ");
		test_command("Select * FROM Sample");
		test_command("  Select  *  FROM  Sample  ");
		test_command("Select Id FROM Sample WHERE Id = 12");
		test_command("  Select  Id  FROM  Sample  WHERE  Id  =  12  ");
		test_command("Select Name FROM Sample WHERE Name = \"Baba\"");
		test_command("  Select  Name  FROM  Sample  WHERE  Name  =  \"Baba\"  ");
		test_command("Select Name FROM Sample WHERE Name = \"Baba\" ORDER BY Name");
		// test_command("Select Id, Name FROM Sample WHERE Id=5 AND Name=\"Baba\" OR Id=6 AND Name!=\"Baba\" ORDER BY Name");

		// wrong commands
		test_command("Select FROM Sample", Condition::SHOULD_FAIL);
		test_command("Select Name Sample", Condition::SHOULD_FAIL);
		test_command("Select Name FROM", Condition::SHOULD_FAIL);
		test_command("Select FROM", Condition::SHOULD_FAIL);
		test_command("Select Sample", Condition::SHOULD_FAIL);
		test_command("Select", Condition::SHOULD_FAIL);
		test_command("Select Id, Name FROM Sample WHERE", Condition::SHOULD_FAIL);
		test_command("Select Id, Name FROM Sample WHERE Id", Condition::SHOULD_FAIL);
		test_command("Select Id, Name FROM Sample WHERE Name !=", Condition::SHOULD_FAIL);
		test_command("Select Id, Name FROM Sample WHERE != \"Baba\"", Condition::SHOULD_FAIL);
		test_command("Select Id, Name FROM Sample WHERE Name \"Baba\"", Condition::SHOULD_FAIL);
		test_command("Select Id, Name FROM Sample WHERE Id == 1 AND", Condition::SHOULD_FAIL);
		test_command("Select Id, Name FROM Sample WHERE Id == 1 AND Name", Condition::SHOULD_FAIL);
		test_command("Select Id, Name FROM Sample WHERE Id == 1 AND ==", Condition::SHOULD_FAIL);
		test_command("Select Id, Name FROM Sample WHERE Name AND Id > 5", Condition::SHOULD_FAIL);
		test_command("Select Id, Name FROM Sample WHERE != \"Baba\" AND Id > 5", Condition::SHOULD_FAIL);
	}

	/* Remove command */ {
		
		// correct commands
		test_command("Remove FROM Sample");
		test_command("  Remove  FROM  Sample  ");
		test_command("Remove FROM Sample WHERE Id = 12");

		// wrong commands
		test_command("Remove Sample", Condition::SHOULD_FAIL);
		test_command("Remove FROM", Condition::SHOULD_FAIL);
		test_command("Remove", Condition::SHOULD_FAIL);
		// conditions after WHERE clause are parsed identically to those for
		// Select command, so no need to test again
	}

	/* Insert command */ {

		// by requirement
		test_command("Insert INTO Sample {(1, \"Test\", 1), (2, \"something else\", 100)}");
		
		// correct commands
		test_command("Insert INTO Sample {(12)}");
		test_command("Insert INTO Sample {(12), (13)}");
		test_command("Insert INTO Sample {(12, \"Some text\")}");
		test_command("Insert INTO Sample {(12, \"Some text\", \"abc\", 0), (12, \"Some more text\", \"efg\", 1)}");
		test_command("  Insert  INTO  Sample  {  (  12  ,  \"Some text\")  ,  (  13  ,  \"Some more text\"  )  }  ");

		// wrong commands
		test_command("Insert Sample {(12)}", Condition::SHOULD_FAIL);
		test_command("Insert INTO {(12)}", Condition::SHOULD_FAIL);
		test_command("Insert INTO Sample", Condition::SHOULD_FAIL);
		test_command("Insert INTO", Condition::SHOULD_FAIL);
		test_command("Insert Sample", Condition::SHOULD_FAIL);
		test_command("Insert {(12)}", Condition::SHOULD_FAIL);
		test_command("Insert INTO Sample (12)}", Condition::SHOULD_FAIL);
		test_command("Insert INTO Sample {(12)", Condition::SHOULD_FAIL);
		test_command("Insert INTO Sample (12)", Condition::SHOULD_FAIL);
		test_command("Insert INTO Sample {12)", Condition::SHOULD_FAIL);
		test_command("Insert INTO Sample (12}", Condition::SHOULD_FAIL);
		test_command("Insert INTO Sample 12)", Condition::SHOULD_FAIL);
		test_command("Insert INTO Sample (12", Condition::SHOULD_FAIL);
		test_command("Insert INTO Sample 12", Condition::SHOULD_FAIL);
		test_command("Insert INTO Sample {()}", Condition::SHOULD_FAIL);
		test_command("Insert INTO Sample {(12 \"Some text\")}", Condition::SHOULD_FAIL);
		test_command("Insert INTO Sample {(12, , \"Some text\")}", Condition::SHOULD_FAIL);
		test_command("Insert INTO Sample {(12, \"Some text\") (13, \"Some more text\")}", Condition::SHOULD_FAIL);
		test_command("Insert INTO Sample {(12, \"Some text\"), , (14, \"Some even more text\")}", Condition::SHOULD_FAIL);
	}
}

}
