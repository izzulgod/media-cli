#pragma once

#include <memory>
#include <string>
#include <vector>
#include "backend/IMediaBackend.hpp"
#include "config/ConfigManager.hpp"

namespace mediacli {

class BackendFactory {
public:
    static std::unique_ptr<IMediaBackend> create(const std::string& name, const ConfigManager& config);
    static std::vector<std::string> availableBackends();
};

} // namespace mediacli
