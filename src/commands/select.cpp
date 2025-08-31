#include "select.hpp"

#include "../btree.hpp"
#include "../cli/parser.hpp"
#include "../data_types.hpp"
#include "../schema.hpp"
#include "../statement.hpp"

#include <cstring>

#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <unordered_map>


namespace fmisql {

// static bool columns_contain(const std::vector<std::string_view> &columns,
//                             std::string_view target) {

// 	if (columns.empty()) return true;

// 	for (std::string_view column : columns) {
// 		if (column == target) return true;
// 	}

// 	return false;
// }


// helper type for the visitor
template<class... Ts>
struct overloaded : Ts... { using Ts::operator()...; };
// explicit deduction guide (not needed as of C++20)
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;


struct ColumProps {
	sql_types::Id type_id;
	std::size_t offset;
	// std::size_t print_width;
};

void select(const std::vector<std::string_view> &column_names,
            std::string_view table_name,
            const sql_types::Condition &condition) {

	SchemaRow schema_row = BplusTree::get_schema_row_by_table_name(table_name);
	Statement statement = parse_line(schema_row.original_sql);
	BplusTree &table_BplusTree = BplusTree::get_table(schema_row.root_page);
	int rows_selected = 0;
	std::unordered_map<std::string_view, ColumProps> column_props;

	std::uint32_t offset = 0;
	bool condition_column_found = condition.column_name.empty();
	for (sql_types::Column column : statement.create_columns) {
		if (column.name == condition.column_name) {
			std::visit(overloaded{
				[&](sql_types::Int _) {
					if (column.type_id != sql_types::Id::INT) {
						throw std::runtime_error("invalid comparison with Int value");
					}
				},
				[&](sql_types::String _) {
					if (column.type_id != sql_types::Id::STRING) {
						throw std::runtime_error("invalid comparison with String value");
					}
				},
				[&](sql_types::Date _) {
					if (column.type_id != sql_types::Id::DATE) {
						throw std::runtime_error("invalid comparison with Date value");
					}
				}
			}, condition.value);

			condition_column_found = true;
		}
		column_props[column.name] = ColumProps{ column.type_id, offset };
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

	for (std::string_view column_name : column_names) {
		if (column_props.find(column_name) == column_props.end()) {
			throw std::runtime_error("column " + std::string(column_name) + " doesn't exist");
		}
	}
	if (!condition_column_found) {
		throw std::runtime_error("column " + std::string(condition.column_name) + " doesn't exist");
	}


	// for (void *cell : table_BplusTree) {

	// 	// if there is no condition (i.e. condition.column_name is empty) the
	// 	// condition column value gets constructed in a bit of a sketchy way
	// 	switch (column_props[condition.column_name].type_id) {
	// 	case sql_types::Id::INT:
	// 		if (!condition.op(*(sql_types::Int *)(((std::uint8_t *)cell) + key_size + column_props[condition.column_name].offset))) {
	// 			continue;
	// 		}
	// 		break;
	// 	case sql_types::Id::STRING: {
	// 		std::string_view value((char *)(((std::uint8_t *)cell) + key_size + column_props[condition.column_name].offset), sql_types::max_string_size);
	// 		value.remove_suffix(value.size() - value.find('\0'));
	// 		if (!condition.op(value)) {
	// 			continue;
	// 		}
	// 	}
	// 		break;
	// 	case sql_types::Id::DATE:
	// 		if (condition.op(*(sql_types::Date *)(((std::uint8_t *)cell) + key_size + column_props[condition.column_name].offset))) {
	// 			continue;
	// 		}
	// 		break;
	// 	}

	// 	if (column_names.empty()) {
	// 		for (sql_types::Column column : statement.create_columns) {
	// 			switch (column.type_id) {
	// 			case sql_types::Id::INT: {
	// 				sql_types::Int value = *(sql_types::Int *)(((std::uint8_t *)cell) + key_size + column_props[column.name].offset);
	// 				if () {

	// 				}
	// 			}
	// 				break;
	// 			case sql_types::Id::STRING: {
	// 				std::string_view value((char *)(((std::uint8_t *)cell) + key_size + column_props[column.name].offset), sql_types::max_string_size);
	// 				if (value.size() > column_props[column.name].print_width) {
	// 					column_props[column.name].print_width = value.size();
	// 				}
	// 			}
	// 				break;
	// 			case sql_types::Id::DATE: {
	// 				sql_types::Date value = *(sql_types::Date *)(((std::uint8_t *)cell) + key_size + column_props[column.name].offset);
	// 				if () {

	// 				}
	// 			}
	// 				break;
	// 			}
	// 		}

	// 	} else {
	// 		for (std::string_view column_name : column_names) {
	// 			switch (column_props[column_name].type_id) {
	// 			case sql_types::Id::INT:
	// 				std::cout << "| " << std::setw(column_name.size())
	// 							<< *(sql_types::Int *)(((std::uint8_t *)cell) + key_size + column_props[column_name].offset)
	// 							<< ' ';
	// 				break;
	// 			case sql_types::Id::STRING: {
	// 				std::cout << "| \"" << std::setw(column_name.size() - 2)
	// 							<< std::string_view((char *)(((std::uint8_t *)cell) + key_size + column_props[column_name].offset), sql_types::max_string_size)
	// 							<< "\" ";
	// 			}
	// 				break;
	// 			case sql_types::Id::DATE:
	// 				std::cout << "| " << std::setw(column_name.size())
	// 							<< *(sql_types::Date *)(((std::uint8_t *)cell) + key_size + column_props[column_name].offset)
	// 							<< ' ';
	// 				break;
	// 			}
	// 		}
	// 	}

	// 	std::cout << "|\n";
	// 	rows_selected++;
	// }


	std::size_t chars_printed = 0;
	if (column_names.empty()) {
		for (sql_types::Column column : statement.create_columns) {
			std::cout << "| " << column.name << ' ';
			chars_printed += (2 + column.name.size() + 1);
		}
	} else {
		for (std::string_view column_name : column_names) {
			std::cout << "| " << column_name << ' ';
			chars_printed += (2 + column_name.size() + 1);
		}
	}
	std::cout << '|';
	chars_printed += 1;
	std::cout << '\n' << std::string(chars_printed, '-') << '\n';


	// full table scan for now
	// an index B+ Tree is needed for faster search
	for (void *cell : table_BplusTree) {

		// if there is no condition (i.e. condition.column_name is empty) the
		// condition column value gets constructed in a bit of a sketchy way
		switch (column_props[condition.column_name].type_id) {
		case sql_types::Id::INT:
			if (!condition.op(*(sql_types::Int *)(((std::uint8_t *)cell) + key_size + column_props[condition.column_name].offset))) {
				continue;
			}
			break;
		case sql_types::Id::STRING: {
			std::string_view value((char *)(((std::uint8_t *)cell) + key_size + column_props[condition.column_name].offset), sql_types::max_string_size);
			value.remove_suffix(value.size() - value.find('\0'));
			if (!condition.op(value)) {
				continue;
			}
		}
			break;
		case sql_types::Id::DATE:
			if (condition.op(*(sql_types::Date *)(((std::uint8_t *)cell) + key_size + column_props[condition.column_name].offset))) {
				continue;
			}
			break;
		}

		if (column_names.empty()) {
			for (sql_types::Column column : statement.create_columns) {
				switch (column.type_id) {
				case sql_types::Id::INT:
					std::cout << "| " << std::setw(column.name.size())
								<< *(sql_types::Int *)(((std::uint8_t *)cell) + key_size + column_props[column.name].offset)
								<< ' ';
					break;
				case sql_types::Id::STRING: {
					std::cout << "| \"" << std::setw(column.name.size() - 2)
								<< std::string_view((char *)(((std::uint8_t *)cell) + key_size + column_props[column.name].offset), sql_types::max_string_size)
								<< "\" ";
				}
					break;
				case sql_types::Id::DATE:
					std::cout << "| " << std::setw(column.name.size())
								<< *(sql_types::Date *)(((std::uint8_t *)cell) + key_size + column_props[column.name].offset)
								<< ' ';
					break;
				}
			}

		} else {
			for (std::string_view column_name : column_names) {
				switch (column_props[column_name].type_id) {
				case sql_types::Id::INT:
					std::cout << "| " << std::setw(column_name.size())
								<< *(sql_types::Int *)(((std::uint8_t *)cell) + key_size + column_props[column_name].offset)
								<< ' ';
					break;
				case sql_types::Id::STRING: {
					std::cout << "| \"" << std::setw(column_name.size() - 2)
								<< std::string_view((char *)(((std::uint8_t *)cell) + key_size + column_props[column_name].offset), sql_types::max_string_size)
								<< "\" ";
				}
					break;
				case sql_types::Id::DATE:
					std::cout << "| " << std::setw(column_name.size())
								<< *(sql_types::Date *)(((std::uint8_t *)cell) + key_size + column_props[column_name].offset)
								<< ' ';
					break;
				}
			}
		}

		std::cout << "|\n";
		rows_selected++;
	}

	std::cout << "Total " << rows_selected << " rows selected\n";
}

} // namespace fmisql
