#include "history/HistoryManager.hpp"
#include "utils/FileUtils.hpp"
#include "utils/Logger.hpp"
#include <algorithm>
#include <chrono>

namespace mediacli {

HistoryManager::HistoryManager(std::filesystem::path historyPath) {
    if (historyPath.empty()) {
        path_ = FileUtils::getConfigDir() / "history.json";
    } else {
        path_ = std::move(historyPath);
    }
    load();
}

void HistoryManager::add(MediaInfo item) {
    item.timestamp = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();

    // Remove existing duplicate if any
    auto it = std::remove_if(history_.begin(), history_.end(), [&](const MediaInfo& info) {
        return info.id == item.id && info.backend_name == item.backend_name;
    });
    history_.erase(it, history_.end());

    // Insert at front (most recent first)
    history_.insert(history_.begin(), std::move(item));

    if (history_.size() > kMaxHistoryEntries) {
        history_.resize(kMaxHistoryEntries);
    }

    save();
}

std::vector<MediaInfo> HistoryManager::getAll() const {
    return history_;
}

void HistoryManager::clear() {
    history_.clear();
    save();
}

bool HistoryManager::load() {
    nlohmann::json j;
    if (FileUtils::readJson(path_, j) && j.is_array()) {
        history_ = j.get<std::vector<MediaInfo>>();
        LOG_INFO("History loaded ({} items)", history_.size());
        return true;
    }
    return false;
}

bool HistoryManager::save() {
    nlohmann::json j = history_;
    return FileUtils::writeJson(path_, j);
}

} // namespace mediacli
