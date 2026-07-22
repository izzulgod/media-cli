#pragma once

#include "backend/IMediaBackend.hpp"

namespace mediacli {

class YouTubeBackend : public IMediaBackend {
public:
    YouTubeBackend() = default;

    std::string name() const override { return "youtube"; }
    std::vector<MediaInfo> search(const std::string& query, int limit = 10) override;
    std::string getStreamUrl(const MediaInfo& media, VideoQuality quality = VideoQuality::Best) override;
    bool download(const MediaInfo& media, const std::string& downloadPath, VideoQuality quality = VideoQuality::Best) override;
    void setCookieBrowser(const std::string& browserName) override;

private:
    std::string buildFormatFlag(VideoQuality quality) const;
    std::string getCookieArgs() const;

    std::string cookie_browser_;
};

} // namespace mediacli
