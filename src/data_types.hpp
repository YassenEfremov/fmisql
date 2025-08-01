#ifndef DATA_TYPES_HPP
#define DATA_TYPES_HPP

#include <cstdint>
#include <cstring>
#include <string_view>
#include <variant>
#include <ctime>


namespace fmisql::sql_types {

enum class Id {
	INT,
	STRING,
	DATE
};

using Int = int;
using String = std::string_view;
using Date = std::time_t;

constexpr std::size_t max_string_size = 256;
constexpr std::size_t max_int_size = sizeof Int;
constexpr std::size_t max_date_size = sizeof Date;

using Value = std::variant<Int, String, Date>;

struct Column { std::string_view name; sql_types::Id type_id; };

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
