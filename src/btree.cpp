#include "btree.hpp"

#include "data_types.hpp"
#include "pager.hpp"
#include "schema.hpp"

#include <cstdint>
#include <cstring>

#include <stdexcept>
#include <iostream>


namespace fmisql {

Node::Node(sql_types::Int number, std::uint32_t value_size) : page(number) {
	this->data = Pager::get_page(number);
	if (value_size != 0) *this->value_size() = value_size;

	this->pair_size = leaf_node_key_size + *this->value_size();
	this->max_pairs = leaf_node_space_for_cells / this->pair_size;

	this->leaf_node_right_split_count = (this->max_pairs + 1) / 2;
	this->leaf_node_left_split_count = this->max_pairs + 1 - this->leaf_node_right_split_count;
}

sql_types::Int Node::number_of_table(std::string_view name) {

	Node node(0, schema_row_size);

	SchemaRow row("", 0, "");
	for (int i = 0; i < *node.pair_count(); i++) {
		row.deserialize(node.pair_value(i));
		if (row.table_name == name) {
			return row.page;
		}
	}

	throw std::runtime_error("There is no such table!");
}

std::uint8_t *Node::is_root() {
	return (std::uint8_t *)this->data + is_root_offset;
}

std::uint32_t *Node::pair_count() {
	return (std::uint32_t *)((std::uint8_t *)this->data + leaf_node_num_cells_offset);
}

Node::Type Node::get_type() {
	return (Node::Type)*((std::uint8_t *)this->data + node_type_offset);
}

void Node::set_type(Type type) {
	*((std::uint8_t *)this->data + node_type_offset) = (std::uint8_t)type;
}

void *Node::leaf_pair(int pair_index) {
	return (std::uint8_t *)this->data + leaf_node_header_size + pair_index * this->pair_size;
}

std::uint32_t *Node::pair_key(int pair_index) {
	return (std::uint32_t *)this->leaf_pair(pair_index);
}

void *Node::pair_value(int pair_index) {
	return (std::uint8_t *)this->leaf_pair(pair_index) + leaf_node_key_size;
}

std::uint32_t *Node::value_size() {
	return (std::uint32_t *)((std::uint8_t *)this->data + leaf_node_value_size_offset);
}

std::uint32_t *Node::next_leaf() {
	return (std::uint32_t *)((std::uint8_t *)this->data + leaf_node_next_leaf_offset);
}

std::uint32_t *Node::key_count() {
	return (std::uint32_t *)((std::uint8_t *)this->data + internal_node_num_keys_offset);
}

std::uint32_t *Node::right_child() {
	return (std::uint32_t *)((std::uint8_t *)this->data + internal_node_right_child_offset);
}

std::uint32_t *Node::internal_pair(int pos) {
	return (std::uint32_t *)((std::uint8_t *)this->data + internal_node_header_size + pos * internal_node_cell_size);
}

std::uint32_t *Node::child(int pos) {
	std::uint32_t key_count = *this->key_count();
	if (pos > key_count) {
		throw std::runtime_error("child position out of bounds");
	} else if (pos == key_count) {
		return this->right_child();
	} else {
		return this->internal_pair(pos);
	}
}

std::uint32_t *Node::key(int pos) {
	return (std::uint32_t *)((std::uint8_t *)this->internal_pair(pos) + internal_node_child_size);
}

std::uint32_t Node::max_key() {
	switch (this->get_type()) {
	case Type::INTERNAL:
		return *this->key(*this->key_count() - 1);
		break;
	case Type::LEAF:
		return *this->pair_count() - 1;
		break;
	default:
		throw std::logic_error("invalid node type");
	}
}

void Node::leaf_insert(int pos, std::uint32_t key, void *value) {

	std::uint32_t pair_count = *this->pair_count();
	if (pos < pair_count) {
		for (int j = pair_count; j > pos; j--) {
			std::memcpy(this->leaf_pair(j),
			            this->leaf_pair(j - 1),
			            this->pair_size);
		}
	}

	*this->pair_key(pos) = key;
	// value.serialize(this->pair_value(pos));
	std::memcpy(this->pair_value(pos), value, *this->value_size());

	*this->pair_count() += 1;
}

void Node::internal_insert(std::uint32_t key, void *value) {
	std::uint32_t left_pos = 0;
	std::uint32_t right_pos = *this->key_count();
	std::uint32_t mid_pos = (left_pos + right_pos) / 2;

	while (left_pos != right_pos) {
		if (key <= *this->key(mid_pos)) {
			right_pos = mid_pos;
		} else {
			left_pos = mid_pos + 1;
		}
		mid_pos = (left_pos + right_pos) / 2;
	}

	Node child(mid_pos >= *this->key_count() ? *this->right_child() : *this->child(mid_pos));	
	switch (child.get_type()) {
	case Type::INTERNAL:
		return child.internal_insert(key, value);
	case Type::LEAF:
		return child.insert(key, value);
	default:
		throw std::logic_error("invalid node type");
	}
}

void Node::insert(std::uint32_t key, void *value) {
	switch (this->get_type()) {
	case Type::INTERNAL:
	{
		this->internal_insert(key, value);
		break;
	}
	case Type::LEAF:
	{
		std::uint32_t left_pos = 0;
		std::uint32_t right_pos = *this->pair_count();
		std::uint32_t mid_pos = (left_pos + right_pos) / 2;

		while (left_pos != right_pos) {
			if (key < *this->pair_key(mid_pos)) {
				right_pos = mid_pos;
			} else if (key > *this->pair_key(mid_pos)) {
				left_pos = mid_pos + 1;
			} else {
				throw std::runtime_error("duplicate key");
			}
			mid_pos = (left_pos + right_pos) / 2;
		}

		if (*this->pair_count() >= this->max_pairs) {
			// throw std::logic_error("splitting not implemented yet");
			this->split_insert(mid_pos, key, value);
		} else {
			this->leaf_insert(mid_pos, key, value);
		}
		break;
	}
	default:
		throw std::logic_error("invalid node type");
	}
}

sql_types::Int Node::start_leaf() {
	switch (this->get_type()) {
	case Type::INTERNAL:
	{
		Node leftmost(*this->child(0));
		return leftmost.start_leaf();
	}
	case Type::LEAF:
		return this->page;
	default:
		throw std::logic_error("invalid node type");
	}
}

void Node::split_insert(int pos, std::uint32_t key, void *value) {

	int new_right_page = Pager::get_unused_page_number();
	Node new_right(new_right_page, *this->value_size());
	*new_right.is_root() = 0;
	new_right.set_type(Type::LEAF);
	*new_right.next_leaf() = 0;
	for (int i = this->max_pairs; i >= 0; i--) {
		Node *dst_node = nullptr;
		if (i >= this->leaf_node_left_split_count) {
			dst_node = &new_right;
		} else {
			dst_node = this;
		}

		void *dst_pair = dst_node->leaf_pair(i % this->leaf_node_left_split_count);

		if (i == pos) {
			std::memcpy(dst_pair, &key, leaf_node_key_size);
			std::memcpy((std::uint8_t *)dst_pair + leaf_node_value_offset, value, *this->value_size());
		} else if (i > pos) {
			std::memcpy(dst_pair, this->leaf_pair(i - 1), leaf_node_key_size + *this->value_size());
		} else {
			std::memcpy(dst_pair, this->leaf_pair(i), leaf_node_key_size + *this->value_size());
		}
	}

	*this->pair_count() = this->leaf_node_left_split_count;
	*new_right.pair_count() = this->leaf_node_right_split_count;

	if (*this->is_root()) {
		int new_left_page = Pager::get_unused_page_number();
		std::memcpy(Pager::get_page(new_left_page), this->data, page_size);
		Node new_left(new_left_page, *this->value_size());
		*new_left.is_root() = 0;
		new_left.set_type(Type::LEAF);
		*new_left.next_leaf() = new_right_page;

		this->set_type(Type::INTERNAL);
		*this->is_root() = true;
		*this->key_count() = 1;
		*this->child(0) = new_left_page;
		*this->key(0) = new_left.max_key();
		*this->right_child() = new_right_page;
	} else {
		throw std::logic_error("not implemented yet");
	}
}

} // namespace fmisql
