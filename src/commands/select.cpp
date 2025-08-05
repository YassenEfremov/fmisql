#include "select.hpp"

#include "../btree.hpp"
#include "../cli/parser.hpp"
#include "../data_types.hpp"
#include "../schema.hpp"
#include "../statement.hpp"

#include <cstring>

#include <iostream>
#include <string_view>
#include <vector>


namespace fmisql {

static bool columns_contain(const std::vector<std::string_view> &columns,
                            std::string_view target) {

	if (columns.empty()) return true;

	for (std::string_view column : columns) {
		if (column == target) return true;
	}

	return false;
}

void select(const std::vector<std::string_view> &column_names,
            std::string_view table_name) {

	SchemaRow schema_row = BplusTree::get_schema_row_by_table_name(table_name);
	Statement statement = parse_line(schema_row.original_sql);
	BplusTree &table_BplusTree = BplusTree::get_table(schema_row.root_page);
	std::uint8_t *buffer = new std::uint8_t[table_BplusTree.get_value_size()];
	int rows_selected = 0;

	std::cout << '|';
	if (column_names.empty()) {
		for (sql_types::Column column : statement.create_columns) {
			std::cout << ' ' << column.name << " |";
		}
	} else {
		for (std::string_view column : column_names) {
			std::cout << ' ' << column << " |";
		}
	}
	std::cout << "\n------------------\n";



	for (void *cell : table_BplusTree) {

		std::uint32_t offset = 0;
		int i = 0;
		for (sql_types::Column column : statement.create_columns) {
			if (column_names.empty() || (i < column_names.size() && column.name == column_names[i])) {
				switch (column.type_id) {
				case sql_types::Id::INT:
					std::cout << *(sql_types::Int *)(((std::uint8_t *)cell) + key_size + offset) << "    ";
					offset += sql_types::max_int_size;
					break;
				case sql_types::Id::STRING:
					std::cout << std::string_view((char *)(((std::uint8_t *)cell) + key_size + offset), sql_types::max_string_size) << "    ";
					offset += sql_types::max_string_size;
					break;
				case sql_types::Id::DATE:
					// TODO
					offset += sql_types::max_date_size;
					break;
				}
				i++;
			}
		}

		// row.deserialize(cell);
		// for () {
		// 	std::cout << '|';
		// 	if (columns_contain(columns, "ID")) {
		// 		std::cout << ' ' << row.ID << " |";
		// 	}
		// 	if (columns_contain(columns, "Name")) {
		// 		std::cout << " \"" << row.Name << "\" |";
		// 	}
		// 	if (columns_contain(columns, "Value")) {
		// 		std::cout << ' ' << row.Value << " |";
		// 	}
		// 	std::cout << '\n';
		// }
		rows_selected++;
		std::cout << '\n';
	}

	delete[] buffer;

	std::cout << "Total " << rows_selected << " rows selected\n";
}

} // namespace fmisql
