#pragma once

#include <string>
#include <vector>

namespace db {

struct ColumnInfo
{
    std::string name;
    std::string dataType;
    bool nullable = true;
};

struct TableInfo
{
    std::string schema;
    std::string name;

    std::vector<ColumnInfo> columns;
    bool columnsLoaded = false;
};

struct SchemaInfo
{
    std::string name;
    std::vector<TableInfo> tables;
};

} // namespace db
