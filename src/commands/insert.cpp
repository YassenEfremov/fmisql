#include "insert.hpp"

#include "../btree.hpp"
#include "../constants.hpp"
#include "../data_types.hpp"
#include "../cli/parser.hpp"
#include "../schema.hpp"
#include "../statement.hpp"

#include <cstdint>
#include <cstring>

#include <iostream>
#include <string_view>
#include <stdexcept>
#include <variant>
#include <vector>


namespace fmisql {


// helper type for the visitor
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;


// Example inset statements:
// Insert INTO Sample {(15, "Test message", 120)}
// Insert INTO Sample {(16, "farlands", 99)}
// Insert INTO Sample {(1, "asd", 12), (2, "asd", 12), (3, "asd", 12), (4, "asd", 12), (5, "ddd", 5)}
// Insert INTO Sample {(6, "asd", 12), (7, "asd", 12), (8, "asd", 12), (9, "asd", 12), (10, "ddd", 5)}
// Insert INTO Sample {(11, "asd", 12), (12, "asd", 12), (13, "asd", 12), (14, "asd", 12), (15, "ddd", 5)}
// Insert INTO Sample {(15, "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", 120)}

void insert(std::string_view table_name, const std::vector<std::vector<sql_types::Value>> &rows) {

	BplusTree &schema_BplusTree = BplusTree::get_schema();

	// TODO: use some form of a select statement instead of a loop
	SchemaRow row("", 0, "");
	for (int i = 0; i < schema_BplusTree.get_cell_count(); i++) {
		row.deserialize(schema_BplusTree.get_cell_value(i));

		if (row.table_name == table_name) {

			BplusTree &table_BplusTree = BplusTree::get_table(row.root_page);
			for (std::vector<sql_types::Value> row : rows) {
				std::uint8_t *buffer = new std::uint8_t[table_BplusTree.get_value_size()];
				std::size_t offset = 0;
				// deserialize
				for (sql_types::Value value : row) {
					std::visit(overloaded{
						[&buffer, &offset](sql_types::Int value) {
							*(sql_types::Int *)(buffer + offset) = value;
							offset += sql_types::max_int_size;
						},
						[&buffer, &offset](sql_types::String value) {
							if (value.size() > sql_types::max_string_size)
								throw std::runtime_error("max sql string size is " + std::to_string(sql_types::max_string_size) + " characters");
							std::memcpy(buffer + offset, value.data(), value.size());
							offset += sql_types::max_string_size;
						},
						[&buffer, &offset](sql_types::Date value) {
							*(sql_types::Date *)(buffer + offset) = value;
							offset += sql_types::max_date_size;
						}
					}, value);
				}

				table_BplusTree.insert(table_BplusTree.get_cell_count(), buffer);
			}
		}
	}
}

} // namespace fmisql
