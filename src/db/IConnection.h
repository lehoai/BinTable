#pragma once

#include "db/ConnectionConfig.h"
#include "db/QueryResult.h"

#include <string>

namespace db {
    class IConnection {
    public:
        virtual ~IConnection() = default;

        virtual bool Connect(const ConnectionConfig &config) = 0;

        virtual void Disconnect() = 0;

        [[nodiscard]] virtual bool IsConnected() const = 0;

        virtual std::vector<std::string> LoadDatabases() = 0;
        virtual std::vector<std::string> LoadSchemas() = 0;
        virtual std::vector<std::string> LoadTables(std::string schema) = 0;

        virtual QueryResult Execute(const std::string &sql) = 0;

        [[nodiscard]] virtual const std::string &LastError() const = 0;
    };
} // namespace db
