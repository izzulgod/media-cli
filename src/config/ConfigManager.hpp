#pragma once

#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "core/Types.hpp"

namespace mediacli {

class ConfigManager {
public:
    explicit ConfigManager(std::filesystem::path configPath = "");

    bool load();
    bool save();

    std::string getTheme() const;
    void setTheme(const std::string& theme);

    std::string getPlayerName() const;
    void setPlayerName(const std::string& player);

    std::string getBackendName() const;
    void setBackendName(const std::string& backend);

    std::string getDownloadPath() const;
    void setDownloadPath(const std::string& path);

    VideoQuality getVideoQuality() const;
    void setVideoQuality(VideoQuality quality);

    std::string getCookieBrowser() const;
    void setCookieBrowser(const std::string& browser);

    // Generic JSON accessors
    template <typename T>
    T get(const std::string& key, const T& defaultValue) const {
        if (data_.contains(key)) {
            try {
                return data_[key].get<T>();
            } catch (...) {}
        }
        return defaultValue;
    }

    template <typename T>
    void set(const std::string& key, const T& value) {
        data_[key] = value;
    }

private:
    void setDefaults();

    std::filesystem::path path_;
    nlohmann::json data_;
};

} // namespace mediacli
