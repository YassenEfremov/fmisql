#include "cli/cli.hpp"
// #include "commands/create_table.hpp"
// #include "commands/drop_table.hpp"
// #include "commands/list_tables.hpp"
// #include "tables_info.hpp"

#include <iostream>


int main() {

	cli();

	// tables_info_add("table1");
	// tables_info_add("table2");
	// tables_info_add("table3");
	// tables_info_add("table4");

	// auto tables = tables_info_list();
	// for (auto table : tables) {
	// 	std::cout << "[] " << table << '\n';
	// }

	// tables_info_remove("table1");

	// create_table("students", {{"ID", INT}, {"Name", STRING}, {"BDay", DATE}});

	// auto tables = list_tables();
	// for (auto table : tables) {
	// 	std::cout << "[] " << table << '\n';
	// }

	// drop_table("students");

	return 0;
}
