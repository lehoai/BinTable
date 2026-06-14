#include "ui/ResultsTable.h"

#include <imgui.h>

namespace ui {

void DrawQueryResultTable(const db::QueryResult& result, const char* tableId, float width, float height)
{
    if (!result.success)
    {
        if (!result.error.empty())
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "%s", result.error.c_str());
        return;
    }

    if (result.ColumnCount() == 0)
    {
        ImGui::TextDisabled("OK");
        return;
    }

    constexpr ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable
                                      | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;

    if (ImGui::BeginTable(tableId, static_cast<int>(result.ColumnCount()), flags, ImVec2(width, height)))
    {
        for (const auto& column : result.columns)
            ImGui::TableSetupColumn(column.c_str());
        ImGui::TableHeadersRow();

        for (const auto& row : result.rows)
        {
            ImGui::TableNextRow();
            for (const auto& value : row)
            {
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(value.c_str());
            }
        }

        ImGui::EndTable();
    }
}

} // namespace ui
