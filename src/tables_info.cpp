#include "tables_info.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_set>


void tables_info_add(std::string_view table_name) {
	std::ofstream tables_info_file(tables_info_filename, std::ios::app);
	tables_info_file << table_name << '\n';
}

void tables_info_remove(std::string_view table_name) {
	std::fstream tables_info_file(tables_info_filename);
	std::unordered_set<std::string> table_names;

	auto table_name_it = table_names.end();
	std::string curr_name;
	while (tables_info_file >> curr_name) {
		table_names.insert(curr_name);
		if (curr_name == table_name)
			table_name_it = table_names.find(curr_name);
	}

	if (table_name_it != table_names.end()) {
		table_names.erase(table_name_it);
	} else {
		throw std::runtime_error(
			"Table " + std::string(table_name) + " doesn't exist!\n"
		);
	}

	tables_info_file.close();
	tables_info_file.open(tables_info_filename, std::ios::out |
	                                            std::ios::trunc);
	for (const std::string &name : table_names) {
		tables_info_file << name << '\n';
	}
}

std::unordered_set<std::string> tables_info_list() {
	std::ifstream tables_info_file(tables_info_filename);
	std::unordered_set<std::string> table_names;

	std::string curr_name;
	while (tables_info_file >> curr_name) {
		table_names.insert(curr_name);
	}

	return table_names;
}
