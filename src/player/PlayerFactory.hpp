#pragma once

#include <memory>
#include <string>
#include <vector>
#include "player/IMediaPlayer.hpp"

namespace mediacli {

class PlayerFactory {
public:
    static std::unique_ptr<IMediaPlayer> create(const std::string& name);
    static std::vector<std::string> availablePlayers();
};

} // namespace mediacli
