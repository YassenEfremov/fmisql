#ifndef CLASSES_HPP
#define CLASSES_HPP

#include "data_types.hpp"

#include <vector>
#include <string>
#include <string_view>


class Column {
public:
	Column(std::string_view name, DataType type) : name(name), type(type) {}

	std::string_view name;
	DataType type;
private:
};



class Table {
public:
	Table(const std::vector<Column> &columns) : columns(columns) {}

private:
	std::vector<Column> columns;


	std::vector<int> pages;
};


#endif // CLASSES_HPP

