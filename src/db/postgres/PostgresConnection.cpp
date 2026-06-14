#include "db/postgres/PostgresConnection.h"

#include <libpq-fe.h>

#include <sstream>

namespace db::postgres {

PostgresConnection::~PostgresConnection()
{
    Disconnect();
}

bool PostgresConnection::Connect(const ConnectionConfig& config)
{
    Disconnect();

    std::ostringstream conninfo;
    conninfo << "host=" << config.host
             << " port=" << config.port
             << " dbname=" << config.database
             << " user=" << config.user
             << " password=" << config.password
             << " connect_timeout=5";

    m_conn = PQconnectdb(conninfo.str().c_str());

    if (PQstatus(m_conn) != CONNECTION_OK)
    {
        m_lastError = PQerrorMessage(m_conn);
        Disconnect();
        return false;
    }

    m_lastError.clear();
    return true;
}

void PostgresConnection::Disconnect()
{
    if (m_conn)
    {
        PQfinish(m_conn);
        m_conn = nullptr;
    }
}

bool PostgresConnection::IsConnected() const
{
    return m_conn != nullptr && PQstatus(m_conn) == CONNECTION_OK;
}

QueryResult PostgresConnection::Execute(const std::string& sql)
{
    QueryResult result;

    if (!IsConnected())
    {
        result.error = "Not connected";
        return result;
    }

    PGresult* res = PQexec(m_conn, sql.c_str());
    const ExecStatusType status = PQresultStatus(res);

    if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK)
    {
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
    for (int row = 0; row < rowCount; ++row)
    {
        std::vector<std::string> rowValues;
        rowValues.reserve(static_cast<size_t>(columnCount));
        for (int col = 0; col < columnCount; ++col)
        {
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

const std::string& PostgresConnection::LastError() const
{
    return m_lastError;
}

} // namespace db::postgres
