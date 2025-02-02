#ifndef PARSER_HPP
#define PARSER_HPP

#include "../classes.hpp"
#include "../data_types.hpp"
#include "../statement.hpp"

#include <cstddef>

#include <string_view>
#include <vector>


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
 * @brief Parse the given command line
 */
Statement parse_line(std::string_view line);

} // namespace fmisql


#endif // PARSER_HPP
