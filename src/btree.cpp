#include "btree.hpp"

#include <cstdint>


namespace fmisql {

LeafNode::LeafNode(void *data) : data(data) {}

std::uint32_t *LeafNode::get_cell_count() {
	return (std::uint32_t *)((std::uint8_t *)this->data + LEAF_NODE_NUM_CELLS_OFFSET);
}

void *LeafNode::get_cell(int cell_number) {
	return (std::uint8_t *)this->data + LEAF_NODE_HEADER_SIZE + cell_number * LEAF_NODE_CELL_SIZE;
}

std::uint32_t *LeafNode::get_cell_key(int cell_number) {
	return (std::uint32_t *)this->get_cell(cell_number);
}

void *LeafNode::get_cell_value(int cell_number) {
	return (std::uint8_t *)this->get_cell(cell_number) + LEAF_NODE_KEY_SIZE;
}

} // namespace fmisql
