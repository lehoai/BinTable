#pragma once

#include <string>
#include <vector>

namespace db {

struct QueryResult
{
    bool success = false;
    std::string error;

    std::vector<std::string> columns;
    std::vector<std::vector<std::string>> rows;

    size_t RowCount() const { return rows.size(); }
    size_t ColumnCount() const { return columns.size(); }
};

} // namespace db
