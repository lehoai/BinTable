#pragma once

#include "db/ConnectionConfig.h"
#include <future>
#include <string>

namespace ui {
    struct TestConnectionResult {
        bool success = false;
        std::string message;
    };

    enum class PopupAction { None, Save };

    struct PopupResult {
        PopupAction action = PopupAction::None;
        db::ConnectionConfig config;
    };

    class NewConnectionPopup {
    public:
        void open();

        PopupResult Draw();

    private:
        db::ConnectionConfig m_config;
        int m_dbTypeIndex = 0;
        std::string m_statusMessage;
        bool m_pendingOpen = false;

        bool m_isTesting = false;
        std::future<TestConnectionResult> m_future;

        bool m_isOpen = false;

        void StartTest();
    };
} // namespace ui
