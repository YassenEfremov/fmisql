#include "btree.hpp"

#include "data_types.hpp"
#include "pager.hpp"
#include "schema.hpp"

#include <cstdint>
#include <cstring>

#include <stdexcept>
#include <iostream>


namespace fmisql {

LeafNode::LeafNode(sql_types::Int number, std::uint32_t value_size) {
	this->data = Pager::get_page(number);
	if (value_size != 0) *this->get_value_size() = value_size;
	// std::cout << "debug: using value size: " << *this->get_value_size() << '\n';
}

sql_types::Int LeafNode::number_of_table(std::string_view name) {

	LeafNode node(0, schema_row_size);

	SchemaRow row("", 0, "");
	for (int i = 0; i < *node.get_pair_count(); i++) {
		row.deserialize(node.get_pair_value(i));
		if (row.table_name == name) {
			return row.page;
		}
	}

	throw std::runtime_error("There is no such table!");
}

std::uint32_t *LeafNode::get_pair_count() {
	return (std::uint32_t *)((std::uint8_t *)this->data + LEAF_NODE_NUM_CELLS_OFFSET);
}

LeafNode::Type LeafNode::get_type() {
	return (LeafNode::Type)*((std::uint8_t *)this->data + NODE_TYPE_OFFSET);
}

void LeafNode::set_type(Type type) {
	*((std::uint8_t *)this->data + NODE_TYPE_OFFSET) = (std::uint8_t)type;
}

void *LeafNode::get_pair(int pair_index) {
	return (std::uint8_t *)this->data + LEAF_NODE_HEADER_SIZE + pair_index * (LEAF_NODE_KEY_SIZE + *this->get_value_size());
}

std::uint32_t *LeafNode::get_pair_key(int pair_index) {
	return (std::uint32_t *)this->get_pair(pair_index);
}

void *LeafNode::get_pair_value(int pair_index) {
	return (std::uint8_t *)this->get_pair(pair_index) + LEAF_NODE_KEY_SIZE;
}

std::uint32_t *LeafNode::get_value_size() {
	return (std::uint32_t *)((std::uint8_t *)this->data + leaf_node_value_size_offset);
}

void LeafNode::insert_at(int pos, std::uint32_t key, void *value) {

	std::uint32_t pair_count = *this->get_pair_count();
	if (pos < pair_count) {
		for (int j = pair_count; j > pos; j--) {
			std::memcpy(this->get_pair(j),
			            this->get_pair(j - 1),
			            LEAF_NODE_KEY_SIZE + *this->get_value_size());
		}
	}

	*this->get_pair_key(pos) = key;
	// value.serialize(this->get_pair_value(pos));
	std::memcpy(this->get_pair_value(pos), value, *this->get_value_size());

	*this->get_pair_count() += 1;
}

void LeafNode::insert(std::uint32_t key, void *value) {

	std::uint32_t pair_count = *this->get_pair_count();

	if (pair_count >= (LEAF_NODE_SPACE_FOR_CELLS / (LEAF_NODE_KEY_SIZE + *this->get_value_size()))) {
		this->split_insert(key, value);
	}

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

void LeafNode::split_insert(std::uint32_t key, void *value) {

	// TODO
	throw std::logic_error("splitting not implemented yet");
}

} // namespace fmisql
