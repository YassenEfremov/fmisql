#ifndef PAGER_HPP
#define PAGER_HPP

#include <cstdint>

#include <fstream>
#include <vector>


namespace fmisql {

class Pager {
public:
	static std::fstream db_file;
	static std::uintmax_t file_size;
	static std::vector<void *> pages;
	static int file_page_count;

	static void init();

	static int create_page();

	static void *get_page(int page_number);

	static int get_unused_page_number();

	static void flush(int page_number);

	static void deinit();

};

} // namespace fmisql


#endif // PAGER_HPP
