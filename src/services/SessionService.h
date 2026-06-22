#pragma once
#include <future>
#include <optional>

#include "db/ConnectionConfig.h"
#include "db/IConnection.h"
#include "db/SchemaInfo.h"

namespace services {
    enum class SessionStatus { Connecting, Connected, Failed };

    struct Session {
        int id = 0;
        db::ConnectionConfig config;
        std::unique_ptr<db::IConnection> connection;
        std::vector<db::DatabaseInfo> databases;
        SessionStatus status = SessionStatus::Connecting;
        std::string statusMessage;

        [[nodiscard]] bool IsConnected() const {
            return connection && connection->IsConnected();
        }
    };

    struct TestResult {
        bool success = false;
        std::string message;
    };

    class SessionService {
    public:
        // ---- Profiles (saved connection configs)
        [[nodiscard]] const std::vector<db::ConnectionConfig> &Profiles() const { return m_profiles; }
        void SaveProfile(const db::ConnectionConfig &config) { m_profiles.push_back(config); }

        // ---- Connections (Multi-sessions)
        void ConnectAsync(const db::ConnectionConfig &config);

        void Disconnect(int sessionId);

        [[nodiscard]] const std::vector<Session> &Sessions() const { return m_sessions; }

        // ---- call each frame from Application, check for all running connection
        void Poll();

        // ---- Test new connection
        void TestAsync(const db::ConnectionConfig &config);

        std::optional<TestResult> PollTestResult();

        [[nodiscard]] bool IsTesting() const { return m_testing; }

        // ---- Metadata query
        void LoadSchemasAsync(int sessionId, const std::string &dbName);

        void LoadTablesAsync(int sessionId, const std::string &dbName, const std::string &schemaName);

        db::QueryResult Execute(int sessionId, const std::string &sql);

    private:
        struct ConnectOutcome {
            bool ok = false;
            std::string error;
            std::unique_ptr<db::IConnection> connection;
            std::vector<db::DatabaseInfo> databases;
        };

        struct ConnectPending {
            int sessionId = 0;
            std::future<ConnectOutcome> future;
        };

        struct SchemaOutcome {
            bool ok = false;
            std::string error;
            std::vector<db::SchemaInfo> schemas;
        };

        struct SchemaPending {
            int sessionId = 0;
            std::string dbName;
            std::future<SchemaOutcome> future;
        };

        struct TableOutcome {
            bool ok = false;
            std::string error;
            std::vector<db::TableInfo> tables;
        };

        struct TablePending {
            int sessionId = 0;
            std::string dbName;
            std::string schemaName;
            std::future<TableOutcome> future;
        };

        Session *Find(int id);

        std::vector<db::ConnectionConfig> m_profiles;
        std::vector<Session> m_sessions;
        int m_nextId = 1;

        ConnectPending m_connectionPending;
        SchemaPending m_schemaPending;
        TablePending m_tablePending;

        std::future<TestResult> m_TestFuture;
        bool m_testing = false;
    };
}
