#include "interp.hpp"

#include <iostream>
#include <string>
#include <vector>


void interpret(std::vector<std::string> tokens) {

	std::string command = tokens[0];

	if (command == "CreateTable") {

		if (tokens.size() < 3) {
			std::cout << "not enough arguments given\n";
		} else {
			// TODO
		}

	} else if (command == "DropTable") {

		if (tokens.size() < 2) {
			std::cout << "not enough arguments given\n";
		} else {
			// TODO
		}

	} else if (command == "ListTables") {

		if (tokens.size() != 1) {
			std::cout << "too many arguments given\n";
		} else {
			// TODO
		}

	} else if (command == "TableInfo") {

		if (tokens.size() < 2) {
			std::cout << "not enough arguments given\n";
		} else {
			// TODO
		}

	} else if (command == "Select") {

	} else if (command == "Remove") {

	} else if (command == "Insert") {

	} else {
		std::cout << command << ": unknown command\n";
	}
}
