#include "ui/MainMenu.hpp"
#include "ui/Theme.hpp"

namespace mediacli::ui {

MainMenu::MainMenu(std::function<void(int)> onSelect) {
    MenuOption option;
    option.on_enter = [this, onSelect]() {
        if (onSelect) onSelect(selected_);
    };

    menu_component_ = Menu(&entries_, &selected_, option);
    container_ = Container::Vertical({ menu_component_ });

    renderer_ = Renderer(container_, [this]() -> Element {
        auto menu_box = vbox(Elements{
            text(" Select Option ") | bold | color(Color::Yellow) | center,
            text(""),
            menu_component_->Render() | border | color(Color::Cyan),
        }) | size(WIDTH, EQUAL, 36);

        return vbox(Elements{
            vbox(Elements{
                text("  __  __ _____ ____  ___    _     ____ _     ___ ") | bold | color(Color::Cyan),
                text(" |  \\/  | ____|  _ \\|_ _|  / \\   / ___| |   |_ _|") | bold | color(Color::Cyan),
                text(" | |\\/| |  _| | | | || |  / _ \\ | |   | |    | | ") | bold | color(Color::Blue),
                text(" | |  | | |___| |_| || | / ___ \\| |___| |___ | | ") | bold | color(Color::Blue),
                text(" |_|  |_|_____|____/|___/_/   \\_\\ \\____|_____|___|") | bold | color(Color::Magenta),
            }) | center,
            text(""),
            text(" 󰎁 Modern C++20 Modular Terminal Media Client") | center | color(Color::GrayLight) | bold,
            text(""),
            menu_box | center,
            text(""),
            text(" [↑/↓] Navigate   [Enter] Select   [q/ESC] Exit") | dim | center,
        });
    });
}

Component MainMenu::getComponent() {
    return renderer_;
}

} // namespace mediacli::ui
