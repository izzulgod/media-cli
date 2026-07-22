#include "ui/FavoritesView.hpp"

namespace mediacli::ui {

FavoritesView::FavoritesView(std::function<void(const MediaInfo&)> onSelect, std::function<void()> onBack)
    : results_view_(onSelect, onBack) {}

void FavoritesView::setFavorites(const std::vector<MediaInfo>& favorites) {
    results_view_.setResults(favorites, "Favorite Media");
}

Component FavoritesView::getComponent() {
    return results_view_.getComponent();
}

} // namespace mediacli::ui
