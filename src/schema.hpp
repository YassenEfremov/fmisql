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
	sql_types::String name;
	sql_types::Int page;

	void serialize(void *dst) {
		std::memcpy((std::uint8_t *)dst, &this->name, sizeof name);
	}

	void deserialize(void *src) {
		std::memcpy(&this->name, (std::uint8_t *)src, sizeof name);
	}
};


// constexpr int page_size = 4096;
// constexpr int table_max_pages = 100;
// constexpr int row_size = 4 + 256 + 4;

// constexpr int rows_per_page = page_size / row_size;


class Schema {
public:
	static int rows_count;
	static void *pages[table_max_pages];

	static void add(std::string_view name) {
		SchemaRow row;
		name.copy(row.name, max_table_name_size);
		row.serialize(Schema::row_slot(Schema::rows_count));
		rows_count++;
	}

	static void remove(std::string_view name) {

	}

	static void list() {
		SchemaRow row;
		for (int i = 0; i < Schema::rows_count; i++) {
			row.deserialize(Schema::row_slot(i));
			std::cout << "    " << row.name << '\n';
		}
	}

	static void *row_slot(int row_number) {
		int page_number = row_number / rows_per_page;
		void *page = pages[page_number];
		if (page == nullptr) {
			page = pages[page_number] = new std::uint8_t[page_size];
		}
		int row_offset = row_number % rows_per_page;
		int byte_offset = row_offset * row_size;
		return (std::uint8_t *)page + byte_offset;
	}

	static void free() {
		for (int i = 0; pages[i]; i++) {
			delete[] (std::uint8_t *)pages[i];
		}
	}
};

} // namespace fmisql


#endif // SCHEMA_HPP
