#ifndef PAGER_HPP
#define PAGER_HPP

#include <cstdint>
#include <cstring>


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


constexpr int page_size = 4096;
constexpr int table_max_pages = 100;
constexpr int row_size = 4 + 256 + 4;

constexpr int rows_per_page = page_size / row_size;


class Pager {
public:
	static int rows_count;
	static void *pages[table_max_pages];

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


#endif // PAGER_HPP
