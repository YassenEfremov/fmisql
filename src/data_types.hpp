#ifndef DATA_TYPES_HPP
#define DATA_TYPES_HPP


namespace fmisql::sql_types {

enum class Id {
	INT,
	DATE,
	STRING
};

using Int = int;
using String = char[256];
using Date = int;

} // namespace fmisql


#endif // DATA_TYPES_HPP
