#include "list_tables.hpp"

#include "../btree.hpp"

#include <iostream>


namespace fmisql {
	
void list_tables() {

	LeafNode node(0, schema_row_size);

	std::cout << "There are " << *node.get_pair_count() << " table/s in the database:\n";

	SchemaRow row("", 0, "");
	for (int i = 0; i < *node.get_pair_count(); i++) {
		row.deserialize(node.get_pair_value(i));

		std::cout << "    " << row.table_name << '\n';
	}
}

} // namespace fmisql
