#pragma once

#include <future>

#include "db/ConnectionConfig.h"
#include "db/IConnection.h"
#include "db/SchemaInfo.h"
#include "ui/DocumentTab.h"
#include <memory>
#include <string>
#include <vector>

namespace app {
    struct TestConnectionResult {
        bool success = false;
        std::string message;
    };

    class Application {
    public:
        Application();

        void RenderUI();

    private:
        void Connect();

        void Disconnect();

        void RefreshSchema();

        void LoadTableColumns(db::TableInfo &table) const;

        void DrawToolbar();

        void DrawSidebar();

        void DrawSchemaPanel() const;

        void DrawConnectionPopup();

        void AddQueryTab();

        void OpenTableTab(const std::string &schemaName, const std::string &tableName);

        void RunQueryTab(ui::DocumentTab &tab) const;

        void RunTableTab(ui::DocumentTab &tab) const;

        std::unique_ptr<db::IConnection> m_connection;
        db::ConnectionConfig m_connectionConfig;
        std::string m_statusMessage;
        bool m_showConnectionPopup = false;

        std::vector<db::SchemaInfo> m_schemas;
        std::string m_selectedSchema;
        std::string m_selectedTable;

        std::vector<ui::DocumentTab> m_tabs;
        int m_nextTabId = 1;

        bool m_isConnecting = false;
        std::future<TestConnectionResult> m_future;
    };
} // namespace app
