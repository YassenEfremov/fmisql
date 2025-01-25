#include "insert.hpp"

#include "../btree.hpp"
#include "../constants.hpp"
#include "../data_types.hpp"

#include <cstring>

#include <string_view>
#include <stdexcept>
#include <vector>


namespace fmisql {

// Example inset statements:
// Insert INTO Sample {(1, "Test message", 120)}
// Insert INTO Sample {(50, "asd", 12), (60, "asd", 12), (51, "asd", 12), (53, "asd", 12), (52, "asd", 12)}

void insert(std::string_view table_name, const std::vector<sql_types::ExampleRow> rows) {

	LeafNode node;
	std::uint32_t pair_count = *node.get_pair_count();

	std::cout << "starting from: " << pair_count << '\n';
	for (int i = 0; i < rows.size(); i++) {

		if (pair_count >= LEAF_NODE_MAX_CELLS) {
			// TODO
			throw std::logic_error("splitting not implemented yet");
		}

		try {
			// node.insert(rows[i].ID, rows[i]);
		} catch (const std::runtime_error &e) {
			std::cout << "error: " << e.what() << '\n';
		}
	}
}

} // namespace fmisql
