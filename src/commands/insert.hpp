#ifndef INSERT_HPP
#define INSERT_HPP

#include "../pager.hpp"

#include <string_view>
#include <vector>


namespace fmisql {

void insert(std::string_view table_name, const std::vector<ExampleRow> rows);

} // namespace fmisql


#endif // INSERT_HPP

