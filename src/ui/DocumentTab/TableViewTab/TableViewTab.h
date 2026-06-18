#pragma once

#include "ui/DocumentTab/DocumentData.h"

#include <functional>

#include "ui/TableView/ResultsTable.h"

namespace ui {
    class TableViewTab {
    public:
        void DrawTableViewTab(DocumentData &tab, bool canRun,
                              const std::function<void(DocumentData &)> &onRefresh);
    private:
        static void buildBreadCrumb();

        static void buildColumns(float width, float height);

        static void buildConstraints(float width, float height);

        static void buildIndexes(float width, float height);

        void buildDSL(float width, float height);

        TableView m_tableView;
        std::string m_dslText = "CREATE TABLE users COLUMN()";
    };
}
