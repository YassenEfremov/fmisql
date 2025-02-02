#!/usr/bin/bash


FMISQL_EXEC_NAME="/home/yassen/FMI/SecondYear/DataStructures/Project/fmisql/build/fmisql"
FMISQL_DB_NAME="/home/yassen/FMI/SecondYear/DataStructures/Project/fmisql/build/fmisql.db"

GREEN="\x1b[32m"
RED="\x1b[31m"
CYAN="\x1b[36m"
RESET="\x1b[0m"


function test_command() {
	printf "Test:\n\"$1\"\n"
	rm -f $FMISQL_DB_NAME
	output=$(printf "$1\n" | $FMISQL_EXEC_NAME)
	result=$(printf "$output" | grep "Table Sample created!")
	if [ -z "$result" ]; then
		if [ "$2" = "should fail" ]; then
			printf "$CYAN failed... OK$RESET\n"
		else
			printf "$RED  FAILED$RESET\n"
			printf "$output\n"
			exit 1
		fi
	else
		if [ "$2" = "should fail" ]; then
			printf "$RED  FAILED$RESET\n"
			printf "$output\n"
			exit 1
		else
			printf "$GREEN  OK$RESET\n"
		fi
	fi
	printf "\n"
}


# CreateTable

# correct commands

# with one column
test_command "CreateTable Sample (id:Int)"
test_command "CreateTable Sample (id: Int)"
test_command "  CreateTable  Sample  (  id  :  Int  )  "
# with multiple columns
test_command "CreateTable Sample (id:Int, name:String)"
test_command "CreateTable Sample (id:Int,name:String)"
test_command "CreateTable Sample (id:Int ,name:String)"
test_command "CreateTable Sample (id: Int, name: String)"
test_command "CreateTable Sample (id :Int, name :String)"
test_command "  CreateTable  Sample  (  id  :  Int  ,  name  :  String  )  "

# wrong commands

# with one column
test_command "CreateTable Sample id:Int)"						"should fail"
test_command "CreateTable Sample  id  :  Int  )  "				"should fail"
test_command "CreateTable Sample (id:Int"						"should fail"
test_command "CreateTable Sample  (  id  :  Int  "				"should fail"
test_command "CreateTable Sample id:Int"						"should fail"
test_command "CreateTable Sample  id  :  Int  "					"should fail"
test_command "CreateTable Sample (idInt)"						"should fail"
test_command "CreateTable Sample (id Int)"						"should fail"
test_command "CreateTable Sample (id, )"						"should fail"
test_command "CreateTable Sample (, Int)"						"should fail"
test_command "CreateTable Sample (,)"							"should fail"
test_command "CreateTable Sample ()"							"should fail"
test_command "CreateTable Sample (  )"							"should fail"
test_command "CreateTable Sample ("								"should fail"
test_command "CreateTable Sample )"								"should fail"
# with multiple columns
# test_command "CreateTable Sample id:Int, name:String)"			"should fail"
# test_command "CreateTable Sample  id  :  Int  ,  name  :  String  )  "	"should fail"
# test_command "CreateTable Sample (id:Int, name:String"			"should fail"
# test_command "CreateTable Sample  (  id : Int, name:String "	"should fail"
# test_command "CreateTable Sample id:Int, name:String"			"should fail"
# test_command "CreateTable Sample  id : Int , name : String "	"should fail"
# test_command "CreateTable Sample (idInt, name:String )"			"should fail"


# # DropTable

# # correct commands
# test_command "DropTable Sample"
# test_command "  DropTable  Sample  "

# # wrong commands
# test_command "DropTable"										"should fail"
# test_command "  DropTable  "									"should fail"
