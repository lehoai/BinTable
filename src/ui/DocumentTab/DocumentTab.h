#pragma once

#include <functional>

#include "QueryTab/QueryTab.h"
#include "TableViewTab/TableViewTab.h"

namespace ui {

    class DocumentTab {
    public:
        void DrawDocumentTabs(std::vector<DocumentData> &tabs,
                              bool canRun,
                              const std::function<void(DocumentData &)> &onRunQuery,
                              const std::function<void(DocumentData &)> &onRefreshTable,
                              const std::function<void()> &onNewQueryTab);

    private:
        QueryTab m_queryTab;
        TableViewTab m_tableViewTab;
    };
} // namespace ui
