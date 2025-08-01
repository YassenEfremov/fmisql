#include "parser.hpp"
#include "cmd_seq.hpp"
#include "util.hpp"

#include <iostream>


int main() {

	std::cout << "--------------------------------------------------------------------------------\n";
	fmisql::test::test_parser();
	std::cout << "--------------------------------------------------------------------------------\n";
	fmisql::test::test_full();
	std::cout << "================================================================================\n";

	std::cout << "Results:\n"
		<< fmisql::test::total << " total, "
		<< fmisql::test::passed << GREEN " passed, " RESET
		<< fmisql::test::failed << RED " failed" RESET;

	return 0;
}
