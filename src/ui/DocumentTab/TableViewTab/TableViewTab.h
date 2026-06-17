#pragma once

#include "ui/DocumentTab/DocumentData.h"

#include <functional>

#include "ui/TableView/ResultsTable.h"

namespace ui {
    class TableViewTab {
    public:
        void DrawTableViewTab(const DocumentData &tab, bool canRun,
                              const std::function<void(DocumentData &)> &onRefresh);
    private:
        TableView m_tableView;
    };
}
