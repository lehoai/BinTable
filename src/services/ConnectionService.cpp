//
// Created by Administrator on 6/16/2026.
//

#include "ConnectionService.h"

std::vector<services::TreeNode> services::ConnectionService::BuildTree() {
    if (m_initialized) {
        return m_trees;
    }

    // TODO: hard code

    TreeNode root1;
    root1.label = "Postgres - Prod";
    root1.type = NodeType::Server;

    TreeNode database1;
    database1.label = "ecommerce";
    database1.type = NodeType::Database;

    TreeNode schema1;
    schema1.label = "public";
    schema1.type = NodeType::Schema;

    TreeNode table1;
    table1.label = "Users";
    table1.type = NodeType::Table;
    schema1.children.push_back(table1);

    TreeNode table2;
    table2.label = "Product";
    table2.type = NodeType::Table;
    schema1.children.push_back(table2);

    TreeNode view1;
    view1.label = "active_users";
    view1.type = NodeType::View;
    schema1.children.push_back(view1);

    TreeNode func1;
    func1.label = "get_user_by_id";
    func1.type = NodeType::Function;
    schema1.children.push_back(func1);

    database1.children.push_back(schema1);

    root1.children.push_back(database1);


    TreeNode database2;
    database2.label = "bank";
    database2.type = NodeType::Database;
    root1.children.push_back(database2);

    m_trees.push_back(root1);

    m_initialized = true;

    return m_trees;
}
