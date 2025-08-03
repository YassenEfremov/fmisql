#include "list_tables.hpp"

#include "../btree.hpp"
#include "../constants.hpp"
#include "../schema.hpp"
#include "../pager.hpp"

#include <cstdint>

#include <iostream>


namespace fmisql {
	
void list_tables() {

	BplusTree &schema_BplusTree = BplusTree::get_schema();
	std::uint32_t table_count = schema_BplusTree.get_total_cell_count();

	std::cout << "There are " << table_count << " table/s in the database:\n";

	SchemaRow row;
	for (void *cell : schema_BplusTree) {
		row.deserialize(((std::uint8_t *)cell) + key_size);

		std::cout << "    " << row.table_name << '\n';
	}
}

} // namespace fmisql
