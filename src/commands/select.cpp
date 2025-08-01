#include "select.hpp"

#include "../btree.hpp"
#include "../data_types.hpp"

#include <cstring>

#include <iostream>
#include <string_view>
#include <vector>


namespace fmisql {

static bool columns_contain(const std::vector<std::string_view> &columns,
                            std::string_view target) {

	if (columns.empty()) return true;

	for (std::string_view column : columns) {
		if (column == target) return true;
	}

	return false;
}

void select(const std::vector<std::string_view> &columns,
            std::string_view table_name) {

	// Node node(Node::number_of_table(table_name));
	// int leaf_number = node.start_leaf();
	// // std::cout << "start leaf: " << leaf_number << '\n';
	// int rows_selected = 0;

	// std::cout << '|';
	// for (std::string_view column : columns) {
	// 	std::cout << ' ' << column << " |";
	// }
	// std::cout << '\n';
	// std::cout << "------------------\n";

	// while (leaf_number) {
	// 	Node leaf(leaf_number);
	// 	sql_types::ExampleRow row;
	// 	for (int i = 0; i < *leaf.pair_count(); i++, rows_selected++) {
	// 		row.deserialize(leaf.pair_value(i));

	// 		std::cout << '|';
	// 		if (columns_contain(columns, "ID")) {
	// 			std::cout << ' ' << row.ID << " |";
	// 		}
	// 		if (columns_contain(columns, "Name")) {
	// 			std::cout << " \"" << row.Name << "\" |";
	// 		}
	// 		if (columns_contain(columns, "Value")) {
	// 			std::cout << ' ' << row.Value << " |";
	// 		}
	// 		std::cout << '\n';
	// 	}
	// 	leaf_number = *leaf.next_leaf();
	// 	// std::cout << "next leaf: " << leaf_number << '\n';
	// }

	// std::cout << "Total " << rows_selected << " row/s selected\n";
}

} // namespace fmisql
