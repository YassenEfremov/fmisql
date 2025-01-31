#include "table_info.hpp"

#include "../btree.hpp"
#include "../constants.hpp"
#include "../data_types.hpp"
#include "../cli/parser.hpp"
#include "../statement.hpp"

#include <cstdint>

#include <iostream>
#include <string_view>


namespace fmisql {

void table_info(std::string_view name) {

	Node node(0, schema_row_size);

	SchemaRow row("", 0, "");
	for (int i = 0; i < *node.pair_count(); i++) {
		row.deserialize(node.pair_value(i));

		if (row.table_name == name) {
			std::cout << "Table " << name << " : ";

			Statement statement = parse_line(row.sql);

			std::cout << '(';
			for (int i = 0; i < statement.create_columns.size(); i++) {
				std::cout << statement.create_columns[i].name << ':';
				switch (statement.create_columns[i].type_id) {
				case sql_types::Id::INT:
					std::cout << "Int";
					break;
				case sql_types::Id::STRING:
					std::cout << "String";
					break;
				case sql_types::Id::DATE:
					break;
				}
				if (i != statement.create_columns.size() - 1) std::cout << ", ";
			}
			std::cout << ")\n";

			Node table_node(row.page);
			std::uint32_t row_count = *table_node.pair_count();
			std::cout << "Total " << row_count << " rows (" <<
				static_cast<double>(row_count * example_row_size)/1000 <<
				" KB data) in the table\n";

			return;
		}
	}

	std::cout << "There is no such table!\n";
}

} // namespace fmisql
