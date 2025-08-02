#include "btree.hpp"

#include "data_types.hpp"
#include "pager.hpp"
#include "cli/parser.hpp"
#include "schema.hpp"
#include "statement.hpp"

#include <cstdint>
#include <cstring>

#include <stdexcept>
#include <iostream>
#include <vector>


namespace fmisql {

// Node::Node(sql_types::Int number, std::uint32_t value_size) : page(number) {
// 	this->data = Pager::get_page(number);
// 	if (value_size != 0) *this->value_size() = value_size;

// 	this->pair_size = leaf_node_key_size + *this->value_size();
// 	this->max_pairs = leaf_node_space_for_cells / this->pair_size;

// 	this->leaf_node_right_split_count = (this->max_pairs + 1) / 2;
// 	this->leaf_node_left_split_count = this->max_pairs + 1 - this->leaf_node_right_split_count;
// }

// sql_types::Int Node::number_of_table(std::string_view name) {

// 	Node node(0, schema_row_size);

// 	SchemaRow row("", 0, "");
// 	for (int i = 0; i < *node.pair_count(); i++) {
// 		row.deserialize(node.pair_value(i));
// 		if (row.table_name == name) {
// 			return row.root_page;
// 		}
// 	}

// 	throw std::runtime_error("There is no such table!");
// }

// std::uint8_t *Node::is_root() {
// 	return (std::uint8_t *)this->data + is_root_offset;
// }

// std::uint32_t *Node::pair_count() {
// 	return (std::uint32_t *)((std::uint8_t *)this->data + leaf_node_num_cells_offset);
// }

// Node::Type Node::get_type() {
// 	return (Node::Type)*((std::uint8_t *)this->data + node_type_offset);
// }

// void Node::set_type(Type type) {
// 	*((std::uint8_t *)this->data + node_type_offset) = (std::uint8_t)type;
// }

// void *Node::leaf_pair(int pair_index) {
// 	return (std::uint8_t *)this->data + leaf_node_header_size + pair_index * this->pair_size;
// }

// std::uint32_t *Node::pair_key(int pair_index) {
// 	return (std::uint32_t *)this->leaf_pair(pair_index);
// }

// void *Node::pair_value(int pair_index) {
// 	return (std::uint8_t *)this->leaf_pair(pair_index) + leaf_node_key_size;
// }

// std::uint32_t *Node::value_size() {
// 	return (std::uint32_t *)((std::uint8_t *)this->data + leaf_node_value_size_offset);
// }

// std::uint32_t *Node::next_leaf() {
// 	return (std::uint32_t *)((std::uint8_t *)this->data + leaf_node_next_leaf_offset);
// }

// std::uint32_t *Node::key_count() {
// 	return (std::uint32_t *)((std::uint8_t *)this->data + internal_node_num_keys_offset);
// }

// std::uint32_t *Node::right_child() {
// 	return (std::uint32_t *)((std::uint8_t *)this->data + internal_node_right_child_offset);
// }

// std::uint32_t *Node::internal_pair(int pos) {
// 	return (std::uint32_t *)((std::uint8_t *)this->data + internal_node_header_size + pos * internal_node_cell_size);
// }

// std::uint32_t *Node::child(int pos) {
// 	std::uint32_t key_count = *this->key_count();
// 	if (pos > key_count) {
// 		throw std::runtime_error("child position out of bounds");
// 	} else if (pos == key_count) {
// 		return this->right_child();
// 	} else {
// 		return this->internal_pair(pos);
// 	}
// }

// std::uint32_t *Node::key(int pos) {
// 	return (std::uint32_t *)((std::uint8_t *)this->internal_pair(pos) + internal_node_child_size);
// }

// std::uint32_t Node::max_key() {
// 	switch (this->get_type()) {
// 	case Type::INTERNAL:
// 		return *this->key(*this->key_count() - 1);
// 		break;
// 	case Type::LEAF:
// 		return *this->pair_count() - 1;
// 		break;
// 	default:
// 		throw std::logic_error("invalid node type");
// 	}
// }

// void Node::leaf_insert(int pos, std::uint32_t key, void *value) {

// 	std::uint32_t pair_count = *this->pair_count();
// 	if (pos < pair_count) {
// 		for (int j = pair_count; j > pos; j--) {
// 			std::memcpy(this->leaf_pair(j),
// 			            this->leaf_pair(j - 1),
// 			            this->pair_size);
// 		}
// 	}

// 	*this->pair_key(pos) = key;
// 	// value.serialize(this->pair_value(pos));
// 	std::memcpy(this->pair_value(pos), value, *this->value_size());

// 	*this->pair_count() += 1;
// }

// void Node::internal_insert(std::uint32_t key, void *value) {
// 	std::uint32_t left_pos = 0;
// 	std::uint32_t right_pos = *this->key_count();
// 	std::uint32_t mid_pos = (left_pos + right_pos) / 2;

// 	while (left_pos != right_pos) {
// 		if (key <= *this->key(mid_pos)) {
// 			right_pos = mid_pos;
// 		} else {
// 			left_pos = mid_pos + 1;
// 		}
// 		mid_pos = (left_pos + right_pos) / 2;
// 	}

// 	Node child(mid_pos >= *this->key_count() ? *this->right_child() : *this->child(mid_pos));	
// 	switch (child.get_type()) {
// 	case Type::INTERNAL:
// 		return child.internal_insert(key, value);
// 	case Type::LEAF:
// 		return child.insert(key, value);
// 	default:
// 		throw std::logic_error("invalid node type");
// 	}
// }

// void Node::insert(std::uint32_t key, void *value) {
// 	switch (this->get_type()) {
// 	case Type::INTERNAL:
// 	{
// 		this->internal_insert(key, value);
// 		break;
// 	}
// 	case Type::LEAF:
// 	{
// 		std::uint32_t left_pos = 0;
// 		std::uint32_t right_pos = *this->pair_count();
// 		std::uint32_t mid_pos = (left_pos + right_pos) / 2;

// 		while (left_pos != right_pos) {
// 			if (key < *this->pair_key(mid_pos)) {
// 				right_pos = mid_pos;
// 			} else if (key > *this->pair_key(mid_pos)) {
// 				left_pos = mid_pos + 1;
// 			} else {
// 				throw std::runtime_error("duplicate key");
// 			}
// 			mid_pos = (left_pos + right_pos) / 2;
// 		}

// 		if (*this->pair_count() >= this->max_pairs) {
// 			// throw std::logic_error("splitting not implemented yet");
// 			this->split_insert(mid_pos, key, value);
// 		} else {
// 			this->leaf_insert(mid_pos, key, value);
// 		}
// 		break;
// 	}
// 	default:
// 		throw std::logic_error("invalid node type");
// 	}
// }

// sql_types::Int Node::start_leaf() {
// 	switch (this->get_type()) {
// 	case Type::INTERNAL:
// 	{
// 		Node leftmost(*this->child(0));
// 		return leftmost.start_leaf();
// 	}
// 	case Type::LEAF:
// 		return this->page;
// 	default:
// 		throw std::logic_error("invalid node type");
// 	}
// }

// void Node::split_insert(int pos, std::uint32_t key, void *value) {

// 	int new_right_page = Pager::get_unused_page_number();
// 	Node new_right(new_right_page, *this->value_size());
// 	*new_right.is_root() = 0;
// 	new_right.set_type(Type::LEAF);
// 	*new_right.next_leaf() = 0;
// 	for (int i = this->max_pairs; i >= 0; i--) {
// 		Node *dst_node = nullptr;
// 		if (i >= this->leaf_node_left_split_count) {
// 			dst_node = &new_right;
// 		} else {
// 			dst_node = this;
// 		}

// 		void *dst_pair = dst_node->leaf_pair(i % this->leaf_node_left_split_count);

// 		if (i == pos) {
// 			std::memcpy(dst_pair, &key, leaf_node_key_size);
// 			std::memcpy((std::uint8_t *)dst_pair + leaf_node_value_offset, value, *this->value_size());
// 		} else if (i > pos) {
// 			std::memcpy(dst_pair, this->leaf_pair(i - 1), leaf_node_key_size + *this->value_size());
// 		} else {
// 			std::memcpy(dst_pair, this->leaf_pair(i), leaf_node_key_size + *this->value_size());
// 		}
// 	}

// 	*this->pair_count() = this->leaf_node_left_split_count;
// 	*new_right.pair_count() = this->leaf_node_right_split_count;

// 	if (*this->is_root()) {
// 		int new_left_page = Pager::get_unused_page_number();
// 		std::memcpy(Pager::get_page(new_left_page), this->data, page_size);
// 		Node new_left(new_left_page, *this->value_size());
// 		*new_left.is_root() = 0;
// 		new_left.set_type(Type::LEAF);
// 		*new_left.next_leaf() = new_right_page;

// 		this->set_type(Type::INTERNAL);
// 		*this->is_root() = true;
// 		*this->key_count() = 1;
// 		*this->child(0) = new_left_page;
// 		*this->key(0) = new_left.max_key();
// 		*this->right_child() = new_right_page;
// 	} else {
// 		throw std::logic_error("not implemented yet");
// 	}
// }

// void write_at_byte(void *dst, std::size_t byte, void *data, std::size_t size) {

// }

BplusTree &BplusTree::create(const std::vector<sql_types::Column> &columns) {
	BplusTree *table_BplusTree = new BplusTree(columns);
	BplusTrees.insert({table_BplusTree->get_root_page(), table_BplusTree});
	return *table_BplusTree;
}

BplusTree &BplusTree::get_table(std::uint32_t root_page) {
	if (root_page == 0) {
		if (Pager::file_page_count == 0) {
			// it follows that BplusTrees is empty
			create({
				sql_types::Column{"", sql_types::Id::STRING},
				sql_types::Column{"", sql_types::Id::INT},
				sql_types::Column{"", sql_types::Id::STRING},
			});
		} else {
			BplusTrees.insert({0, new BplusTree(0)});
		}

	} else {
		if (BplusTrees.find(root_page) == BplusTrees.end()) {
			BplusTrees.insert({root_page, new BplusTree(root_page)});
		}
	}

	return *BplusTrees.at(root_page);
}

BplusTree &BplusTree::get_schema() {
	return get_table(0);
}

void BplusTree::delete_all_trees() {
	for (auto page_and_tree : BplusTrees) {
		delete page_and_tree.second;
	}
	BplusTrees.clear();
}

std::uint32_t BplusTree::get_root_page() const { return this->root_page; }
std::size_t BplusTree::get_value_size() const { return this->value_size; }
std::uint32_t BplusTree::get_cell_count() const { return this->cell_count; }

void BplusTree::insert(std::uint32_t key, void *value) {

	Node node{ Pager::get_page(this->root_page) };

	std::uint32_t cell_start = + 1 + 4 + 4 + this->cell_count * this->value_size;
	*(uint32_t *)(((std::uint8_t *)node.data) + cell_start) = key;
	std::memcpy(
		(((std::uint8_t *)node.data) + cell_start + sizeof std::uint32_t),
		value,
		this->value_size
	);

	this->cell_count++;
	*(std::uint32_t *)(((std::uint8_t *)node.data) + 1) = this->cell_count;
}

void *BplusTree::get_cell_value(std::size_t i) {
	Node node{ Pager::get_page(this->root_page) };
	return (((std::uint8_t *)node.data) + 1 + 4 + 4 + i * this->value_size + sizeof std::uint32_t);
}

std::unordered_map<std::uint32_t, BplusTree *> BplusTree::BplusTrees;

// private constructors
BplusTree::BplusTree(const std::vector<sql_types::Column> &columns)
	: root_page(Pager::create_page()),
	  cell_count(0),
	  columns(columns),
	  value_size(0) {

	Node node{ Pager::get_page(this->root_page) };

	*(std::uint8_t *)node.data = 0x00;	// type
	*(uint32_t *)(((std::uint8_t *)node.data) + 1) = 0;	// cell count
	*(uint32_t *)(((std::uint8_t *)node.data) + 1 + 4) = 0;	// next leaf
	for (sql_types::Column column : this->columns) {
		switch (column.type_id) {
		case sql_types::Id::INT:
			this->value_size += sql_types::max_int_size;
			break;
		case sql_types::Id::STRING:
			this->value_size += sql_types::max_string_size;
			break;
		case sql_types::Id::DATE:
			this->value_size += sql_types::max_date_size;
			break;
		}	
	}
	if (value_size > page_size - 1 - 4 - 4 - 4) {
		throw std::runtime_error("TODO: overflow pages");
	}
	this->value_size = value_size;
}

BplusTree::BplusTree(std::uint32_t root_page)
	: root_page(root_page),
	  cell_count(0),
	  columns({}),
	  value_size(0) {
	
	Node node{ Pager::get_page(this->root_page) };

	this->cell_count = *(std::uint32_t *)(((std::uint8_t *)node.data) + 1);

	if (root_page == 0) {
		// special case - load schema B+ Tree
		this->columns = {
			sql_types::Column{"", sql_types::Id::STRING},
			sql_types::Column{"", sql_types::Id::INT},
			sql_types::Column{"", sql_types::Id::STRING},
		};
		this->value_size = schema_row_size;

	} else {
		// normal case - load any B+ Tree
		// it is guaranteed that the schema B+ Tree exists at this point

		BplusTree &schema_BplusTree = get_schema();

		// TODO: use some form of a select statement instead of a loop
		SchemaRow row("", 0, "");
		for (int i = 0; i < schema_BplusTree.get_cell_count(); i++) {
			row.deserialize(schema_BplusTree.get_cell_value(i));

			if (row.root_page == root_page) {
				Statement statement = parse_line(row.original_sql);

				for (sql_types::Column column : statement.create_columns) {
					switch (column.type_id) {
					case sql_types::Id::INT:
						this->columns.push_back(sql_types::Column{
							"",
							sql_types::Id::INT
						});
						this->value_size += sql_types::max_int_size;
						break;
					case sql_types::Id::STRING:
						this->columns.push_back(sql_types::Column{
							"",
							sql_types::Id::STRING
						});
						this->value_size += sql_types::max_string_size;
						break;
					case sql_types::Id::DATE:
						this->columns.push_back(sql_types::Column{
							"",
							sql_types::Id::DATE
						});
						this->value_size += sql_types::max_date_size;
						break;
					}
				}

				break;
			}
		}
	}
}

} // namespace fmisql
