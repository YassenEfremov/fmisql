#include "parser.hpp"

#include <cstring>

#include <sstream>
#include <string>
#include <vector>


std::vector<std::string> parse(std::string &line) {

    std::vector<std::string> tokens;

	std::string token;
	std::istringstream line_stream(line);
	while (line_stream >> token) {
		tokens.push_back(token);
	}

	return tokens;
}
