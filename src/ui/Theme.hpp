#pragma once

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/color.hpp>

namespace mediacli::ui {

using namespace ftxui;

struct ThemeColors {
    Color primary = Color::Cyan;
    Color secondary = Color::Blue;
    Color accent = Color::Magenta;
    Color background = Color::Default;
    Color foreground = Color::White;
    Color muted = Color::GrayDark;
    Color success = Color::Green;
    Color warning = Color::Yellow;
    Color error = Color::Red;
};

inline ThemeColors currentTheme() {
    return ThemeColors{};
}

inline Element applyHeader(const std::string& title) {
    return vbox(Elements{
        text(" 󰎁 MediaCLI — " + title) | bold | color(Color::Cyan),
        separator() | color(Color::Blue),
    });
}

} // namespace mediacli::ui
