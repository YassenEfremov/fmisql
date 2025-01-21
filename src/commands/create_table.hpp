#ifndef CREATE_TABLE_HPP
#define CREATE_TABLE_HPP

#include "../data_types.hpp"

#include <cstdint>

#include <string>
#include <string_view>
#include <unordered_map>


// temporary row structure
struct ExampleRow {
    std::uint32_t ID;
    char Name[256];
    std::uint32_t Value;
};


void create_table(std::string_view name,
                  const std::unordered_map<std::string_view, DataType> &columns);


#endif // CREATE_TABLE_HPP

