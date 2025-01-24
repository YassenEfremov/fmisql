#ifndef PAGER_HPP
#define PAGER_HPP

#include <cstdint>
#include <cstring>

#include <filesystem>
#include <fstream>
#include <iostream>


namespace fs = std::filesystem;

namespace fmisql {

constexpr const char *db_filename = "fmisql.db";

constexpr int page_size = 4096;
constexpr int table_max_pages = 100;
constexpr int row_size = 4 + 256 + 4;

constexpr int rows_per_page = page_size / row_size;


// temporary row structure
struct ExampleRow {
    std::uint32_t ID;
    char Name[256];
    std::uint32_t Value;

	void serialize(void *dst) {
		std::memcpy((std::uint8_t *)dst, &this->ID, sizeof ID);
		std::memcpy((std::uint8_t *)dst + sizeof ID, &this->Name, sizeof Name);
		std::memcpy((std::uint8_t *)dst + sizeof ID + sizeof Name, &this->Value, sizeof Value);
	}

	void deserialize(void *src) {
		std::memcpy(&this->ID, (std::uint8_t *)src, sizeof ID);
		std::memcpy(&this->Name, (std::uint8_t *)src + sizeof ID, sizeof Name);
		std::memcpy(&this->Value, (std::uint8_t *)src + sizeof ID + sizeof Name, sizeof Value);
	}
};



class Pager {
public:
	static std::fstream db_file;
	static std::uintmax_t file_size;
	static int rows_count;
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

		rows_count = file_size / row_size;

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
		int row_offset = (row_number % rows_per_page) * row_size;
		return (std::uint8_t *)page + row_offset;
	}

	static void *get_page(int page_number) {
		if (page_number > table_max_pages) {
			throw std::runtime_error("Page number out of bounds\n");
		}

		if (pages[page_number] == nullptr) {
			// miss
			void *new_page = new std::uint8_t[page_size];
			int pages_count = file_size / page_size;

			if (file_size % page_size) {
				// partial page
				pages_count++;
			}

			if (page_number < pages_count) {
				std::cout << "seeking to " << page_number * page_size << '\n';
				db_file.seekg(page_number * page_size);
				db_file.read((char *)new_page, page_size);
				if (db_file.fail()) std::cerr << "fail?\n";
			}

			pages[page_number] = new_page;
		}

		return pages[page_number];
	}

	static void flush(int page_number, int size) {
		if (pages[page_number] == nullptr) {
			std::cerr << "cannot flush null page\n";
			return;
		}

		std::cout << "writing " << size << " chars\n";
		db_file.seekp(page_number * page_size);
		db_file.write((char *)pages[page_number], size);
	}
};

/**
 * @brief Main function that starts the database
 */
void db_init();

void db_deinit();

} // namespace fmisql


#endif // PAGER_HPP
