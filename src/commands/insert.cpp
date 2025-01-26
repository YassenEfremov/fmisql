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
// Insert INTO Sample {(1, "Test message", 120)}
// Insert INTO Sample {(50, "asd", 12), (60, "asd", 12), (51, "asd", 12), (53, "asd", 12), (52, "asd", 12)}

void insert(std::string_view table_name, const std::vector<sql_types::ExampleRow> rows) {

	try {
		LeafNode node(LeafNode::number_of_table(table_name));
		std::cout << "debug: table value size: " << *node.get_value_size() << '\n';
		std::uint32_t pair_count = *node.get_pair_count();

		std::cout << "debug: starting from: " << pair_count << '\n';
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
