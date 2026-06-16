#include "ui/NewConnectionPopup.h"
#include "db/ConnectionFactory.h"

#include "Controls.h"
#include <imgui.h>

namespace ui {
    void NewConnectionPopup::open() {
        m_pendingOpen = true;
        m_isOpen = true;
        m_statusMessage.clear();
        m_isTesting = false;
    }

    PopupResult NewConnectionPopup::Draw() {
        constexpr auto kPopupId = "Connection##popup";
        PopupResult result;

        if (m_pendingOpen) {
            ImGui::OpenPopup(kPopupId);
            m_pendingOpen = false;
        }

        // center parent
        const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal(kPopupId, &m_isOpen, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (!m_isOpen) {
                ImGui::CloseCurrentPopup();
            }
            if (m_isTesting) {
                if (m_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                    auto [success, message] = m_future.get();
                    m_isTesting = false;
                    m_statusMessage = message;
                    // if (success) {
                    //     ImGui::CloseCurrentPopup();
                    //     justConnected = true;
                    // }
                }
            }

            ImGui::PushItemWidth(controls::getDpiSize(300.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(controls::getDpiSize(8.0f), controls::getDpiSize(7.0f)));
            controls::Combo("Database Type", {"Postgresql", "MariaDb", "Sqlite"}, m_dbTypeIndex);
            controls::InputText("Connection Name", m_config.connectionName);
            controls::InputText("Host", m_config.host);
            controls::InputText("Port", m_config.port);
            controls::InputText("Database", m_config.database);
            controls::InputText("User", m_config.user);
            controls::Password("Password", m_config.password);
            ImGui::PopStyleVar();
            ImGui::PopItemWidth();

            if (!m_statusMessage.empty())
                ImGui::TextWrapped("%s", m_statusMessage.c_str());

            ImGui::Separator();

            constexpr float kCloseWidth = 80.0f;
            constexpr float kSaveWidth = 120.0f;
            const float spacing = ImGui::GetStyle().ItemSpacing.x;
            // Right edge stays constant for the whole row, so compute it once before
            // drawing anything (GetContentRegionAvail() shrinks as the cursor moves).
            const float rightEdge = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x;

            ImGui::BeginDisabled(m_isTesting);
            if (controls::Button(m_isTesting ? "Connecting...##test" : "Test Connection", 120.0f))
                StartTest();
            ImGui::EndDisabled();

            // Use scaled widths here to match what controls::Button actually renders.
            float rightWidth = controls::getDpiSize(kCloseWidth);
            rightWidth += spacing + controls::getDpiSize(kSaveWidth);

            ImGui::SameLine(rightEdge - rightWidth);

            if (controls::Button("Close", kCloseWidth))
                ImGui::CloseCurrentPopup();

            ImGui::SameLine();
            ImGui::BeginDisabled(m_isTesting);
            if (controls::Button(m_isTesting ? "Connecting...##save" : "Save Connection", kSaveWidth)) {
                result.action = PopupAction::Save;
                result.config = m_config;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndDisabled();


            ImGui::EndPopup();
        }
        return result;
    }

    void NewConnectionPopup::StartTest() {
        m_isTesting = true;
        db::ConnectionConfig config = m_config;
        int dbIndexType = m_dbTypeIndex;
        m_future = std::async(std::launch::async, [config, dbIndexType]() {
            TestConnectionResult result;
            const auto dbType = static_cast<db::DbType>(dbIndexType);
            const auto conn = db::CreateConnection(dbType);
            if (!conn) {
                result.message = "Unsupported connection type";
                return result;
            }

            result.success = conn->Connect(config);
            result.message = result.success ? "Connected" : conn->LastError();
            return result;
        });
    }
} // namespace ui
