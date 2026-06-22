#include "SessionService.h"

#include <iostream>

#include "db/ConnectionFactory.h"

void services::SessionService::ConnectAsync(const db::ConnectionConfig &config) {
    Session s;
    s.id = m_nextId++;
    s.config = config;
    s.status = SessionStatus::Connecting;
    s.statusMessage = "Connecting...";
    const int id = s.id;
    m_sessions.push_back(std::move(s));

    m_connectionPending.sessionId = id;
    m_connectionPending.future = std::async(std::launch::async, [config] {
        ConnectOutcome out;
        auto conn = db::CreateConnection(config.dbType);
        if (!conn) {
            out.error = "Unsupported database type";
            return out;
        }
        if (!conn->Connect(config)) {
            out.error = conn->LastError();
            return out;
        }
        // TODO: load databases
        // out.schemas = LoadSchemas();
        for (auto databases = conn->LoadDatabases(); auto &d: databases) {
            out.databases.emplace_back(std::move(d));
        }
        out.connection = std::move(conn);
        out.ok = true;

        return out;
    });
}

void services::SessionService::Disconnect(int sessionId) {
    if (const Session *s = Find(sessionId); s && s->connection) {
        s->connection->Disconnect();
    }
    std::erase_if(m_sessions, [sessionId](const Session &s) { return s.id == sessionId; });
}

void services::SessionService::Poll() {
    // poll for load connection pending
    if (m_connectionPending.future.valid() &&
        m_connectionPending.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        auto [ok, error, connection, databases] = m_connectionPending.future.get();
        if (Session *s = Find(m_connectionPending.sessionId)) {
            if (ok) {
                s->connection = std::move(connection);
                s->databases = std::move(databases);
                s->status = SessionStatus::Connected;
                s->statusMessage = "Connected";
            } else {
                s->status = SessionStatus::Failed;
                s->statusMessage = error;
                // TODO: display error to UI
                std::cout << error << std::endl;
            }
        }
    }

    // poll for load schema pending
    if (m_schemaPending.future.valid()
        && m_schemaPending.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        auto [ok, error, schemas] = m_schemaPending.future.get();
        if (Session *s = Find(m_schemaPending.sessionId)) {
            const auto db = std::ranges::find_if(s->databases,
                                                 [&](const db::DatabaseInfo &d) {
                                                     return d.name == m_schemaPending.dbName;
                                                 });
            if (ok) {
                if (db != s->databases.end()) {
                    db->schemas = std::move(schemas);
                    db->schemaState = db::LoadState::Loaded;
                }
            } else {
                s->status = SessionStatus::Failed;
                s->statusMessage = error;
                db->schemaState = db::LoadState::Failed;
                // TODO: display error to UI
                std::cout << error << std::endl;
            }
        }
    }

    // poll for load table pending
    if (m_tablePending.future.valid()
        && m_tablePending.future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        auto [ok, error, table] = m_tablePending.future.get();
        if (Session *s = Find(m_tablePending.sessionId)) {
            const auto db = std::ranges::find_if(s->databases,
                                                 [&](const db::DatabaseInfo &d) {
                                                     return d.name == m_tablePending.dbName;
                                                 });
            const auto schema = std::ranges::find_if(db->schemas,
                                                     [&](const db::SchemaInfo &si) {
                                                         return si.name == m_tablePending.schemaName;
                                                     });
            if (ok && schema != db->schemas.end()) {
                schema->tables = std::move(table);
                schema->tableState = db::LoadState::Loaded;
            } else {
                s->status = SessionStatus::Failed;
                s->statusMessage = error;
                schema->tableState = db::LoadState::Failed;
                // TODO: display error to UI
                std::cout << error << std::endl;
            }
        }
    }
}

void services::SessionService::TestAsync(const db::ConnectionConfig &config) {
    m_testing = true;
    m_TestFuture = std::async(std::launch::async, [config]() {
        TestResult r;
        const auto conn = db::CreateConnection(config.dbType);
        // create new connection every request cuz this is a test
        if (!conn) {
            r.message = "Unsupported database type";
            return r;
        }
        r.success = conn->Connect(config);
        r.message = r.success ? "Connected" : conn->LastError();
        return r;
    });
}

std::optional<services::TestResult> services::SessionService::PollTestResult() {
    if (!m_testing) return std::nullopt;
    if (m_TestFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
        return std::nullopt;
    }
    m_testing = false;
    return m_TestFuture.get();
}

void services::SessionService::LoadSchemasAsync(const int sessionId, const std::string &dbName) {
    Session *s = Find(sessionId);
    if (!s) return;

    const auto it = std::ranges::find_if(s->databases,
                                         [&](const db::DatabaseInfo &d) { return d.name == dbName; }
    );
    if (it == s->databases.end() || it->schemaState != db::LoadState::NotLoaded) {
        return;
    }
    it->schemaState = db::LoadState::Loading;

    m_schemaPending.sessionId = sessionId;
    m_schemaPending.dbName = dbName;
    m_schemaPending.future = std::async(std::launch::async, [s] {
        SchemaOutcome out;
        for (auto schemas = s->connection->LoadSchemas(); auto &ss: schemas) {
            out.schemas.emplace_back(std::move(ss));
        }
        out.ok = true;
        return out;
    });
}

void services::SessionService::LoadTablesAsync(const int sessionId, const std::string &dbName,
                                               const std::string &schemaName) {
    Session *s = Find(sessionId);
    if (!s) return;

    const auto it = std::ranges::find_if(s->databases,
                                         [&](const db::DatabaseInfo &d) { return d.name == dbName; }
    );

    const auto schema = std::ranges::find_if(it->schemas, [&](const db::SchemaInfo &ss) {
        return ss.name == schemaName;
    });

    if (schema == it->schemas.end() || schema->tableState != db::LoadState::NotLoaded) {
        return;
    }

    m_tablePending.sessionId = sessionId;
    m_tablePending.dbName = dbName;
    m_tablePending.schemaName = schemaName;
    m_tablePending.future = std::async(std::launch::async, [schemaName, s] {
        TableOutcome out;
        for (auto tables = s->connection->LoadTables(schemaName); auto &ss: tables) {
            out.tables.emplace_back(schemaName, std::move(ss));
        }
        out.ok = true;
        return out;
    });
}

db::QueryResult services::SessionService::Execute(const int sessionId, const std::string &sql) {
    const Session *s = Find(sessionId);
    if (!s || !s->connection) {
        db::QueryResult r;
        r.error = "Not connected";
        r.success = false;
        return r;
    }
    return s->connection->Execute(sql);
}

services::Session *services::SessionService::Find(int id) {
    const auto it = std::ranges::find_if(m_sessions, [id](const Session &s) { return s.id == id; });
    return it == m_sessions.end() ? nullptr : &*it;
}
