#include "select.hpp"

#include "../btree.hpp"
#include "../data_types.hpp"
#include "../pager.hpp"

#include <cstring>

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

	LeafNode node;

	std::cout << '|';
	for (std::string_view column : columns) {
		std::cout << ' ' << column << " |";
	}
	std::cout << '\n';
	std::cout << "------------------\n";

	sql_types::ExampleRow row;
	for (int i = 0; i < *node.get_pair_count(); i++) {
		row.deserialize(Pager::row_slot(i));

		std::cout << '|';
		if (columns_contain(columns, "ID")) {
			std::cout << ' ' << row.ID << " |";
		}
		if (columns_contain(columns, "Name")) {
			std::cout << " \"" << row.Name << "\" |";
		}
		if (columns_contain(columns, "Value")) {
			std::cout << ' ' << row.Value << " |";
		}
		std::cout << '\n';
	}

	std::cout << "Total " << *node.get_pair_count() << " row/s selected\n";
}

} // namespace fmisql
