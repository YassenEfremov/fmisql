#include "statement.hpp"

#include "data_types.hpp"


namespace fmisql {

Statement::Statement(
	Type type,
	std::string_view table_name,
    std::vector<Column> create_columns,
    std::vector<std::string_view> select_columns,
	std::vector<sql_types::ExampleRow> insert_rows
) :
	type(type),
	table_name(table_name),
	create_columns(create_columns),
	select_columns(select_columns),
	insert_rows(insert_rows)
{}

} // namespace fmisql
