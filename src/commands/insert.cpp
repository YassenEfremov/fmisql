#include "insert.hpp"

#include "../btree.hpp"
#include "../constants.hpp"
#include "../data_types.hpp"

#include <cstring>

#include <iostream>
#include <string_view>
#include <stdexcept>
#include <vector>


namespace fmisql {

// Example inset statements:
// Insert INTO Sample {(15, "Test message", 120)}
// Insert INTO Sample {(16, "farlands", 99)}
// Insert INTO Sample {(1, "asd", 12), (2, "asd", 12), (3, "asd", 12), (4, "asd", 12), (5, "ddd", 5)}
// Insert INTO Sample {(6, "asd", 12), (7, "asd", 12), (8, "asd", 12), (9, "asd", 12), (10, "ddd", 5)}
// Insert INTO Sample {(11, "asd", 12), (12, "asd", 12), (13, "asd", 12), (14, "asd", 12), (15, "ddd", 5)}

void insert(std::string_view table_name, const std::vector<sql_types::ExampleRow> rows) {

	try {
		Node node(Node::number_of_table(table_name));
		// std::cout << "debug: table value size: " << *node.value_size() << '\n';
		std::uint32_t pair_count = *node.pair_count();

		// std::cout << "debug: starting from: " << pair_count << '\n';
		for (int i = 0; i < rows.size(); i++) {

			// std::cout << "should insert row with id: " << rows[i].ID << '\n';
			std::uint8_t buf[example_row_size];
			rows[i].serialize(buf);
			node.insert(rows[i].ID, buf);
		}

	} catch (const std::runtime_error &e) {
		std::cout << e.what() << '\n';
	}
}

} // namespace fmisql
