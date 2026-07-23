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
    
    void setResults(const std::vector<MediaInfo>& items, 
                    const std::string& titleHeader = "Search Results",
                    const std::string& backLabel = "← Back to Search");
    Component getComponent();

private:
    std::string title_header_{"Search Results"};
    std::string back_label_{"← Back to Search"};
    std::vector<MediaInfo> items_;
    std::vector<std::string> display_entries_;
    int selected_{0};

    Component menu_component_;
    Component btn_back_;
    Component container_;
    Component renderer_;

    void updateEntries();
};

} // namespace mediacli::ui
