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

using Int = std::uint32_t;
using String = std::string_view;
using Date = std::time_t;

constexpr std::size_t max_string_size = 256;
constexpr std::size_t max_int_size = sizeof(Int);
constexpr std::size_t max_date_size = sizeof(Date);

using Value = std::variant<Int, String, Date>;

struct Column { std::string_view name; sql_types::Id type_id; };

} // namespace fmisql


#endif // DATA_TYPES_HPP
