#include "cli.hpp"

#include "parser.hpp"
#include "../commands/insert.hpp"
#include "../commands/select.hpp"
#include "../statement.hpp"
#include "../schema.hpp"

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
			Statement statement = parse_line(line);

			switch (statement.type) {
			case Statement::Type::CREATE_TABLE:
				// for (auto tc : table_columns) {
				// 	std::cout << tc.name << " - ";
				// 	switch (tc.type) {
				// 		case DataType::INT: std::cout << "INT\n"; break;
				// 		case DataType::STRING: std::cout << "STRING\n"; break;
				// 		case DataType::DATE: std::cout << "DATE\n"; break;
				// 	}
				// }

				// create_table(table_name, table_columns);
				// Table table(table_columns);

				Schema::add(statement.table_name);
				std::cout << "Table " << statement.table_name << " created!\n";
				break;

			case Statement::Type::DROP_TABLE:
				// TODO
				break;

			case Statement::Type::LIST_TABLES:
				std::cout << "There are " << Schema::rows_count << " table/s in the database:\n";
				Schema::list();
				break;

			case Statement::Type::TABLE_INFO:
				// TODO
				break;

			case Statement::Type::SELECT:
				select(statement.select_columns, statement.table_name);
				break;

			case Statement::Type::REMOVE:
				// TODO
				break;

			case Statement::Type::INSERT:
				insert(statement.table_name, statement.insert_rows);
				break;

			default:
				break;
			}
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
