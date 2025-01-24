#ifndef CLASSES_HPP
#define CLASSES_HPP

#include "data_types.hpp"

#include <vector>
#include <string>
#include <string_view>


namespace fmisql {

class Column {
public:
	Column(std::string_view name, sql_types::Id type)
		: name(name), type(type) {}

	std::string_view name;
	sql_types::Id type;
private:
};



class Table {
public:
	Table(const std::vector<Column> &columns) : columns(columns) {}

private:
	std::vector<Column> columns;


	std::vector<int> pages;
};

} // namespace fmisql


#endif // CLASSES_HPP

