#include "drop_table.hpp"

#include "../btree.hpp"
#include "../constants.hpp"
#include "../data_types.hpp"
#include "../pager.hpp"
#include "../statement.hpp"
#include "../schema.hpp"

#include <string_view>


namespace fmisql {
	
void drop_table(std::string_view table_name) {

	BplusTree &schema_BplusTree = BplusTree::get_schema();
	SchemaRow schema_row = BplusTree::get_schema_row_by_table_name(table_name);

	// TODO: for this to work properly a lot of stuff with the pager needs to change

	BplusTree::drop(schema_row.root_page);
	schema_BplusTree.remove(schema_row.root_page);

	std::cout << "Table " << table_name << " dropped!\n";
}
	
} // namespace fmisql
