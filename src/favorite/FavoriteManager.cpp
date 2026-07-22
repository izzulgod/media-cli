#include "favorite/FavoriteManager.hpp"
#include "utils/FileUtils.hpp"
#include "utils/Logger.hpp"
#include <algorithm>
#include <chrono>

namespace mediacli {

FavoriteManager::FavoriteManager(std::filesystem::path favoritePath) {
    if (favoritePath.empty()) {
        path_ = FileUtils::getConfigDir() / "favorites.json";
    } else {
        path_ = std::move(favoritePath);
    }
    load();
}

void FavoriteManager::add(MediaInfo item) {
    if (isFavorite(item.id, item.backend_name)) return;

    item.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    favorites_.insert(favorites_.begin(), std::move(item));
    save();
}

void FavoriteManager::remove(const std::string& id, const std::string& backendName) {
    auto it = std::remove_if(favorites_.begin(), favorites_.end(), [&](const MediaInfo& info) {
        return info.id == id && info.backend_name == backendName;
    });
    if (it != favorites_.end()) {
        favorites_.erase(it, favorites_.end());
        save();
    }
}

bool FavoriteManager::isFavorite(const std::string& id, const std::string& backendName) const {
    return std::any_of(favorites_.begin(), favorites_.end(), [&](const MediaInfo& info) {
        return info.id == id && info.backend_name == backendName;
    });
}

void FavoriteManager::toggleFavorite(const MediaInfo& item) {
    if (isFavorite(item.id, item.backend_name)) {
        remove(item.id, item.backend_name);
    } else {
        add(item);
    }
}

std::vector<MediaInfo> FavoriteManager::getAll() const {
    return favorites_;
}

bool FavoriteManager::load() {
    nlohmann::json j;
    if (FileUtils::readJson(path_, j) && j.is_array()) {
        favorites_ = j.get<std::vector<MediaInfo>>();
        LOG_INFO("Favorites loaded ({} items)", favorites_.size());
        return true;
    }
    return false;
}

bool FavoriteManager::save() {
    nlohmann::json j = favorites_;
    return FileUtils::writeJson(path_, j);
}

} // namespace mediacli
