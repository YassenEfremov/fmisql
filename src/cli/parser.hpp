#ifndef PARSER_HPP
#define PARSER_HPP

#include "../data_types.hpp"

#include <string>
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


// /**
//  * @brief Parse the given command line
//  */
// std::vector<std::string> parse_line(std::string &line);

/**
 * @brief Parse the given table columns
 */
std::unordered_map<std::string, DataType> parse_columns(std::string &columns);

/**
 * @brief Parse the given table inserts
 */
std::vector<std::string> parse_inserts(std::string &inserts);

/**
 * @brief Interpret the command
 */
void interpret(std::string_view line);


#endif // PARSER_HPP
