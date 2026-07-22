#pragma once

#include <vector>
#include <filesystem>
#include "core/MediaInfo.hpp"

namespace mediacli {

class HistoryManager {
public:
    explicit HistoryManager(std::filesystem::path historyPath = "");

    void add(MediaInfo item);
    std::vector<MediaInfo> getAll() const;
    void clear();
    bool load();
    bool save();

private:
    std::filesystem::path path_;
    std::vector<MediaInfo> history_;
    static constexpr size_t kMaxHistoryEntries = 500;
};

} // namespace mediacli
