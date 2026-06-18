//
// Created by Administrator on 6/17/2026.
//

#include "TableViewTab.h"

#include "IconsFontAwesome6.h"
#include "imgui.h"
#include "ui/Controls.h"
#include "ui/TableView/ResultsTable.h"

void ui::TableViewTab::DrawTableViewTab(DocumentData &tab, bool canRun,
                                        const std::function<void(DocumentData &)> &onRefresh) {
    buildBreadCrumb();
    ImGui::Dummy(ImVec2(0, 5));

    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, ui::controls::getDpiSize(6)));
    ImGui::PushStyleVar(ImGuiStyleVar_TabBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_Tab, style::kBgTransparent);
    ImGui::PushStyleColor(ImGuiCol_TabActive, style::kBtnHoverBg);
    ImGui::PushStyleColor(ImGuiCol_TabHovered, style::kBtnHoverBg); // hover

    if (ImGui::BeginTabBar("##TableViewTabs", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll)) {
        ImGuiTabItemFlags tabFlags = ImGuiTabItemFlags_None;

        if (tab.focusRequested) {
            tabFlags |= ImGuiTabItemFlags_SetSelected;
            tab.focusRequested = false;
        }

        ImGui::PushID(tab.id);

        if (ImGui::BeginTabItem(ICON_FA_TABLE_CELLS " Data", &tab.open, tabFlags)) {
            if (controls::IconButton("Refresh", ICON_FA_ROTATE, ui::style::kToolbarRefresh)) {
            }
            ImGui::SameLine();
            if (controls::IconButton("Add row", ICON_FA_PLUS, ui::style::kToolbarConnect)) {
            }

            const ImVec2 avail = ImGui::GetContentRegionAvail();
            m_tableView.DrawQueryResultTable(tab.result, "##results", avail.x, avail.y);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(ICON_FA_LIST_UL " Columns", &tab.open, tabFlags)) {
            const ImVec2 avail = ImGui::GetContentRegionAvail();
            buildColumns(avail.x, avail.y);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(ICON_FA_LINK " Constraints", &tab.open, tabFlags)) {
            const ImVec2 avail = ImGui::GetContentRegionAvail();
            buildConstraints(avail.x, avail.y);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(ICON_FA_BARS_STAGGERED " Indexes", &tab.open, tabFlags)) {
            const ImVec2 avail = ImGui::GetContentRegionAvail();
            buildIndexes(avail.x, avail.y);

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem(ICON_FA_CODE " DSL", &tab.open, tabFlags)) {
            const ImVec2 avail = ImGui::GetContentRegionAvail();
            buildDSL(avail.x, avail.y);

            ImGui::EndTabItem();
        }

        ImGui::PopID();
        ImGui::EndTabBar();
    }

    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(3);
}

void ui::TableViewTab::buildBreadCrumb() {
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 5));

    // const float iconSize = ImGui::GetStyle().FontSizeBase - 1;
    // const float iconRightSize = iconSize - 2;
    //
    // ImGui::PushFont(nullptr, iconSize);
    // ImGui::TextColored(style::kIconServer, ICON_FA_SERVER);
    // ImGui::PopFont();
    // ImGui::SameLine();
    // ImGui::TextColored(style::kIconGroup, "Production");
    //
    // ImGui::SameLine();
    // ImGui::PushFont(nullptr, iconRightSize);
    // ImGui::TextUnformatted(ICON_FA_CHEVRON_RIGHT);
    // ImGui::PopFont();
    //
    // ImGui::SameLine();
    // ImGui::PushFont(nullptr, iconSize);
    // ImGui::TextColored(style::kIconDatabase, ICON_FA_DATABASE);
    // ImGui::PopFont();
    // ImGui::SameLine();
    // ImGui::TextColored(style::kIconGroup, "app");
    //
    // ImGui::SameLine();
    // ImGui::PushFont(nullptr, iconRightSize);
    // ImGui::TextUnformatted(ICON_FA_CHEVRON_RIGHT);
    // ImGui::PopFont();
    //
    // ImGui::SameLine();
    // ImGui::PushFont(nullptr, iconSize);
    // ImGui::TextColored(style::kIconSchema, ICON_FA_LAYER_GROUP);
    // ImGui::PopFont();
    // ImGui::SameLine();
    // ImGui::TextColored(style::kIconGroup, "public");
    //
    // ImGui::SameLine();
    // ImGui::PushFont(nullptr, iconRightSize);
    // ImGui::TextUnformatted(ICON_FA_CHEVRON_RIGHT);
    // ImGui::PopFont();
    //
    // ImGui::SameLine();
    // ImGui::PushFont(nullptr, iconSize);
    // ImGui::TextColored(style::kIconTable, ICON_FA_TABLE);
    // ImGui::PopFont();
    // ImGui::SameLine();
    // ImGui::TextUnformatted("orders");

    ImGui::TextUnformatted("SELECT * FROM users LIMIT 0, 10000");

    ImGui::Dummy(ImVec2(0, 5));
    ImGui::Separator();
}

void ui::TableViewTab::buildColumns(float width, const float height) {
    constexpr ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable
                                      | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY |
                                      ImGuiTableFlags_SizingStretchSame;

    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, style::kBgTransparent);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, style::kBtnHoverBg);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, style::kBtnHeldBg);

    if (ImGui::BeginTable("#schemas", 6, flags, ImVec2(width, height))) {
        ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_None, 0.2);
        ImGui::TableSetupColumn("NAME");
        ImGui::TableSetupColumn("TYPE");
        ImGui::TableSetupColumn("NULLABLE");
        ImGui::TableSetupColumn("DEFAULT");
        ImGui::TableSetupColumn("KEY");

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 10.0f));
        ImGui::TableHeadersRow();
        ImGui::PopStyleVar();

        ImGui::EndTable();
    }

    ImGui::PopStyleColor(3);
}

void ui::TableViewTab::buildConstraints(float width, float height) {
    constexpr ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable
                                      | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY |
                                      ImGuiTableFlags_SizingStretchSame;

    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, style::kBgTransparent);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, style::kBtnHoverBg);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, style::kBtnHeldBg);

    if (ImGui::BeginTable("#schemas", 4, flags, ImVec2(width, height))) {
        ImGui::TableSetupColumn("NAME");
        ImGui::TableSetupColumn("TYPE");
        ImGui::TableSetupColumn("COLUMN");
        ImGui::TableSetupColumn("REFERENCES");

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 10.0f));
        ImGui::TableHeadersRow();
        ImGui::PopStyleVar();

        ImGui::EndTable();
    }

    ImGui::PopStyleColor(3);
}

void ui::TableViewTab::buildIndexes(float width, float height) {
    constexpr ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable
                                      | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY |
                                      ImGuiTableFlags_SizingStretchSame;

    ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, style::kBgTransparent);
    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, style::kBtnHoverBg);
    ImGui::PushStyleColor(ImGuiCol_HeaderActive, style::kBtnHeldBg);

    if (ImGui::BeginTable("#schemas", 4, flags, ImVec2(width, height))) {
        ImGui::TableSetupColumn("NAME");
        ImGui::TableSetupColumn("COLUMNS");
        ImGui::TableSetupColumn("UNIQUE");
        ImGui::TableSetupColumn("METHOD");

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 10.0f));
        ImGui::TableHeadersRow();
        ImGui::PopStyleVar();

        ImGui::EndTable();
    }

    ImGui::PopStyleColor(3);
}

void ui::TableViewTab::buildDSL(float width, float height) {
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, style::kBgTransparent);
    ImGui::PushStyleColor(ImGuiCol_Border, ImGui::GetStyle().Colors[ImGuiCol_Separator]);
    if (ImGui::InputTextMultiline("##query", m_dslText.data(), m_dslText.size(), ImVec2(-1, 150))) {
    }
    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(2);
}
