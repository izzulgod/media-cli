#pragma once

#include <ftxui/component/component.hpp>
#include <functional>
#include <vector>
#include <string>
#include "core/MediaInfo.hpp"

namespace mediacli::ui {

using namespace ftxui;

class ResultsView {
public:
    ResultsView(std::function<void(const MediaInfo&)> onSelect, std::function<void()> onBack);
    
    void setResults(const std::vector<MediaInfo>& items, const std::string& titleHeader = "Search Results");
    Component getComponent();

private:
    std::string title_header_{"Search Results"};
    std::vector<MediaInfo> items_;
    std::vector<std::string> display_entries_;
    int selected_{0};

    Component menu_component_;
    Component container_;
    Component renderer_;

    void updateEntries();
};

} // namespace mediacli::ui
