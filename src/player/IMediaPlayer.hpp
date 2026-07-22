#pragma once

#include <string>
#include "core/Types.hpp"

namespace mediacli {

class IMediaPlayer {
public:
    virtual ~IMediaPlayer() = default;

    virtual std::string name() const = 0;
    virtual void play(const std::string& streamUrl, const std::string& title, VideoQuality quality) = 0;
    virtual void stop() = 0;
    virtual bool isPlaying() const = 0;
};

} // namespace mediacli
