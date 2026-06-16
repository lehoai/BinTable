#include "ui/DocumentTabs.h"

#include "ui/ResultsTable.h"

#include <imgui.h>

#include <algorithm>
#include <vector>

#include "Controls.h"

namespace ui {

namespace {

constexpr size_t kQueryBufferSize = 8192;

void DrawQueryTab(DocumentTab& tab, bool canRun, const std::function<void(DocumentTab&)>& onRun)
{
    std::vector<char> buf(kQueryBufferSize, '\0');
    const size_t copyLen = std::min(tab.queryText.size(), kQueryBufferSize - 1);
    std::copy_n(tab.queryText.begin(), copyLen, buf.begin());

    if (ImGui::InputTextMultiline("##query", buf.data(), buf.size(), ImVec2(-1, 150)))
        tab.queryText.assign(buf.data());

    ImGui::BeginDisabled(!canRun);
    if (ImGui::Button("bận ơi"))
        onRun(tab);
    ImGui::EndDisabled();

    if (!tab.statusMessage.empty())
    {
        ImGui::SameLine();
        ImGui::TextUnformatted(tab.statusMessage.c_str());
    }

    ImGui::Separator();

    const ImVec2 avail = ImGui::GetContentRegionAvail();
    DrawQueryResultTable(tab.result, "##results", avail.x, avail.y);
}

void DrawTableViewTab(DocumentTab& tab, bool canRun, const std::function<void(DocumentTab&)>& onRefresh)
{
    ImGui::Text("%s.%s", tab.schemaName.c_str(), tab.tableName.c_str());

    ImGui::SameLine();
    ImGui::BeginDisabled(!canRun);
    if (ImGui::Button("Refresh"))
        onRefresh(tab);
    ImGui::EndDisabled();

    if (!tab.statusMessage.empty())
    {
        ImGui::SameLine();
        ImGui::TextUnformatted(tab.statusMessage.c_str());
    }

    ImGui::Separator();

    const ImVec2 avail = ImGui::GetContentRegionAvail();
    DrawQueryResultTable(tab.result, "##results", avail.x, avail.y);
}

} // namespace

void DrawDocumentTabs(std::vector<DocumentTab>& tabs, const bool canRun,
    const std::function<void(DocumentTab&)>& onRunQuery,
    const std::function<void(DocumentTab&)>& onRefreshTable,
    const std::function<void()>& onNewQueryTab)
{
    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(12, ui::controls::getDpiSize(6)));
    if (ImGui::BeginTabBar("##DocumentTabs", ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyScroll))
    {
        if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
            onNewQueryTab();

        for (auto& tab : tabs)
        {
            ImGuiTabItemFlags tabFlags = ImGuiTabItemFlags_None;
            if (tab.focusRequested)
            {
                tabFlags |= ImGuiTabItemFlags_SetSelected;
                tab.focusRequested = false;
            }

            ImGui::PushID(tab.id);
            if (ImGui::BeginTabItem(tab.title.c_str(), &tab.open, tabFlags))
            {
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

    tabs.erase(std::remove_if(tabs.begin(), tabs.end(), [](const DocumentTab& tab) { return !tab.open; }), tabs.end());
    ImGui::PopStyleVar(2);
}

} // namespace ui
