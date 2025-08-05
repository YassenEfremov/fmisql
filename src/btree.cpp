#include "btree.hpp"

#include "data_types.hpp"
#include "pager.hpp"
#include "cli/parser.hpp"
#include "schema.hpp"
#include "statement.hpp"

#include <cstdint>
#include <cstring>

#include <iostream>
#include <stdexcept>
#include <string>
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

	if (BplusTrees.find(root_page) == BplusTrees.end()) {
		if (Pager::pages.empty()) {
			if (root_page == 0) {
				create({
					sql_types::Column{"", sql_types::Id::STRING},
					sql_types::Column{"", sql_types::Id::INT},
					sql_types::Column{"", sql_types::Id::STRING},
				});
			} else {
				throw std::runtime_error("three with root page " + std::to_string(root_page) + " doesn't exist");
			}
		} else {
			if (root_page != 0 && root_page >= Pager::pages.size()) {
				throw std::runtime_error("three with root page " + std::to_string(root_page) + " doesn't exist");
			}
			BplusTrees.insert({root_page, new BplusTree(root_page)});
		}
	}

	return *BplusTrees.at(root_page);
}

BplusTree &BplusTree::get_schema() {
	return get_table(0);
}

SchemaRow BplusTree::get_schema_row_by_table_page(std::uint32_t page_number) {
	BplusTree &schema_BplusTree = get_schema();

	// TODO: use some form of a select statement instead of a loop
	SchemaRow row;
	for (void *cell : schema_BplusTree) {
		row.deserialize(((std::uint8_t *)cell) + key_size);

		if (row.root_page == page_number) {
			return row;
		}
	}

	// throw std::runtime_error("no table with page " + std::to_string(page_number));
	throw std::runtime_error("There is no such table!");
}

SchemaRow BplusTree::get_schema_row_by_table_name(std::string_view table_name) {
	BplusTree &schema_BplusTree = get_schema();

	// TODO: use some form of a select statement instead of a loop
	SchemaRow row;
	for (void *cell : schema_BplusTree) {
		row.deserialize(((std::uint8_t *)cell) + key_size);

		if (row.table_name == table_name) {
			return row;
		}
	}

	// throw std::runtime_error("no table with name " + std::string(table_name));
	throw std::runtime_error("There is no such table!");
}

void BplusTree::delete_all_trees() {
	for (auto page_and_tree : BplusTrees) {
		delete page_and_tree.second;
	}
	BplusTrees.clear();
}

std::uint32_t BplusTree::get_root_page() const { return this->root_page; }
std::size_t BplusTree::get_value_size() const { return this->value_size; }
std::uint32_t BplusTree::get_total_cell_count() const { return this->total_cell_count; }

void BplusTree::insert(std::uint32_t key, void *value) {

	// Node node(Pager::get_page(this->root_page));

	// std::uint32_t cell_start = 1 + 4 + 4 + this->cell_count * (key_size + this->value_size);
	// if (cell_start + key_size + this->value_size > page_size) {
	// 	throw std::runtime_error("NEED TO SPLIT");
	// }

	// *(uint32_t *)(((std::uint8_t *)node.data) + cell_start) = key;
	// std::memcpy(
	// 	(((std::uint8_t *)node.data) + cell_start + key_size),
	// 	value,
	// 	this->value_size
	// );

	// this->cell_count++;
	// *(std::uint32_t *)(((std::uint8_t *)node.data) + 1) = this->cell_count;

	rec_insert(this->root_page, key, value);
	this->total_cell_count++;
}

BplusTree::Iterator::Iterator(std::uint32_t page_number,
	                          std::uint32_t cell_number,
							  std::uint32_t value_size)
	: page_number(page_number),
	  cell_number(cell_number),
	  value_size(value_size),
	  value_buffer(nullptr) {}

BplusTree::Iterator &BplusTree::Iterator::operator++() {
	this->cell_number++;
	Node node(this->page_number, this->value_size);

	if (this->cell_number >= node.get_cell_count()) {
		node = Node(node.get_next_leaf(), this->value_size);
		if (node.get_page_number() == 0) {
			// end of B+ Tree
			this->page_number = 0;
			this->cell_number = 7;
		} else {
			this->page_number = node.get_page_number();
			this->cell_number = 0;
		}
	}

	return *this;
}

bool BplusTree::Iterator::operator==(const Iterator &other) const {
	return this->page_number == other.page_number && this->cell_number == other.cell_number;
}

bool BplusTree::Iterator::operator!=(const Iterator &other) const {
	return !(*this == other);
}

void *&BplusTree::Iterator::operator*() {
	Node node(this->page_number, this->value_size);
	this->value_buffer = node.get_cell(this->cell_number);
	return this->value_buffer;
}

// const void *&BplusTree::Iterator::operator*() const {
// 	Node node(this->page_number, this->value_size);
// 	this->current_value = node.get_cell(this->cell_number);
// 	return this->current_value;
// }

BplusTree::Iterator BplusTree::begin() const {
	if (this->total_cell_count == 0) {
		return end();
	}

	Node node(this->root_page, this->value_size);
	std::uint32_t start_page;

	switch (node.get_type()) {
	case NodeType::LEAF:
		start_page = 0;
		break;

	case NodeType::INTERIOR: {
		Node child(*(std::uint32_t *)node.get_cell_value(0), this->value_size);

		// go to the leftmost bottom child
		while (child.get_type() != NodeType::LEAF) {
			child = Node(*(std::uint32_t *)child.get_cell_value(0), this->value_size);
		}
		start_page = child.get_page_number();
	}
		break;
	}

	return Iterator(start_page, 0, this->value_size);
}

BplusTree::Iterator BplusTree::end() const {
	// schema page 0 can fit only 7 cells, so this is just past the end
	return Iterator(0, 7, this->value_size);
}


// private stuff

std::unordered_map<std::uint32_t, BplusTree *> BplusTree::BplusTrees;

BplusTree::BplusTree(const std::vector<sql_types::Column> &columns)
	: total_cell_count(0),
	  columns(columns),
	  value_size(0) {

	Node node(NodeType::LEAF, this->value_size);
	this->root_page = node.get_page_number();

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
	if (key_size + value_size > page_size - type_size - cell_count_size - next_leaf_size) {
		throw std::runtime_error("TODO: overflow pages");
	}
	this->value_size = value_size;
}

BplusTree::BplusTree(std::uint32_t root_page)
	: root_page(root_page),
	  total_cell_count(0),
	  columns({}),
	  value_size(0) {
	
	Node node(this->root_page, this->value_size);

	switch (node.get_type()) {
	case NodeType::LEAF:
		this->total_cell_count = node.get_cell_count();
		break;

	case NodeType::INTERIOR: {
		Node child(*(std::uint32_t *)node.get_cell_value(0), this->value_size);

		// go to the leftmost bottom child
		while (child.get_type() != NodeType::LEAF) {
			child = Node(*(std::uint32_t *)child.get_cell_value(0), this->value_size);
		}

		// traverse all of the leaf pages
		while (child.get_page_number() != 0) {
			this->total_cell_count += child.get_cell_count();
			child = Node(child.get_next_leaf(), this->value_size);
		}
	}
		break;
	}

	if (root_page == 0) {
		// special case - load schema B+ Tree
		this->columns = {
			sql_types::Column{"", sql_types::Id::STRING},
			sql_types::Column{"", sql_types::Id::INT},
			sql_types::Column{"", sql_types::Id::STRING},
		};
		this->value_size = schema_row_size;

	} else {
		SchemaRow schema_row = get_schema_row_by_table_page(this->root_page);
		Statement statement = parse_line(schema_row.original_sql);
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
	}
}

std::uint32_t BplusTree::leaf_insert(std::uint32_t leaf_page_number, std::size_t pos, std::uint32_t key, void *value) {

	Node node(leaf_page_number, this->value_size);

	std::uint32_t cell_start = type_size
	                         + cell_count_size
							 + next_leaf_size
							 + node.get_cell_count() * (key_size + this->value_size);
	if (cell_start + key_size + this->value_size > page_size) {
		std::cout << "splitting LEAF...\n";
		
		// left node gets one more cell - could be the other way around, doesn't matter
		int right_split_count = (node.get_cell_count() + 1) / 2;
		int left_split_count = node.get_cell_count() + 1 - right_split_count;

		Node right_node(NodeType::LEAF, this->value_size);
		right_node.set_cell_count(right_split_count);
		right_node.set_next_leaf(0);
		for (int i = 0; i <= node.get_cell_count(); i++) {
			Node *dst_node = i < right_split_count ? &node : &right_node;
			std::size_t dst_i = i % left_split_count;

			if (i == pos) {
				dst_node->set_cell(dst_i, key, value);
			} else if (i > pos) {
				dst_node->set_cell(dst_i - 1, node.get_cell_key(i - 1), node.get_cell_value(i - 1));
			} else {
				dst_node->set_cell(dst_i, node.get_cell_key(i), node.get_cell_value(i));
			}
		}

		if (node.get_page_number() == this->root_page) {
			// create new left node and convert current node to interior node

			Node left_node(NodeType::LEAF, this->value_size);
			left_node.set_cell_count(left_split_count);
			left_node.set_next_leaf(right_node.get_page_number());
			for (int i = 0; i < left_split_count; i++) {
				left_node.set_cell(i, node.get_cell_key(i), node.get_cell_value(i));
			}

			node.set_type(NodeType::INTERIOR);
			node.set_cell_count(1);
			node.set_right_child(right_node.get_page_number());
			node.set_value_size(interior_value_size);
			std::uint32_t temp_value = left_node.get_page_number();
			node.set_cell(0, left_node.get_cell_key(left_split_count - 1), &temp_value);

		} else {
			// insert into parent interior node
			// throw std::runtime_error("TODO: insert into parent");
			node.set_cell_count(left_split_count);
			node.set_next_leaf(right_node.get_page_number());
			return right_node.get_page_number();
		}

	} else {
		if (pos < node.get_cell_count()) {
			for (int j = node.get_cell_count(); j > pos; j--) {
				node.set_cell(j, node.get_cell_key(j - 1), node.get_cell_value(j - 1));
			}
		}

		node.set_cell(pos, key, value);
		node.set_cell_count(node.get_cell_count() + 1);
	}

	return 0;
}

std::uint32_t BplusTree::rec_insert(std::uint32_t page_number, std::uint32_t key, void *value) {

	Node node(page_number, this->value_size);

	switch (node.get_type()) {
	case NodeType::LEAF: {
		std::uint32_t left_pos = 0;
		std::uint32_t right_pos = node.get_cell_count();
		std::uint32_t mid_pos = (left_pos + right_pos) / 2;

		while (left_pos != right_pos) {
			if (key <= node.get_cell_key(mid_pos)) {
				right_pos = mid_pos;
			} else {
				left_pos = mid_pos + 1;
			}
			mid_pos = (left_pos + right_pos) / 2;
		}

		return leaf_insert(page_number, mid_pos, key, value);
	}
		break;

	case NodeType::INTERIOR: {
		std::uint32_t left_pos = 0;
		std::uint32_t right_pos = node.get_cell_count();
		std::uint32_t mid_pos = (left_pos + right_pos) / 2;

		while (left_pos != right_pos) {
			if (key <= node.get_cell_key(mid_pos)) {
				right_pos = mid_pos;
			} else {
				left_pos = mid_pos + 1;
			}
			mid_pos = (left_pos + right_pos) / 2;
		}

		std::uint32_t child_page_number = mid_pos >= node.get_cell_count() ?
			node.get_right_child()
			:
			*(std::uint32_t *)node.get_cell_value(mid_pos);

		std::uint32_t new_leaf_node_page_number = rec_insert(child_page_number, key, value);

		if (new_leaf_node_page_number != 0) {
			std::uint32_t cell_start = type_size
			                         + cell_count_size
									 + right_child_size
									 + node.get_cell_count() * (key_size + interior_value_size);
			if (cell_start + key_size + interior_value_size > page_size) {
				throw std::runtime_error("TODO: split interior node");
				// std::cout << "splitting INTERIOR...\n";
				// return new_interior_node_number;

			} else {
				Node child_node(child_page_number, this->value_size);
				std::uint32_t child_node_max_key = child_node.get_cell_key(child_node.get_cell_count() - 1);
				
				std::uint32_t left_pos = 0;
				std::uint32_t right_pos = node.get_cell_count();
				std::uint32_t mid_pos = (left_pos + right_pos) / 2;

				while (left_pos != right_pos) {
					if (child_node_max_key <= node.get_cell_key(mid_pos)) {
						right_pos = mid_pos;
					} else {
						left_pos = mid_pos + 1;
					}
					mid_pos = (left_pos + right_pos) / 2;
				}

				if (mid_pos < node.get_cell_count()) {
					for (int j = node.get_cell_count(); j > mid_pos; j--) {
						node.set_cell(j, node.get_cell_key(j - 1), node.get_cell_value(j - 1));
					}
				} else {
					// right child handling
					node.set_right_child(new_leaf_node_page_number);
				}

				node.set_cell(mid_pos, child_node_max_key, &child_page_number);
				node.set_cell_count(node.get_cell_count() + 1);
			}
		}

		return 0;
	}
		break;

	default:
		throw std::runtime_error("invalid node type (how?)");
	}
}

BplusTree::Node::Node(int page_number, std::uint32_t value_size)
	: page_number(page_number), data(Pager::get_page(page_number)) {

	std::uint8_t type_value = *(std::uint8_t *)this->data;
	switch (type_value) {
	case 0:
		this->type = NodeType::LEAF;
		this->value_size = value_size;
		break;
	case 1:
		this->type = NodeType::INTERIOR;
		this->value_size = interior_value_size;
		break;
	}
	this->cell_count = *(std::uint32_t *)(((std::uint8_t *)this->data) + type_size);
}

BplusTree::Node::Node(NodeType type, std::uint32_t value_size)
	: page_number(Pager::create_page()) {

	this->data = Pager::get_page(this->page_number);
	set_type(type);
	set_cell_count(0);

	switch (this->type) {
	case NodeType::LEAF:
		set_next_leaf(0);
		this->value_size = value_size;
		break;
	case NodeType::INTERIOR:
		set_right_child(0);
		this->value_size = interior_value_size;
		break;
	}
}

BplusTree::NodeType BplusTree::Node::get_type() const { return this->type; }
std::uint32_t BplusTree::Node::get_cell_count() const { return this->cell_count; }
std::uint32_t BplusTree::Node::get_page_number() const { return this->page_number; }

void *BplusTree::Node::get_cell(std::size_t i) {
	switch (this->type) {
	case NodeType::LEAF:
		return ((std::uint8_t *)this->data)
			+ type_size
			+ cell_count_size
			+ next_leaf_size
			+ i * (key_size + this->value_size);
		break;
	case NodeType::INTERIOR:
		return ((std::uint8_t *)this->data)
			+ type_size
			+ cell_count_size
			+ right_child_size
			+ i * (this->value_size + key_size);
		break;
	default:
		throw std::runtime_error("invalid node type (how?)");
	}
}

std::uint32_t BplusTree::Node::get_cell_key(std::size_t i) {
	switch (this->type) {
	case NodeType::LEAF:
		return *(std::uint32_t *)(((std::uint8_t *)this->data)
			+ type_size
			+ cell_count_size
			+ next_leaf_size
			+ i * (key_size + this->value_size));
		break;
	case NodeType::INTERIOR:
		return *(std::uint32_t *)(((std::uint8_t *)this->data)
			+ type_size
			+ cell_count_size
			+ right_child_size
			+ i * (this->value_size + key_size) + this->value_size);
		break;
	default:
		throw std::runtime_error("invalid node type (how?)");
	}
}

void *BplusTree::Node::get_cell_value(std::size_t i) {
	switch (this->type) {
	case NodeType::LEAF:
		return ((std::uint8_t *)this->data)
			+ type_size
			+ cell_count_size
			+ next_leaf_size
			+ i * (key_size + this->value_size) + key_size;
		break;
	case NodeType::INTERIOR:
		return ((std::uint8_t *)this->data)
			+ type_size
			+ cell_count_size
			+ right_child_size
			+ i * (this->value_size + key_size);
		break;
	default:
		throw std::runtime_error("invalid node type (how?)");
	}
}

void BplusTree::Node::set_type(NodeType type) {
	this->type = type;
	switch (this->type) {
	case NodeType::LEAF:
		*(std::uint8_t *)this->data = 0x00;
		break;
	case NodeType::INTERIOR:
		*(std::uint8_t *)this->data = 0x01;
		break;
	}
}

void BplusTree::Node::set_cell_count(std::uint32_t cell_count) {
	this->cell_count = cell_count;
	*(uint32_t *)(((std::uint8_t *)this->data) + type_size) = cell_count;
}

void BplusTree::Node::set_value_size(std::uint32_t value_size) {
	this->value_size = value_size;
}

void BplusTree::Node::set_cell(std::size_t i, std::uint32_t key, void *value) {
	switch (this->type) {
	case NodeType::LEAF:
		*(uint32_t *)(((std::uint8_t *)this->data)
			+ type_size
			+ cell_count_size
			+ next_leaf_size
			+ i * (key_size + this->value_size)) = key;
		std::memcpy(
			((std::uint8_t *)this->data)
				+ type_size
				+ cell_count_size
				+ next_leaf_size
				+ i * (key_size + this->value_size) + key_size,
			value,
			this->value_size
		);
		break;
	case NodeType::INTERIOR:
		*(uint32_t *)(((std::uint8_t *)this->data)
			+ type_size
			+ cell_count_size
			+ right_child_size
			+ i * (this->value_size + key_size)) = *(std::uint32_t *)value;
		*(uint32_t *)(((std::uint8_t *)this->data)
			+ type_size
			+ cell_count_size
			+ right_child_size
			+ i * (this->value_size + key_size) + this->value_size) = key;
		break;
	}
}

std::uint32_t BplusTree::Node::get_next_leaf() const {
	return *(uint32_t *)(((std::uint8_t *)this->data) + type_size + cell_count_size);
}

void BplusTree::Node::set_next_leaf(std::uint32_t next_leaf) {
	*(uint32_t *)(((std::uint8_t *)this->data) + type_size + cell_count_size) = next_leaf;
}

std::uint32_t BplusTree::Node::get_right_child() const {
	return *(uint32_t *)(((std::uint8_t *)this->data) + type_size + cell_count_size);
}

void BplusTree::Node::set_right_child(std::uint32_t right_child) {
	*(uint32_t *)(((std::uint8_t *)this->data) + type_size + cell_count_size) = right_child;
}

} // namespace fmisql
