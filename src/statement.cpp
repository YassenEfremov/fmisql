#include "statement.hpp"


namespace fmisql {

Statement::Statement(
	Type type,
	std::string_view table_name,
    std::vector<std::string_view> select_columns,
	std::vector<ExampleRow> insert_rows
) :
	type(type),
	table_name(table_name),
	select_columns(select_columns),
	insert_rows(insert_rows)
{}

} // namespace fmisql
