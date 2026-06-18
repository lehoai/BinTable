//
// Created by Administrator on 6/17/2026.
//

#include "QueryTab.h"

#include <imgui.h>

#include "IconsFontAwesome6.h"
#include "ui/Controls.h"
#include "ui/TableView/ResultsTable.h"
#include "misc/cpp/imgui_stdlib.h"

// TODO: hard code
static const std::vector<std::string> connectionList = {"Postgres - Prod", "Mysql - Stg"};

void ui::QueryTab::DrawQueryTab(DocumentData &tab, const bool canRun,
                                const std::function<void(DocumentData &)> &onRun) {
    ImGui::BeginDisabled(!canRun);
    ImGui::PushItemWidth(controls::getDpiSize(250.0f));
    controls::Combo("##openingConnection", connectionList, m_selectedConnectionIndex, ImVec2(8, 6));
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
    ImGui::SameLine();
    if (controls::IconButton("Commit", ICON_FA_CHECK, style::kToolbarSave)) {
    }
    ImGui::SameLine();
    if (controls::IconButton("Rollback", ICON_FA_ROTATE_LEFT, style::kToolbarDisconnect)) {
    }
    ImGui::EndDisabled();

    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, style::kBgTransparent);
    ImGui::PushStyleColor(ImGuiCol_Border, ImGui::GetStyle().Colors[ImGuiCol_Separator]);
    ImGui::InputTextMultiline("##query", &tab.queryText, ImVec2(-1, 150));
    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(2);

    if (!tab.statusMessage.empty()) {
        ImGui::SameLine();
        ImGui::TextUnformatted(tab.statusMessage.c_str());
    }

    ImGui::Separator();

    const ImVec2 avail = ImGui::GetContentRegionAvail();

    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, ui::controls::getDpiSize(6)));
    ImGui::PushStyleVar(ImGuiStyleVar_TabBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Tab, style::kBgTransparent);
    ImGui::PushStyleColor(ImGuiCol_TabActive, style::kBtnHoverBg);
    ImGui::PushStyleColor(ImGuiCol_TabHovered, style::kBtnHoverBg); // hover

    if (ImGui::BeginTabBar("##ResultTabs", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll)) {
        ImGuiTabItemFlags tabFlags = ImGuiTabItemFlags_None;

        if (tab.focusRequested) {
            tabFlags |= ImGuiTabItemFlags_SetSelected;
            tab.focusRequested = false;
        }

        if (ImGui::BeginTabItem("Result Data", &tab.open, tabFlags)) {
            m_tableView.DrawQueryResultTable(tab.result, "##results", avail.x, avail.y);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Message", &tab.open, tabFlags)) {
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Execution Plan", &tab.open, tabFlags)) {
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(3);
}
