#include "cli.hpp"

#include "parser.hpp"

#include <iostream>
#include <string>
#include <vector>


namespace fmisql
{

// const int line_length = 100;

void cli() {
	std::string line;
	// line.reserve(line_length);

	while (true) {

		std::cout << "FMISql> ";
		std::getline(std::cin, line);

		if (line == "Quit" || std::cin.eof()) {
			break;
		} else if (!line.empty()) {
			parse_line(line);
		}
	}

	std::cout << (std::cin.eof() ? "\n" : "") << "Goodbye, master ;(\n";


	// Alternative:
	// while (true) {

	// 	char line[line_length];
	// 	std::cout << "FMISql> ";
	// 	std::cin >> line;
	// 	// std::cout << '!' << line << '!' << '\n';

	// 	if (std::strcmp(line, "Quit") == 0 || std::cin.eof()) {
	// 		std::cout << "Goodbye, master ;(\n";
	// 		break;
	// 	} else {
	// 		// TODO
	// 		std::cout << line << ": unknown command\n";
	// 	}
	// }
}

} // namespace fmisql
