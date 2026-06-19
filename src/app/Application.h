#pragma once

#include <string>
#include <vector>

#include "db/IConnection.h"
#include "db/SchemaInfo.h"
#include "ui/NewConnectionPopup.h"
#include "ui/SideBar/SideBar.h"
#include "../ui/DocumentTab/DocumentTab.h"

namespace app {
    class Application {
    public:
        Application();

        void RenderUI();

    private:
        static void RefreshSchema();

        static void LoadTableColumns(db::IConnection &conn, db::TableInfo &table);

        void DrawToolbar();

        static void DrawSchemaPanel();

        void AddQueryTab();

        void OpenTableTab(int sessionId, const std::string &schemaName, const std::string &tableName);

        void RunQueryTab(ui::DocumentData &tab);

        void RunTableTab(ui::DocumentData &tab);

        // ========================== services =========================
        services::SessionService m_sessionService;
        int m_currentSessionId = -1; // UI state

        // ========================== child UI =========================
        ui::NewConnectionPopup m_connectionPopup{m_sessionService};
        ui::SideBar m_sideBar{m_sessionService};
        ui::DocumentTab m_documentTab;

        std::vector<ui::DocumentData> m_tabs;
        int m_nextTabId = 1;
    };
} // namespace app
