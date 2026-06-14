#include "ui/ConnectionPanel.h"
#include "Controls.h"
#include <imgui.h>

namespace ui {
    void DrawConnectionForm(db::ConnectionConfig &config, const bool isConnected, const std::string &statusMessage,
                            const bool isConnecting,
                            const std::function<void()> &onConnect, const std::function<void()> &onDisconnect) {
        ImGui::PushItemWidth(300.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8.0f, 7.0f));
        ImGui::BeginDisabled(isConnected);
        controls::Combo("Database Type", {"Postgresql", "MariaDb", "Sqlite"}, config.dbType);
        controls::InputText("Connection Name", config.connectionName);
        controls::InputText("Host", config.host);
        controls::InputText("Port", config.port);
        controls::InputText("Database", config.database);
        controls::InputText("User", config.user);
        controls::Password("Password", config.password);
        ImGui::EndDisabled();
        ImGui::PopStyleVar();
        ImGui::PopItemWidth();

        if (!statusMessage.empty())
            ImGui::TextWrapped("%s", statusMessage.c_str());

        ImGui::Separator();

        constexpr float kCloseWidth = 80.0f;
        constexpr float kSaveWidth = 120.0f;
        const float spacing = ImGui::GetStyle().ItemSpacing.x;
        // Right edge stays constant for the whole row, so compute it once before
        // drawing anything (GetContentRegionAvail() shrinks as the cursor moves).
        const float rightEdge = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x;

        ImGui::BeginDisabled(isConnecting);
        if (controls::Button(isConnecting ? "Connecting...##test" : "Test Connection", 120.0f))
            onConnect();
        ImGui::EndDisabled();

        float rightWidth = kCloseWidth;
        if (!isConnected)
            rightWidth += spacing + kSaveWidth;

        ImGui::SameLine(rightEdge - rightWidth);

        if (controls::Button("Close", kCloseWidth))
            ImGui::CloseCurrentPopup();

        if (!isConnected) {
            ImGui::SameLine();
            ImGui::BeginDisabled(isConnecting);
            if (controls::Button(isConnecting ? "Connecting...##save" : "Save Connection", kSaveWidth))
                onConnect();
            ImGui::EndDisabled();
        }
    }
} // namespace ui
