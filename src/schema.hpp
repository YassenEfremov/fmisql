#ifndef SCHEMA_HPP
#define SCHEMA_HPP

#include "data_types.hpp"
#include "btree.hpp"

#include <string_view>
#include <stdexcept>
#include <string>
#include <vector>

#include <cstdint>
#include <cstring>


namespace fmisql {

// extern BplusTree schema_BplusTree;

struct SchemaRow {
	char table_name[sql_types::max_string_size];
	uint32_t root_page;
	char original_sql[sql_types::max_string_size];

	SchemaRow(std::string_view table_name, int root_page, std::string_view original_sql)
		: table_name{0}, root_page(root_page), original_sql{0} {
		// i know, this is really stupid
		if (table_name.size() > sql_types::max_string_size)
			throw std::runtime_error("max table name size is " + std::to_string(sql_types::max_string_size) + " characters");
		if (original_sql.size() > sql_types::max_string_size)
			throw std::runtime_error("max sql string size is " + std::to_string(sql_types::max_string_size) + " characters");
		table_name.copy(this->table_name, table_name.size());
		original_sql.copy(this->original_sql, original_sql.size());
	}

	void serialize(void *dst) const {
		std::memcpy(dst,
		            &this->table_name,
		            sizeof this->table_name);
		std::memcpy((std::uint8_t *)dst + sizeof this->table_name,
		            &this->root_page,
		            sizeof this->root_page);
		std::memcpy((std::uint8_t *)dst + sizeof this->table_name + sizeof this->root_page,
		            &this->original_sql,
		            sizeof this->original_sql);
	}

	void deserialize(void *src) {
		std::memcpy(&this->table_name,
		            src,
		            sizeof this->table_name);
		std::memcpy(&this->root_page,
		            (std::uint8_t *)src + sizeof this->table_name,
		            sizeof this->root_page);
		std::memcpy(&this->original_sql,
		            (std::uint8_t *)src + sizeof this->table_name + sizeof this->root_page,
					sizeof this->original_sql);
	}
};

} // namespace fmisql


#endif // SCHEMA_HPP
