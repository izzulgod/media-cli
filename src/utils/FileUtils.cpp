#include "utils/FileUtils.hpp"
#include "utils/Logger.hpp"
#include <fstream>
#include <cstdlib>

namespace mediacli {

std::filesystem::path FileUtils::expandTilde(const std::filesystem::path& path) {
    std::string p = path.string();
    if (!p.empty() && p[0] == '~') {
        const char* home = std::getenv("HOME");
        if (home) {
            return std::filesystem::path(home) / p.substr(1 == p.size() ? 1 : (p[1] == '/' ? 2 : 1));
        }
    }
    return path;
}

std::filesystem::path FileUtils::getConfigDir() {
    const char* xdg_config = std::getenv("XDG_CONFIG_HOME");
    std::filesystem::path dir;
    if (xdg_config && *xdg_config != '\0') {
        dir = std::filesystem::path(xdg_config) / "mediacli";
    } else {
        const char* home = std::getenv("HOME");
        if (home) {
            dir = std::filesystem::path(home) / ".config" / "mediacli";
        } else {
            dir = std::filesystem::current_path() / ".config";
        }
    }
    std::error_code ec;
    std::filesystem::create_directories(dir, ec);
    return dir;
}

bool FileUtils::readJson(const std::filesystem::path& filePath, nlohmann::json& outJson) {
    auto expanded = expandTilde(filePath);
    if (!std::filesystem::exists(expanded)) {
        return false;
    }

    std::ifstream file(expanded);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open JSON file for reading: {}", expanded.string());
        return false;
    }

    try {
        file >> outJson;
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        LOG_ERROR("JSON parse error in {}: {}", expanded.string(), e.what());
        return false;
    }
}

bool FileUtils::writeJson(const std::filesystem::path& filePath, const nlohmann::json& inJson) {
    auto expanded = expandTilde(filePath);
    std::error_code ec;
    if (expanded.has_parent_path()) {
        std::filesystem::create_directories(expanded.parent_path(), ec);
    }

    std::ofstream file(expanded);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open JSON file for writing: {}", expanded.string());
        return false;
    }

    try {
        file << inJson.dump(4);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed writing JSON to {}: {}", expanded.string(), e.what());
        return false;
    }
}

} // namespace mediacli
