#include "btree.hpp"

#include "data_types.hpp"
#include "pager.hpp"
#include "schema.hpp"

#include <cstdint>

#include <stdexcept>
#include <iostream>


namespace fmisql {

LeafNode::LeafNode(void *data) : data(data) {}

std::uint32_t *LeafNode::get_pair_count() {
	return (std::uint32_t *)((std::uint8_t *)this->data + LEAF_NODE_NUM_CELLS_OFFSET);
}

void *LeafNode::get_pair(int pair_index) {
	return (std::uint8_t *)this->data + LEAF_NODE_HEADER_SIZE + pair_index * leaf_node_pair_size;
}

std::uint32_t *LeafNode::get_pair_key(int pair_index) {
	return (std::uint32_t *)this->get_pair(pair_index);
}

void *LeafNode::get_pair_value(int pair_index) {
	return (std::uint8_t *)this->get_pair(pair_index) + LEAF_NODE_KEY_SIZE;
}

void LeafNode::insert_at(int pos, std::uint32_t key, SchemaRow value) {

	std::uint32_t pair_count = *this->get_pair_count();
	if (pos < pair_count) {
		for (int j = pair_count; j > pos; j--) {
			std::memcpy(this->get_pair(j),
			            this->get_pair(j - 1),
			            leaf_node_pair_size);
		}
	}

	*this->get_pair_key(pos) = key;
	value.serialize(this->get_pair_value(pos));

	*this->get_pair_count() += 1;
}

void LeafNode::insert(std::uint32_t key, SchemaRow value) {

	std::uint32_t pair_count = *this->get_pair_count();

	std::uint32_t left_pos = 0;
	std::uint32_t right_pos = *this->get_pair_count();
	std::uint32_t mid_pos = (left_pos + right_pos) / 2;

	while (left_pos != right_pos) {
		if (key < *this->get_pair_key(mid_pos)) {
			right_pos = mid_pos;
		} else if (key > *this->get_pair_key(mid_pos)) {
			left_pos = mid_pos + 1;
		} else {
			throw std::runtime_error("duplicate key");
		}
		mid_pos = (left_pos + right_pos) / 2;
	}

	this->insert_at(mid_pos, key, value);
}

} // namespace fmisql
