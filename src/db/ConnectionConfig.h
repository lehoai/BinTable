#pragma once

#include <string>

namespace db {
    struct ConnectionConfig {
        int dbType = 0;
        std::string connectionName = "new connection";
        std::string host = "localhost";
        std::string port = "5432";
        std::string database;
        std::string user;
        std::string password;
    };
} // namespace db
