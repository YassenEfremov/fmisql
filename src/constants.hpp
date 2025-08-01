#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include "data_types.hpp"

#include <cstdint>


namespace fmisql {

constexpr const char db_filename[] = "fmisql.db";

constexpr int page_size = 4096;
constexpr int table_max_pages = 100; // temporary
constexpr int schema_row_size = sql_types::max_string_size +
                                sql_types::max_int_size +
                                sql_types::max_string_size;

constexpr int example_row_size = sizeof(sql_types::Int) +
                                 sizeof(sql_types::String) +
                                 sizeof(sql_types::Int);

// common node header layout
constexpr std::uint32_t node_type_size = sizeof(std::uint8_t);
constexpr std::uint32_t node_type_offset = 0;
constexpr std::uint32_t is_root_size = sizeof(std::uint8_t);
constexpr std::uint32_t is_root_offset = node_type_size;
constexpr std::uint32_t parent_pointer_size = sizeof(std::uint32_t);
constexpr std::uint32_t parent_pointer_offset = is_root_offset + is_root_size;
constexpr std::uint8_t common_node_header_size =
    node_type_size + is_root_size + parent_pointer_size;

// leaf node header layout
constexpr std::uint32_t leaf_node_num_cells_size = sizeof(std::uint32_t);
constexpr std::uint32_t leaf_node_num_cells_offset = common_node_header_size;
constexpr std::uint32_t leaf_node_value_size_size = sizeof(std::uint32_t);
constexpr std::uint32_t leaf_node_value_size_offset = common_node_header_size + leaf_node_num_cells_size;
constexpr std::uint32_t leaf_node_next_leaf_size = sizeof(std::uint32_t);
constexpr std::uint32_t leaf_node_next_leaf_offset = leaf_node_value_size_offset +
                                                     leaf_node_value_size_size;
constexpr std::uint32_t leaf_node_header_size = common_node_header_size +
                                                leaf_node_num_cells_size +
                                                leaf_node_value_size_size +
                                                leaf_node_next_leaf_size;

// leaf node body layout
constexpr std::uint32_t leaf_node_key_size = sizeof(std::uint32_t);
constexpr std::uint32_t leaf_node_key_offset = 0;
// not constant, depends on the columns
// const std::uint32_t leaf_node_value_size;
constexpr std::uint32_t leaf_node_value_offset = leaf_node_key_offset + leaf_node_key_size;
// not constant, depends on the columns
// constexpr std::uint32_t leaf_node_pair_size = leaf_node_key_size + leaf_node_value_size;
constexpr std::uint32_t leaf_node_space_for_cells = page_size - leaf_node_header_size;
// not constant, depends on the columns
// constexpr std::uint32_t leaf_node_max_cells = leaf_node_space_for_cells / leaf_node_pair_size;

// not constant, depends on the columns
// constexpr int rows_per_page = (page_size - leaf_node_header_size) / (leaf_node_pair_size);

// internal node header layout
constexpr std::uint32_t internal_node_num_keys_size = sizeof(std::uint32_t);
constexpr std::uint32_t internal_node_num_keys_offset = common_node_header_size;
constexpr std::uint32_t internal_node_right_child_size = sizeof(std::uint32_t);
constexpr std::uint32_t internal_node_right_child_offset =
    internal_node_num_keys_offset + internal_node_num_keys_size;
constexpr std::uint32_t internal_node_header_size = common_node_header_size +
                                           internal_node_num_keys_size +
                                           internal_node_right_child_size;

// internal node body layout
constexpr std::uint32_t internal_node_key_size = sizeof(std::uint32_t);
constexpr std::uint32_t internal_node_child_size = sizeof(std::uint32_t);
constexpr std::uint32_t internal_node_cell_size =
    internal_node_child_size + internal_node_key_size;

} // namespace fmisql


#endif // CONSTANTS_HPP
