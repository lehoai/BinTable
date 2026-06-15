#pragma once
#include <memory>
#include <vector>

#include "ConnectionConfig.h"
#include "IConnection.h"
#include "SchemaInfo.h"

namespace db {
    struct ConnectionSession {
        int id = 0;
        ConnectionConfig config;
        std::unique_ptr<IConnection> connection;
        std::vector<SchemaInfo> schemas;
        std::string statusMessage;

        [[nodiscard]] bool IsConnected() const {
            return connection && connection->IsConnected();
        }
    };

    class ConnectionManager {
    public:
        [[nodiscard]] const std::vector<ConnectionConfig> &profiles() const { return m_profiles; }
        void SaveProfile(const ConnectionConfig &config) { m_profiles.push_back(config); }

        int OpenSession(const ConnectionConfig &config);

        void CloseSession(int id);

        ConnectionSession *Find(int id);

        [[nodiscard]] const std::vector<std::unique_ptr<ConnectionSession> > &GetSessions() const { return m_sessions; }

    private:
        std::vector<ConnectionConfig> m_profiles;
        std::vector<std::unique_ptr<ConnectionSession> > m_sessions;
        int m_nextId = 1;
    };
}
