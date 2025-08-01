#ifndef TEST_UTIL_HPP
#define TEST_UTIL_HPP

#define GREEN "\x1b[32m"
#define RED "\x1b[31m"
#define CYAN "\x1b[36m"
#define RESET "\x1b[0m"


namespace fmisql::test {

enum class Condition {
	SHOULD_PASS,
	SHOULD_FAIL
};

extern int total;
extern int passed;
extern int failed;

} // namespace fmisql::test


#endif // TEST_UTIL_HPP
