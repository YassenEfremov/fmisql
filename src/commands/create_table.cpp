#include "create_table.hpp"

#include "../tables_info.hpp"
#include "../data_types.hpp"

#include <fstream>
#include <string>
#include <unordered_map>


void create_table(const std::string &name,
                  const std::unordered_map<std::string, DataType> columns) {
	
	std::ofstream table_file(name + ".table");
	tables_info_add(name);
}
