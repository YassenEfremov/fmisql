#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "pager.hpp"

#include <string_view>
#include <vector>


namespace fmisql {

struct Statement {

    enum class Type {
        CREATE_TABLE,
        DROP_TABLE,
        LIST_TABLES,
        TABLE_INFO,
        SELECT,
        REMOVE,
        INSERT,
        INVALID
    };

    Statement(
        Type type,
        std::string_view table_name = "",
        std::vector<std::string_view> select_columns = {},
        std::vector<ExampleRow> insert_rows = {}
    );

    Type type;

    std::string_view table_name;
    // columns for CreateTable
    std::vector<std::string_view> select_columns;
    // Remove?
    std::vector<ExampleRow> insert_rows;
};

} // namespace fmisql


#endif // STATEMENT_HPP
