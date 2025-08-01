cmake_minimum_required(VERSION 3.27)
project(fmisql
	VERSION 0.1.0
	DESCRIPTION "FMISql"
	HOMEPAGE_URL ""
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSION FALSE)

add_executable(fmisql
	src/cli/cli.cpp
	src/cli/parser.cpp
	src/commands/create_table.cpp
	# src/commands/drop_table.cpp
	src/commands/list_tables.cpp
	src/commands/table_info.cpp
	src/commands/insert.cpp
	src/commands/select.cpp
	src/btree.cpp
	src/fmisql.cpp
	src/pager.cpp
	src/schema.cpp
	src/statement.cpp
	src/main.cpp
)

add_executable(test
	src/cli/cli.cpp
	src/cli/parser.cpp
	src/commands/create_table.cpp
	# src/commands/drop_table.cpp
	src/commands/list_tables.cpp
	src/commands/table_info.cpp
	src/commands/insert.cpp
	src/commands/select.cpp
	src/btree.cpp
	src/fmisql.cpp
	src/pager.cpp
	src/schema.cpp
	src/statement.cpp

	# test/util.cpp
	# test/parser.cpp
	# test/cmd_seq.cpp
	# test/main.cpp
	test/cli.cpp
)

if (MSVC)
	# compliant by default?
else()
	target_compile_options(fmisql PRIVATE -pedantic-errors -g)
endif()
