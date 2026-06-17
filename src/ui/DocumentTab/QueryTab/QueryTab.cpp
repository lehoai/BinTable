//
// Created by Administrator on 6/17/2026.
//

#include "QueryTab.h"

#include <imgui.h>

#include "IconsFontAwesome6.h"
#include "ui/Controls.h"
#include "ui/TableView/ResultsTable.h"

constexpr size_t kQueryBufferSize = 8192;

void ui::QueryTab::DrawQueryTab(DocumentData &tab, bool canRun, const std::function<void(DocumentData &)> &onRun) {
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
