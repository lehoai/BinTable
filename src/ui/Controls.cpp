#include <imgui.h>
#include "Controls.h"
#include "misc/cpp/imgui_stdlib.h"
//
// Created by lehoai on 6/14/26.
//
#include "Controls.h"

bool ui::controls::Button(const char *label) {
    return ImGui::Button(label, ImVec2(0, 25));
}

bool ui::controls::Button(const char *label, const float width) {
    return ImGui::Button(label, ImVec2(width, 25));
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
