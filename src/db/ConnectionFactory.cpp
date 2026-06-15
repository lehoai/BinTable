//
// Created by lehoai on 6/14/26.
//
#include "ConnectionFactory.h"
#include "postgres/PostgresConnection.h"

std::unique_ptr<db::IConnection> db::CreateConnection(DbType dbType) {
    switch (dbType) {
        case DbType::Postgres:
            return std::make_unique<postgres::PostgresConnection>();
        default:
            return nullptr;
    }
}
