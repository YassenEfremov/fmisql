#ifndef TABLES_INFO_HPP
#define TABLES_INFO_HPP

#include <string>
#include <string_view>
#include <unordered_set>


constexpr const char *tables_info_filename = "fmisql_tables.info";


// void tables_info_load();

void tables_info_add(std::string_view table_name);
void tables_info_remove(std::string_view table_name);
std::unordered_set<std::string> tables_info_list();


#endif // TABLES_INFO_HPP
