#pragma once

#include <ftxui/component/component.hpp>
#include <functional>
#include <vector>
#include "core/MediaInfo.hpp"
#include "ui/ResultsView.hpp"

namespace mediacli::ui {

using namespace ftxui;

class HistoryView {
public:
    HistoryView(std::function<void(const MediaInfo&)> onSelect,
                std::function<void()> onClear,
                std::function<void()> onBack);

    void setHistory(const std::vector<MediaInfo>& history);
    Component getComponent();

private:
    ResultsView results_view_;
    Component container_;
    Component renderer_;
};

} // namespace mediacli::ui
