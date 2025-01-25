#include "create_table.hpp"

#include "../btree.hpp"
#include "../data_types.hpp"
#include "../statement.hpp"

#include <iostream>
#include <string_view>


namespace fmisql {

// Example create statements:
// CreateTable Sample (ID:Int, Name:String, Value:Int)

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

	SchemaRow row(name, 0, original_sql);

	LeafNode node;
	std::uint32_t pair_count = *node.get_pair_count();

	std::cout << "starting from: " << pair_count << '\n';

	if (pair_count >= LEAF_NODE_MAX_CELLS) {
		// TODO
		throw std::logic_error("splitting not implemented yet");
	}

	try {
		std::cout << "hash: " << std::hash<std::string_view>()(name) << '\n';
		node.insert(std::hash<std::string_view>()(name), row);
	} catch (const std::runtime_error &e) {
		std::cout << "error: " << e.what() << '\n';
	}

	std::cout << "Table " << name << " created!\n";
}

} // namespace fmisql
