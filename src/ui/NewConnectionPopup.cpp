#include "ui/NewConnectionPopup.h"
#include "services/SessionService.h"

#include "Controls.h"
#include <imgui.h>

#include "dark_style.h"
#include "IconsFontAwesome6.h"

namespace ui {
    void NewConnectionPopup::open() {
        m_pendingOpen = true;
        m_isOpen = true;
        m_statusMessage.clear();
    }

    PopupResult NewConnectionPopup::Draw() {
        constexpr auto kPopupId = "New Connection##popup";
        PopupResult result;

        if (m_pendingOpen) {
            ImGui::OpenPopup(kPopupId);
            m_pendingOpen = false;
        }

        // center parent
        const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        // custom style popup
        ImGui::PushStyleColor(ImGuiCol_TitleBg, style::kBgDefault);
        ImGui::PushStyleColor(ImGuiCol_TitleBgActive, style::kBgDefault);

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8.0f, 12.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8.0f, 0.0f));


        const auto isPopupOpen = ImGui::BeginPopupModal(kPopupId, &m_isOpen, ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(2);

        if (isPopupOpen) {
            if (!m_isOpen) {
                ImGui::CloseCurrentPopup();
            }

            if (const auto r = m_service.PollTestResult()) {
                m_statusMessage = r->message;
            }

            ImGui::PushItemWidth(controls::getDpiSize(350.0f));
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                                ImVec2(controls::getDpiSize(8.0f), controls::getDpiSize(10.0f)));

            ImGui::Separator();

            controls::Combo("Database Type", {"Postgresql", "MariaDb", "Sqlite"}, m_dbTypeIndex);
            m_config.dbType = static_cast<db::DbType>(m_dbTypeIndex);
            controls::InputText("Connection Name", m_config.connectionName, "Your easy to remember name");
            controls::InputText("Host", m_config.host, "Database server address");
            controls::InputText("Port", m_config.port, "Database server listening port");
            controls::InputText("Database", m_config.database, "Database name");
            controls::InputText("User", m_config.user, "Database username");
            controls::Password("Password", m_config.password);
            ImGui::PopStyleVar();
            ImGui::PopItemWidth();

            if (!m_statusMessage.empty())
                ImGui::TextWrapped("%s", m_statusMessage.c_str());

            ImGui::Separator();

            const float kCloseWidth = controls::getDpiSize(80.0f);
            const float kSaveWidth = controls::getDpiSize(150.0f);
            const float spacing = ImGui::GetStyle().ItemSpacing.x;
            // Right edge stays constant for the whole row, so compute it once before
            // drawing anything (GetContentRegionAvail() shrinks as the cursor moves).
            const float rightEdge = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x;

            ImGui::BeginDisabled(m_service.IsTesting());
            if (controls::IconButton(m_service.IsTesting() ? "Connecting..." : "Test Connection",
                                     ICON_FA_PLUG_CIRCLE_CHECK,
                                     style::kToolbarConnect)) {
                m_service.TestAsync(m_config);
            }
            ImGui::EndDisabled();

            // kCloseWidth/kSaveWidth are already DPI-scaled, use them directly.
            const float rightWidth = kCloseWidth + spacing + kSaveWidth;

            ImGui::SameLine(rightEdge - rightWidth);

            if (controls::IconButton("Close", ICON_FA_XMARK, style::kToolbarDisconnect, kCloseWidth))
                ImGui::CloseCurrentPopup();

            ImGui::SameLine();
            ImGui::BeginDisabled(m_service.IsTesting());
            if (controls::IconButton(m_service.IsTesting() ? "Connecting..." : "Save Connection",
                                     ICON_FA_FLOPPY_DISK,
                                     style::kToolbarSave, kSaveWidth)) {
                result.action = PopupAction::Save;
                result.config = m_config;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndDisabled();

            ImGui::Dummy(ImVec2(0, 4.0f));

            ImGui::EndPopup();
        }

        return result;
    }
} // namespace ui
