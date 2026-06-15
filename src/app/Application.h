#pragma once

#include "db/IConnection.h"
#include "db/SchemaInfo.h"
#include "ui/DocumentTab.h"
#include <string>
#include <vector>

#include "db/ConnectionManager.h"
#include "ui/NewConnectionPopup.h"

namespace app {

    class Application {
    public:
        Application();

        void RenderUI();

    private:
        void RefreshSchema();

        db::ConnectionSession* ActiveSession();;

        static void LoadTableColumns(db::IConnection &conn, db::TableInfo &table);

        void DrawToolbar();

        void DrawSidebar();

        void DrawSchemaPanel() const;

        void AddQueryTab();

        void OpenTableTab(const std::string &schemaName, const std::string &tableName);

        void RunQueryTab(ui::DocumentTab &tab);

        void RunTableTab(ui::DocumentTab &tab);

        db::ConnectionManager m_connections;
        int m_activeSessionId = -1;

        ui::NewConnectionPopup m_connectionPopup;

        std::string m_selectedSchema;
        std::string m_selectedTable;

        std::vector<ui::DocumentTab> m_tabs;
        int m_nextTabId = 1;
    };
} // namespace app
