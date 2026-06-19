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

    Pending p;
    p.sessionId = id;
    p.future = std::async(std::launch::async, [config] {
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
        for (auto databases = conn->LoadDatabases(); auto& d : databases) {
            out.databases.emplace_back(std::move(d));
        }
        out.connection = std::move(conn);
        out.ok = true;

        return out;
    });
    m_pending.push_back(std::move(p));
}

void services::SessionService::Disconnect(int sessionId) {
    if (const Session *s = Find(sessionId); s && s->connection) {
        s->connection->Disconnect();
    }
    std::erase_if(m_sessions, [sessionId](const Session &s) { return s.id == sessionId; });
}

void services::SessionService::Poll() {
    for (auto it = m_pending.begin(); it != m_pending.end();) {
        if (it->future.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
            ++it;
            continue;
        }
        auto [ok, error, connection, databases] = it->future.get();
        if (Session *s = Find(it->sessionId)) {
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
        it = m_pending.erase(it);
    }
}

void services::SessionService::TestAsync(const db::ConnectionConfig &config) {
    m_testing = true;
    m_TestFuture = std::async(std::launch::async, [config]() {
        TestResult r;
        const auto conn = db::CreateConnection(config.dbType); // create new connection every request cuz this is a test
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
    auto it = std::ranges::find_if(m_sessions, [id](const Session &s) { return s.id == id; });
    return it == m_sessions.end() ? nullptr : &*it;
}
