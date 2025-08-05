#ifndef TEST_CMD_SEQ_HPP
#define TEST_CMD_SEQ_HPP

#include "util.hpp"

#include <string_view>
#include <vector>


namespace fmisql::test {

extern void test_command_sequence(std::vector<std::string_view> lines, Condition condition = Condition::SHOULD_PASS);

void test_command_setup_and_repeat(std::vector<std::string_view> setup_lines,
                                   std::string_view repeat_line,
                                   std::size_t n,
	                               Condition condition = Condition::SHOULD_PASS);

extern void test_command_sequences();

} // namespace fmisql::test

#endif // TEST_CMD_SEQ_HPP
