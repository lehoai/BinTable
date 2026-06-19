#pragma once

#include <string>
#include <functional>
#include "services/SessionService.h"

namespace ui {
    using OpenTabFn = std::function<void(int, const std::string &, const std::string &)>;

    class SideBar {
    public:
        explicit SideBar(services::SessionService &service) : m_service(service) {
        };

        void Draw(const OpenTabFn &onOpenTable);

    private:
        std::string m_filterText;
        std::string m_selectedKey;
        services::SessionService &m_service;
    };
}
