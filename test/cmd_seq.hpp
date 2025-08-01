#ifndef TEST_CMD_SEQ_HPP
#define TEST_CMD_SEQ_HPP

#include "util.hpp"

#include <string_view>
#include <vector>


namespace fmisql::test {

extern void test_command_sequence(std::vector<std::string_view> lines, Condition condition = Condition::SHOULD_PASS);

extern void test_full();

} // namespace fmisql::test

#endif // TEST_CMD_SEQ_HPP
