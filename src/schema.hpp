#ifndef SCHEMA_HPP
#define SCHEMA_HPP

#include "data_types.hpp"

#include <string_view>

#include <cstdint>
#include <cstring>


namespace fmisql {

constexpr std::size_t max_table_name_size = 256;


struct SchemaRow {
	sql_types::String table_name;
	sql_types::Int page;
	sql_types::String sql;

	SchemaRow(std::string_view table_name, int page, std::string_view sql)
		: page (page) {
		
		table_name.copy(this->table_name, sizeof this->table_name, 0);
		sql.copy(this->sql, sizeof this->sql, 0);
	}

	void serialize(void *dst) const {
		std::memcpy(dst,
		            &this->table_name,
		            sizeof this->table_name);
		std::memcpy((std::uint8_t *)dst + sizeof this->table_name,
		            &this->page,
		            sizeof this->page);
		std::memcpy((std::uint8_t *)dst + sizeof this->table_name + sizeof this->page,
		            &this->sql,
		            sizeof this->sql);
	}

	void deserialize(void *src) {
		std::memcpy(&this->table_name,
		            src,
		            sizeof this->table_name);
		std::memcpy(&this->page,
		            (std::uint8_t *)src + sizeof this->table_name,
		            sizeof this->page);
		std::memcpy(&this->sql,
		            (std::uint8_t *)src + sizeof this->table_name + sizeof this->page,
					sizeof this->sql);
	}
};

} // namespace fmisql


#endif // SCHEMA_HPP
