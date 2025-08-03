#ifndef SCHEMA_HPP
#define SCHEMA_HPP

#include "data_types.hpp"

#include <string_view>

#include <cstdint>


namespace fmisql {

struct SchemaRow {
	char table_name[sql_types::max_string_size];
	std::uint32_t root_page;
	char original_sql[sql_types::max_string_size];

	SchemaRow();
	SchemaRow(std::string_view table_name, int root_page, std::string_view original_sql);

	void serialize(void *dst) const;
	void deserialize(void *src);
};

} // namespace fmisql


#endif // SCHEMA_HPP
