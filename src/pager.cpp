#include "pager.hpp"

#include "constants.hpp"

#include <cstdint>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


namespace fs = std::filesystem;

namespace fmisql {

std::fstream Pager::db_file;

int Pager::file_page_count = 0;
std::uintmax_t Pager::file_size;
std::vector<void *> Pager::pages{};


void Pager::init() {
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

	file_page_count = file_size / page_size;
	pages.resize(file_page_count);
}

int Pager::create_page() {
	int page_number = get_unused_page_number();
	pages.push_back(new std::uint8_t[page_size]);
	return page_number;
}

void *Pager::get_page(int page_number) {
	if (page_number >= file_page_count) {
		throw std::runtime_error("page number " + std::to_string(page_number) + " doesn't exist");
	}

	if (pages[page_number] == nullptr) {
		// cache miss
		void *new_page = new std::uint8_t[page_size];

		// std::cout << "debug: seeking to " << page_number * page_size << '\n';
		db_file.seekg(page_number * page_size);
		db_file.read((char *)new_page, page_size);
		if (db_file.fail())
			std::cerr << "error while reading database file\n";

		pages[page_number] = new_page;

		// if (page_number >= file_page_count) {
		// 	file_page_count++;
		// }
	}

	return pages[page_number];
}

int Pager::get_unused_page_number() {
	int page_number = file_page_count;
	file_page_count++;
	return page_number;
}

void Pager::flush(int page_number) {
	if (pages[page_number] == nullptr) {
		std::cerr << "cannot flush null page\n";
		return;
	}

	db_file.seekp(page_number * page_size);
	db_file.write((char *)pages[page_number], page_size);
}

void Pager::deinit() {
	for (int i = 0; i < pages.size(); i++) {
		if (Pager::pages[i] != nullptr) {
			// TODO: don't write unchanged pages!
	
			// std::cout << "debug: writing page\n";
			Pager::flush(i);
			delete[] (std::uint8_t *)Pager::pages[i];
			Pager::pages[i] = nullptr;
		}
	}

	Pager::db_file.close();
}

} // namespace fmisql
