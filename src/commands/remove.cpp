#include "remove.hpp"

#include "../btree.hpp"
#include "../cli/parser.hpp"
#include "../data_types.hpp"
#include "../schema.hpp"
#include "../statement.hpp"

#include <functional>
#include <iostream>
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
	for (sql_types::Column column : statement.create_columns) {
		column_type_id = column.type_id;

		if (column.name.empty() || column.name == condition.column_name) {
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


	for (auto it = table_BplusTree.begin(); it != table_BplusTree.end(); ) {

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



		// std::visit(overloaded{
		// 	[&](sql_types::Int lhs) {
		// 		if (!std::holds_alternative<sql_types::Int>(value))
		// 			throw std::runtime_error("invalid comparison of Int column");
		// 		return lhs == std::get<sql_types::Int>(value);
		// 	},
		// 	[&](sql_types::String lhs) {
		// 		if (!std::holds_alternative<sql_types::String>(value))
		// 			throw std::runtime_error("invalid comparison of String column");
		// 		if (std::get<sql_types::String>(value).size() > sql_types::max_string_size)
		// 			throw std::runtime_error("max sql string size is " + std::to_string(sql_types::max_string_size) + " characters");
		// 		return lhs == std::get<sql_types::String>(value);
		// 	},
		// 	[&](sql_types::Date lhs) {
		// 		if (!std::holds_alternative<sql_types::Date>(value))
		// 			throw std::runtime_error("invalid comparison of Date column");
		// 		return lhs == std::get<sql_types::Date>(value);
		// 	}
		// }, lhs);
	}

	// SchemaRow schema_row = BplusTree::get_schema_row_by_table_name(table_name);
	// Statement statement = parse_line(schema_row.original_sql);
	// BplusTree &table_BplusTree = BplusTree::get_table(schema_row.root_page);

	// SchemaRow row;
	// for (void *cell : table_BplusTree) {
	// 	std::visit(overloaded{
	// 		[&](sql_types::Int value) {
	// 			if (statement.create_columns[i].type_id != sql_types::Id::INT)
	// 				throw std::runtime_error("Int value doesn't match the column it's in");
	// 			*(sql_types::Int *)(buffer + offset) = value;
	// 			offset += sql_types::max_int_size;
	// 		},
	// 		[&](sql_types::String value) {
	// 			if (statement.create_columns[i].type_id != sql_types::Id::STRING)
	// 				throw std::runtime_error("String value doesn't match the column it's in");
	// 			if (value.size() > sql_types::max_string_size)
	// 				throw std::runtime_error("max sql string size is " + std::to_string(sql_types::max_string_size) + " characters");
	// 			std::memcpy(buffer + offset, value.data(), value.size());
	// 			offset += sql_types::max_string_size;
	// 		},
	// 		[&](sql_types::Date value) {
	// 			if (statement.create_columns[i].type_id != sql_types::Id::DATE)
	// 				throw std::runtime_error("DATE value doesn't match the column it's in");
	// 			*(sql_types::Date *)(buffer + offset) = value;
	// 			offset += sql_types::max_date_size;
	// 		}
	// 	}, value);
	// }

	std::cout << rows_removed << " rows removed.\n";
}

} // namespace fmisql
