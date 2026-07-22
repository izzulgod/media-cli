#pragma once

#include <string>
#include <chrono>
#include <nlohmann/json.hpp>

namespace mediacli {

struct MediaInfo {
    std::string id;
    std::string title;
    std::string channel;
    std::string duration;
    std::string views;
    std::string url;
    std::string thumbnail_url;
    std::string backend_name;
    int64_t timestamp{0}; // Epoch timestamp when played/added

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MediaInfo, id, title, channel, duration, views, url, thumbnail_url, backend_name, timestamp)
};

} // namespace mediacli
