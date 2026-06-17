#pragma once
#include <imgui.h>

namespace ui::style {
    // ---- SideBar icon colors (ImVec4 for ImGui::TextColored) ----
    constexpr ImVec4 kIconServer{0.50f, 0.83f, 0.65f, 1.0f};
    constexpr ImVec4 kIconDatabase{0.36f, 0.61f, 0.84f, 1.0f};
    constexpr ImVec4 kIconSchema{0.79f, 0.64f, 0.43f, 1.0f};
    constexpr ImVec4 kIconGroup{0.48f, 0.51f, 0.55f, 1.0f}; // Tables / Views / Functions group
    constexpr ImVec4 kIconTable{0.44f, 0.69f, 0.83f, 1.0f};
    constexpr ImVec4 kIconView{0.71f, 0.55f, 0.83f, 1.0f};
    constexpr ImVec4 kIconFunction{0.50f, 0.75f, 0.56f, 1.0f};

    // ---- Text colors ----
    constexpr ImVec4 kTextError{1.0f, 0.4f, 0.4f, 1.0f};

    // ---- IconButton colors (ImU32 for ImDrawList) ----
    constexpr ImU32 kTransparent = IM_COL32(0, 0, 0, 0);
    constexpr ImU32 kBtnHoverBg = IM_COL32(40, 48, 60, 255);
    constexpr ImU32 kBtnHeldBg = IM_COL32(55, 66, 82, 255);
    constexpr ImU32 kBtnHoverBorder = IM_COL32(78, 90, 106, 255);
    constexpr ImU32 kBtnTextHover = IM_COL32(238, 242, 246, 255);
    constexpr ImU32 kBtnTextNormal = IM_COL32(200, 206, 214, 255);

    // ---- Toolbar icon colors (ImU32 for IconButton) ----
    constexpr ImU32 kToolbarConnect = IM_COL32(108, 198, 160, 255);
    constexpr ImU32 kToolbarDisconnect = IM_COL32(217, 138, 138, 255);
    constexpr ImU32 kToolbarRefresh = IM_COL32(91, 155, 213, 255);
    constexpr ImU32 kToolbarNewQuery = IM_COL32(255, 255, 255, 255);
    constexpr ImU32 kToolbarOpen = IM_COL32(210, 167, 102, 255);
    constexpr ImU32 kToolbarSave = IM_COL32(95, 176, 168, 255);
    constexpr ImU32 kToolbarImport = IM_COL32(90, 169, 196, 255);
    constexpr ImU32 kToolbarExport = IM_COL32(181, 139, 212, 255);

    constexpr ImVec4 kBgTransparent{0, 0, 0, 0};
}
