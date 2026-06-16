#include <imgui.h>
#include "Controls.h"
#include "ui/dark_style.h"
#include "misc/cpp/imgui_stdlib.h"

inline float ui::controls::getDpiSize(const float size) {
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
    ImDrawList* dl    = ImGui::GetWindowDrawList();

    ImFont* font     = ImGui::GetFont();       // font đã merge: chữ + icon
    const float   textSize = ImGui::GetFontSize();   // size hiện hành
    const float   iconSize = textSize - 1;   // icon nhỏ hơn một chút
    constexpr float spacing = 8.0f;

    // Đo từng phần ở đúng size của nó — cùng một font merge
    const ImVec2 iSz = font->CalcTextSizeA(iconSize, FLT_MAX, 0.0f, icon);
    const ImVec2 tSz = font->CalcTextSizeA(textSize, FLT_MAX, 0.0f, label);

    const float contentW = iSz.x + spacing + tSz.x;
    const ImVec2 btnSz(contentW + 15 * 2, getDpiSize(25.0f));

    ImVec2 p = ImGui::GetCursorScreenPos();
    const bool pressed = ImGui::InvisibleButton(label, btnSz);
    const bool hovered = ImGui::IsItemHovered();
    // const bool held    = ImGui::IsItemActive();


    const ImU32 bgColor = hovered ? style::kBtnHoverBg : style::kTransparent;
    dl->AddRectFilled(p, ImVec2(p.x + btnSz.x, p.y + btnSz.y),
                      bgColor, 3.0f);

    const ImU32 borderColor = hovered ? style::kBtnHoverBorder : style::kTransparent;
    dl->AddRect(p, ImVec2(p.x + btnSz.x, p.y + btnSz.y), borderColor, 3.0f);


    const ImU32 textColor = hovered ? style::kBtnTextHover : style::kBtnTextNormal;

    const float yMid = p.y + btnSz.y * 0.5f;
    float x     = p.x + 15;


    dl->AddText(font, iconSize,
                ImVec2(x, yMid - iSz.y * 0.5f), iconColor, icon);


    x += iSz.x + spacing;
    dl->AddText(font, textSize,
                ImVec2(x, yMid - tSz.y * 0.5f), textColor, label);

    return pressed;
}

void ui::controls::InputText(const char *label, std::string &value, const char *placeholder) {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 6));
    if (placeholder == nullptr)
        ImGui::InputText(label, &value, 0);
    else
        ImGui::InputTextWithHint(label, placeholder, &value, 0);
    ImGui::PopStyleVar();
}

void ui::controls::Password(const char *label, std::string &value) {
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 6));
    ImGui::InputText(label, &value, ImGuiInputTextFlags_Password);
    ImGui::PopStyleVar();
}

bool ui::controls::Combo(const char *label, const std::vector<std::string> &items, int &selectedIndex) {
    bool changed = false;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 6));
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
    ImGui::PopStyleVar();
    return changed;
}
