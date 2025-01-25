#include "list_tables.hpp"

#include "../btree.hpp"

#include <iostream>


namespace fmisql {
	
void list_tables() {

	LeafNode node;

	// std::cout << '|';
	// for (std::string_view column : columns) {
	// 	std::cout << ' ' << column << " |";
	// }
	// std::cout << '\n';
	// std::cout << "------------------\n";

	std::cout << "There are " << *node.get_pair_count() << " table/s in the database:\n";

	SchemaRow row("", 0, "");
	for (int i = 0; i < *node.get_pair_count(); i++) {
		row.deserialize(Pager::row_slot(i));

		std::cout << "    " << row.table_name << '\n';

		// std::cout << '|';
		// if (columns_contain(columns, "ID")) {
		// 	std::cout << ' ' << row.ID << " |";
		// }
		// if (columns_contain(columns, "Name")) {
		// 	std::cout << " \"" << row.Name << "\" |";
		// }
		// if (columns_contain(columns, "Value")) {
		// 	std::cout << ' ' << row.Value << " |";
		// }
		// std::cout << '\n';
	}

	// std::cout << "Total " << *node.get_pair_count() << " row/s selected\n";
}

} // namespace fmisql
