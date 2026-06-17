#include "DocumentTab.h"
#include <imgui.h>

#include <algorithm>
#include <vector>

#include "../Controls.h"
#include "../dark_style.h"

void ui::DocumentTab::DrawDocumentTabs(std::vector<DocumentData> &tabs, const bool canRun,
                                       const std::function<void(DocumentData &)> &onRunQuery,
                                       const std::function<void(DocumentData &)> &onRefreshTable,
                                       const std::function<void()> &onNewQueryTab) {
    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, ui::controls::getDpiSize(6)));
    ImGui::PushStyleVar(ImGuiStyleVar_TabBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Tab, style::kBgTransparent);
    ImGui::PushStyleColor(ImGuiCol_TabActive, style::kBtnHoverBg);
    ImGui::PushStyleColor(ImGuiCol_TabHovered, style::kBtnHoverBg); // hover


    if (ImGui::BeginTabBar("##DocumentTabs", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll)) {
        if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
            onNewQueryTab();

        for (auto &tab: tabs) {
            ImGuiTabItemFlags tabFlags = ImGuiTabItemFlags_None;
            if (tab.focusRequested) {
                tabFlags |= ImGuiTabItemFlags_SetSelected;
                tab.focusRequested = false;
            }

            ImGui::PushID(tab.id);
            if (ImGui::BeginTabItem(tab.title.c_str(), &tab.open, tabFlags)) {
                if (tab.type == DocumentType::Query)
                    m_queryTab.DrawQueryTab(tab, canRun, onRunQuery);
                else
                    m_tableViewTab.DrawTableViewTab(tab, canRun, onRefreshTable);

                ImGui::EndTabItem();
            }
            ImGui::PopID();
        }

        ImGui::EndTabBar();
    }

    std::erase_if(tabs, [](const DocumentData &tab) { return !tab.open; });
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(3);
}
