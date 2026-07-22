#pragma once

#include <string>
#include <vector>
#include "core/MediaInfo.hpp"
#include "core/Types.hpp"

namespace mediacli {

class IMediaBackend {
public:
    virtual ~IMediaBackend() = default;

    virtual std::string name() const = 0;
    virtual std::vector<MediaInfo> search(const std::string& query, int limit = 10) = 0;
    virtual std::string getStreamUrl(const MediaInfo& media, VideoQuality quality = VideoQuality::Best) = 0;
    virtual bool download(const MediaInfo& media, const std::string& downloadPath, VideoQuality quality = VideoQuality::Best) = 0;
    virtual void setCookieBrowser(const std::string& browserName) = 0;
};

} // namespace mediacli
