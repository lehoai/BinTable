#pragma once

#include <functional>

#include "ui/DocumentTab/DocumentData.h"

namespace ui {
    class QueryTab {
    public:
        void DrawQueryTab(ui::DocumentData &tab, bool canRun, const std::function<void(DocumentData &)> &onRun);
    };
}
