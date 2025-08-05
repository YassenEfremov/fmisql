#ifndef SELECT_HPP
#define SELECT_HPP

#include <string_view>
#include <vector>


namespace fmisql {

void select(const std::vector<std::string_view> &column_names,
            std::string_view table_name);

} // namespace fmisql


#endif // SELECT_HPP

