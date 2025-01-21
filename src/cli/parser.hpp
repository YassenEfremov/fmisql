#ifndef PARSER_HPP
#define PARSER_HPP

#include "../data_types.hpp"

#include <string_view>
#include <vector>
#include <unordered_map>


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


std::unordered_map<std::string_view, DataType>
parse_columns(std::string_view columns_str);

std::vector<std::string_view> parse_inserts(std::string_view columns_str);

/**
 * @brief Parse the given command line
 */
void parse_line(std::string_view line);


#endif // PARSER_HPP
