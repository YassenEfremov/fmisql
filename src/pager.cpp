#include "pager.hpp"

#include <cstdint>

#include <fstream>


namespace fmisql {

std::fstream Pager::db_file;

int Pager::page_count;
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

		std::cout << "writing page\n";
		Pager::flush(i);
		delete[] (std::uint8_t *)Pager::pages[i];
		Pager::pages[i] = nullptr;
	}

	// int additional_rows_count = Pager::rows_count % rows_per_page;
	// if (additional_rows_count > 0) {
	// 	if (Pager::pages[full_page_count] != nullptr) {
	// 		Pager::flush(full_page_count, additional_rows_count * row_size);
	// 		delete[] (std::uint8_t *)Pager::pages[full_page_count];
	// 		Pager::pages[full_page_count] = nullptr;
	// 	}
	// }

	Pager::db_file.close();
	// for (int i = 0; Pager::pages[i]; i++) {
	// 	delete[] (std::uint8_t *)Pager::pages[i];
	// }
}

} // namespace fmisql
