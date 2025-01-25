#ifndef SCHEMA_HPP
#define SCHEMA_HPP

#include "data_types.hpp"
#include "pager.hpp"

#include <vector>
#include <string>
#include <string_view>
#include <iostream>

#include <cstdint>
#include <cstring>


namespace fmisql {

constexpr std::size_t max_table_name_size = 256;


struct SchemaRow {
	sql_types::String table_name;
	sql_types::Int page;
	sql_types::String sql;

	SchemaRow(std::string_view table_name, int page, std::string_view sql)
		: page (page) {
		
		table_name.copy(this->table_name, sizeof this->table_name, 0);
		sql.copy(this->sql, sizeof this->sql, 0);
	}

	void serialize(void *dst) const {
		std::memcpy(dst,
		            &this->table_name,
		            sizeof this->table_name);
		std::memcpy((std::uint8_t *)dst + sizeof this->table_name,
		            &this->page,
		            sizeof this->page);
		std::memcpy((std::uint8_t *)dst + sizeof this->table_name + sizeof this->page,
		            &this->sql,
		            sizeof this->sql);
	}

	void deserialize(void *src) {
		std::memcpy(&this->table_name,
		            src,
		            sizeof this->table_name);
		std::memcpy(&this->page,
		            (std::uint8_t *)src + sizeof this->table_name,
		            sizeof this->page);
		std::memcpy(&this->sql,
		            (std::uint8_t *)src + sizeof this->table_name + sizeof this->page,
					sizeof this->sql);
	}
};


// constexpr int page_size = 4096;
// constexpr int table_max_pages = 100;
// constexpr int row_size = 4 + 256 + 4;

// constexpr int rows_per_page = page_size / row_size;


class Schema {
public:
	// static std::uint32_t table_count;

	// static int rows_count;
	static void *pages[table_max_pages];

	// static void add(std::string_view table_name) {
	// 	SchemaRow row;
	// 	table_name.copy(row.table_name, max_table_name_size);
	// 	row.serialize(Schema::row_slot(Schema::rows_count));
	// 	rows_count++;
	// }

	// static void remove(std::string_view table_name) {

	// }

	// static void list() {
	// 	SchemaRow row;
	// 	for (int i = 0; i < Schema::rows_count; i++) {
	// 		row.deserialize(Schema::row_slot(i));
	// 		std::cout << "    " << row.table_name << '\n';
	// 	}
	// }

	// static void *row_slot(int row_number) {
	// 	int page_number = row_number / rows_per_page;
	// 	void *page = pages[page_number];
	// 	if (page == nullptr) {
	// 		page = pages[page_number] = new std::uint8_t[page_size];
	// 	}
	// 	int row_offset = row_number % rows_per_page;
	// 	int byte_offset = row_offset * schema_row_size;
	// 	return (std::uint8_t *)page + byte_offset;
	// }

	// static void free() {
	// 	for (int i = 0; pages[i]; i++) {
	// 		delete[] (std::uint8_t *)pages[i];
	// 	}
	// }
};

} // namespace fmisql


#endif // SCHEMA_HPP
