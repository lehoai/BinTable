//
// Created by Administrator on 6/16/2026.
//

#include "SideBar.h"
#include "ui/Controls.h"
#include "ui/dark_style.h"
#include "IconsFontAwesome6.h"
#include <imgui.h>
#include <format>

constexpr ImGuiTreeNodeFlags baseFlags =
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_OpenOnDoubleClick |
        ImGuiTreeNodeFlags_FramePadding |
        ImGuiTreeNodeFlags_SpanAvailWidth;

constexpr ImGuiTreeNodeFlags leafFlags =
        baseFlags |
        ImGuiTreeNodeFlags_Leaf |
        ImGuiTreeNodeFlags_NoTreePushOnOpen;

void ui::SideBar::Draw() {
    const float iconSize = ImGui::GetStyle().FontSizeBase - 1;
    const float sidebarWidth = ui::controls::getDpiSize(240.0f);
    ImGui::BeginChild("##sidebar", ImVec2(sidebarWidth, 0), ImGuiChildFlags_Borders);

    ImGui::PushItemWidth(controls::getDpiSize(225.0f));

    ImGui::TextUnformatted("Database");
    ImGui::Separator();

    controls::InputText("##filter_text", m_filterText, "Search schema, tables...");
    ImGui::Separator();

    // draw tree
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 6));
    for (auto trees = m_connService.BuildTree(); auto &tree: trees) {
        // server
        auto serverId = std::format("##server{}", tree.label);
        const bool serverOpen = ImGui::TreeNodeEx(serverId.c_str(), baseFlags);
        ImGui::SameLine();
        ImGui::PushFont(nullptr, iconSize);
        ImGui::TextColored(style::kIconServer, ICON_FA_SERVER);
        ImGui::PopFont();
        ImGui::SameLine();
        ImGui::TextUnformatted(tree.label.c_str());

        if (serverOpen) {
            // database

            for (auto &db: tree.children) {
                auto dbId = std::format("##db{}", db.label);
                const bool dbOpen = ImGui::TreeNodeEx(dbId.c_str(), baseFlags);
                ImGui::SameLine();
                ImGui::PushFont(nullptr, iconSize);
                ImGui::TextColored(style::kIconDatabase, ICON_FA_DATABASE);
                ImGui::PopFont();
                ImGui::SameLine();
                ImGui::TextUnformatted(db.label.c_str());
                if (dbOpen) {
                    // schema
                    for (auto &schema: db.children) {
                        auto schemaId = std::format("##schema{}{}", db.label, schema.label);
                        const bool schemaOpen = ImGui::TreeNodeEx(schemaId.c_str(), baseFlags);
                        ImGui::SameLine();
                        ImGui::PushFont(nullptr, iconSize);
                        ImGui::TextColored(style::kIconSchema, ICON_FA_LAYER_GROUP);
                        ImGui::PopFont();
                        ImGui::SameLine();
                        ImGui::TextUnformatted(schema.label.c_str());

                        if (schemaOpen) {
                            // group table folder
                            const bool groupTablesOpen = ImGui::TreeNodeEx("##tables", baseFlags);
                            ImGui::SameLine();
                            ImGui::PushFont(nullptr, iconSize);
                            ImGui::TextColored(style::kIconGroup, ICON_FA_TABLE_LIST);
                            ImGui::PopFont();
                            ImGui::SameLine();
                            ImGui::TextUnformatted("Tables");

                            if (groupTablesOpen) {
                                for (auto &table: schema.children) {
                                    if (table.type != services::NodeType::Table) continue;
                                    auto tableId = std::format("##table{}", table.label);
                                    auto tmpFlags = leafFlags;
                                    if (tableId == m_selectedKey) {
                                        tmpFlags |= ImGuiTreeNodeFlags_Selected;
                                    }
                                    ImGui::TreeNodeEx(tableId.c_str(), tmpFlags);
                                    if (ImGui::IsItemClicked()) {
                                        m_selectedKey = tableId;
                                    }
                                    ImGui::SameLine();
                                    ImGui::PushFont(nullptr, iconSize);
                                    ImGui::TextColored(style::kIconTable, ICON_FA_TABLE);
                                    ImGui::PopFont();
                                    ImGui::SameLine();
                                    ImGui::TextUnformatted(table.label.c_str());
                                }

                                ImGui::TreePop();
                            }

                            const bool viewGroupOpen = ImGui::TreeNodeEx("##views", baseFlags);
                            ImGui::SameLine();
                            ImGui::PushFont(nullptr, iconSize);
                            ImGui::TextColored(style::kIconGroup, ICON_FA_TABLE_COLUMNS);
                            ImGui::PopFont();
                            ImGui::SameLine();
                            ImGui::TextUnformatted("Views");

                            if (viewGroupOpen) {
                                for (auto &view: schema.children) {
                                    if (view.type != services::NodeType::View) continue;
                                    auto viewId = std::format("##view{}", view.label);
                                    auto tmpFlags = leafFlags;
                                    if (viewId == m_selectedKey) {
                                        tmpFlags |= ImGuiTreeNodeFlags_Selected;
                                    }
                                    ImGui::TreeNodeEx(viewId.c_str(), tmpFlags);
                                    if (ImGui::IsItemClicked()) {
                                        m_selectedKey = viewId;
                                    }
                                    ImGui::SameLine();
                                    ImGui::PushFont(nullptr, iconSize);
                                    ImGui::TextColored(style::kIconView, ICON_FA_EYE);
                                    ImGui::PopFont();
                                    ImGui::SameLine();
                                    ImGui::TextUnformatted(view.label.c_str());
                                }
                                ImGui::TreePop();
                            }

                            const bool functionGroupOpen = ImGui::TreeNodeEx("##functions", baseFlags);

                            ImGui::SameLine();
                            ImGui::PushFont(nullptr, iconSize);
                            ImGui::TextColored(style::kIconGroup, ICON_FA_GEARS);
                            ImGui::PopFont();
                            ImGui::SameLine();
                            ImGui::TextUnformatted("Functions");

                            if (functionGroupOpen) {
                                for (auto &func: schema.children) {
                                    if (func.type != services::NodeType::Function) continue;
                                    auto funcId = std::format("##function{}", func.label);
                                    auto tmpFlags = leafFlags;
                                    if (funcId == m_selectedKey) {
                                        tmpFlags |= ImGuiTreeNodeFlags_Selected;
                                    }
                                    ImGui::TreeNodeEx(funcId.c_str(), tmpFlags);
                                    if (ImGui::IsItemClicked()) {
                                        m_selectedKey = funcId;
                                    }
                                    ImGui::SameLine();
                                    ImGui::PushFont(nullptr, iconSize);
                                    ImGui::TextColored(style::kIconFunction, ICON_FA_GEAR);
                                    ImGui::PopFont();
                                    ImGui::SameLine();
                                    ImGui::TextUnformatted(func.label.c_str());
                                }

                                ImGui::TreePop();
                            }
                            ImGui::TreePop();
                        }
                    }

                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
    }
    ImGui::PopStyleVar();

    ImGui::PopItemWidth();

    ImGui::EndChild();
}
