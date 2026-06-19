#include "db/postgres/PostgresConnection.h"

#include <libpq-fe.h>

#include <sstream>

namespace db::postgres {
    const char *keywords[] = {"host", "port", "dbname", "user", "password", "connect_timeout", nullptr};

    PostgresConnection::~PostgresConnection() {
        Disconnect();
    }

    bool PostgresConnection::Connect(const ConnectionConfig &config) {
        Disconnect();

        const char *values[] = {
            config.host.c_str(), config.port.c_str(), config.database.empty() ? "postgres" : config.database.c_str(),
            config.user.c_str(), config.password.c_str(), "5", nullptr
        };
        m_conn = PQconnectdbParams(keywords, values, 0);

        if (PQstatus(m_conn) != CONNECTION_OK) {
            m_lastError = PQerrorMessage(m_conn);
            Disconnect();
            return false;
        }

        m_lastError.clear();
        return true;
    }

    void PostgresConnection::Disconnect() {
        if (m_conn) {
            PQfinish(m_conn);
            m_conn = nullptr;
        }
    }

    bool PostgresConnection::IsConnected() const {
        return m_conn != nullptr && PQstatus(m_conn) == CONNECTION_OK;
    }

    QueryResult PostgresConnection::Execute(const std::string &sql) {
        QueryResult result;

        if (!IsConnected()) {
            result.error = "Not connected";
            return result;
        }

        PGresult *res = PQexec(m_conn, sql.c_str());
        const ExecStatusType status = PQresultStatus(res);

        if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK) {
            result.error = PQresultErrorMessage(res);
            m_lastError = result.error;
            PQclear(res);
            return result;
        }

        const int columnCount = PQnfields(res);
        for (int col = 0; col < columnCount; ++col)
            result.columns.emplace_back(PQfname(res, col));

        const int rowCount = PQntuples(res);
        result.rows.reserve(static_cast<size_t>(rowCount));
        for (int row = 0; row < rowCount; ++row) {
            std::vector<std::string> rowValues;
            rowValues.reserve(static_cast<size_t>(columnCount));
            for (int col = 0; col < columnCount; ++col) {
                if (PQgetisnull(res, row, col))
                    rowValues.emplace_back("NULL");
                else
                    rowValues.emplace_back(PQgetvalue(res, row, col));
            }
            result.rows.push_back(std::move(rowValues));
        }

        PQclear(res);
        result.success = true;
        return result;
    }

    const std::string &PostgresConnection::LastError() const {
        return m_lastError;
    }

    std::vector<std::string> PostgresConnection::LoadDatabases() {
        std::vector<std::string> result;

        if (!IsConnected()) {
            m_lastError = "Not connected";
            return result;
        }

        PGresult *res = PQexec(m_conn,
                               "SELECT datname FROM pg_database "
                               "WHERE datistemplate = false ORDER BY datname;"
        );

        if (PQresultStatus(res) != PGRES_TUPLES_OK) {
            m_lastError = PQresultErrorMessage(res);
            PQclear(res);
            return result;
        }

        const int rowCount = PQntuples(res);
        result.reserve(rowCount);
        for (int row = 0; row < rowCount; ++row) {
            result.emplace_back(PQgetvalue(res, row, 0));
        }

        PQclear(res);

        return result;
    }
} // namespace db::postgres
