#pragma once

#include <string>

#include "services/ConnectionService.h"

namespace ui {
    class SideBar {
    public:
        void Draw();

    private:
        std::string m_filterText;
        std::string m_selectedKey;
        services::ConnectionService m_connService;
    };
}
