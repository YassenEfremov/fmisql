#ifndef BTREE_HPP
#define BTREE_HPP

#include "constants.hpp"
#include "data_types.hpp"
#include "schema.hpp"

#include <string_view>
#include <unordered_map>
#include <vector>

#include <cstddef>
#include <cstdint>


namespace fmisql {

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
     * @brief Removes the B+ Tree with the given page number and unregisters it.
     *        Throws if such a tree doesn't exist.
     */
    static void drop(std::uint32_t page_number);

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

    /**
     * @brief Removes the key-value pair that corresponds to the given key from
     *        the B+ tree.
     */
    void remove(std::uint32_t key);

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
    static std::unordered_map<std::uint32_t, BplusTree> BplusTrees;

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
        std::uint32_t get_cell_key(std::size_t i) const;
        void *get_cell_value(std::size_t i);
        std::uint32_t get_max_key() const;

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

    std::uint32_t leaf_insert(Node &node, std::size_t pos,
                              std::uint32_t key, void *value);
    std::uint32_t interior_insert(Node &node, std::size_t pos,
                                  std::uint32_t key, std::uint32_t value);
    std::uint32_t rec_insert(std::uint32_t page_number,
                             std::uint32_t key, void *value);

    // There are 5 cases when removing cells from leaf nodes:
    // 1. removing cells doesn't require any updates to the tree
    // 2. removing cells (when there IS a next leaf) requires taking cells
    //    from that next leaf
    // 3. removing cells (when there IS a next leaf) requires taking cells
    //    from that next leaf, but that next leaf has too few cells, so
    //    we need to merge the two cells
    // 4. removing cells (when there is NO next leaf) requires taking cells
    //    from the previous leaf
    // 5. removing cells (when there is NO next leaf) requires taking cells
    //    from the previous leaf, but that previous leaf has too few cells,
    //    so we need to merge the two leafs

    // There are 5 cases when removing cells from interior nodes:
    // 1. removing cells doesn't require any updates to the tree
    // 2. removing cells (when there IS a next interior) requires taking cells
    //    from that next interior
    // 3. removing cells (when there IS a next interior) requires taking cells
    //    from that next interior, but that next interior has too few cells, so
    //    we need to merge the two cells
    // 4. removing cells (when there is NO next interior) requires taking cells
    //    from the previous interior
    // 5. removing cells (when there is NO next interior) requires taking cells
    //    from the previous interior, but that previous interior has too few
    //    cells, so we need to merge the two interiors

    // final case: when the parent of the nodes we are deleting from is the root
    // 1. we are deleting from leafs and need to merge, so the merged node
    //    becomes the new root
    // 2. we are deleting from interiors and need to merge, so the merged node
    //    becomes the new root
    struct RemoveStatus {
        enum {
            // common
            NOTHING_TO_DO,
            // these are when there is a next leaf
            TOOK_FROM_RIGHT_SIBLING,
            MERGED,
            // these are when this is the last leaf
            // we can't access the previous leaf, so we don't know wether we
            // need to take from it or merge with it
            LEFT_SIBLING
        } action;
        std::uint32_t max_key;
    };
    RemoveStatus leaf_remove(Node &node, std::size_t pos, std::uint32_t key);
    RemoveStatus interior_remove(std::uint32_t page_number, std::uint32_t key);
    RemoveStatus rec_remove(std::uint32_t page_number, std::uint32_t key);

    std::uint32_t root_page;
    std::uint32_t total_cell_count;
    std::vector<sql_types::Column> columns;
    std::size_t value_size;
};

} // namespace fmisql


#endif // BTREE_HPP
