#ifndef REMOVE_HPP
#define REMOVE_HPP

#include "../data_types.hpp"

#include <functional>
#include <string_view>
#include <vector>


namespace fmisql {

void remove(std::string_view table_name, sql_types::Condition condition);

} // namespace fmisql


#endif // REMOVE_HPP

