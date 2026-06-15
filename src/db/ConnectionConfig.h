#pragma once

#include <string>

namespace db {
    enum class DbType { Postgres = 0, MariaDb = 1, Sqlite = 2 };

    struct ConnectionConfig {
        DbType dbType = DbType::Postgres;
        std::string connectionName = "new connection";
        std::string host = "localhost";
        std::string port = "5432";
        std::string database;
        std::string user;
        std::string password;
    };
} // namespace db
