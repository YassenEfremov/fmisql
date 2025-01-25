#ifndef TABLE_INFO_HPP
#define TABLE_INFO_HPP

// #include "../data_types.hpp"

#include <string_view>
// #include <unordered_map>


// struct TableInfo {
// 	std::unordered_map<std::string, DataType> columns;
// 	long records;
// };

namespace fmisql {
	
void table_info(std::string_view name);

} // namespace fmisql


#endif // TABLE_INFO_HPP

