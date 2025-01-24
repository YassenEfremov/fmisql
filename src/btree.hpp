#ifndef BTREE_HPP
#define BTREE_HPP

#include <cstddef>
#include <cstdint>


constexpr int page_size = 4096;
constexpr int table_max_pages = 100;
constexpr int row_size = 4 + 256 + 4;



/*
 * Common Node Header Layout
 */
const std::uint32_t NODE_TYPE_SIZE = sizeof(std::uint8_t);
const std::uint32_t NODE_TYPE_OFFSET = 0;
const std::uint32_t IS_ROOT_SIZE = sizeof(std::uint8_t);
const std::uint32_t IS_ROOT_OFFSET = NODE_TYPE_SIZE;
const std::uint32_t PARENT_POINTER_SIZE = sizeof(std::uint32_t);
const std::uint32_t PARENT_POINTER_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE;
const std::uint8_t COMMON_NODE_HEADER_SIZE =
    NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE;

/*
 * Leaf Node Header Layout
 */
const std::uint32_t LEAF_NODE_NUM_CELLS_SIZE = sizeof(std::uint32_t);
constexpr std::uint32_t LEAF_NODE_NUM_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;
constexpr std::uint32_t LEAF_NODE_HEADER_SIZE =
    COMMON_NODE_HEADER_SIZE + LEAF_NODE_NUM_CELLS_SIZE;

/*
 * Leaf Node Body Layout
 */
constexpr std::uint32_t LEAF_NODE_KEY_SIZE = sizeof(std::uint32_t);
const std::uint32_t LEAF_NODE_KEY_OFFSET = 0;
const std::uint32_t LEAF_NODE_VALUE_SIZE = row_size;
const std::uint32_t LEAF_NODE_VALUE_OFFSET =
    LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE;
constexpr std::uint32_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;
const std::uint32_t LEAF_NODE_SPACE_FOR_CELLS = page_size - LEAF_NODE_HEADER_SIZE;
constexpr std::uint32_t LEAF_NODE_MAX_CELLS =
    LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;


constexpr int rows_per_page = (page_size - LEAF_NODE_HEADER_SIZE) / (LEAF_NODE_CELL_SIZE);



/**
 * Leaf Node format:
 * 
 * +---------------------------+
 * |                           |
 * |       TODO                |
 * |                           |
 * |                           |
 * +---------------------------+
 */

class LeafNode {
public:
    LeafNode(void *data);

    std::uint32_t *get_cell_count();

    // cell access
    void *get_cell(int cell_number);
    std::uint32_t *get_cell_key(int cell_number);
    void *get_cell_value(int cell_number);

private:
    void *data;

	bool is_root;
	void *parent;
	std::size_t cell_count;
	void *values;
};


class BTree {
public:

private:
};


#endif // BTREE_HPP
