#include "config/ConfigManager.hpp"
#include "utils/FileUtils.hpp"
#include "utils/Logger.hpp"

namespace mediacli {

ConfigManager::ConfigManager(std::filesystem::path configPath) {
    if (configPath.empty()) {
        path_ = FileUtils::getConfigDir() / "config.json";
    } else {
        path_ = std::move(configPath);
    }
    setDefaults();
}

void ConfigManager::setDefaults() {
    data_["theme"] = "dark";
    data_["player"] = "mpv";
    data_["backend"] = "youtube";
    data_["download_path"] = "~/Downloads/MediaCLI";
    data_["video_quality"] = "best";
    data_["cookie_browser"] = "";
}

bool ConfigManager::load() {
    nlohmann::json loadedJson;
    if (FileUtils::readJson(path_, loadedJson)) {
        for (auto& [key, val] : loadedJson.items()) {
            data_[key] = val;
        }
        LOG_INFO("Config loaded from {}", path_.string());
        return true;
    }
    LOG_INFO("Config file not found. Creating default config at {}", path_.string());
    save();
    return false;
}

bool ConfigManager::save() {
    bool ok = FileUtils::writeJson(path_, data_);
    if (ok) {
        LOG_INFO("Config saved to {}", path_.string());
    } else {
        LOG_ERROR("Failed to save config to {}", path_.string());
    }
    return ok;
}

std::string ConfigManager::getTheme() const {
    return get<std::string>("theme", "dark");
}

void ConfigManager::setTheme(const std::string& theme) {
    set("theme", theme);
}

std::string ConfigManager::getPlayerName() const {
    return get<std::string>("player", "mpv");
}

void ConfigManager::setPlayerName(const std::string& player) {
    set("player", player);
}

std::string ConfigManager::getBackendName() const {
    return get<std::string>("backend", "youtube");
}

void ConfigManager::setBackendName(const std::string& backend) {
    set("backend", backend);
}

std::string ConfigManager::getDownloadPath() const {
    return get<std::string>("download_path", "~/Downloads/MediaCLI");
}

void ConfigManager::setDownloadPath(const std::string& path) {
    set("download_path", path);
}

VideoQuality ConfigManager::getVideoQuality() const {
    std::string str = get<std::string>("video_quality", "best");
    return stringToVideoQuality(str);
}

void ConfigManager::setVideoQuality(VideoQuality quality) {
    set("video_quality", videoQualityToString(quality));
}

std::string ConfigManager::getCookieBrowser() const {
    return get<std::string>("cookie_browser", "");
}

void ConfigManager::setCookieBrowser(const std::string& browser) {
    set("cookie_browser", browser);
}

} // namespace mediacli
