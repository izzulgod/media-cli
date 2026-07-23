#include "player/mpv/MpvPlayer.hpp"
#include "utils/Process.hpp"
#include "utils/Logger.hpp"
#include <sstream>
#include <vector>

namespace mediacli {

static std::string buildYtdlFormat(VideoQuality quality) {
    switch (quality) {
        case VideoQuality::AudioOnly:
            return "ba/b";
        case VideoQuality::Low_360p:
            return "bv*[height<=360]+ba/b[height<=360]/b";
        case VideoQuality::Medium_720p:
            return "bv*[height<=720]+ba/b[height<=720]/b";
        case VideoQuality::High_1080p:
            return "bv*[height<=1080]+ba/b[height<=1080]/b";
        case VideoQuality::Best:
        default:
            return "bv*+ba/b";
    }
}

void MpvPlayer::play(const std::string& streamUrl, const std::string& title, VideoQuality quality) {
    if (!Process::isCommandAvailable("mpv")) {
        LOG_ERROR("mpv executable not found in PATH!");
        return;
    }

    std::vector<std::string> urls;
    std::istringstream iss(streamUrl);
    std::string line;
    while (std::getline(iss, line)) {
        if (!line.empty()) {
            urls.push_back(line);
        }
    }

    std::ostringstream cmd;
    cmd << "mpv ";

    if (quality == VideoQuality::AudioOnly) {
        cmd << "--no-video ";
    }

    cmd << "--title=\"MediaCLI - " << title << "\" ";

    if (urls.size() >= 2) {
        // Dual stream output from yt-dlp -g (Line 1 = Video URL, Line 2 = Audio URL)
        std::string video_url = urls[0];
        std::string audio_url = urls[1];

        if (quality == VideoQuality::AudioOnly) {
            cmd << "\"" << audio_url << "\"";
        } else {
            cmd << "--audio-file=\"" << audio_url << "\" \"" << video_url << "\"";
        }
    } else if (urls.size() == 1) {
        std::string single_url = urls[0];
        if (single_url.find("http://") == 0 || single_url.find("https://") == 0) {
            if (single_url.find("googlevideo.com") != std::string::npos) {
                cmd << "\"" << single_url << "\"";
            } else {
                cmd << "--ytdl-format=\"" << buildYtdlFormat(quality) << "\" \"" << single_url << "\"";
            }
        } else {
            cmd << "\"" << single_url << "\"";
        }
    }

    is_playing_ = true;
    LOG_INFO("Starting mpv playback: {}", title);
    Process::runInteractive(cmd.str());
    is_playing_ = false;
    LOG_INFO("Mpv playback finished: {}", title);
}

void MpvPlayer::stop() {
    is_playing_ = false;
}

bool MpvPlayer::isPlaying() const {
    return is_playing_;
}

} // namespace mediacli
