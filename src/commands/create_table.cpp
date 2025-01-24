#include "create_table.hpp"

#include "../tables_info.hpp"
#include "../data_types.hpp"

#include <fstream>
#include <string>
#include <string_view>
#include <unordered_map>

namespace fmisql {

void create_table(std::string_view name,
                  const std::unordered_map<std::string_view, sql_types::Id> &columns) {
	
	std::ofstream table_file(std::string(name) + ".table");
	tables_info_add(name);
}

} // namespace fmisql
