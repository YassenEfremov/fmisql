#include "cli.hpp"

#include "parser.hpp"
#include "../commands/create_table.hpp"
#include "../commands/list_tables.hpp"
#include "../commands/table_info.hpp"
#include "../commands/insert.hpp"
#include "../commands/select.hpp"
#include "../statement.hpp"

#include <iostream>
#include <string>


namespace fmisql {

void cli() {
	std::string line;

	while (true) {

		std::cout << "FMISql> ";
		std::getline(std::cin, line);

		if (line == "Quit" || std::cin.eof()) {
			break;
		} else if (!line.empty()) {
			Statement statement = parse_line(line);

			switch (statement.type) {
			case Statement::Type::CREATE_TABLE:
				create_table(statement.table_name, statement.create_columns,
					line);
				break;

			case Statement::Type::DROP_TABLE:
				// TODO
				break;

			case Statement::Type::LIST_TABLES:
				list_tables();
				break;

			case Statement::Type::TABLE_INFO:
				table_info(statement.table_name);
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
