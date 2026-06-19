//
// Created by Administrator on 6/16/2026.
//

#include "SideBar.h"
#include "ui/Controls.h"
#include "ui/dark_style.h"
#include "IconsFontAwesome6.h"
#include <imgui.h>

constexpr ImGuiTreeNodeFlags baseFlags =
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_OpenOnDoubleClick |
        ImGuiTreeNodeFlags_FramePadding |
        ImGuiTreeNodeFlags_SpanAvailWidth;

constexpr ImGuiTreeNodeFlags leafFlags =
        baseFlags |
        ImGuiTreeNodeFlags_Leaf |
        ImGuiTreeNodeFlags_NoTreePushOnOpen;

// m_selectedKey format: "t:label" = table, "v:label" = view, "f:label" = function.
// Zero-allocation comparison — no temporary string created per frame.
static bool keyMatches(const std::string &key, const char type, const std::string &label) {
    return key.size() == 2 + label.size()
           && key[0] == type && key[1] == ':'
           && key.compare(2, std::string::npos, label) == 0;
}

void ui::SideBar::Draw(const OpenTabFn &onOpenTable) {
    const float iconSize = ImGui::GetStyle().FontSizeBase - 1;
    const float sidebarWidth = ui::controls::getDpiSize(240.0f);
    ImGui::BeginChild("##sidebar", ImVec2(sidebarWidth, 0), ImGuiChildFlags_Borders);

    ImGui::PushItemWidth(controls::getDpiSize(225.0f));

    ImGui::TextUnformatted("Database");
    ImGui::Separator();

    controls::InputText("##filter_text", m_filterText, "Search schema, tables...");
    ImGui::Separator();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 6));
    for (auto &session: m_service.Sessions()) {
        // server layer
        ImGui::PushID(session.id);
        const bool serverOpen = ImGui::TreeNodeEx("##n", baseFlags);
        ImGui::SameLine();
        ImGui::PushFont(nullptr, iconSize);
        ImGui::TextColored(style::kIconServer, ICON_FA_SERVER);
        ImGui::PopFont();
        ImGui::SameLine();
        ImGui::TextUnformatted(session.config.connectionName.c_str());

        // status server connect
        if (session.status == services::SessionStatus::Connecting) {
            ImGui::SameLine();
            ImGui::TextDisabled("Connecting...");
        } else if (session.status == services::SessionStatus::Failed) {
            ImGui::SameLine();
            ImGui::TextColored(style::kTextError, "(failed)");
        }

        if (serverOpen) {
            for (const auto &[name, schemas]: session.databases) {
                // db layer
                ImGui::PushID(name.c_str());
                const bool dbOpen = ImGui::TreeNodeEx("##n", baseFlags);
                ImGui::SameLine();
                ImGui::PushFont(nullptr, iconSize);
                ImGui::TextColored(style::kIconDatabase, ICON_FA_DATABASE);
                ImGui::PopFont();
                ImGui::SameLine();
                ImGui::TextUnformatted(name.c_str());

                if (dbOpen) {
                    for (auto &schema: schemas) {
                        // schema layer
                        ImGui::PushID(schema.name.c_str());
                        const bool schemaOpen = ImGui::TreeNodeEx("##n", baseFlags);
                        ImGui::SameLine();
                        ImGui::PushFont(nullptr, iconSize);
                        ImGui::TextColored(style::kIconSchema, ICON_FA_LAYER_GROUP);
                        ImGui::PopFont();
                        ImGui::SameLine();
                        ImGui::TextUnformatted(schema.name.c_str());

                        if (schemaOpen) {
                            // --- Tables group ---
                            ImGui::PushID("tables");
                            const bool tablesOpen = ImGui::TreeNodeEx("##n", baseFlags);
                            ImGui::SameLine();
                            ImGui::PushFont(nullptr, iconSize);
                            ImGui::TextColored(style::kIconGroup, ICON_FA_TABLE_LIST);
                            ImGui::PopFont();
                            ImGui::SameLine();
                            ImGui::TextUnformatted("Tables");
                            if (tablesOpen) {
                                for (auto &table: schema.tables) {
                                    // table layer
                                    ImGui::PushID(table.name.c_str());
                                    auto tmpFlags = leafFlags;
                                    if (keyMatches(m_selectedKey, 't', table.name))
                                        tmpFlags |= ImGuiTreeNodeFlags_Selected;
                                    ImGui::TreeNodeEx("##n", tmpFlags);
                                    if (ImGui::IsItemClicked()) {
                                        m_selectedKey = "t:";
                                        m_selectedKey += std::to_string(session.id);
                                        m_selectedKey += table.name;
                                    }
                                    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                                        onOpenTable(session.id, schema.name, table.name);
                                    ImGui::SameLine();
                                    ImGui::PushFont(nullptr, iconSize);
                                    ImGui::TextColored(style::kIconTable, ICON_FA_TABLE);
                                    ImGui::PopFont();
                                    ImGui::SameLine();
                                    ImGui::TextUnformatted(table.name.c_str());
                                    ImGui::PopID();
                                }
                                ImGui::TreePop();
                            }
                            ImGui::PopID(); // "tables"

                            // --- Views group ---
                            ImGui::PushID("views");
                            const bool viewsOpen = ImGui::TreeNodeEx("##n", baseFlags);
                            ImGui::SameLine();
                            ImGui::PushFont(nullptr, iconSize);
                            ImGui::TextColored(style::kIconGroup, ICON_FA_TABLE_COLUMNS);
                            ImGui::PopFont();
                            ImGui::SameLine();
                            ImGui::TextUnformatted("Views");
                            if (viewsOpen) {
                                for (auto &view: schema.views) {
                                    // views layer
                                    ImGui::PushID(view.c_str());
                                    auto tmpFlags = leafFlags;
                                    if (keyMatches(m_selectedKey, 'v', view))
                                        tmpFlags |= ImGuiTreeNodeFlags_Selected;
                                    ImGui::TreeNodeEx("##n", tmpFlags);
                                    if (ImGui::IsItemClicked()) {
                                        m_selectedKey = "v:";
                                        m_selectedKey += std::to_string(session.id);
                                        m_selectedKey += view;
                                    }
                                    ImGui::SameLine();
                                    ImGui::PushFont(nullptr, iconSize);
                                    ImGui::TextColored(style::kIconView, ICON_FA_EYE);
                                    ImGui::PopFont();
                                    ImGui::SameLine();
                                    ImGui::TextUnformatted(view.c_str());
                                    ImGui::PopID();
                                }
                                ImGui::TreePop();
                            }
                            ImGui::PopID(); // "views"

                            // --- Functions group ---
                            ImGui::PushID("functions");
                            const bool funcsOpen = ImGui::TreeNodeEx("##n", baseFlags);
                            ImGui::SameLine();
                            ImGui::PushFont(nullptr, iconSize);
                            ImGui::TextColored(style::kIconGroup, ICON_FA_GEARS);
                            ImGui::PopFont();
                            ImGui::SameLine();
                            ImGui::TextUnformatted("Functions");
                            if (funcsOpen) {
                                for (auto &func: schema.functions) {
                                    // functions layer
                                    ImGui::PushID(func.c_str());
                                    auto tmpFlags = leafFlags;
                                    if (keyMatches(m_selectedKey, 'f', func))
                                        tmpFlags |= ImGuiTreeNodeFlags_Selected;
                                    ImGui::TreeNodeEx("##n", tmpFlags);
                                    if (ImGui::IsItemClicked()) {
                                        m_selectedKey = "f:";
                                        m_selectedKey += std::to_string(session.id);
                                        m_selectedKey += func;
                                    }
                                    ImGui::SameLine();
                                    ImGui::PushFont(nullptr, iconSize);
                                    ImGui::TextColored(style::kIconFunction, ICON_FA_GEAR);
                                    ImGui::PopFont();
                                    ImGui::SameLine();
                                    ImGui::TextUnformatted(func.c_str());
                                    ImGui::PopID();
                                }
                                ImGui::TreePop();
                            }
                            ImGui::PopID(); // "functions"

                            ImGui::TreePop(); // schema content
                        }
                        ImGui::PopID(); // schema label
                    }
                    ImGui::TreePop(); // db content
                }
                ImGui::PopID(); // db label
            }
            ImGui::TreePop(); // server content
        }
        ImGui::PopID(); // server label
    }
    ImGui::PopStyleVar();

    ImGui::PopItemWidth();

    ImGui::EndChild();
}
