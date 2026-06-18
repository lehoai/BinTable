#include "ResultsTable.h"

#include "ui/dark_style.h"
#include "imgui_internal.h"
#include <imgui.h>

#include "../Controls.h"

namespace ui {
    void TableView::DrawQueryResultTable(const db::QueryResult &result, const char *tableId, float width,
                                         const float height) {
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

        ImGui::PushFont(nullptr, 16);

        ImGui::PushStyleColor(ImGuiCol_TableHeaderBg, style::kBgTransparent);
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, style::kBtnHoverBg);
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, style::kBtnHeldBg);

        // ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 3.0f));

        if (ImGui::BeginTable(tableId, static_cast<int>(result.ColumnCount()), flags)) {
            for (const auto &column: result.columns)
                ImGui::TableSetupColumn(column.c_str());

            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 10.0f));
            ImGui::TableHeadersRow();
            ImGui::PopStyleVar();

            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 2.0f));

            constexpr float inputPadY = 8.0f;
            const float inputH = ImGui::GetTextLineHeight() + inputPadY * 2.0f;
            const float rowH = inputH + 4.0f;

            ImGuiListClipper clipper;
            clipper.Begin(static_cast<int>(result.rows.size()));
            while (clipper.Step()) {
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
                    ImGui::TableNextRow(ImGuiTableRowFlags_None, rowH);
                    const int colCount = static_cast<int>(result.rows[i].size());
                    for (int j = 0; j < colCount; ++j) {
                        ImGui::TableNextColumn();

                        if (m_cellEdit.row == i && m_cellEdit.col == j) {
                            if (m_cellEdit.focusNext) {
                                ImGui::SetKeyboardFocusHere();
                                m_cellEdit.focusNext = false;
                            }

                            const float off = (rowH - inputH) * 0.5f;
                            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + off);
                            ImGui::SetNextItemWidth(-FLT_MIN);
                            controls::InputText("##cell_edit", m_cellEdit.buf, "", ImVec2(2, inputPadY),
                                                ImVec4(0, 0, 0, 1.0f));

                            if (ImGui::IsItemDeactivatedAfterEdit() || ImGui::IsKeyPressed(ImGuiKey_Escape)) {
                                m_cellEdit.row = m_cellEdit.col = -1;
                            }
                        } else {
                            ImGui::PushID(i);
                            ImGui::PushID(j);
                            ImGui::AlignTextToFramePadding();
                            if (ImGui::Selectable(result.rows[i][j].c_str(), false,
                                                  ImGuiSelectableFlags_AllowDoubleClick, ImVec2(0, 0)))
                                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                                    m_cellEdit.row = i;
                                    m_cellEdit.col = j;
                                    m_cellEdit.buf = result.rows[i][j];
                                    m_cellEdit.focusNext = true;
                                }
                            ImGui::PopID();
                            ImGui::PopID();
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
        }
        ImGui::PopFont();
        ImGui::PopStyleColor(3);
    }
} // namespace ui
