#include "ui/SettingsView.hpp"
#include "ui/Theme.hpp"
#include <algorithm>

namespace mediacli::ui {

SettingsView::SettingsView(ConfigManager& config, std::function<void()> onSave, std::function<void()> onBack)
    : config_(config) {

    toggle_player_ = Radiobox(&players_, &selected_player_);
    toggle_quality_ = Radiobox(&qualities_, &selected_quality_);
    toggle_browser_ = Radiobox(&browsers_, &selected_browser_);
    input_path_ = Input(&download_path_, "Enter download path...");

    auto btn_save = Button(" 󰆓 Save Settings ", [this, onSave]() {
        config_.setPlayerName(players_[selected_player_]);
        config_.setVideoQuality(stringToVideoQuality(qualities_[selected_quality_]));
        config_.setCookieBrowser(selected_browser_ == 0 ? "" : browsers_[selected_browser_]);
        config_.setDownloadPath(download_path_);
        config_.save();
        if (onSave) onSave();
    });

    auto btn_back = Button(" ← Cancel ", [onBack]() {
        if (onBack) onBack();
    });

    container_ = Container::Vertical({
        toggle_player_,
        toggle_quality_,
        toggle_browser_,
        input_path_,
        Container::Horizontal({
            btn_save,
            btn_back
        })
    });

    renderer_ = Renderer(container_, [this, btn_save, btn_back]() -> Element {
        auto content_box = vbox(Elements{
            applyHeader("Settings"),
            text(""),
            hbox(Elements{
                vbox(Elements{
                    text(" Media Player:") | bold | color(Color::Cyan),
                    toggle_player_->Render() | border,
                }),
                text("  "),
                vbox(Elements{
                    text(" Default Quality:") | bold | color(Color::Cyan),
                    toggle_quality_->Render() | border,
                }),
                text("  "),
                vbox(Elements{
                    text(" Import Cookies From:") | bold | color(Color::Cyan),
                    toggle_browser_->Render() | border,
                }),
            }),
            text(""),
            text(" Default Download Directory:") | bold | color(Color::Cyan),
            input_path_->Render() | border | color(Color::White),
            text(""),
            hbox(Elements{
                btn_save->Render() | color(Color::Green),
                text("  "),
                btn_back->Render() | color(Color::Red),
            }) | center,
            text(""),
            text(" [Tab] Navigate Fields   [ESC] Cancel") | dim | align_right,
        }) | size(WIDTH, EQUAL, 84);

        return vbox(Elements{ content_box | center });
    });

    renderer_ = CatchEvent(renderer_, [onBack](Event event) {
        if (event == Event::Escape) {
            if (onBack) onBack();
            return true;
        }
        return false;
    });

    loadFromConfig();
}

void SettingsView::loadFromConfig() {
    std::string player = config_.getPlayerName();
    auto p_it = std::find(players_.begin(), players_.end(), player);
    if (p_it != players_.end()) {
        selected_player_ = std::distance(players_.begin(), p_it);
    }

    std::string quality = videoQualityToString(config_.getVideoQuality());
    auto q_it = std::find(qualities_.begin(), qualities_.end(), quality);
    if (q_it != qualities_.end()) {
        selected_quality_ = std::distance(qualities_.begin(), q_it);
    }

    std::string browser = config_.getCookieBrowser();
    if (browser.empty()) {
        selected_browser_ = 0;
    } else {
        auto b_it = std::find(browsers_.begin(), browsers_.end(), browser);
        if (b_it != browsers_.end()) {
            selected_browser_ = std::distance(browsers_.begin(), b_it);
        } else {
            selected_browser_ = 0;
        }
    }

    download_path_ = config_.getDownloadPath();
}

Component SettingsView::getComponent() {
    return renderer_;
}

} // namespace mediacli::ui
