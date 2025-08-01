#include "table_info.hpp"

#include "../btree.hpp"
#include "../constants.hpp"
#include "../data_types.hpp"
#include "../cli/parser.hpp"
#include "../statement.hpp"
#include "../schema.hpp"

#include <cstdint>

#include <iostream>
#include <string_view>


namespace fmisql {

void table_info(std::string_view name) {

	BplusTree &schema_BplusTree = BplusTree::get_schema();

	// TODO: use some form of a select statement instead of a loop
	SchemaRow row("", 0, "");
	for (int i = 0; i < schema_BplusTree.get_cell_count(); i++) {
		row.deserialize(schema_BplusTree.get_cell_value(i));

		if (row.table_name == name) {
			std::cout << "Table " << name << " : ";

			Statement statement = parse_line(row.original_sql);

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

			BplusTree &table_BplusTree = BplusTree::get_table(row.root_page);
			std::uint32_t row_count = table_BplusTree.get_cell_count();
			std::cout << "Total " << row_count << " rows ("
				<< static_cast<double>(row_count * table_BplusTree.get_value_size())/1000
				<< " KB data) in the table\n";

			return;
		}
	}

	throw std::runtime_error("There is no such table!");
}

} // namespace fmisql
