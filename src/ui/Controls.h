#pragma once
#include <string>
#include <vector>

namespace ui::controls {
    bool Button(const char *label);

    bool Button(const char *label, float width);

    void InputText(const char *label, std::string &value, const char *placeholder = nullptr);

    void Password(const char *label, std::string &value);

    bool Combo(const char *label, const std::vector<std::string> &items, int &selectedIndex);
}
