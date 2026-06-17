#include "app/Application.h"

#include "db/postgres/PostgresConnection.h"
#include "ui/NewConnectionPopup.h"
#include "ui/DocumentTabs.h"
#include "ui/Controls.h"
#include "ui/dark_style.h"

#include <imgui.h>
#include <algorithm>

#include "IconsFontAwesome6.h"

namespace app {
    Application::Application() {
        AddQueryTab();
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

        m_sideBar.Draw([this](const std::string &schema, const std::string &table) {
            OpenTableTab(schema, table);
        });

        ImGui::SameLine();

        const float schemaPanelWidth = ui::controls::getDpiSize(300.0f);
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

        if (const auto [action, config] = m_connectionPopup.Draw(); action == ui::PopupAction::Save) {
            // TODO: save to disk
            if (const int id = m_connections.OpenSession(config); id != -1) {
                m_activeSessionId = id;
                RefreshSchema();
            }
        }
    }

    void Application::DrawToolbar() {
        if (ui::controls::IconButton("Connect", ICON_FA_PLUG, ui::style::kToolbarConnect))
            m_connectionPopup.open();

        ImGui::SameLine();
        if (ui::controls::IconButton("Disconnect", ICON_FA_PLUG_CIRCLE_XMARK, ui::style::kToolbarDisconnect)) {
        }

        ImGui::SameLine();
        const float btnH = ui::controls::getDpiSize(25.0f);
        const float textH = ImGui::GetTextLineHeight();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (btnH - textH) * 0.5f);
        ImGui::TextUnformatted("|");

        ImGui::SameLine();
        if (ui::controls::IconButton("Refresh Schema", ICON_FA_ROTATE, ui::style::kToolbarRefresh))
            RefreshSchema();

        ImGui::SameLine();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (btnH - textH) * 0.5f);
        ImGui::TextUnformatted("|");

        ImGui::SameLine();
        if (ui::controls::IconButton("New Query", ICON_FA_FILE, ui::style::kToolbarNewQuery))
            AddQueryTab();

        ImGui::SameLine();
        if (ui::controls::IconButton("Open", ICON_FA_FOLDER_OPEN, ui::style::kToolbarOpen)) {
        }

        ImGui::SameLine();
        if (ui::controls::IconButton("Save", ICON_FA_FLOPPY_DISK, ui::style::kToolbarSave)) {
        }

        ImGui::SameLine();
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + (btnH - textH) * 0.5f);
        ImGui::TextUnformatted("|");

        ImGui::SameLine();
        if (ui::controls::IconButton("Import", ICON_FA_FILE_IMPORT, ui::style::kToolbarImport)) {
        }

        ImGui::SameLine();
        if (ui::controls::IconButton(" Export", ICON_FA_FILE_EXPORT, ui::style::kToolbarExport)) {
        }
    }

    // void Application::DrawSidebar() {
    //     ImGui::TextUnformatted("Database");
    //     ImGui::Separator();
    //
    //     const db::ConnectionSession *s = ActiveSession();
    //     if (!s || !s->IsConnected()) {
    //         ImGui::TextDisabled("Not connected");
    //         return;
    //     }
    //     if (s->schemas.empty()) {
    //         ImGui::TextDisabled("No tables found");
    //         return;
    //     }
    //
    //     for (auto &[name, tables]: s->schemas) {
    //         ImGuiTreeNodeFlags schemaFlags = ImGuiTreeNodeFlags_SpanAvailWidth;
    //         if (name == "public")
    //             schemaFlags |= ImGuiTreeNodeFlags_DefaultOpen;
    //
    //         if (ImGui::TreeNodeEx(name.c_str(), schemaFlags)) {
    //             for (auto &table: tables) {
    //                 ImGuiTreeNodeFlags tableFlags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen
    //                                                 | ImGuiTreeNodeFlags_SpanAvailWidth;
    //                 if (name == m_selectedSchema && table.name == m_selectedTable)
    //                     tableFlags |= ImGuiTreeNodeFlags_Selected;
    //
    //                 ImGui::TreeNodeEx(table.name.c_str(), tableFlags);
    //
    //                 if (ImGui::IsItemClicked()) {
    //                     m_selectedSchema = name;
    //                     m_selectedTable = table.name;
    //                     LoadTableColumns(s->connection, table);
    //                 }
    //                 if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
    //                     OpenTableTab(name, table.name);
    //             }
    //             ImGui::TreePop();
    //         }
    //     }
    // }

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

        // TODO: remove fake data when real query is ready
        tab.result.success = true;
        tab.result.columns = { "id " ICON_FA_KEY, "username", "email", "role", "created_at", "is_active", "age", "balance" };
        tab.result.rows = {
            { "1",  "alice",        "alice@example.com",   "admin",  "2024-01-15 09:00:00", "true",  "28", "1500.00"  },
            { "2",  "bob",          "bob@example.com",     "user",   "2024-02-20 14:30:00", "true",  "34", "320.50"   },
            { "3",  "carol",        "carol@example.com",   "user",   "2024-03-05 11:15:00", "false", "22", "0.00"     },
            { "4",  "dave",         "dave@example.com",    "editor", "2024-03-18 08:45:00", "true",  "41", "870.75"   },
            { "5",  "eve",          "eve@example.com",     "user",   "2024-04-01 16:00:00", "true",  "29", "150.25"   },
            { "6",  "frank",        "frank@example.com",   "admin",  "2024-04-10 10:20:00", "true",  "37", "4200.00"  },
            { "7",  "grace",        "grace@example.com",   "user",   "2024-05-02 13:00:00", "false", "25", "0.00"     },
            { "8",  "heidi",        "heidi@example.com",   "editor", "2024-05-14 09:30:00", "true",  "31", "990.10"   },
            { "9",  "ivan",         "ivan@example.com",    "user",   "2024-06-01 17:45:00", "true",  "45", "55.00"    },
            { "10", "judy",         "judy@example.com",    "user",   "2024-06-20 12:00:00", "true",  "27", "2100.80"  },
            { "11", "karl",         "karl@example.com",    "editor", "2024-07-03 08:00:00", "false", "33", "0.00"     },
            { "12", "laura",        "laura@example.com",   "user",   "2024-07-15 15:30:00", "true",  "38", "430.60"   },
            { "13", "mallory",      "mallory@example.com", "user",   "2024-08-08 10:10:00", "true",  "23", "75.00"    },
            { "14", "川崎さん",         "niaj@example.com",    "admin",  "2024-08-22 11:00:00", "true",  "50", "9800.00"  },
            { "15", "olivia",       "olivia@example.com",  "user",   "2024-09-05 14:00:00", "true",  "26", "310.40"   },
            { "16", "peggy",        "peggy@example.com",   "editor", "2024-09-18 09:15:00", "false", "32", "0.00"     },
            { "17", "rupert",       "rupert@example.com",  "user",   "2024-10-01 16:45:00", "true",  "44", "620.00"   },
            { "18", "sybil",        "sybil@example.com",   "user",   "2024-10-14 13:30:00", "true",  "30", "180.90"   },
            { "19", "trent",        "trent@example.com",   "editor", "2024-11-02 08:20:00", "true",  "36", "750.00"   },
            { "20", "victor",       "victor@example.com",  "user",   "2024-11-20 10:00:00", "false", "24", "0.00"     },
        };
        tab.statusMessage = std::to_string(tab.result.RowCount()) + " row(s) [fake data]";

        m_tabs.push_back(std::move(tab));
        // RunTableTab(m_tabs.back()); // TODO: re-enable when connected
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
