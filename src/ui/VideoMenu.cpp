#include "ui/VideoMenu.hpp"
#include "ui/Theme.hpp"
#include <fmt/format.h>
#include <array>

namespace mediacli::ui {

static const std::array<std::string, 10> kSpinnerFrames = {
    "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"
};

VideoMenu::VideoMenu(std::function<void(VideoMenuAction)> onAction) {
    MenuOption option;
    option.on_enter = [this, onAction]() {
        if (is_loading_) return;
        switch (selected_) {
            case 0: if (onAction) onAction(VideoMenuAction::Play); break;
            case 1: if (onAction) onAction(VideoMenuAction::Download); break;
            case 2: if (onAction) onAction(VideoMenuAction::OpenBrowser); break;
            case 3: if (onAction) onAction(VideoMenuAction::Favorite); break;
            case 4: if (onAction) onAction(VideoMenuAction::Back); break;
        }
    };

    menu_component_ = Menu(&entries_, &selected_, option);
    container_ = Container::Vertical({ menu_component_ });

    renderer_ = Renderer(container_, [this]() -> Element {
        Element loading_status = emptyElement();
        if (is_loading_) {
            std::string spinner = kSpinnerFrames[spinner_step_ % kSpinnerFrames.size()];
            std::string msg = fmt::format(" {} {} Please wait...", spinner, loading_msg_);
            loading_status = text(msg) | color(Color::Yellow) | bold | border;
        }

        auto content_box = vbox(Elements{
            applyHeader("Media Action Menu"),
            text(""),
            vbox(Elements{
                text(" Title    : " + media_.title) | bold | color(Color::White),
                text(" Channel  : " + media_.channel) | color(Color::Cyan),
                text(" Duration : " + media_.duration + "  |  Views: " + media_.views) | color(Color::GrayLight),
            }) | border | color(Color::Blue),
            text(""),
            loading_status,
            menu_component_->Render() | border | color(is_loading_ ? Color::GrayDark : Color::Cyan),
            text(""),
            text(" [↑/↓] Navigate   [Enter] Select Action   [ESC] Main Menu") | dim | align_right,
        }) | size(WIDTH, EQUAL, 84);

        return vbox(Elements{ content_box | center });
    });

    renderer_ = CatchEvent(renderer_, [this, onAction](Event event) {
        if (event == Event::Escape) {
            if (!is_loading_ && onAction) {
                onAction(VideoMenuAction::Back);
                return true;
            }
        }
        return false;
    });
}

void VideoMenu::setMedia(const MediaInfo& media, bool isFavorite) {
    media_ = media;
    is_favorite_ = isFavorite;
    selected_ = 0;
    is_loading_ = false;
    loading_msg_.clear();
    spinner_step_ = 0;
    updateEntries();
}

void VideoMenu::setLoading(bool loading, const std::string& msg) {
    is_loading_ = loading;
    loading_msg_ = msg;
    if (loading) {
        spinner_step_ = 0;
    }
}

void VideoMenu::tickSpinner() {
    spinner_step_++;
}

void VideoMenu::updateEntries() {
    entries_ = {
        "󰐊  Play Stream",
        "  Download File",
        "󰖟  Open in Browser",
        is_favorite_ ? "  Remove from Favorites" : "  Add to Favorites",
        "←  Back"
    };
}

Component VideoMenu::getComponent() {
    return renderer_;
}

} // namespace mediacli::ui
