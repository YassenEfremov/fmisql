#include "insert.hpp"

#include "../btree.hpp"
#include "../pager.hpp"

#include <cstring>

#include <string_view>
#include <vector>


namespace fmisql {

// Insert INTO Sample {(1, "Test message", 120)}

void insert(std::string_view table_name, const std::vector<ExampleRow> rows) {

	int cell_number = 0;

	for (const ExampleRow &row : rows) {

		LeafNode node(Pager::get_page(0));

		std::uint32_t cell_count = *node.get_cell_count();
		if (cell_count >= LEAF_NODE_MAX_CELLS) {
			// TODO
			throw std::logic_error("splitting not implemented yet");
		}	

		std::cout << "cell_number: " << cell_number << '\n';
		std::cout << "cells in page 0: " << cell_count << '\n';
		if (cell_number < cell_count) {
			// move cells to the right
			for (int i = cell_count; i > cell_number; i--) {
				std::memcpy(node.get_cell(i), node.get_cell(i - 1), LEAF_NODE_CELL_SIZE);
			}
		}

		*node.get_cell_count() += 1;
		*node.get_cell_key(cell_number) = cell_number;
		row.serialize(node.get_cell_value(cell_number));

		cell_number++;
	}
}

} // namespace fmisql
