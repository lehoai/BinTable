#pragma once

#include "db/QueryResult.h"

namespace ui {
    class TableView {
    public:
        void DrawQueryResultTable(const db::QueryResult &result, const char *tableId, float width = 0.0f,
                                  float height = 0.0f);
    };
}
