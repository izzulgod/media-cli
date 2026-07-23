#pragma once

#include <memory>
#include <string>
#include <vector>
#include "core/MediaInfo.hpp"
#include "core/Types.hpp"
#include "config/ConfigManager.hpp"
#include "history/HistoryManager.hpp"
#include "favorite/FavoriteManager.hpp"
#include "backend/IMediaBackend.hpp"
#include "player/IMediaPlayer.hpp"

namespace mediacli {

class Application {
public:
    Application();
    ~Application() = default;

    // Initialization
    void init();
    void reloadBackendAndPlayer();

    // Business Logic API used by UI
    std::vector<MediaInfo> search(const std::string& query, int limit = 20);
    std::string getStreamUrl(const MediaInfo& media);
    bool playStreamUrl(const MediaInfo& media, const std::string& streamUrl);
    bool playMedia(const MediaInfo& media);
    bool downloadMedia(const MediaInfo& media);
    bool openInBrowser(const MediaInfo& media);

    // History & Favorites management
    std::vector<MediaInfo> getHistory() const;
    void clearHistory();
    std::vector<MediaInfo> getFavorites() const;
    bool isFavorite(const MediaInfo& media) const;
    void toggleFavorite(const MediaInfo& media);

    // Config accessors
    ConfigManager& getConfig() { return config_; }
    const ConfigManager& getConfig() const { return config_; }

    // Currently selected item for VideoMenu
    void setSelectedMedia(const MediaInfo& media) { selected_media_ = media; }
    const MediaInfo& getSelectedMedia() const { return selected_media_; }

    // Last search results
    void setLastSearchResults(const std::vector<MediaInfo>& results) { last_results_ = results; }
    const std::vector<MediaInfo>& getLastSearchResults() const { return last_results_; }

private:
    ConfigManager config_;
    HistoryManager history_;
    FavoriteManager favorites_;
    std::unique_ptr<IMediaBackend> backend_;
    std::unique_ptr<IMediaPlayer> player_;

    MediaInfo selected_media_;
    std::vector<MediaInfo> last_results_;
};

} // namespace mediacli
