#pragma once

#include "db/IConnection.h"
#include "db/SchemaInfo.h"
#include <string>
#include <vector>

#include "db/ConnectionManager.h"
#include "ui/NewConnectionPopup.h"
#include "ui/SideBar/SideBar.h"
#include "../ui/DocumentTab/DocumentTab.h"

namespace app {

    class Application {
    public:
        Application();

        void RenderUI();

    private:
        void RefreshSchema();

        static void LoadTableColumns(db::IConnection &conn, db::TableInfo &table);

        void DrawToolbar();

        void DrawSchemaPanel() const;

        void AddQueryTab();

        void OpenTableTab(const std::string &schemaName, const std::string &tableName);

        void RunQueryTab(ui::DocumentData &tab);

        void RunTableTab(ui::DocumentData &tab);

        // ========================== services =========================
        db::ConnectionManager m_connections;
        int m_activeSessionId = -1;

        // ========================== child UI =========================
        ui::NewConnectionPopup m_connectionPopup;
        ui::SideBar m_sideBar;
        ui::DocumentTab m_documentTab;

        // ========================== state =========================
        std::string m_selectedSchema;
        std::string m_selectedTable;

        std::vector<ui::DocumentData> m_tabs;
        int m_nextTabId = 1;
    };
} // namespace app
