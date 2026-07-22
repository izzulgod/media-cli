#pragma once

#include <ftxui/component/component.hpp>
#include <functional>
#include <vector>
#include <string>
#include <atomic>
#include "core/MediaInfo.hpp"

namespace mediacli::ui {

using namespace ftxui;

enum class VideoMenuAction {
    Play,
    Download,
    OpenBrowser,
    Favorite,
    Back
};

class VideoMenu {
public:
    VideoMenu(std::function<void(VideoMenuAction)> onAction);
    
    void setMedia(const MediaInfo& media, bool isFavorite);
    Component getComponent();

    void setLoading(bool loading, const std::string& msg = "");
    void tickSpinner();
    bool isLoading() const { return is_loading_; }

private:
    MediaInfo media_;
    bool is_favorite_{false};
    int selected_{0};

    std::atomic<bool> is_loading_{false};
    std::string loading_msg_;
    int spinner_step_{0};

    std::vector<std::string> entries_;
    Component menu_component_;
    Component container_;
    Component renderer_;

    void updateEntries();
};

} // namespace mediacli::ui
