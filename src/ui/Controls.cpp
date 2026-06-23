#include <imgui.h>
#include "Controls.h"

#include "imgui_internal.h"
#include "ui/dark_style.h"
#include "misc/cpp/imgui_stdlib.h"

float ui::controls::getDpiSize(const float size) {
    const float scale = ImGui::GetStyle().FontScaleDpi;
    return size * scale;
}

bool ui::controls::Button(const char *label) {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    const auto btn = ImGui::Button(label, ImVec2(0, getDpiSize(25.0f)));
    ImGui::PopStyleVar(2);
    return btn;
}

bool ui::controls::Button(const char *label, const float width) {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.0f);
    const auto btn = ImGui::Button(label, ImVec2(getDpiSize(width), getDpiSize(25.0f)));
    ImGui::PopStyleVar(2);
    return btn;
}

bool ui::controls::IconButton(const char *label, const char *icon, const ImU32 &iconColor) {
    ImDrawList *dl = ImGui::GetWindowDrawList();

    ImFont *font = ImGui::GetFont(); // font đã merge: chữ + icon
    const float textSize = ImGui::GetFontSize(); // size hiện hành
    const float iconSize = textSize - 1; // icon nhỏ hơn một chút
    constexpr float spacing = 8.0f;

    // Đo từng phần ở đúng size của nó — cùng một font merge
    const ImVec2 iSz = font->CalcTextSizeA(iconSize, FLT_MAX, 0.0f, icon);
    const ImVec2 tSz = font->CalcTextSizeA(textSize, FLT_MAX, 0.0f, label);

    const float contentW = iSz.x + spacing + tSz.x;
    const ImVec2 btnSz(contentW + 15 * 2, getDpiSize(25.0f));

    const ImVec2 p = ImGui::GetCursorScreenPos();
    const bool pressed = ImGui::InvisibleButton(label, btnSz);
    const bool hovered = ImGui::IsItemHovered();
    // const bool held    = ImGui::IsItemActive();

    const ImU32 bgColor = hovered ? style::kBtnHoverBg : style::kTransparent;
    dl->AddRectFilled(p, ImVec2(p.x + btnSz.x, p.y + btnSz.y),
                      bgColor, 3.0f);

    const ImU32 borderColor = hovered ? style::kBtnHoverBorder : style::kTransparent;
    dl->AddRect(p, ImVec2(p.x + btnSz.x, p.y + btnSz.y), borderColor, 3.0f);


    const ImU32 textColor = hovered ? style::kBtnTextHover : style::kBtnTextNormal;

    const float yMid = ImFloor(p.y + btnSz.y * 0.5f);
    float x = p.x + 15;


    dl->AddText(font, iconSize,
                ImVec2(x, ImFloor(yMid - iSz.y * 0.5f + 2)), iconColor, icon); // TODO: hard code move 2px to bottom


    x += iSz.x + spacing;
    dl->AddText(font, textSize,
                ImVec2(x, ImFloor(yMid - tSz.y * 0.5f)), textColor, label);

    return pressed;
}

bool ui::controls::IconButton(const char *label, const char *icon, const ImU32 &iconColor, float contentW) {
    ImDrawList *dl = ImGui::GetWindowDrawList();

    ImFont *font = ImGui::GetFont(); // font đã merge: chữ + icon
    const float textSize = ImGui::GetFontSize(); // size hiện hành
    const float iconSize = textSize - 1; // icon nhỏ hơn một chút
    constexpr float spacing = 8.0f;

    // Đo từng phần ở đúng size của nó — cùng một font merge
    const ImVec2 iSz = font->CalcTextSizeA(iconSize, FLT_MAX, 0.0f, icon);
    const ImVec2 tSz = font->CalcTextSizeA(textSize, FLT_MAX, 0.0f, label);

    const ImVec2 btnSz(contentW, getDpiSize(25.0f));

    const ImVec2 p = ImGui::GetCursorScreenPos();
    const bool pressed = ImGui::InvisibleButton(label, btnSz);
    const bool hovered = ImGui::IsItemHovered();
    // const bool held    = ImGui::IsItemActive();


    const ImU32 bgColor = hovered ? style::kBtnHoverBg : style::kTransparent;
    dl->AddRectFilled(p, ImVec2(p.x + btnSz.x, p.y + btnSz.y),
                      bgColor, 3.0f);

    const ImU32 borderColor = hovered ? style::kBtnHoverBorder : style::kTransparent;
    dl->AddRect(p, ImVec2(p.x + btnSz.x, p.y + btnSz.y), borderColor, 3.0f);


    const ImU32 textColor = hovered ? style::kBtnTextHover : style::kBtnTextNormal;

    const float yMid = ImFloor(p.y + btnSz.y * 0.5f);
    float x = p.x + 15;


    dl->AddText(font, iconSize,
                ImVec2(x, ImFloor(yMid - iSz.y * 0.5f + 2)), iconColor, icon);


    x += iSz.x + spacing;
    dl->AddText(font, textSize,
                ImVec2(x, ImFloor(yMid - tSz.y * 0.5f)), textColor, label);

    return pressed;
}

void ui::controls::InputText(const char *label, std::string &value, const char *placeholder, const ImVec2 padding,
                             const ImVec4 bg) {
    auto dpiPadding = ImVec2(getDpiSize(padding.x), getDpiSize(padding.y));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, dpiPadding);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, bg);
    ImGui::PushStyleColor(ImGuiCol_Border, ImGui::GetStyle().Colors[ImGuiCol_Separator]);
    if (placeholder == nullptr)
        ImGui::InputText(label, &value, 0);
    else
        ImGui::InputTextWithHint(label, placeholder, &value, 0);
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);
}

void ui::controls::Password(const char *label, std::string &value) {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 10));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, style::kBgTransparent);
    ImGui::PushStyleColor(ImGuiCol_Border, ImGui::GetStyle().Colors[ImGuiCol_Separator]);

    ImGui::InputText(label, &value, ImGuiInputTextFlags_Password);

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);
}

bool ui::controls::Combo(const char *label, const std::vector<std::string> &items, int &selectedIndex,
                         const ImVec2 padding) {
    bool changed = false;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, padding);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, style::kBgTransparent);
    ImGui::PushStyleColor(ImGuiCol_Border, ImGui::GetStyle().Colors[ImGuiCol_Separator]);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, style::kBgTransparent);
    ImGui::PushStyleColor(ImGuiCol_Button, style::kBgTransparent);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, style::kBgTransparent);

    if (ImGui::BeginCombo(label, items[selectedIndex].c_str(), ImGuiComboFlags_None)) {
        for (int i = 0; i < items.size(); i++) {
            bool isSelected = (selectedIndex == i);
            if (ImGui::Selectable(items[i].c_str(), isSelected)) {
                selectedIndex = i;
                changed = true;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(5);

    return changed;
}
