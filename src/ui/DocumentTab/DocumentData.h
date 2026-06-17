#pragma once

#include "db/QueryResult.h"

#include <string>

namespace ui {
    enum class DocumentType {
        Query,
        TableView
    };

    struct DocumentData {
        DocumentType type = DocumentType::Query;
        int id = 0;
        int connectionId = -1;
        std::string title;
        bool open = true;
        bool focusRequested = false;

        // Query tab state
        std::string queryText = "SELECT 1;";

        // Table view tab state
        std::string schemaName;
        std::string tableName;

        // Shared result/status
        db::QueryResult result;
        std::string statusMessage;
    };
}
