#ifndef TEST_PARSER_HPP
#define TEST_PARSER_HPP

#include "util.hpp"

#include <string_view>


namespace fmisql::test {

/**
 * @brief Tests whether the syntax of the given command is correct.
 *        Default condition is SHOULD_PASS.
 */
void test_command(std::string_view line, Condition condition = Condition::SHOULD_PASS);

/**
 * @brief Tests the entire syntax of our custom SQL-like language
 */
void test_parser();

} // namespace fmisql::test


#endif // TEST_PARSER_HPP
