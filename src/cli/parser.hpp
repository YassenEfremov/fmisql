#ifndef PARSER_HPP
#define PARSER_HPP

#include "../data_types.hpp"
#include "../statement.hpp"

#include <string_view>
#include <vector>

#include <cstddef>


// struct Token {


// }
// enum Token {
// 	// commands
// 	CREATE_TABLE,
// 	DROP_TABLE,
// 	LIST_TABLES,
// 	TABLE_INFO,
// 	SELECT,
// 	REMOVE,
// 	INSERT,

// 	// select specific
// 	WHERE,
// 	NOT,
// 	AND,
// 	OR,
// 	ORDER_BY,
// 	JOIN,
// 	DISTINCT,


// };

namespace fmisql {

/**
 * @brief Removes leading and trailing spaces from the given string
 */
void trim(std::string_view &str);

/**
 * @brief Parse the given command line
 */
Statement parse_line(std::string_view line);

} // namespace fmisql


#endif // PARSER_HPP
