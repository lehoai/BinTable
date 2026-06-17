#pragma once

#include "ui/DocumentTab/DocumentData.h"

#include <functional>

namespace ui {
    class TableViewTab {
    public:
        void DrawTableViewTab(const DocumentData &tab, bool canRun,
                              const std::function<void(DocumentData &)> &onRefresh);
    };
}
