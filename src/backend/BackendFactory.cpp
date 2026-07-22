#include "backend/BackendFactory.hpp"
#include "backend/youtube/YouTubeBackend.hpp"
#include "utils/Logger.hpp"

namespace mediacli {

std::unique_ptr<IMediaBackend> BackendFactory::create(const std::string& name, const ConfigManager& config) {
    if (name == "youtube" || name.empty()) {
        auto backend = std::make_unique<YouTubeBackend>();
        backend->setCookieBrowser(config.getCookieBrowser());
        return backend;
    }

    LOG_WARN("Unknown backend '{}', falling back to 'youtube'", name);
    auto backend = std::make_unique<YouTubeBackend>();
    backend->setCookieBrowser(config.getCookieBrowser());
    return backend;
}

std::vector<std::string> BackendFactory::availableBackends() {
    return {"youtube"};
}

} // namespace mediacli
