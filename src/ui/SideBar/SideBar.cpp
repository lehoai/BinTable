//
// Created by Administrator on 6/16/2026.
//

#include "SideBar.h"
#include "ui/Controls.h"
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
        ImGui::TextColored(ImVec4(0.50f, 0.83f, 0.65f, 1.0f), ICON_FA_SERVER);
        ImGui::SameLine();
        ImGui::TextUnformatted(tree.label.c_str());

        if (serverOpen) {
            // database

            for (auto &db: tree.children) {
                auto dbId = std::format("##db{}", db.label);
                const bool dbOpen = ImGui::TreeNodeEx(dbId.c_str(), baseFlags);
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.36f, 0.61f, 0.84f, 1.0f), ICON_FA_DATABASE);
                ImGui::SameLine();
                ImGui::TextUnformatted(db.label.c_str());
                if (dbOpen) {
                    // schema
                    for (auto &schema: db.children) {
                        auto schemaId = std::format("##schema{}{}", db.label, schema.label);
                        const bool schemaOpen = ImGui::TreeNodeEx(schemaId.c_str(), baseFlags);
                        ImGui::SameLine();
                        ImGui::TextColored(ImVec4(0.79f, 0.64f, 0.43f, 1.0f), ICON_FA_LAYER_GROUP);
                        ImGui::SameLine();
                        ImGui::TextUnformatted(schema.label.c_str());

                        if (schemaOpen) {
                            // group table folder
                            const bool groupTablesOpen = ImGui::TreeNodeEx("##tables", baseFlags);
                            ImGui::SameLine();
                            ImGui::TextColored(ImVec4(0.48f, 0.51f, 0.55f, 1.0f), ICON_FA_TABLE_LIST);
                            ImGui::SameLine();
                            ImGui::TextUnformatted("Tables");

                            if (groupTablesOpen) {
                                for (auto &table: schema.children) {
                                    if (table.type != services::NodeType::Table) continue;
                                    auto tableId = std::format("##table{}", table.label);
                                    ImGui::TreeNodeEx(tableId.c_str(), leafFlags);
                                    ImGui::SameLine();
                                    ImGui::TextColored(ImVec4(0.44f, 0.69f, 0.83f, 1.0f), ICON_FA_TABLE);
                                    ImGui::SameLine();
                                    ImGui::TextUnformatted(table.label.c_str());
                                }

                                ImGui::TreePop();
                            }

                            const bool viewGroupOpen = ImGui::TreeNodeEx("##views", baseFlags);
                            ImGui::SameLine();
                            ImGui::TextColored(ImVec4(0.48f, 0.51f, 0.55f, 1.0f), ICON_FA_TABLE_COLUMNS);
                            ImGui::SameLine();
                            ImGui::TextUnformatted("Views");

                            if (viewGroupOpen) {
                                for (auto &view: schema.children) {
                                    if (view.type != services::NodeType::View) continue;
                                    auto viewId = std::format("##view{}", view.label);
                                    ImGui::TreeNodeEx(viewId.c_str(), leafFlags);
                                    ImGui::SameLine();
                                    ImGui::TextColored(ImVec4(0.71f, 0.55f, 0.83f, 1.0f), ICON_FA_EYE);
                                    ImGui::SameLine();
                                    ImGui::TextUnformatted(view.label.c_str());
                                }
                                ImGui::TreePop();
                            }

                            const bool functionGroupOpen = ImGui::TreeNodeEx("##functions", baseFlags);

                            ImGui::SameLine();
                            ImGui::TextColored(ImVec4(0.48f, 0.51f, 0.55f, 1.0f), ICON_FA_GEARS);
                            ImGui::SameLine();
                            ImGui::TextUnformatted("Functions");

                            if (functionGroupOpen) {
                                for (auto &view: schema.children) {
                                    if (view.type != services::NodeType::Function) continue;
                                    auto viewId = std::format("##function{}", view.label);
                                    ImGui::TreeNodeEx(viewId.c_str(), leafFlags);
                                    ImGui::SameLine();
                                    ImGui::TextColored(ImVec4(0.50f, 0.75f, 0.56f, 1.0f), ICON_FA_GEAR);
                                    ImGui::SameLine();
                                    ImGui::TextUnformatted(view.label.c_str());
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
