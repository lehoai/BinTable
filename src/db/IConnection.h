#pragma once

#include "db/ConnectionConfig.h"
#include "db/QueryResult.h"

#include <string>

namespace db {

class IConnection
{
public:
    virtual ~IConnection() = default;

    virtual bool Connect(const ConnectionConfig& config) = 0;
    virtual void Disconnect() = 0;
    virtual bool IsConnected() const = 0;

    virtual QueryResult Execute(const std::string& sql) = 0;

    virtual const std::string& LastError() const = 0;
};

} // namespace db
