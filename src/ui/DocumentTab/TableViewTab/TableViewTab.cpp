//
// Created by Administrator on 6/17/2026.
//

#include "TableViewTab.h"

#include "imgui.h"
#include "ui/TableView/ResultsTable.h"

void ui::TableViewTab::DrawTableViewTab(const DocumentData &tab, bool canRun,
                                        const std::function<void(DocumentData &)> &onRefresh) {
    const ImVec2 avail = ImGui::GetContentRegionAvail();
    DrawQueryResultTable(tab.result, "##results", avail.x, avail.y);
}
