#include "ui/AboutView.hpp"
#include "ui/Theme.hpp"
#include "utils/Process.hpp"

namespace mediacli::ui {

AboutView::AboutView(std::function<void()> onBack) {
    auto btn_github = Button(" 󰖟 Open GitHub Repository ", []() {
        std::string cmd = "xdg-open \"https://github.com/izzulgod/media-cli\" >/dev/null 2>&1 &";
        Process::runInteractive(cmd);
    });

    auto btn_back = Button(" ← Back ", [onBack]() {
        if (onBack) onBack();
    });

    container_ = Container::Vertical({
        Container::Horizontal({
            btn_github,
            btn_back
        })
    });

    renderer_ = Renderer(container_, [this, btn_github, btn_back]() -> Element {
        auto content_box = vbox(Elements{
            applyHeader("About"),
            text(""),
            vbox(Elements{
                text("  __  __ _____ ____  ___    _     ____ _     ___ ") | bold | color(Color::Cyan),
                text(" |  \\/  | ____|  _ \\|_ _|  / \\   / ___| |   |_ _|") | bold | color(Color::Cyan),
                text(" | |\\/| |  _| | | | || |  / _ \\ | |   | |    | | ") | bold | color(Color::Blue),
                text(" | |  | | |___| |_| || | / ___ \\| |___| |___ | | ") | bold | color(Color::Blue),
                text(" |_|  |_|_____|____/|___/_/   \\_\\ \\____|_____|___|") | bold | color(Color::Magenta),
            }) | center,
            text(""),
            text(" Version 1.0.0 — Modern C++20 Modular Terminal Media Client") | center | color(Color::GrayLight),
            text(" Elegant, lightweight TUI media player & manager") | center | dim,
            text(""),
            vbox(Elements{
                text(" Developer & Project Information:") | bold | color(Color::Yellow),
                text(" • Author/Developer : izzulgod") | color(Color::Green) | bold,
                text(" • GitHub Repository: https://github.com/izzulgod/media-cli") | color(Color::Cyan),
                text(" • Core Architecture: C++20, Modular IMediaBackend & IMediaPlayer") | color(Color::White),
                text(" • Backend Provider : YouTube (powered by yt-dlp)") | color(Color::White),
                text(" • Media Player     : mpv Player") | color(Color::White),
                text(" • Libraries        : FTXUI, nlohmann/json, fmt & spdlog") | color(Color::White),
            }) | border | color(Color::Blue),
            text(""),
            hbox(Elements{
                btn_github->Render() | color(Color::Cyan),
                text("  "),
                btn_back->Render() | color(Color::Red),
            }) | center,
            text(""),
            text(" [Tab] Navigate Buttons   [Enter] Open GitHub / Back   [ESC] Menu") | dim | align_right,
        }) | size(WIDTH, EQUAL, 84);

        return vbox(Elements{
            content_box | center
        });
    });

    renderer_ = CatchEvent(renderer_, [onBack](Event event) {
        if (event == Event::Escape) {
            if (onBack) onBack();
            return true;
        }
        return false;
    });
}

Component AboutView::getComponent() {
    return renderer_;
}

} // namespace mediacli::ui
