#pragma once

#include "db/ConnectionConfig.h"

#include <functional>
#include <string>

namespace ui {

// Renders the connection fields and connect/disconnect button within the current window/popup.
void DrawConnectionForm(db::ConnectionConfig& config, bool isConnected, const std::string& statusMessage, bool isConnecting,
    const std::function<void()>& onConnect, const std::function<void()>& onDisconnect);

} // namespace ui
