#pragma once

#include <string>
#include <vector>

namespace services {
    enum class NodeType {
        Server, Database, Schema, Table, View, Function
    };

    struct TreeNode {
        NodeType type;
        std::string label;
        std::vector<TreeNode> children;

        [[nodiscard]] bool HasChildren() const {
            return !children.empty();
        }
    };

    class ConnectionService {
    public:
        std::vector<TreeNode> BuildTree();

    private:
        std::vector<TreeNode> m_trees;
        bool m_initialized = false;
    };
}
