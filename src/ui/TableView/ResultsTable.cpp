#include "ResultsTable.h"

#include <format>

#include "ui/dark_style.h"
#include "imgui_internal.h"
#include <imgui.h>

#include "../Controls.h"

namespace ui {
    struct CellEdit {
        int row = -1, col = -1;
        std::string buf;
        bool focusNext = false;
    };

    static CellEdit cellEdits{};


    void DrawQueryResultTable(const db::QueryResult &result, const char *tableId, float width, float height) {
        if (!result.success) {
            if (!result.error.empty())
                ImGui::TextColored(style::kTextError, "%s", result.error.c_str());
            return;
        }

        if (result.ColumnCount() == 0) {
            ImGui::TextDisabled("OK");
            return;
        }

        constexpr ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable
                                          | ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY |
                                          ImGuiTableFlags_SizingStretchSame;

        ImFont *font = ImGui::GetFont();
        const auto oldSize = font->Scale;
        font->Scale = 1.1f;
        ImGui::PushFont(font);

        ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, style::kBgTransparent);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, style::kBtnHoverBg);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, style::kBtnHeldBg);

        // ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 3.0f));

        if (ImGui::BeginTable(tableId, static_cast<int>(result.ColumnCount()), flags, ImVec2(width, height))) {
            for (const auto &column: result.columns)
                ImGui::TableSetupColumn(column.c_str());

            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 10.0f));
            ImGui::TableHeadersRow();
            ImGui::PopStyleVar();

            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 2.0f));

            constexpr float inputPadY = 8.0f;
            const float inputH = ImGui::GetTextLineHeight() + inputPadY * 2.0f;
            const float rowH   = inputH + 4.0f;

            ImGuiListClipper clipper;
            clipper.Begin(static_cast<int>(result.rows.size()));
            while (clipper.Step()) {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
                    ImGui::TableNextRow(ImGuiTableRowFlags_None, rowH);
                    for (int j = 0; j < result.rows[i].size(); j++) {
                        ImGui::TableNextColumn();

                        if (cellEdits.row == i && cellEdits.col == j) {
                            // ImGui::InputText("##cell_edit", &(cellEdits.buf), 0);
                            if (cellEdits.focusNext) {
                                ImGui::SetKeyboardFocusHere();
                                cellEdits.focusNext = false;
                            }

                            const float off = (rowH - inputH) * 0.5f;
                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + off);
                            ImGui::SetNextItemWidth(-FLT_MIN);
                            controls::InputText("##cell_edit", cellEdits.buf, "", ImVec2(2, inputPadY),
                                                ImVec4(0, 0, 0, 1.0f));

                            if (ImGui::IsItemDeactivatedAfterEdit() || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                                cellEdits.row = cellEdits.col = -1;
                            }
                        } else {
                            ImGui::AlignTextToFramePadding();
                            if (ImGui::Selectable(std::format("{}##cell{}{}", result.rows[i][j], i, j).c_str(), false,
                                                  ImGuiSelectableFlags_AllowDoubleClick, ImVec2(0, 0)))
                                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                                    cellEdits.row = i;
                                    cellEdits.col = j;
                                    cellEdits.buf = result.rows[i][j];
                                    cellEdits.focusNext = true;
                                }
                        }
                    }
                    if (ImGui::TableGetHoveredRow() == i + 1) {
                        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, style::kBtnHeldBg);
                    }
                }
            }
            clipper.End();

            ImGui::PopStyleVar();

            ImGui::EndTable();

            ImGui::PopStyleColor(3);


            ImGui::PopFont();
            font->Scale = oldSize;
        }
    }
} // namespace ui
