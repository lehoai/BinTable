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

    [[nodiscard]] size_t RowCount() const { return rows.size(); }
    [[nodiscard]] size_t ColumnCount() const { return columns.size(); }
};

} // namespace db
