#ifndef DATA_TYPES_HPP
#define DATA_TYPES_HPP

#include <cstdint>
#include <cstring>
#include <variant>


namespace fmisql::sql_types {

enum class Id {
	INT,
	DATE,
	STRING
};

using Int = int;
using String = char[256];
using Date = long;	// for now just so that it's different from Int

using Value = std::variant<Int, Date>; // strings need to be reworked

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
		std::memcpy(&this->ID, src, sizeof this->ID);
		std::memcpy(&this->Name, (std::uint8_t *)src + sizeof this->ID, sizeof this->Name);
		std::memcpy(&this->Value, (std::uint8_t *)src + sizeof this->ID + sizeof this->Name, sizeof this->Value);
	}
};

} // namespace fmisql


#endif // DATA_TYPES_HPP
