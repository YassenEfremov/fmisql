#ifndef TEST_PARSER_HPP
#define TEST_PARSER_HPP

#include "util.hpp"

#include <string_view>


namespace fmisql::test {

extern void test_command(std::string_view line, Condition condition = Condition::SHOULD_PASS);

extern void test_parser();

} // namespace fmisql::test


#endif // TEST_PARSER_HPP
