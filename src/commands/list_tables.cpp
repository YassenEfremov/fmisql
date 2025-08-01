#include "list_tables.hpp"

#include "../btree.hpp"
#include "../schema.hpp"
#include "../pager.hpp"

#include <cstdint>

#include <iostream>


namespace fmisql {
	
void list_tables() {

	BplusTree &schema_BplusTree = BplusTree::get_schema();
	std::uint32_t table_count = schema_BplusTree.get_cell_count();

	std::cout << "There are " << table_count << " table/s in the database:\n";

	SchemaRow row("", 0, "");
	for (int i = 0; i < table_count; i++) {
		row.deserialize(schema_BplusTree.get_cell_value(i));

		std::cout << "    " << row.table_name << '\n';
	}
}

} // namespace fmisql
