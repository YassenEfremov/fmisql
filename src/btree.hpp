#ifndef BTREE_HPP
#define BTREE_HPP

#include "constants.hpp"
#include "data_types.hpp"
#include "pager.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>


namespace fmisql {



/**
 * Leaf Node format:
 * 
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |      type     |    is_root    |                        parent |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | parent                        |                    pair_count |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | pair_count                    |                           key |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * | key                           |             value             |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                              ...                              |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                              key                              |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                             value                             |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                              ...                              |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

class LeafNode {
public:
    LeafNode(void *data = Pager::get_page(0));

    std::uint32_t *get_pair_count();

    // pair access
    void *get_pair(int pos);
    std::uint32_t *get_pair_key(int pos);
    void *get_pair_value(int pos);

    /**
     * @brief Inserts the given key-value pair at the given position, shifting
     *        all pairs after it to the right
     */
    void insert_at(int pos, std::uint32_t key, sql_types::ExampleRow value);

    /**
     * @brief Uses binary search to find the best place to insert the given
     *        key-value pair. Throws an exception if the pair is already in the
     *        tree.
     */
    void insert(std::uint32_t key, sql_types::ExampleRow value);

private:
    void *data;

	// bool is_root;
	// void *parent;
	// std::size_t cell_count;
	// void *values;
};


class BTree {
public:

private:
};

} // namespace fmisql


#endif // BTREE_HPP
