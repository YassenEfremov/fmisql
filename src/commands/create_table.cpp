#include "create_table.hpp"

#include "../btree.hpp"
#include "../constants.hpp"
#include "../data_types.hpp"
#include "../pager.hpp"
#include "../statement.hpp"
#include "../schema.hpp"

#include <cstdint>

#include <iostream>
#include <string>
#include <string_view>
#include <vector>


namespace fmisql {

// Example create statements:
// CreateTable Sample (ID:Int, Name:String, Value:Int)
// CreateTable Sample2 (ID:Int, Name:String, Value:Int)
// CreateTable Sample3 (ID:Int, Name:String, Value:Int)
// CreateTable Sample4 (ID:Int, Name:String, Value:Int)
// CreateTable Sample5 (ID:Int, Name:String, Value:Int)
// CreateTable Sample6 (ID:Int, Name:String, Value:Int)
// CreateTable Sample7 (ID:Int, Name:String, Value:Int)

void create_table(std::string_view table_name, const std::vector<sql_types::Column> &columns,
	std::string_view original_sql) {

	BplusTree &schema_BplusTree = BplusTree::get_schema();

	// TODO: use some form of a select statement instead of a loop
	SchemaRow row("", 0, "");
	for (int i = 0; i < schema_BplusTree.get_cell_count(); i++) {
		row.deserialize(schema_BplusTree.get_cell_value(i));

		if (row.table_name == table_name) {
			throw std::runtime_error("Table " + std::string(table_name) + " already exists!");
		}
	}

	BplusTree &table_BplusTree = BplusTree::create(columns);
	SchemaRow schema_row(table_name, table_BplusTree.get_root_page(), original_sql);

	std::uint8_t buffer[schema_row_size];
	schema_row.serialize(buffer);
	schema_BplusTree.insert(schema_row.root_page, buffer);

	std::cout << "Table " << table_name << " created!\n";
}

} // namespace fmisql
