#pragma once

#include <ftxui/component/component.hpp>
#include <functional>
#include <vector>
#include <string>
#include "config/ConfigManager.hpp"

namespace mediacli::ui {

using namespace ftxui;

class SettingsView {
public:
    SettingsView(ConfigManager& config, std::function<void()> onSave, std::function<void()> onBack);

    void loadFromConfig();
    Component getComponent();

private:
    ConfigManager& config_;

    std::vector<std::string> players_{"mpv"};
    int selected_player_{0};

    std::vector<std::string> qualities_{"audio_only", "360p", "720p", "1080p", "best"};
    int selected_quality_{4};

    std::vector<std::string> browsers_{"none", "firefox", "chrome", "chromium", "brave", "edge"};
    int selected_browser_{0};

    std::string download_path_;

    Component input_path_;
    Component toggle_player_;
    Component toggle_quality_;
    Component toggle_browser_;
    Component container_;
    Component renderer_;
};

} // namespace mediacli::ui
