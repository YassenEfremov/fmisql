#include "statement.hpp"

#include "data_types.hpp"


namespace fmisql {

Statement::Statement(
	Type type,
	std::string_view table_name,
    const std::vector<sql_types::Column> &create_columns,
    const std::vector<std::string_view> &select_columns,
	const std::vector<std::vector<sql_types::Value>> &insert_rows
) :
	type(type),
	table_name(table_name),
	create_columns(create_columns),
	select_columns(select_columns),
	insert_rows(insert_rows)
{}

} // namespace fmisql
