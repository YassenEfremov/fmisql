#ifndef BTREE_HPP
#define BTREE_HPP

#include "constants.hpp"
#include "data_types.hpp"
#include "schema.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <vector>
#include <unordered_map>


namespace fmisql {

// OLD FORMATS

/**
 * Leaf Node format:
 * 
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |      type     |    is_root    |                        parent >
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * < parent                        |                    pair_count >
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * < pair_count                    |                    value_size >
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * < value_size                    |                     next_leaf >
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * < next_leaf                     |                           key >
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * < key                           |             value             >
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * <                              ...                              |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                              key                              |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                             value                             >
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * <                              ...                              |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

/**
 * Internal Node format:
 * 
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |      type     |    is_root    |                        parent >
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * < parent                        |                     key_count >
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * < key_count                     |                   right_child >
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * < right_child                   |                         value >
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * < value                         |              key              >
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * <                              ...                              |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                             value                             |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                              key                              |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                              ...                              |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

// class Node {
// public:
//     enum class Type {
//         LEAF,
//         INTERNAL
//     };

//     Node(sql_types::Int number, std::uint32_t value_size = 0);

//     static sql_types::Int number_of_table(std::string_view name);

//     // for both leaf and internal nodes
//     Type get_type();
//     void set_type(Type type);
//     std::uint8_t *is_root();

//     // only for leaf nodes
//     std::uint32_t *pair_count();
//     void *leaf_pair(int pos);
//     std::uint32_t *pair_key(int pos);
//     void *pair_value(int pos);
//     std::uint32_t *value_size();
//     std::uint32_t *next_leaf();

//     // only for internal nodes
//     std::uint32_t *key_count();
//     std::uint32_t *right_child();
//     std::uint32_t *internal_pair(int pos);
//     std::uint32_t *child(int pos);
//     std::uint32_t *key(int pos);

//     // for both leaf and internal nodes
//     std::uint32_t max_key();

//     /**
//      * @brief Uses binary search to find the best place to insert the given
//      *        key-value pair. Throws an exception if the pair is already in the
//      *        tree.
//      */
//     void insert(std::uint32_t key, void *value);

//     sql_types::Int start_leaf();

//     // TODO
//     // ~Node();

// private:
//     /**
//      * @brief Inserts the given key-value pair at the given position, shifting
//      *        all pairs after it to the right
//      */
//     void leaf_insert(int pos, std::uint32_t key, void *value);

//     /**
//      * @brief Searches internal nodes and inserts when it finds the target leaf
//      */
//     void internal_insert(std::uint32_t key, void *value);

//     /**
//      * @brief Split leaf node to insert
//      */
//     void split_insert(int pos, std::uint32_t key, void *value);

//     void *data;
//     int page;

//     int pair_size;
//     int max_pairs;

//     std::uint32_t leaf_node_right_split_count;
//     std::uint32_t leaf_node_left_split_count;

// 	// bool is_root;
// 	// void *parent;
// 	// std::size_t cell_count;
// 	// void *values;
// };











/**
 *  Leaf format:
 * 
 *     0                   1                   2                   3
 *     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     type      |                          cell_count          >>
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    <<              |                           next_leaf          >>
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    <<              |                              key             >>
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    <<              |                             value            >>
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    <<                             ...                              |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                              key                              |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                             value                            >>
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    <<                             ...                              |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */

/**
 *  Interior format:
 * 
 *     0                   1                   2                   3
 *     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     type      |                          cell_count          >>
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    <<              |                          right_child         >>
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    <<              |                             value            >>
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    <<              |                              key             >>
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    <<              |                              ...              |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                             value                             |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                              key                              |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                              ...                              |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */


/**
 * @brief A class that represents the B+ tree used to store table data.
 */
class BplusTree {
public:
    /**
     * @brief Creates a new B+ Tree and registers it.
     */
    static BplusTree &create(const std::vector<sql_types::Column> &columns);

    /**
     * @brief Returns a registered B+ Tree with the given page number. Throws if
     *        such a tree doesn't exist.
     */
    static BplusTree &get_table(std::uint32_t page_number);
    
    /**
     * @brief Returns the registered schema B+ Tree (or creates it if it doesn't
     *        exist).
     */
    static BplusTree &get_schema();
    
    static SchemaRow get_schema_row_by_table_page(std::uint32_t page_number);
    
    static SchemaRow get_schema_row_by_table_name(std::string_view table_name);

    /**
     * @brief Deletes all registered B+ Trees (but not their pages!)
     */
    static void delete_all_trees();

    /**
     * @brief Returns the root page number of the B+ tree.
     */
    std::uint32_t get_root_page() const;

    /**
     * @brief Returns the size of values stored in the B+ tree.
     */
    std::size_t get_value_size() const;

    /**
     * @brief Returns the total number of key-value pairs in the B+ tree.
     */
    std::uint32_t get_total_cell_count() const;

    /**
     * @brief Inserts a key-value pair into the B+ tree.
     */
    void insert(std::uint32_t key, void *value);

    class Iterator {
    public:
        Iterator(std::uint32_t page_number,
                 std::uint32_t cell_number,
                 std::uint32_t value_size);

        Iterator &operator++();
        bool operator==(const Iterator &other) const;
        bool operator!=(const Iterator &other) const;
        void *&operator*();
        // const void *&operator*() const;

    private:
        std::uint32_t page_number;
        std::uint32_t cell_number;
        std::uint32_t value_size;
        void *value_buffer; // only because operator* has to return a reference
    };

    Iterator begin() const;
    Iterator end() const;

private:
    static std::unordered_map<std::uint32_t, BplusTree *> BplusTrees;

    BplusTree(const std::vector<sql_types::Column> &columns);
    BplusTree(std::uint32_t page_number);

    enum class NodeType {
        LEAF,
        INTERIOR
    };

    class Node {
    public:
        Node(int page_number, std::uint32_t value_size);
        Node(NodeType type, std::uint32_t value_size);

        // common
        NodeType get_type() const;
        std::uint32_t get_cell_count() const;
        std::uint32_t get_page_number() const;
        std::uint32_t get_value_size() const;
        void *get_cell(std::size_t i);
        std::uint32_t get_cell_key(std::size_t i);
        void *get_cell_value(std::size_t i);

        void set_type(NodeType type);
        void set_cell_count(std::uint32_t cell_count);
        void set_value_size(std::uint32_t value_size);
        void set_cell(std::size_t i, std::uint32_t key, void *value);

        // leaf only
        std::uint32_t get_next_leaf() const;
        void set_next_leaf(std::uint32_t next_leaf);

        // interior only
        std::uint32_t get_right_child() const;
        void set_right_child(std::uint32_t right_child);

    private:
        NodeType type;
        std::uint32_t cell_count;
        std::uint32_t page_number;
        std::uint32_t value_size;
        void *data;
    };

    std::uint32_t leaf_insert(std::uint32_t leaf_page_number, std::size_t pos, std::uint32_t key, void *value);
    std::uint32_t rec_insert(std::uint32_t page_number, std::uint32_t key, void *value);

    std::uint32_t root_page;
    std::uint32_t total_cell_count;
    std::vector<sql_types::Column> columns;
    std::size_t value_size;
};

} // namespace fmisql


#endif // BTREE_HPP
