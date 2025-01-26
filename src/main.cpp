#include "cli/cli.hpp"
#include "pager.hpp"
// #include "commands/create_table.hpp"
// #include "commands/drop_table.hpp"
// #include "commands/list_tables.hpp"

#include <iostream>


int main() {

	fmisql::db_init();

	fmisql::cli();

	fmisql::db_deinit();

	// create_table("students", {{"ID", INT}, {"Name", STRING}, {"BDay", DATE}});

	// auto tables = list_tables();
	// for (auto table : tables) {
	// 	std::cout << "[] " << table << '\n';
	// }

	// drop_table("students");

	return 0;
}
