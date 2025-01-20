#include "drop_table.hpp"

#include "../tables_info.hpp"

#include <filesystem>
#include <string>


namespace fs = std::filesystem;

void drop_table(const std::string &name) {
	fs::remove(name + ".table");
	tables_info_remove(name);
}
