#ifndef PARSER_HPP
#define PARSER_HPP

#include "../classes.hpp"
#include "../data_types.hpp"
#include "../statement.hpp"

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

std::vector<Column> parse_create_columns(std::string_view columns_str);

std::vector<sql_types::ExampleRow> parse_inserts(std::string_view columns_str);

std::vector<std::string_view> parse_select_columns(std::string_view columns_str);

void parse_conds(std::string_view columns_str);

/**
 * @brief Parse the given command line
 */
Statement parse_line(std::string_view line);

} // namespace fmisql


#endif // PARSER_HPP
