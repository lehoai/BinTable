#pragma once
#include <string>
#include <vector>
#include <imgui.h>

namespace ui::controls {

    float getDpiSize(float size);

    bool Button(const char *label);

    bool Button(const char *label, float width);

    bool IconButton(const char *label, const char *icon, const ImU32& iconColor);

    bool IconButton(const char *label, const char *icon, const ImU32& iconColor, float contentW);

    void InputText(const char *label, std::string &value, const char *placeholder = nullptr);

    void Password(const char *label, std::string &value);

    bool Combo(const char *label, const std::vector<std::string> &items, int &selectedIndex, ImVec2 padding = ImVec2(8, 10));
}
