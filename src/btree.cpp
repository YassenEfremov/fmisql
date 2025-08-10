#include "btree.hpp"

#include "data_types.hpp"
#include "pager.hpp"
#include "cli/parser.hpp"
#include "schema.hpp"
#include "statement.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <cstdint>
#include <cstring>


namespace fmisql {

// void write_at_byte(void *dst, std::size_t byte, void *data, std::size_t size) {

// }

BplusTree &BplusTree::create(const std::vector<sql_types::Column> &columns) {
	BplusTree table_BplusTree(columns);
	BplusTrees.insert({table_BplusTree.get_root_page(), table_BplusTree});
	return BplusTrees.at(table_BplusTree.get_root_page());
}

void BplusTree::drop(std::uint32_t page_number) {
	BplusTrees.erase(page_number);
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
				throw std::runtime_error("B+ Tree with root page " + std::to_string(root_page) + " doesn't exist");
			}
		} else {
			if (root_page != 0 && root_page >= Pager::pages.size()) {
				throw std::runtime_error("B+ Tree with root page " + std::to_string(root_page) + " doesn't exist");
			}
			BplusTrees.insert({root_page, BplusTree(root_page)});
		}
	}

	return BplusTrees.at(root_page);
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
	BplusTrees.clear();
}

std::uint32_t BplusTree::get_root_page() const { return this->root_page; }
std::size_t BplusTree::get_value_size() const { return this->value_size; }
std::uint32_t BplusTree::get_total_cell_count() const { return this->total_cell_count; }

void BplusTree::insert(std::uint32_t key, void *value) {
	rec_insert(this->root_page, key, value);
	this->total_cell_count++;
}

void BplusTree::remove(std::uint32_t key) {
	rec_remove(this->root_page, key);
	this->total_cell_count--;
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
		if (node.get_next_leaf() == 0) {
			// end of B+ Tree
			this->page_number = 0;
			this->cell_number = 7;
		} else {
			this->page_number = node.get_next_leaf();
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
		start_page = this->root_page;
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

std::unordered_map<std::uint32_t, BplusTree> BplusTree::BplusTrees;

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

std::uint32_t BplusTree::leaf_insert(Node &node, std::size_t pos, std::uint32_t key, void *value) {

	std::uint32_t cell_start = leaf_header_size
								+ node.get_cell_count() * (key_size + this->value_size);
	if (cell_start + key_size + this->value_size > page_size) {
		std::cout << "splitting LEAF...\n";
		
		// left node gets one more cell - could have been the other way around, doesn't matter
		int right_split_count = (node.get_cell_count() + 1) / 2;
		int left_split_count = node.get_cell_count() + 1 - right_split_count;

		Node right_node(NodeType::LEAF, this->value_size);
		right_node.set_cell_count(right_split_count);
		// note: the loop below runs ONE MORE time than the number of cells in node
		for (int i = 0; i <= node.get_cell_count(); i++) {
			Node *dst_node = i < left_split_count ? &node : &right_node;
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

std::uint32_t BplusTree::interior_insert(Node &node, std::size_t pos, std::uint32_t key, std::uint32_t value) {

	std::uint32_t cell_start = interior_header_size
								+ node.get_cell_count() * (interior_value_size + key_size);
	if (cell_start + key_size + interior_value_size > page_size) {
		std::cout << "splitting INTERIOR...\n";

		// for interior nodes the split counts are the opposite of those for
		// leaf nodes, i.e. the right node gets one more cell
		// this is because of the right child
		int left_split_count = node.get_cell_count() / 2;
		int right_split_count = node.get_cell_count() - left_split_count - 1;

		Node right_node(NodeType::INTERIOR, interior_value_size);
		right_node.set_cell_count(right_split_count);
		right_node.set_right_child(node.get_right_child());
		for (int i = 0; i < node.get_cell_count(); i++) {
			Node *dst_node = i < left_split_count ? &node : &right_node;
			std::size_t dst_i = i % left_split_count;

			dst_node->set_cell(dst_i, node.get_cell_key(i), node.get_cell_value(i));
		}

		if (node.get_page_number() == this->root_page) {
			// create new left node and convert current node to interior node

			Node left_node(NodeType::INTERIOR, interior_value_size);
			left_node.set_cell_count(left_split_count);
			left_node.set_right_child(value);
			for (int i = 0; i < left_split_count; i++) {
				left_node.set_cell(i, node.get_cell_key(i), node.get_cell_value(i));
			}

			node.set_cell_count(1);
			node.set_right_child(right_node.get_page_number());
			std::uint32_t temp_value = left_node.get_page_number();
			node.set_cell(0, left_node.get_cell_key(left_split_count - 1), &temp_value);

		} else {
			// insert into parent interior node
			node.set_cell_count(left_split_count);
			node.set_right_child(value);
			return right_node.get_page_number();
		}

	} else {
		Node new_node(value, this->value_size);
		std::uint32_t new_node_max_key = new_node.get_cell_key(new_node.get_cell_count() - 1);

		if (pos < node.get_cell_count()) {
			for (int j = node.get_cell_count(); j > pos; j--) {
				node.set_cell(j, node.get_cell_key(j - 1), node.get_cell_value(j - 1));
			}
			node.set_cell(pos, new_node_max_key, &value);
		} else {
			Node right_child(node.get_right_child(), this->value_size);
			std::uint32_t right_child_page_number = node.get_right_child();

			node.set_cell(pos, right_child.get_cell_key(right_child.get_cell_count() - 1), &right_child_page_number);
			node.set_right_child(value);
		}
		node.set_cell_count(node.get_cell_count() + 1);
	}

	return 0;
}

std::uint32_t BplusTree::rec_insert(std::uint32_t page_number, std::uint32_t key, void *value) {
	Node node(page_number, this->value_size);

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

	switch (node.get_type()) {
	case NodeType::LEAF:
		return leaf_insert(node, mid_pos, key, value);
		break;

	case NodeType::INTERIOR: {
		std::uint32_t child_page_number = mid_pos < node.get_cell_count() ?
			*(std::uint32_t *)node.get_cell_value(mid_pos)
			:
			node.get_right_child();

		std::uint32_t new_node_page_number = rec_insert(child_page_number, key, value);

		if (new_node_page_number != 0) {
			return interior_insert(node, mid_pos, key, new_node_page_number);
		}

		return 0;
	}
		break;

	default:
		throw std::runtime_error("invalid node type (how?)");
	}
}

BplusTree::RemoveStatus BplusTree::leaf_remove(Node &node, std::size_t pos, std::uint32_t key) {

	std::uint32_t min_cell_count = (((page_size - leaf_header_size) / this->value_size) + 1) / 2;
	if (node.get_page_number() != this->root_page
		&& node.get_cell_count() - 1 < min_cell_count) {
		
		if (node.get_next_leaf() == 0) {
			// this is the last leaf node
			if (pos < node.get_cell_count() - 1) {
				for (int j = pos; j < node.get_cell_count() - 1; j++) {
					node.set_cell(j, node.get_cell_key(j + 1), node.get_cell_value(j + 1));
				}
			}
			node.set_cell_count(node.get_cell_count() - 1);

			return RemoveStatus{
				RemoveStatus::LEFT_SIBLING,
				0
			};

		} else {
			// there is a next leaf node
			Node next_leaf(node.get_next_leaf(), this->value_size);
			if (next_leaf.get_cell_count() - 1 < min_cell_count) {
				// merge with the next leaf node
				// throw std::runtime_error("TODO merge next leaf");
				std::cout << "merging LEAF...\n";

				node.set_next_leaf(next_leaf.get_next_leaf());
				if (pos < node.get_cell_count() - 1) {
					for (int j = pos; j < node.get_cell_count() - 1; j++) {
						node.set_cell(j, node.get_cell_key(j + 1), node.get_cell_value(j + 1));
					}
				}
				for (int j = 0; j < next_leaf.get_cell_count(); j++) {
					node.set_cell(node.get_cell_count() - 1 + j, next_leaf.get_cell_key(j), next_leaf.get_cell_value(j));
				}
				node.set_cell_count(node.get_cell_count() - 1 + next_leaf.get_cell_count());

				return RemoveStatus{
					RemoveStatus::MERGED,
					node.get_cell_key(node.get_cell_count() - 1)
				};

			} else {
				// take the smallest key from the next leaf node
				if (pos < node.get_cell_count() - 1) {
					for (int j = pos; j < node.get_cell_count() - 1; j++) {
						node.set_cell(j, node.get_cell_key(j + 1), node.get_cell_value(j + 1));
					}
				}
				node.set_cell(node.get_cell_count() - 1, next_leaf.get_cell_key(0), next_leaf.get_cell_value(0));
				for (int j = 0; j < next_leaf.get_cell_count() - 1; j++) {
					next_leaf.set_cell(j, next_leaf.get_cell_key(j + 1), next_leaf.get_cell_value(j + 1));
				}
				next_leaf.set_cell_count(next_leaf.get_cell_count() - 1);

				return RemoveStatus{
					RemoveStatus::TOOK_FROM_RIGHT_SIBLING,
					node.get_cell_key(node.get_cell_count() - 1)
				};
			}
		}

	} else {
		if (pos < node.get_cell_count() - 1) {
			for (int j = pos; j < node.get_cell_count() - 1; j++) {
				node.set_cell(j, node.get_cell_key(j + 1), node.get_cell_value(j + 1));
			}
		}
		node.set_cell_count(node.get_cell_count() - 1);

		return RemoveStatus{
			RemoveStatus::NOTHING_TO_DO,
			node.get_cell_key(node.get_cell_count() - 1)
		};
	}
}

BplusTree::RemoveStatus BplusTree::rec_remove(std::uint32_t page_number, std::uint32_t key) {
	Node node(page_number, this->value_size);

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

	switch (node.get_type()) {
	case NodeType::LEAF:
		return leaf_remove(node, mid_pos, key);
		break;

	case NodeType::INTERIOR: {
		std::uint32_t child_page_number = mid_pos < node.get_cell_count() ?
			*(std::uint32_t *)node.get_cell_value(mid_pos)
			:
			node.get_right_child();

		RemoveStatus status = rec_remove(child_page_number, key);
		// we need to update the cells of the current interior node here,
		// because we don't have access to it inside the last call to rec_remove
		switch (status.action) {
		case RemoveStatus::NOTHING_TO_DO:
			break;

		case RemoveStatus::TOOK_FROM_RIGHT_SIBLING:
			node.set_cell(mid_pos, status.max_key, &child_page_number);
			break;

		case RemoveStatus::MERGED:
			if (mid_pos == node.get_cell_count() - 1) {
				node.set_right_child(child_page_number);
				node.set_cell_count(node.get_cell_count() - 1);
				
				std::uint32_t min_cell_count = (((page_size - leaf_header_size) / interior_value_size) + 1) / 2;
				if (node.get_page_number() != this->root_page
					&& node.get_cell_count() < min_cell_count) {
					throw std::runtime_error("TODO merge interior");
				}
			} else {
				node.set_cell(mid_pos + 1, status.max_key, &child_page_number);
				for (int j = mid_pos; j < node.get_cell_count() - 1; j++) {
					node.set_cell(j, node.get_cell_key(j + 1), node.get_cell_value(j + 1));
				}
				node.set_cell_count(node.get_cell_count() - 1);
			}
			break;

		case RemoveStatus::LEFT_SIBLING:
			Node child_leaf(child_page_number, this->value_size);
			Node prev_leaf(*(std::uint32_t *)node.get_cell_value(node.get_cell_count() - 1), this->value_size);
			std::uint32_t min_cell_count = (((page_size - leaf_header_size) / this->value_size) + 1) / 2;
			if (prev_leaf.get_cell_count() - 1 < min_cell_count) {
				// merge with the previous leaf node
				std::cout << "merging prev LEAF...\n";
				node.set_cell_count(node.get_cell_count() - 1);

				if (node.get_page_number() != this->root_page
					&& node.get_cell_count() < min_cell_count) {
					throw std::runtime_error("TODO merge interior");
				}

				prev_leaf.set_next_leaf(0);
				for (int j = 0; j < child_leaf.get_cell_count(); j++) {
					prev_leaf.set_cell(prev_leaf.get_cell_count() + j, child_leaf.get_cell_key(j), child_leaf.get_cell_value(j));
				}
				prev_leaf.set_cell_count(prev_leaf.get_cell_count() + child_leaf.get_cell_count());

				node.set_right_child(prev_leaf.get_page_number());
			} else {
				// take the biggest key from the previous leaf node
				// NOTE: this case cannot occur in the current implementation of
				// the DB because of the constantly incrementing key values
				// (there may be bugs)
				if (mid_pos > 0) {
					for (int j = mid_pos; j > 0; j--) {
						child_leaf.set_cell(j, child_leaf.get_cell_key(j - 1), child_leaf.get_cell_value(j - 1));
					}
				}
				child_leaf.set_cell(0, prev_leaf.get_cell_key(prev_leaf.get_cell_count() - 1),
				                       prev_leaf.get_cell_value(prev_leaf.get_cell_count() - 1));
				prev_leaf.set_cell_count(prev_leaf.get_cell_count() - 1);

				node.set_cell(node.get_cell_count() - 1,
								prev_leaf.get_cell_key(prev_leaf.get_cell_count() - 1),
			                    node.get_cell_value(node.get_cell_count() - 1)); // this is just prev_leaf.page_number
			}
			break;
		}
	}
		break;

	default:
		throw std::runtime_error("invalid node type (how?)");
	}

	return RemoveStatus{};
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
	if (i >= this->cell_count) {
		throw std::runtime_error("cell position out of bounds");
	}

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
	if (i >= this->cell_count) {
		throw std::runtime_error("cell position out of bounds");
	}

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
	if (i >= this->cell_count) {
		throw std::runtime_error("cell position out of bounds");
	}

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
