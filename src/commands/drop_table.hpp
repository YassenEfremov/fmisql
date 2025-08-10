#ifndef DROP_TABLE_HPP
#define DROP_TABLE_HPP

#include <string_view>


namespace fmisql {
	
void drop_table(std::string_view table_name);
	
} // namespace fmisql


#endif // DROP_TABLE_HPP

