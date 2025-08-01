#include "parser.hpp"
#include "cmd_seq.hpp"
#include "util.hpp"

#include <iostream>


int main() {

	fmisql::test::test_parser();
	fmisql::test::test_full();

	std::cout << "Results:\n"
		<< fmisql::test::total << " total, "
		<< fmisql::test::passed << GREEN " passed, " RESET
		<< fmisql::test::failed << RED " failed" RESET;

	return 0;
}
