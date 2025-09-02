#ifndef REMOVE_HPP
#define REMOVE_HPP

#include "../data_types.hpp"

#include <string_view>


namespace fmisql {

void remove(std::string_view table_name, sql_types::Condition condition);

} // namespace fmisql


#endif // REMOVE_HPP

