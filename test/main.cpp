#include "parser.hpp"
#include "cmd_seq.hpp"
#include "util.hpp"

#include <iostream>


int main() {

	std::cout << "--------------------------------------------------------------------------------\n";
	fmisql::test::test_parser();
	std::cout << "--------------------------------------------------------------------------------\n";
	fmisql::test::test_command_sequences();
	std::cout << "================================================================================\n";

	std::cout << "Results:\n"
		<< fmisql::test::total << " total, "
		<< fmisql::test::passed << GREEN " passed, " RESET
		<< fmisql::test::failed << RED " failed" RESET "\n";

	return 0;
}
