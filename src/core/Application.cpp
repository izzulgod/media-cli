#include "core/Application.hpp"
#include "backend/BackendFactory.hpp"
#include "player/PlayerFactory.hpp"
#include "utils/Logger.hpp"
#include "utils/Process.hpp"

namespace mediacli {

Application::Application() {
    init();
}

void Application::init() {
    config_.load();
    reloadBackendAndPlayer();
}

void Application::reloadBackendAndPlayer() {
    backend_ = BackendFactory::create(config_.getBackendName(), config_);
    player_ = PlayerFactory::create(config_.getPlayerName());
}

std::vector<MediaInfo> Application::search(const std::string& query, int limit) {
    if (!backend_) {
        LOG_ERROR("Backend not initialized!");
        return {};
    }
    last_results_ = backend_->search(query, limit);
    return last_results_;
}

bool Application::playMedia(const MediaInfo& media) {
    if (!backend_ || !player_) {
        LOG_ERROR("Backend or Player not initialized!");
        return false;
    }

    LOG_INFO("Fetching stream URL for media: {}", media.title);
    std::string streamUrl = backend_->getStreamUrl(media, config_.getVideoQuality());
    if (streamUrl.empty()) {
        LOG_ERROR("Could not get stream URL for media: {}", media.title);
        return false;
    }

    // Automatically record in history
    history_.add(media);

    player_->play(streamUrl, media.title, config_.getVideoQuality());
    return true;
}

bool Application::downloadMedia(const MediaInfo& media) {
    if (!backend_) {
        LOG_ERROR("Backend not initialized!");
        return false;
    }
    return backend_->download(media, config_.getDownloadPath(), config_.getVideoQuality());
}

bool Application::openInBrowser(const MediaInfo& media) {
    if (media.url.empty()) return false;
    std::string cmd = "xdg-open \"" + media.url + "\" >/dev/null 2>&1 &";
    LOG_INFO("Opening URL in browser: {}", media.url);
    return Process::runInteractive(cmd) == 0;
}

std::vector<MediaInfo> Application::getHistory() const {
    return history_.getAll();
}

void Application::clearHistory() {
    history_.clear();
}

std::vector<MediaInfo> Application::getFavorites() const {
    return favorites_.getAll();
}

bool Application::isFavorite(const MediaInfo& media) const {
    return favorites_.isFavorite(media.id, media.backend_name);
}

void Application::toggleFavorite(const MediaInfo& media) {
    favorites_.toggleFavorite(media);
}

} // namespace mediacli
