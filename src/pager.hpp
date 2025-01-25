#ifndef PAGER_HPP
#define PAGER_HPP

#include "constants.hpp"
#include "data_types.hpp"

#include <cstdint>
#include <cstring>

#include <filesystem>
#include <fstream>
#include <iostream>


namespace fs = std::filesystem;

namespace fmisql {


class Pager {
public:
	static std::fstream db_file;
	static std::uintmax_t file_size;
	static int page_count;
	static void *pages[table_max_pages];

	static void init() {
		db_file.open(db_filename, std::ios::binary | std::ios::in | std::ios::out);
		if (!db_file.is_open()) {
			db_file.clear();
			db_file.open(db_filename, std::ios::binary | std::ios::out);
			db_file.close();
			db_file.open(db_filename, std::ios::binary | std::ios::in | std::ios::out);
		}

		file_size = fs::file_size(db_filename);

		if (file_size % page_size) {
			throw std::runtime_error(
				"Database file doesn't contain an exact number of pages\n"
			);
		}

		page_count = file_size / page_size;

		for (int i = 0; i < table_max_pages; i++) {
			pages[i] = nullptr;
		}
	}

	static void *row_slot(int row_number) {
		int page_number = row_number / rows_per_page;
		// void *page = pages[page_number];
		// if (page == nullptr) {
		// 	page = pages[page_number] = new std::uint8_t[page_size];
		// }
		void *page = get_page(page_number);
		// int row_offset = (row_number % rows_per_page) * row_size;
		int row_offset = LEAF_NODE_HEADER_SIZE
			+ row_number * (LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE)
			+ LEAF_NODE_KEY_SIZE;
		return (std::uint8_t *)page + row_offset;
	}

	static void *get_page(int page_number) {
		if (page_number > table_max_pages) {
			throw std::runtime_error("Page number out of bounds\n");
		}

		if (pages[page_number] == nullptr) {
			// miss
			void *new_page = new std::uint8_t[page_size];

			// if (file_size % page_size) {
			// 	// partial page
			// 	page_count++;
			// }

			if (page_number < page_count) {
				std::cout << "seeking to " << page_number * page_size << '\n';
				db_file.seekg(page_number * page_size);
				db_file.read((char *)new_page, page_size);
				if (db_file.fail()) std::cerr << "fail?\n";
			}

			pages[page_number] = new_page;

			if (page_number >= page_count) {
				page_count = page_number + 1;
			}
		}

		return pages[page_number];
	}

	static void flush(int page_number) {
		if (pages[page_number] == nullptr) {
			std::cerr << "cannot flush null page\n";
			return;
		}

		db_file.seekp(page_number * page_size);
		db_file.write((char *)pages[page_number], page_size);
	}
};

/**
 * @brief Main function that initializes the database
 */
void db_init();

/**
 * @brief Main function that deinitializes the database
 */
void db_deinit();

} // namespace fmisql


#endif // PAGER_HPP
