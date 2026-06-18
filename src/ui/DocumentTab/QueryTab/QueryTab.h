#pragma once

#include <functional>

#include "ui/DocumentTab/DocumentData.h"
#include "ui/TableView/ResultsTable.h"

namespace ui {
    class QueryTab {
    public:
        void DrawQueryTab(DocumentData &tab, bool canRun, const std::function<void(DocumentData &)> &onRun);

    private:
        TableView m_tableView;
        int m_selectedConnectionIndex = 0;
    };
}
