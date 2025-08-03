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

void table_info(std::string_view table_name) {

	SchemaRow schema_row = BplusTree::get_schema_row_by_table_name(table_name);
	Statement statement = parse_line(schema_row.original_sql);
	std::cout << "Table " << table_name << " : (";

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

	BplusTree &table_BplusTree = BplusTree::get_table(schema_row.root_page);
	std::uint32_t row_count = table_BplusTree.get_total_cell_count();
	std::cout << "Total " << row_count << " rows ("
		<< static_cast<double>(row_count * table_BplusTree.get_value_size())/1000
		<< " KB data) in the table\n";

	// throw std::runtime_error("There is no such table!");
}

} // namespace fmisql
