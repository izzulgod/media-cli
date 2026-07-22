#pragma once

#include <ftxui/component/component.hpp>
#include <functional>
#include <vector>
#include <string>

namespace mediacli::ui {

using namespace ftxui;

class MainMenu {
public:
    MainMenu(std::function<void(int)> onSelect);
    Component getComponent();

private:
    int selected_{0};
    std::vector<std::string> entries_{
        "  Search Media",
        "  Watch History",
        "  Favorite Media",
        "⚙  Settings",
        "  About App",
        "󰈆  Exit Application"
    };
    Component menu_component_;
    Component container_;
    Component renderer_;
};

} // namespace mediacli::ui
