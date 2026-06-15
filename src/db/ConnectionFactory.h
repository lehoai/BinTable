#pragma once
#include <memory>

#include "IConnection.h"

namespace db {
    std::unique_ptr<IConnection> CreateConnection(DbType dbType);
}
