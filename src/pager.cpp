#include "pager.hpp"

#include <cstdint>

#include <fstream>


namespace fmisql {

std::fstream Pager::db_file;

int Pager::page_count = 0;
std::uintmax_t Pager::file_size;
void *Pager::pages[table_max_pages];


void db_init() {
	Pager::init();
}

void db_deinit() {

	for (int i = 0; i < Pager::page_count; i++) {
		if (Pager::pages[i] == nullptr) {
			continue;
		}

		// TODO: don't write unchanged pages!

		// std::cout << "debug: writing page\n";
		Pager::flush(i);
		delete[] (std::uint8_t *)Pager::pages[i];
		Pager::pages[i] = nullptr;
	}

	Pager::db_file.close();
	// for (int i = 0; Pager::pages[i]; i++) {
	// 	delete[] (std::uint8_t *)Pager::pages[i];
	// }
}

} // namespace fmisql
