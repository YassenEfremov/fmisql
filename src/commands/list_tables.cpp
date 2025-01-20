#include "list_tables.hpp"

#include "../tables_info.hpp"

#include <string>
#include <unordered_set>


std::unordered_set<std::string> list_tables() {
	return tables_info_list();
}
