#include "app/Application.h"

#include "db/postgres/PostgresConnection.h"
#include "ui/NewConnectionPopup.h"
#include "ui/DocumentTabs.h"
#include "ui/Controls.h"

#include <imgui.h>
#include <algorithm>

namespace app {
    Application::Application() {
        AddQueryTab();
    }

    db::ConnectionSession *Application::ActiveSession() {
        return m_connections.Find(m_activeSessionId);
    }

    void Application::RenderUI() {
        const ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        constexpr ImGuiWindowFlags hostFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
                                               | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                               ImGuiWindowFlags_NoBringToFrontOnFocus
                                               | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoSavedSettings;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::Begin("##amgui_main", nullptr, hostFlags);
        ImGui::PopStyleVar();

        DrawToolbar();
        ImGui::Separator();

        constexpr float sidebarWidth = 240.0f;
        constexpr float schemaPanelWidth = 300.0f;

        ImGui::BeginChild("##sidebar", ImVec2(sidebarWidth, 0), ImGuiChildFlags_Borders);
        DrawSidebar();
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("##center", ImVec2(-schemaPanelWidth, 0), ImGuiChildFlags_Borders);
        ui::DrawDocumentTabs(m_tabs, true,
                             [this](ui::DocumentTab &tab) { RunQueryTab(tab); },
                             [this](ui::DocumentTab &tab) { RunTableTab(tab); },
                             [this] { AddQueryTab(); });
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("##schema", ImVec2(0, 0), ImGuiChildFlags_Borders);
        DrawSchemaPanel();
        ImGui::EndChild();

        ImGui::End();

        const auto [action, config] = m_connectionPopup.Draw();
        if (action == ui::PopupAction::Save) {
            // TODO: save to disk
            const int id = m_connections.OpenSession(config);
            if (id != -1) {
                m_activeSessionId = id;
                RefreshSchema();
            }
        }
    }

    void Application::DrawToolbar() {
        if (ui::controls::Button("Connect", 110.0f))
            m_connectionPopup.open();

        ImGui::SameLine();
        if (ui::controls::Button("Disconnect", 110.0f)) {
        }

        ImGui::SameLine();
        if (ui::controls::Button("Refresh Schema", 110.0f))
            RefreshSchema();

        ImGui::SameLine();
        ImGui::TextUnformatted("|");

        ImGui::SameLine();
        if (ui::controls::Button("New Query", 90.0f))
            AddQueryTab();

        ImGui::SameLine();
        if (ui::controls::Button("Open", 90.0f)) {
        }

        ImGui::SameLine();
        if (ui::controls::Button("Save", 90.0f)) {
        }

        ImGui::SameLine();
        ImGui::TextUnformatted("|");

        ImGui::SameLine();
        if (ui::controls::Button("Import", 90.0f)) {
        }

        ImGui::SameLine();
        if (ui::controls::Button("Export", 90.0f)) {
        }


        // if (isConnected)
        //     ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "Connected: %s@%s:%s/%s",
        //                        m_connectionPopup.m_config.user.c_str(), m_connectionPopup.m_config.host.c_str(),
        //                        m_connectionPopup.m_config.port.c_str(), m_connectionPopup.m_config.database.c_str());
        // else
        //     ImGui::TextDisabled("%s", m_statusMessage.empty() ? "Not connected" : m_statusMessage.c_str());
    }

    void Application::DrawSidebar() {
        ImGui::TextUnformatted("Database");
        ImGui::Separator();

        const db::ConnectionSession *s = ActiveSession();
        if (!s || !s->IsConnected()) {
            ImGui::TextDisabled("Not connected");
            return;
        }
        if (s->schemas.empty()) {
            ImGui::TextDisabled("No tables found");
            return;
        }

        // for (auto &[name, tables]: s->schemas) {
        //     ImGuiTreeNodeFlags schemaFlags = ImGuiTreeNodeFlags_SpanAvailWidth;
        //     if (name == "public")
        //         schemaFlags |= ImGuiTreeNodeFlags_DefaultOpen;
        //
        //     if (ImGui::TreeNodeEx(name.c_str(), schemaFlags)) {
        //         for (auto &table: tables) {
        //             ImGuiTreeNodeFlags tableFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen
        //                                             | ImGuiTreeNodeFlags_SpanAvailWidth;
        //             if (name == m_selectedSchema && table.name == m_selectedTable)
        //                 tableFlags |= ImGuiTreeNodeFlags_Selected;
        //
        //             ImGui::TreeNodeEx(table.name.c_str(), tableFlags);
        //
        //             if (ImGui::IsItemClicked()) {
        //                 m_selectedSchema = name;
        //                 m_selectedTable = table.name;
        //                 LoadTableColumns(s->connection, table);
        //             }
        //             if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
        //                 OpenTableTab(name, table.name);
        //         }
        //         ImGui::TreePop();
        //     }
        // }
    }

    void Application::DrawSchemaPanel() const {
        ImGui::TextUnformatted("Schema");
        ImGui::Separator();

        if (m_selectedTable.empty()) {
            ImGui::TextDisabled("Select a table to inspect its columns");
            return;
        }

        ImGui::TextUnformatted((m_selectedSchema + "." + m_selectedTable).c_str());
        ImGui::Separator();

        // for (auto &schema: m_schemas) {
        //     if (schema.name != m_selectedSchema)
        //         continue;
        //
        //     for (auto &table: schema.tables) {
        //         if (table.name != m_selectedTable)
        //             continue;
        //
        //         if (!table.columnsLoaded) {
        //             ImGui::TextDisabled("Loading columns...");
        //             return;
        //         }
        //
        //         if (constexpr ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
        //             ImGui::BeginTable("##columns", 3, flags)) {
        //             ImGui::TableSetupColumn("Column");
        //             ImGui::TableSetupColumn("Type");
        //             ImGui::TableSetupColumn("Null");
        //             ImGui::TableHeadersRow();
        //
        //             for (const auto &column: table.columns) {
        //                 ImGui::TableNextRow();
        //                 ImGui::TableNextColumn();
        //                 ImGui::TextUnformatted(column.name.c_str());
        //                 ImGui::TableNextColumn();
        //                 ImGui::TextUnformatted(column.dataType.c_str());
        //                 ImGui::TableNextColumn();
        //                 ImGui::TextUnformatted(column.nullable ? "yes" : "no");
        //             }
        //
        //             ImGui::EndTable();
        //         }
        //         return;
        //     }
        // }
    }

    // void Application::DrawConnectionPopup() {
    //     if (m_showConnectionPopup) {
    //         ImGui::OpenPopup("Connection##popup");
    //         m_showConnectionPopup = false;
    //     }
    //
    //     bool popupOpen = true;
    //     if (ImGui::BeginPopupModal("Connection##popup", &popupOpen, ImGuiWindowFlags_AlwaysAutoResize)) {
    //
    //         if (m_isConnecting) {
    //             if (m_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
    //                 auto [success, message] = m_future.get();
    //                 m_isConnecting = false;
    //                 m_statusMessage = message;
    //                 if (success) {
    //                     RefreshSchema();
    //                     ImGui::CloseCurrentPopup();
    //                     return;
    //                 }
    //             }
    //         }
    //
    //         ui::DrawConnectionForm(m_connectionConfig, m_connection->IsConnected(), m_statusMessage, m_isConnecting,
    //                                [this] { Connect(); },
    //                                [this] { Disconnect(); });
    //
    //         ImGui::EndPopup();
    //     }
    // }
    //
    // void Application::Connect() {
    //     m_isConnecting = true;
    //     m_future = std::async(std::launch::async, [this]() {
    //         TestConnectionResult result;
    //         result.success = m_connection->Connect(m_connectionConfig);
    //         result.message = result.success ? "Connected" : m_connection->LastError();
    //         return result;
    //     });
    // }

    void Application::RefreshSchema() {
        // m_schemas.clear();
        //
        // const db::QueryResult result = m_connection->Execute(
        //     "SELECT table_schema, table_name FROM information_schema.tables "
        //     "WHERE table_schema NOT IN ('pg_catalog', 'information_schema') "
        //     "ORDER BY table_schema, table_name;");
        //
        // if (!result.success) {
        //     m_statusMessage = result.error;
        //     return;
        // }
        //
        // for (const auto &row: result.rows) {
        //     const std::string &schemaName = row[0];
        //     const std::string &tableName = row[1];
        //
        //     auto it = std::ranges::find_if(m_schemas,
        //                                    [&](const db::SchemaInfo &schema) { return schema.name == schemaName; });
        //     if (it == m_schemas.end()) {
        //         m_schemas.push_back({schemaName, {}});
        //         it = m_schemas.end() - 1;
        //     }
        //
        //     db::TableInfo table;
        //     table.schema = schemaName;
        //     table.name = tableName;
        //     it->tables.push_back(std::move(table));
        // }
    }

    void Application::LoadTableColumns(db::IConnection &conn, db::TableInfo &table) {
        if (table.columnsLoaded)
            return;

        const std::string sql =
                "SELECT column_name, data_type, is_nullable FROM information_schema.columns "
                "WHERE table_schema = '" + table.schema + "' AND table_name = '" + table.name + "' "
                "ORDER BY ordinal_position;";

        const db::QueryResult result = conn.Execute(sql);
        if (!result.success)
            return;

        table.columns.clear();
        for (const auto &row: result.rows) {
            db::ColumnInfo column;
            column.name = row[0];
            column.dataType = row[1];
            column.nullable = (row[2] == "YES");
            table.columns.push_back(std::move(column));
        }
        table.columnsLoaded = true;
    }

    void Application::AddQueryTab() {
        ui::DocumentTab tab;
        tab.type = ui::TabType::Query;
        tab.id = m_nextTabId++;
        tab.title = "Query " + std::to_string(tab.id);
        tab.queryText = "SELECT 1;";
        tab.focusRequested = true;
        m_tabs.push_back(std::move(tab));
    }

    void Application::OpenTableTab(const std::string &schemaName, const std::string &tableName) {
        for (auto &tab: m_tabs) {
            if (tab.type == ui::TabType::TableView && tab.schemaName == schemaName && tab.tableName == tableName) {
                tab.focusRequested = true;
                return;
            }
        }

        ui::DocumentTab tab;
        tab.type = ui::TabType::TableView;
        tab.id = m_nextTabId++;
        tab.title = schemaName + "." + tableName;
        tab.schemaName = schemaName;
        tab.tableName = tableName;
        tab.focusRequested = true;
        m_tabs.push_back(std::move(tab));
        RunTableTab(m_tabs.back());
    }

    void Application::RunQueryTab(ui::DocumentTab &tab) {
        const db::ConnectionSession *s = m_connections.Find(tab.connectionId);
        if (!s || !s->IsConnected()) {
            tab.statusMessage = "No connection";
            return;
        }
        tab.result = s->connection->Execute(tab.queryText);
        tab.statusMessage = tab.result.success
                                ? ("OK - " + std::to_string(tab.result.RowCount()) + " row(s)")
                                : tab.result.error;
    }

    void Application::RunTableTab(ui::DocumentTab &tab) {
        const db::ConnectionSession *s = m_connections.Find(tab.connectionId);
        if (!s || !s->IsConnected()) {
            tab.statusMessage = "No connection";
            return;
        }
        const std::string sql = "SELECT * FROM \"" + tab.schemaName + "\".\"" + tab.tableName + "\" LIMIT 200;";
        tab.result = s->connection->Execute(sql);
        tab.statusMessage = tab.result.success
                                ? (std::to_string(tab.result.RowCount()) + " row(s)")
                                : tab.result.error;
    }
} // namespace app
