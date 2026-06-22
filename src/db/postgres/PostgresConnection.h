#pragma once

#include "db/IConnection.h"

struct pg_conn; // forward declaration of libpq's opaque PGconn

namespace db::postgres {
    class PostgresConnection final : public IConnection {
    public:
        PostgresConnection() = default;

        ~PostgresConnection() override;

        PostgresConnection(const PostgresConnection &) = delete;

        PostgresConnection &operator=(const PostgresConnection &) = delete;

        bool Connect(const ConnectionConfig &config) override;

        void Disconnect() override;

        [[nodiscard]] bool IsConnected() const override;

        QueryResult Execute(const std::string &sql) override;

        std::vector<std::string> LoadDatabases() override;

        std::vector<std::string> LoadSchemas() override;

        std::vector<std::string> LoadTables(std::string schema) override;

        [[nodiscard]] const std::string &LastError() const override;

    private:

        std::vector<std::string> loadMeta(const char* sql);

        pg_conn *m_conn = nullptr;
        std::string m_lastError;
    };
} // namespace db::postgres
