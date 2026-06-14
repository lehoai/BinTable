#pragma once

#include "ui/DocumentTab.h"

#include <functional>
#include <vector>

namespace ui {

// Renders the center tab bar (SQL query tabs and table grid-view tabs) within the current child window.
// Closed tabs are erased from `tabs` automatically.
void DrawDocumentTabs(std::vector<DocumentTab>& tabs, bool canRun,
    const std::function<void(DocumentTab&)>& onRunQuery,
    const std::function<void(DocumentTab&)>& onRefreshTable,
    const std::function<void()>& onNewQueryTab);

} // namespace ui
