#ifndef TABLE_INFO_HPP
#define TABLE_INFO_HPP

#include "../data_types.hpp"

#include <string>
#include <unordered_map>


struct TableInfo {
	std::unordered_map<std::string, DataType> columns;
	long records;
};


TableInfo table_info(const std::string &name);


#endif // TABLE_INFO_HPP

