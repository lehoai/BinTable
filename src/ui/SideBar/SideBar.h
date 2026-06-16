#pragma once

#include <string>

#include "services/ConnectionService.h"

namespace ui {
    class SideBar {
    public:
        void Draw();

    private:
        std::string m_filterText;
        int m_selectedIndex = -1;
        services::ConnectionService m_connService;
    };
}
