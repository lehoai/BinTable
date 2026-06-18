#pragma once

#include "db/QueryResult.h"

namespace ui {

    struct CellEdit {
        int row = -1, col = -1;
        std::string buf;
        bool focusNext = false;
    };

    class TableView {
    public:
        void DrawQueryResultTable(const db::QueryResult &result, const char *tableId, float width = 0.0f,
                                         float height = 0.0f);
    private:
        CellEdit m_cellEdit;
    };
}
