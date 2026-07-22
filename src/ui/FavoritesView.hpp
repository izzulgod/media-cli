#pragma once

#include <ftxui/component/component.hpp>
#include <functional>
#include <vector>
#include "core/MediaInfo.hpp"
#include "ui/ResultsView.hpp"

namespace mediacli::ui {

using namespace ftxui;

class FavoritesView {
public:
    FavoritesView(std::function<void(const MediaInfo&)> onSelect, std::function<void()> onBack);

    void setFavorites(const std::vector<MediaInfo>& favorites);
    Component getComponent();

private:
    ResultsView results_view_;
};

} // namespace mediacli::ui
