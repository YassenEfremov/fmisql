#ifndef INSERT_HPP
#define INSERT_HPP

#include "../data_types.hpp"

#include <string_view>
#include <vector>


namespace fmisql {

void insert(std::string_view table_name, const std::vector<sql_types::ExampleRow> rows);

} // namespace fmisql


#endif // INSERT_HPP

