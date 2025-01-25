#ifndef CREATE_TABLE_HPP
#define CREATE_TABLE_HPP

#include "../data_types.hpp"
#include "../statement.hpp"

#include <string_view>
#include <vector>


namespace fmisql {

void create_table(std::string_view name, std::vector<Column> columns,
    std::string_view original_sql);

} // namespace fmisql


#endif // CREATE_TABLE_HPP

