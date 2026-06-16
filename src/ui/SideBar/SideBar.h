#pragma once

#include <string>
#include <functional>
#include "services/ConnectionService.h"

namespace ui {
    class SideBar {
    public:
        void Draw(const std::function<void(const std::string&, const std::string&)>& onOpenTable);

    private:
        std::string m_filterText;
        std::string m_selectedKey;
        services::ConnectionService m_connService;
    };
}
