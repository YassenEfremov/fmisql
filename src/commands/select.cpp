#include "select.hpp"

#include "../btree.hpp"
#include "../pager.hpp"

#include <cstring>

#include <string_view>
#include <vector>


namespace fmisql {

void select() {

	LeafNode node(Pager::get_page(0));

	ExampleRow row;
	for (int i = 0; i < *node.get_cell_count(); i++) {
		row.deserialize(Pager::row_slot(i));
		std::cout << "| " << row.ID << " | " << row.Name << " | " << row.Value << " |\n";
	}

	std::cout << "Total " << *node.get_cell_count() << " row/s selected\n";
}

} // namespace fmisql
