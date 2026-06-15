//
// Created by lehoai on 6/14/26.
//

#include "ConnectionManager.h"

#include "ConnectionFactory.h"

int db::ConnectionManager::OpenSession(const ConnectionConfig &config) {
    auto conn = CreateConnection(config.dbType);
    if (!conn || !conn->Connect(config))
        return -1;
    auto session = std::make_unique<ConnectionSession>();
    session->id = m_nextId++;
    session->config = config;
    session->connection = std::move(conn);
    session->statusMessage = "Connected";

    const int id = session->id;
    m_sessions.push_back(std::move(session));
    return id;
}

void db::ConnectionManager::CloseSession(int id) {
    std::erase_if(m_sessions, [id](const auto &s) { return s->id == id; });
}

db::ConnectionSession *db::ConnectionManager::Find(int id) {
    const auto it = std::ranges::find_if(m_sessions, [id](const auto &s) { return s->id == id; });
    return it == m_sessions.end() ? nullptr : it->get();
}
