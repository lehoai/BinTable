#include "ui/DocumentTabs.h"

#include "ui/ResultsTable.h"

#include <imgui.h>

#include <algorithm>
#include <vector>

#include "Controls.h"
#include "dark_style.h"
#include "IconsFontAwesome6.h"

namespace ui {
    namespace {
        constexpr size_t kQueryBufferSize = 8192;

        void DrawQueryTab(DocumentTab &tab, bool canRun, const std::function<void(DocumentTab &)> &onRun) {
            std::vector buf(kQueryBufferSize, '\0');
            const size_t copyLen = std::min(tab.queryText.size(), kQueryBufferSize - 1);
            std::copy_n(tab.queryText.begin(), copyLen, buf.begin());

            ImGui::BeginDisabled(!canRun);
            auto selectedIndex = 0;
            ImGui::PushItemWidth(controls::getDpiSize(250.0f));
            controls::Combo("##openingConnection", {"Postgres - Prod", "Mysql - Stg"}, selectedIndex, ImVec2(8, 6));
            ImGui::SameLine();
            if (controls::IconButton("Run (Ctrl+Enter)", ICON_FA_PLAY, style::kToolbarConnect))
                onRun(tab);
            ImGui::SameLine();
            if (controls::IconButton("Run Selection", ICON_FA_ALIGN_LEFT, style::kToolbarConnect)) {
            }
            ImGui::SameLine();
            if (controls::IconButton("Explain", ICON_FA_SITEMAP, style::kToolbarOpen)) {
            }
            ImGui::SameLine();
            if (controls::IconButton("Format", ICON_FA_WAND_MAGIC_SPARKLES, style::kToolbarExport)) {
            }
            ImGui::EndDisabled();

            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, style::kBgTransparent);
            ImGui::PushStyleColor(ImGuiCol_Border, ImGui::GetStyle().Colors[ImGuiCol_Separator]);
            if (ImGui::InputTextMultiline("##query", buf.data(), buf.size(), ImVec2(-1, 150)))
                tab.queryText.assign(buf.data());
            ImGui::PopStyleVar(1);
            ImGui::PopStyleColor(2);

            if (!tab.statusMessage.empty()) {
                ImGui::SameLine();
                ImGui::TextUnformatted(tab.statusMessage.c_str());
            }

            ImGui::Separator();

            const ImVec2 avail = ImGui::GetContentRegionAvail();
            DrawQueryResultTable(tab.result, "##results", avail.x, avail.y);
        }

        void DrawTableViewTab(const DocumentTab &tab, bool canRun, const std::function<void(DocumentTab &)> &onRefresh) {
            // ImGui::Text("%s.%s", tab.schemaName.c_str(), tab.tableName.c_str());
            //
            // ImGui::SameLine();
            // ImGui::BeginDisabled(!canRun);
            // if (ImGui::Button("Refresh"))
            //     onRefresh(tab);
            // ImGui::EndDisabled();
            //
            // if (!tab.statusMessage.empty()) {
            //     ImGui::SameLine();
            //     ImGui::TextUnformatted(tab.statusMessage.c_str());
            // }
            //
            // ImGui::Separator();

            const ImVec2 avail = ImGui::GetContentRegionAvail();
            DrawQueryResultTable(tab.result, "##results", avail.x, avail.y);
        }
    } // namespace

    void DrawDocumentTabs(std::vector<DocumentTab> &tabs, const bool canRun,
                          const std::function<void(DocumentTab &)> &onRunQuery,
                          const std::function<void(DocumentTab &)> &onRefreshTable,
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
                    if (tab.type == TabType::Query)
                        DrawQueryTab(tab, canRun, onRunQuery);
                    else
                        DrawTableViewTab(tab, canRun, onRefreshTable);

                    ImGui::EndTabItem();
                }
                ImGui::PopID();
            }

            ImGui::EndTabBar();
        }

        std::erase_if(tabs, [](const DocumentTab &tab) { return !tab.open; });
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(3);
    }
} // namespace ui
