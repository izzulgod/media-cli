#include "player/PlayerFactory.hpp"
#include "player/mpv/MpvPlayer.hpp"
#include "utils/Logger.hpp"

namespace mediacli {

std::unique_ptr<IMediaPlayer> PlayerFactory::create(const std::string& name) {
    if (name == "mpv" || name.empty()) {
        return std::make_unique<MpvPlayer>();
    }

    LOG_WARN("Unknown player '{}', falling back to 'mpv'", name);
    return std::make_unique<MpvPlayer>();
}

std::vector<std::string> PlayerFactory::availablePlayers() {
    return {"mpv"};
}

} // namespace mediacli
