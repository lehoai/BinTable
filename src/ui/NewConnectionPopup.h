#pragma once

#include "db/ConnectionConfig.h"
#include <string>

namespace services {
    class SessionService;
}

namespace ui {
    enum class PopupAction { None, Save };

    struct PopupResult {
        PopupAction action = PopupAction::None;
        db::ConnectionConfig config;
    };

    class NewConnectionPopup {
    public:
        explicit NewConnectionPopup(services::SessionService &sessionService) : m_service(sessionService) {}

        void open();

        PopupResult Draw();

    private:
        services::SessionService& m_service;
        db::ConnectionConfig m_config;
        int m_dbTypeIndex = 0;
        std::string m_statusMessage;
        bool m_pendingOpen = false;
        bool m_isOpen = false;
    };
} // namespace ui
