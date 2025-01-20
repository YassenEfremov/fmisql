#ifndef CREATE_TABLE_HPP
#define CREATE_TABLE_HPP

#include "../data_types.hpp"

#include <string>
#include <unordered_map>


void create_table(const std::string &name,
                  const std::unordered_map<std::string, DataType> columns);


#endif // CREATE_TABLE_HPP

