#include "backend/youtube/YouTubeBackend.hpp"
#include "utils/Process.hpp"
#include "utils/Logger.hpp"
#include "utils/FileUtils.hpp"
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip>

namespace mediacli {

static std::string formatDuration(double seconds) {
    if (seconds <= 0) return "Unknown";
    int sec = static_cast<int>(seconds);
    int h = sec / 3600;
    int m = (sec % 3600) / 60;
    int s = sec % 60;
    std::ostringstream oss;
    if (h > 0) {
        oss << h << ":" << std::setw(2) << std::setfill('0') << m << ":" << std::setw(2) << std::setfill('0') << s;
    } else {
        oss << m << ":" << std::setw(2) << std::setfill('0') << s;
    }
    return oss.str();
}

static std::string formatViews(int64_t count) {
    if (count <= 0) return "N/A";
    if (count >= 1'000'000'000) {
        return fmt::format("{:.1f}B", static_cast<double>(count) / 1'000'000'000.0);
    }
    if (count >= 1'000'000) {
        return fmt::format("{:.1f}M", static_cast<double>(count) / 1'000'000.0);
    }
    if (count >= 1'000) {
        return fmt::format("{:.1f}K", static_cast<double>(count) / 1'000.0);
    }
    return std::to_string(count);
}

void YouTubeBackend::setCookieBrowser(const std::string& browserName) {
    cookie_browser_ = browserName;
}

std::string YouTubeBackend::getCookieArgs() const {
    if (!cookie_browser_.empty()) {
        return "--cookies-from-browser " + cookie_browser_ + " ";
    }
    return "";
}

std::string YouTubeBackend::buildFormatFlag(VideoQuality quality) const {
    switch (quality) {
        case VideoQuality::AudioOnly:
            return "-f \"ba/b\"";
        case VideoQuality::Low_360p:
            return "-f \"bv*[height<=360]+ba/b[height<=360]/b\"";
        case VideoQuality::Medium_720p:
            return "-f \"bv*[height<=720]+ba/b[height<=720]/b\"";
        case VideoQuality::High_1080p:
            return "-f \"bv*[height<=1080]+ba/b[height<=1080]/b\"";
        case VideoQuality::Best:
        default:
            return "-f \"bv*+ba/b\"";
    }
}

std::vector<MediaInfo> YouTubeBackend::search(const std::string& query, int limit) {
    std::vector<MediaInfo> results;
    if (query.empty()) return results;

    if (!Process::isCommandAvailable("yt-dlp")) {
        LOG_ERROR("yt-dlp executable not found in PATH!");
        return results;
    }

    std::ostringstream cmd;
    cmd << "yt-dlp " << getCookieArgs()
        << "\"ytsearch" << limit << ":" << query << "\" "
        << "--flat-playlist --ignore-errors --dump-json --skip-download --quiet --no-playlist";

    auto res = Process::execute(cmd.str(), false);
    if (res.exit_code != 0 && res.stdout_output.empty() && !cookie_browser_.empty()) {
        // Fallback without cookies if cookie search failed
        std::ostringstream fallbackCmd;
        fallbackCmd << "yt-dlp \"ytsearch" << limit << ":" << query << "\" "
                    << "--flat-playlist --ignore-errors --dump-json --skip-download --quiet --no-playlist";
        res = Process::execute(fallbackCmd.str(), false);
    }

    std::istringstream iss(res.stdout_output);
    std::string line;
    while (std::getline(iss, line)) {
        if (line.empty()) continue;
        try {
            auto j = nlohmann::json::parse(line);
            MediaInfo info;
            info.id = j.value("id", "");
            info.title = j.value("title", "Unknown Title");
            info.channel = j.value("uploader", j.value("channel", "Unknown Channel"));
            
            if (j.contains("duration") && !j["duration"].is_null() && j["duration"].is_number()) {
                info.duration = formatDuration(j["duration"].get<double>());
            } else if (j.contains("duration_string") && j["duration_string"].is_string()) {
                info.duration = j["duration_string"].get<std::string>();
            } else {
                info.duration = "N/A";
            }

            if (j.contains("view_count") && !j["view_count"].is_null() && j["view_count"].is_number()) {
                info.views = formatViews(j["view_count"].get<int64_t>());
            } else {
                info.views = "N/A";
            }

            if (j.contains("webpage_url") && j["webpage_url"].is_string()) {
                info.url = j["webpage_url"].get<std::string>();
            } else if (j.contains("url") && j["url"].is_string()) {
                info.url = j["url"].get<std::string>();
            } else {
                info.url = "https://www.youtube.com/watch?v=" + info.id;
            }

            info.thumbnail_url = j.value("thumbnail", "");
            info.backend_name = name();

            results.push_back(info);
        } catch (const std::exception& e) {
            LOG_WARN("Failed to parse yt-dlp search result line: {}", e.what());
        }
    }

    LOG_INFO("YouTube search for '{}' returned {} results", query, results.size());
    return results;
}

std::string YouTubeBackend::getStreamUrl(const MediaInfo& media, VideoQuality quality) {
    if (!Process::isCommandAvailable("yt-dlp")) {
        LOG_ERROR("yt-dlp not found");
        return media.url;
    }

    std::ostringstream cmd;
    cmd << "yt-dlp " << getCookieArgs()
        << buildFormatFlag(quality) << " "
        << "-g \"" << media.url << "\"";

    auto res = Process::execute(cmd.str(), false);
    if (res.exit_code == 0 && !res.stdout_output.empty()) {
        std::istringstream iss(res.stdout_output);
        std::string first_url;
        if (std::getline(iss, first_url) && !first_url.empty()) {
            return first_url;
        }
    }

    // Fallback without cookies if cookie extraction failed
    if (!cookie_browser_.empty()) {
        std::ostringstream fallbackCmd;
        fallbackCmd << "yt-dlp "
                    << buildFormatFlag(quality) << " "
                    << "-g \"" << media.url << "\"";
        auto fallbackRes = Process::execute(fallbackCmd.str(), false);
        if (fallbackRes.exit_code == 0 && !fallbackRes.stdout_output.empty()) {
            std::istringstream iss(fallbackRes.stdout_output);
            std::string first_url;
            if (std::getline(iss, first_url) && !first_url.empty()) {
                return first_url;
            }
        }
    }

    LOG_WARN("Failed to fetch stream URL using yt-dlp, falling back to direct URL");
    return media.url;
}

bool YouTubeBackend::download(const MediaInfo& media, const std::string& downloadPath, VideoQuality quality) {
    if (!Process::isCommandAvailable("yt-dlp")) {
        LOG_ERROR("yt-dlp not found");
        return false;
    }

    auto expPath = FileUtils::expandTilde(downloadPath);
    std::error_code ec;
    std::filesystem::create_directories(expPath, ec);

    std::string outTemplate = (expPath / "%(title)s.%(ext)s").string();

    // 1. Try downloading with cookies if configured
    std::ostringstream cmd;
    cmd << "yt-dlp " << getCookieArgs()
        << buildFormatFlag(quality) << " "
        << "--no-playlist "
        << "-o \"" << outTemplate << "\" "
        << "\"" << media.url << "\"";

    LOG_INFO("Downloading media '{}' to {}", media.title, expPath.string());
    int res = Process::runInteractive(cmd.str());

    // 2. If cookie download failed (e.g. format error / expired cookies), retry without cookies
    if (res != 0 && !cookie_browser_.empty()) {
        LOG_WARN("Download with cookies failed. Retrying without cookies...");
        std::ostringstream fallbackCmd;
        fallbackCmd << "yt-dlp "
                    << buildFormatFlag(quality) << " "
                    << "--no-playlist "
                    << "-o \"" << outTemplate << "\" "
                    << "\"" << media.url << "\"";
        res = Process::runInteractive(fallbackCmd.str());
    }

    return (res == 0);
}

} // namespace mediacli
