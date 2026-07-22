#pragma once

#include "player/IMediaPlayer.hpp"

namespace mediacli {

class MpvPlayer : public IMediaPlayer {
public:
    MpvPlayer() = default;

    std::string name() const override { return "mpv"; }
    void play(const std::string& streamUrl, const std::string& title, VideoQuality quality) override;
    void stop() override;
    bool isPlaying() const override;

private:
    bool is_playing_{false};
};

} // namespace mediacli
