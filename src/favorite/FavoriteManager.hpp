#pragma once

#include <vector>
#include <filesystem>
#include "core/MediaInfo.hpp"

namespace mediacli {

class FavoriteManager {
public:
    explicit FavoriteManager(std::filesystem::path favoritePath = "");

    void add(MediaInfo item);
    void remove(const std::string& id, const std::string& backendName);
    bool isFavorite(const std::string& id, const std::string& backendName) const;
    void toggleFavorite(const MediaInfo& item);
    std::vector<MediaInfo> getAll() const;
    bool load();
    bool save();

private:
    std::filesystem::path path_;
    std::vector<MediaInfo> favorites_;
};

} // namespace mediacli
