#include "insert.hpp"

#include "../btree.hpp"
#include "../constants.hpp"
#include "../data_types.hpp"
#include "../debug.hpp"
#include "../cli/parser.hpp"
#include "../schema.hpp"
#include "../statement.hpp"

#include <cstdint>
#include <cstring>

#include <iostream>
#include <string>
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


void insert(std::string_view table_name, const std::vector<std::vector<sql_types::Value>> &rows) {

	SchemaRow schema_row = BplusTree::get_schema_row_by_table_name(table_name);
	Statement statement = parse_line(schema_row.original_sql);
	BplusTree &table_BplusTree = BplusTree::get_table(schema_row.root_page);
	std::uint8_t *buffer = new std::uint8_t[table_BplusTree.get_value_size()]{};

	for (std::vector<sql_types::Value> row : rows) {
		if (row.size() != statement.create_columns.size())
			throw std::runtime_error("one or more rows don't match the table they are being inserted into");

		// deserialize
		std::size_t offset = 0;
		std::size_t i = 0;
		for (sql_types::Value value : row) {
			std::visit(overloaded{
				[&](sql_types::Int value) {
					if (statement.create_columns[i].type_id != sql_types::Id::INT) {
						throw std::runtime_error("Int value doesn't match the column it's in");
					}
					*(sql_types::Int *)(buffer + offset) = value;
					offset += sql_types::max_int_size;
				},
				[&](sql_types::String value) {
					if (statement.create_columns[i].type_id != sql_types::Id::STRING) {
						throw std::runtime_error("String value doesn't match the column it's in");
					}
					if (value.size() > sql_types::max_string_size) {
						throw std::runtime_error("max sql string size is " + std::to_string(sql_types::max_string_size) + " characters");
					}
					std::memset(buffer + offset, 0, sql_types::max_string_size);
					std::memcpy(buffer + offset, value.data(), value.size());
					offset += sql_types::max_string_size;
				},
				[&](sql_types::Date value) {
					if (statement.create_columns[i].type_id != sql_types::Id::DATE) {
						throw std::runtime_error("Date value doesn't match the column it's in");
					}
					*(sql_types::Date *)(buffer + offset) = value;
					offset += sql_types::max_date_size;
				}
			}, value);

			i++;
		}

		DEBUG_PRINT("inserting...\n");
		table_BplusTree.insert(table_BplusTree.get_total_cell_count(), buffer);
	}

	delete[] buffer;

	std::cout << rows.size() << " rows inserted.\n";
}

} // namespace fmisql
