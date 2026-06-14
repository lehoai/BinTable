#pragma once

#include "db/QueryResult.h"

namespace ui {

// Renders the result of a query as a table within the current window/child.
// width/height of 0 fill the remaining content region (see ImGui::BeginTable sizing rules).
void DrawQueryResultTable(const db::QueryResult& result, const char* tableId, float width = 0.0f, float height = 0.0f);

} // namespace ui
