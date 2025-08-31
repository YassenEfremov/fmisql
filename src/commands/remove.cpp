#include "remove.hpp"

#include "../btree.hpp"
#include "../cli/parser.hpp"
#include "../data_types.hpp"
#include "../schema.hpp"
#include "../statement.hpp"

#include <functional>
#include <iostream>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include <cstdint>
#include <cstring>


namespace fmisql {


// helper type for the visitor
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;


void remove(std::string_view table_name, sql_types::Condition condition) {

	SchemaRow schema_row = BplusTree::get_schema_row_by_table_name(table_name);
	Statement statement = parse_line(schema_row.original_sql);
	BplusTree &table_BplusTree = BplusTree::get_table(schema_row.root_page);
	std::uint8_t *buffer = new std::uint8_t[table_BplusTree.get_value_size()];
	int rows_removed = 0;

	std::uint32_t offset = 0;
	sql_types::Id column_type_id;
	bool column_found = condition.column_name.empty();
	for (sql_types::Column column : statement.create_columns) {
		column_type_id = column.type_id;

		if (column.name == condition.column_name) {
			std::visit(overloaded{
				[&](sql_types::Int _) {
					if (column_type_id != sql_types::Id::INT) {
						throw std::runtime_error("invalid comparison with Int value");
					}
				},
				[&](sql_types::String _) {
					if (column_type_id != sql_types::Id::STRING) {
						throw std::runtime_error("invalid comparison with String value");
					}
				},
				[&](sql_types::Date _) {
					if (column_type_id != sql_types::Id::DATE) {
						throw std::runtime_error("invalid comparison with Date value");
					}
				}
			}, condition.value);

			column_found = true;
			break;
		}

		switch (column.type_id) {
		case sql_types::Id::INT:
			offset += sql_types::max_int_size;
			break;
		case sql_types::Id::STRING:
			offset += sql_types::max_string_size;
			break;
		case sql_types::Id::DATE:
			offset += sql_types::max_date_size;
			break;
		}
	}
	if (!column_found) {
		throw std::runtime_error("column " + std::string(condition.column_name) + " doesn't exist");
	}

	// full table scan for now
	// an index B+ Tree is needed for faster search
	for (auto it = table_BplusTree.begin(); it != table_BplusTree.end(); ) {

		// if there is no condition (i.e. condition.column_name is empty) the
		// condition column value gets constructed in a bit of a sketchy way
		switch (column_type_id) {
		case sql_types::Id::INT:
			if (condition.op(*(sql_types::Int *)(((std::uint8_t *)*it) + key_size + offset))) {
				table_BplusTree.remove(*(std::uint32_t *)*it);
				rows_removed++;
				it = table_BplusTree.begin(); // reset it because otherwise ugly things will happen
			} else {
				++it;
			}
			break;
		case sql_types::Id::STRING: {
			std::string_view value((char *)(((std::uint8_t *)*it) + key_size + offset), sql_types::max_string_size);
			value.remove_suffix(value.size() - value.find('\0'));
			if (condition.op(value)) {
				table_BplusTree.remove(*(std::uint32_t *)*it);
				rows_removed++;
				it = table_BplusTree.begin(); // reset it because otherwise ugly things will happen
			} else {
				++it;
			}
		}
			break;
		case sql_types::Id::DATE:
			if (condition.op(*(sql_types::Date *)(((std::uint8_t *)*it) + key_size + offset))) {
				table_BplusTree.remove(*(std::uint32_t *)*it);
				rows_removed++;
				it = table_BplusTree.begin(); // reset it because otherwise ugly things will happen
			} else {
				++it;
			}
			break;
		}
	}

	std::cout << rows_removed << " rows removed.\n";
}

} // namespace fmisql
