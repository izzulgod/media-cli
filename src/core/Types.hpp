#pragma once

#include <string>
#include <string_view>

namespace mediacli {

enum class VideoQuality {
    AudioOnly,
    Low_360p,
    Medium_720p,
    High_1080p,
    Best
};

inline std::string videoQualityToString(VideoQuality q) {
    switch (q) {
        case VideoQuality::AudioOnly: return "audio_only";
        case VideoQuality::Low_360p: return "360p";
        case VideoQuality::Medium_720p: return "720p";
        case VideoQuality::High_1080p: return "1080p";
        case VideoQuality::Best: return "best";
    }
    return "best";
}

inline VideoQuality stringToVideoQuality(std::string_view str) {
    if (str == "audio_only") return VideoQuality::AudioOnly;
    if (str == "360p") return VideoQuality::Low_360p;
    if (str == "720p") return VideoQuality::Medium_720p;
    if (str == "1080p") return VideoQuality::High_1080p;
    return VideoQuality::Best;
}

enum class ViewState {
    MainMenu,
    SearchInput,
    SearchResults,
    VideoMenu,
    History,
    Favorites,
    Settings,
    About,
    Exit
};

} // namespace mediacli
