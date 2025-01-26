#include "create_table.hpp"

#include "../btree.hpp"
#include "../constants.hpp"
#include "../data_types.hpp"
#include "../pager.hpp"
#include "../statement.hpp"

#include <cstdint>

#include <iostream>
#include <string_view>


namespace fmisql {

// Example create statements:
// CreateTable Sample (ID:Int, Name:String, Value:Int)
// CreateTable Sample2 (ID:Int, Name:String, Value:Int)
// CreateTable Sample3 (ID:Int, Name:String, Value:Int)
// CreateTable Sample4 (ID:Int, Name:String, Value:Int)
// CreateTable Sample5 (ID:Int, Name:String, Value:Int)
// CreateTable Sample6 (ID:Int, Name:String, Value:Int)
// CreateTable Sample7 (ID:Int, Name:String, Value:Int)

void create_table(std::string_view name, std::vector<Column> columns,
	std::string_view original_sql) {

	// for (auto tc : table_columns) {
	// 	std::cout << tc.name << " - ";
	// 	switch (tc.type) {
	// 		case DataType::INT: std::cout << "INT\n"; break;
	// 		case DataType::STRING: std::cout << "STRING\n"; break;
	// 		case DataType::DATE: std::cout << "DATE\n"; break;
	// 	}
	// }

	LeafNode node(0, schema_row_size);

	SchemaRow row(name, Pager::page_count, original_sql);

	std::uint32_t row_size = 0;
	for (Column column : columns) {
		switch (column.type_id) {
		case sql_types::Id::INT:
			row_size += sizeof(sql_types::Int);
			break;
		case sql_types::Id::STRING:
			row_size += sizeof(sql_types::String);
			break;
		case sql_types::Id::DATE:
			break;
		}
	}
	LeafNode new_node(Pager::page_count, row_size);

	try {
		std::uint8_t buf[schema_row_size];
		row.serialize(buf);
		node.insert(std::hash<std::string_view>()(name), buf);
	} catch (const std::runtime_error &e) {
		std::cout << "error: " << e.what() << '\n';
	}

	std::cout << "Table " << name << " created!\n";
}

} // namespace fmisql
