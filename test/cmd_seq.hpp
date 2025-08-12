#ifndef TEST_CMD_SEQ_HPP
#define TEST_CMD_SEQ_HPP

#include "util.hpp"

#include <string_view>
#include <vector>


namespace fmisql::test {

/**
 * @brief Tests whether the given sequence of commands works.
 *        Default condition is SHOULD_PASS.
 */
void test_command_sequence(std::vector<std::string_view> lines,
                           Condition condition = Condition::SHOULD_PASS);

/**
 * @brief Executes `setup_lines`, `repeat_line` `n` times and `end_lines` once.
 *        Default condition is SHOULD_PASS.
 */
void test_command_setup_repeat_end(
    std::vector<std::string_view> setup_lines,
    std::string_view repeat_line,
    std::size_t n,
    std::vector<std::string_view> end_lines,
    Condition condition = Condition::SHOULD_PASS
);
	
/**
 * @brief Tests a lot of possible commands sequences
 */	
void test_command_sequences();

} // namespace fmisql::test

#endif // TEST_CMD_SEQ_HPP
