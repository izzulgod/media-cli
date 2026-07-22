#pragma once

#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace mediacli {

class FileUtils {
public:
    static std::filesystem::path expandTilde(const std::filesystem::path& path);
    static std::filesystem::path getConfigDir();
    static bool readJson(const std::filesystem::path& filePath, nlohmann::json& outJson);
    static bool writeJson(const std::filesystem::path& filePath, const nlohmann::json& inJson);
};

} // namespace mediacli
