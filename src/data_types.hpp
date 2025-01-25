#ifndef DATA_TYPES_HPP
#define DATA_TYPES_HPP

#include <cstdint>
#include <cstring>


namespace fmisql::sql_types {

enum class Id {
	INT,
	DATE,
	STRING
};

using Int = int;
using String = char[256];
using Date = int;

// temporary row structure
struct ExampleRow {
    sql_types::Int ID;
    sql_types::String Name;
    sql_types::Int Value;

	void serialize(void *dst) const {
		std::memcpy(dst,
		            &this->ID,
		            sizeof this->ID);
		std::memcpy((std::uint8_t *)dst + sizeof this->ID,
		            &this->Name,
		            sizeof this->Name);
		std::memcpy((std::uint8_t *)dst + sizeof this->ID + sizeof this->Name,
		            &this->Value,
		            sizeof this->Value);
	}

	void deserialize(void *src) {
		std::memcpy(&this->ID, src, sizeof ID);
		std::memcpy(&this->Name, (std::uint8_t *)src + sizeof ID, sizeof Name);
		std::memcpy(&this->Value, (std::uint8_t *)src + sizeof ID + sizeof Name, sizeof Value);
	}
};

} // namespace fmisql


#endif // DATA_TYPES_HPP
