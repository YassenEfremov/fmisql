#ifndef STATEMENT_HPP
#define STATEMENT_HPP

#include "data_types.hpp"

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
        const std::vector<sql_types::Column> &create_columns = {},
        const std::vector<std::string_view> &select_columns = {},
        const std::vector<std::vector<sql_types::Value>> &insert_rows = {}
    );

    Type type;
    std::string_view table_name;
    std::vector<sql_types::Column> create_columns;
    std::vector<std::string_view> select_columns;
    // Remove?
    std::vector<std::vector<sql_types::Value>> insert_rows;
};

} // namespace fmisql


#endif // STATEMENT_HPP
