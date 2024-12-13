#include <iostream>
#include <cstring>
#include <string>


const int line_length = 100;


int main() {

	std::string line;
	// line.reserve(line_length);

	while (true) {

		std::cout << "FMISql> ";
		std::getline(std::cin, line);

		if (line == "Quit" || std::cin.eof()) {
			break;
		} else if (!line.empty()) {
			// TODO
			std::cout << line << ": unknown command\n";
		}
	}

	std::cout << (std::cin.eof() ? "\n" : "") << "Goodbye, master ;(\n";


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

	return 0;
}
