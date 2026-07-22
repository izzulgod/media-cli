#include "player/mpv/MpvPlayer.hpp"
#include "utils/Process.hpp"
#include "utils/Logger.hpp"
#include <sstream>

namespace mediacli {

void MpvPlayer::play(const std::string& streamUrl, const std::string& title, VideoQuality quality) {
    if (!Process::isCommandAvailable("mpv")) {
        LOG_ERROR("mpv executable not found in PATH!");
        return;
    }

    std::ostringstream cmd;
    cmd << "mpv ";
    if (quality == VideoQuality::AudioOnly) {
        cmd << "--no-video ";
    }
    cmd << "--title=\"MediaCLI - " << title << "\" ";
    cmd << "\"" << streamUrl << "\"";

    is_playing_ = true;
    LOG_INFO("Starting playback: {}", title);
    Process::runInteractive(cmd.str());
    is_playing_ = false;
    LOG_INFO("Playback finished: {}", title);
}

void MpvPlayer::stop() {
    is_playing_ = false;
}

bool MpvPlayer::isPlaying() const {
    return is_playing_;
}

} // namespace mediacli
