# MediaCLI

```
  __  __ _____ ____  ___    _     ____ _     ___ 
 |  \/  | ____|  _ \|_ _|  / \   / ___| |   |_ _|
 | |\/| |  _| | | | || |  / _ \ | |   | |    | | 
 | |  | | |___| |_| || | / ___ \| |___| |___ | | 
 |_|  |_|_____|____/|___/_/   \_\ \____|_____|___|
```

MediaCLI is a lightweight, modern, and modular terminal media client written in C++20 using FTXUI. It is designed around a strictly decoupled backend architecture that separates application business logic, user interface rendering, media provider backends, and playback engines.

---

## Architecture Overview

MediaCLI follows a layered domain-driven architecture with clean separation of concerns:

```mermaid
graph TD
    subgraph Presentation Layer
        UI["UI (FTXUI AppUI & Views)"]
    end

    subgraph Application Layer
        APP["Application Controller"]
    end

    subgraph Domain Layer
        BE["IMediaBackend Interface"]
        PL["IMediaPlayer Interface"]
        HIS["HistoryManager"]
        FAV["FavoriteManager"]
        CFG["ConfigManager"]
    end

    subgraph Infrastructure Layer
        YT["YouTubeBackend"]
        MPV["MpvPlayer"]
        FS["JSON File Storage"]
        LOG["spdlog Logger"]
    end

    subgraph External Dependencies
        YTDLP["yt-dlp"]
        MPVBIN["mpv"]
        YOUTUBE["YouTube"]
    end

    UI --> APP
    APP --> BE
    APP --> PL
    APP --> HIS
    APP --> FAV
    APP --> CFG
    BE -.-> YT
    PL -.-> MPV
    HIS --> FS
    FAV --> FS
    CFG --> FS
    APP --> LOG
    YT --> YTDLP
    MPV --> MPVBIN
    YTDLP --> YOUTUBE
```

### Architectural Principles

- **Zero Direct Provider Coupling**: The core Application controller and FTXUI Presentation layer never interact directly with YouTube, yt-dlp, or mpv.
- **Interface Abstraction**: All media providers implement `IMediaBackend`. All media players implement `IMediaPlayer`.
- **Extensible Backend System**: Adding a new provider (e.g., Spotify, SoundCloud, Podcasts, Local Files) requires zero modifications to core Application or UI logic.
- **Asynchronous Execution**: Expensive IO operations (searching, stream URL resolution) run on background threads with real-time TUI braille spinner animations, ensuring the TUI never hangs or blocks.
- **Alternate Screen Buffer Isolation**: FTXUI renders in Fullscreen mode (`ScreenInteractive::Fullscreen()`). Logging output is written strictly to `logs/error.log` to preserve terminal screen cleanliness.

---

## Class Architecture

```mermaid
classDiagram
    direction LR

    class MediaInfo {
        +string id
        +string title
        +string channel
        +string duration
        +string views
        +string url
        +string thumbnail_url
        +string backend_name
        +int64_t timestamp
    }

    class IMediaBackend {
        <<interface>>
        +name() string
        +search(query, limit) vector~MediaInfo~
        +getStreamUrl(media, quality) string
        +download(media, path, quality) bool
        +setCookieBrowser(browserName) void
    }

    class YouTubeBackend {
        -cookie_browser_ : string
        +name() string
        +search(query, limit) vector~MediaInfo~
        +getStreamUrl(media, quality) string
        +download(media, path, quality) bool
        +setCookieBrowser(browserName) void
    }

    class IMediaPlayer {
        <<interface>>
        +name() string
        +play(streamUrl, title, quality) void
        +stop() void
        +isPlaying() bool
    }

    class MpvPlayer {
        -is_playing_ : bool
        +name() string
        +play(streamUrl, title, quality) void
        +stop() void
        +isPlaying() bool
    }

    class BackendFactory {
        +create(name, config) unique_ptr~IMediaBackend~
        +availableBackends() vector~string~
    }

    class PlayerFactory {
        +create(name) unique_ptr~IMediaPlayer~
        +availablePlayers() vector~string~
    }

    class Application {
        -config_ : ConfigManager
        -history_ : HistoryManager
        -favorites_ : FavoriteManager
        -backend_ : unique_ptr~IMediaBackend~
        -player_ : unique_ptr~IMediaPlayer~
        +search(query, limit) vector~MediaInfo~
        +playMedia(media) bool
        +downloadMedia(media) bool
        +openInBrowser(media) bool
        +getHistory() vector~MediaInfo~
        +getFavorites() vector~MediaInfo~
        +toggleFavorite(media) void
    }

    class HistoryManager {
        -history_ : vector~MediaInfo~
        -path_ : path
        +add(item) void
        +getAll() vector~MediaInfo~
        +clear() void
        +load() bool
        +save() bool
    }

    class FavoriteManager {
        -favorites_ : vector~MediaInfo~
        -path_ : path
        +add(item) void
        +remove(id, backendName) void
        +isFavorite(id, backendName) bool
        +toggleFavorite(item) void
        +getAll() vector~MediaInfo~
    }

    class ConfigManager {
        -data_ : json
        -path_ : path
        +load() bool
        +save() bool
        +getTheme() string
        +getPlayerName() string
        +getBackendName() string
        +getDownloadPath() string
        +getVideoQuality() VideoQuality
        +getCookieBrowser() string
    }

    class AppUI {
        -app_ : Application&
        -screen_ : ScreenInteractive
        +run() void
    }

    IMediaBackend <|.. YouTubeBackend
    IMediaPlayer <|.. MpvPlayer
    BackendFactory ..> IMediaBackend : creates
    PlayerFactory ..> IMediaPlayer : creates
    Application o-- IMediaBackend
    Application o-- IMediaPlayer
    Application o-- HistoryManager
    Application o-- FavoriteManager
    Application o-- ConfigManager
    AppUI --> Application : uses
```

---

## Application Control Flow

```mermaid
flowchart TD
    START([main.cpp]) --> INIT[Initialize Logger]
    INIT --> CFG[Load ConfigManager]
    CFG --> FACTORY[Instantiate Backend & Player via Factories]
    FACTORY --> APP[Construct Application Controller]
    APP --> UI[Launch FTXUI AppUI Event Loop]

    UI --> MAIN_MENU{Main Menu}

    MAIN_MENU -->|Search Media| SEARCH_INPUT[Enter Search Query]
    MAIN_MENU -->|Watch History| HISTORY_VIEW[Show Watch History List]
    MAIN_MENU -->|Favorite Media| FAV_VIEW[Show Favorite Media List]
    MAIN_MENU -->|Settings| SETTINGS_VIEW[Show Settings Form]
    MAIN_MENU -->|About App| ABOUT_VIEW[Show About Screen]
    MAIN_MENU -->|Exit Application| EXIT([Exit])

    SEARCH_INPUT --> ASYNC_SEARCH[Async Background Search Thread + Ticker Spinner]
    ASYNC_SEARCH --> RESULTS{Search Results View}

    RESULTS -->|Select Media Item| VIDEO_MENU{Video Action Menu}
    RESULTS -->|ESC| MAIN_MENU

    VIDEO_MENU -->|Play Stream| ASYNC_PLAY[Async Stream URL Resolution + MpvPlayer]
    VIDEO_MENU -->|Download File| DOWNLOAD[YouTubeBackend.download via yt-dlp]
    VIDEO_MENU -->|Open Browser| BROWSER[Open URL via xdg-open]
    VIDEO_MENU -->|Toggle Favorite| FAV_TOGGLE[FavoriteManager.toggleFavorite]
    VIDEO_MENU -->|Back| RESULTS

    ASYNC_PLAY --> SAVE_HISTORY[HistoryManager.add]
    SAVE_HISTORY --> RESULTS

    HISTORY_VIEW -->|Select Item| VIDEO_MENU
    HISTORY_VIEW -->|ESC| MAIN_MENU

    FAV_VIEW -->|Select Item| VIDEO_MENU
    FAV_VIEW -->|ESC| MAIN_MENU

    SETTINGS_VIEW -->|Save Settings| SAVE_CFG[ConfigManager.save + Reload Backend/Player]
    SAVE_CFG --> MAIN_MENU
    SETTINGS_VIEW -->|ESC| MAIN_MENU

    ABOUT_VIEW -->|Open GitHub| OPEN_GH[Open Repository via xdg-open]
    ABOUT_VIEW -->|ESC| MAIN_MENU
```

---

## Directory Structure & Module Responsibilities

```
Media-CLI/
├── CMakeLists.txt                  # Build configuration with FetchContent dependencies
├── README.md                       # Complete technical documentation
├── LICENSE                         # MIT License
├── .gitignore                      # Git exclusion rules
├── src/
│   ├── main.cpp                    # Application entry point
│   ├── core/
│   │   ├── Application.hpp / .cpp  # Central Application controller orchestrator
│   │   ├── MediaInfo.hpp           # Data model struct with JSON serialization
│   │   └── Types.hpp               # VideoQuality, ViewState enums, and helpers
│   ├── backend/
│   │   ├── IMediaBackend.hpp       # Pure virtual interface for media providers
│   │   ├── BackendFactory.hpp/.cpp # Factory for instantiating backend implementations
│   │   └── youtube/
│   │       ├── YouTubeBackend.hpp  # YouTube provider header
│   │       └── YouTubeBackend.cpp  # yt-dlp process runner & JSON extractor
│   ├── player/
│   │   ├── IMediaPlayer.hpp        # Pure virtual interface for media players
│   │   ├── PlayerFactory.hpp/.cpp  # Factory for instantiating media players
│   │   └── mpv/
│   │       ├── MpvPlayer.hpp       # mpv player wrapper header
│   │       └── MpvPlayer.cpp       # Interactive mpv subprocess runner
│   ├── ui/
│   │   ├── App.hpp / .cpp          # FTXUI root controller, screen manager & async worker coordinator
│   │   ├── Theme.hpp               # Color palette definitions & header elements
│   │   ├── MainMenu.hpp / .cpp     # Centered Main Menu component
│   │   ├── SearchView.hpp / .cpp   # Non-blocking search query input & braille spinner
│   │   ├── ResultsView.hpp / .cpp  # Scrollable wide media result table
│   │   ├── VideoMenu.hpp / .cpp    # Context action menu for selected media items
│   │   ├── HistoryView.hpp / .cpp  # Watch history list screen
│   │   ├── FavoritesView.hpp / .cpp# Favorite media list screen
│   │   ├── SettingsView.hpp / .cpp # Interactive configuration form
│   │   └── AboutView.hpp / .cpp    # Centered About page with interactive GitHub link
│   ├── history/
│   │   ├── HistoryManager.hpp/.cpp # Local watch history JSON persistence
│   ├── favorite/
│   │   ├── FavoriteManager.hpp/.cpp# Local favorite media JSON persistence
│   ├── config/
│   │   ├── ConfigManager.hpp / .cpp# User preferences JSON manager
│   └── utils/
│       ├── Logger.hpp / .cpp       # spdlog configuration (file-only sink)
│       ├── Process.hpp / .cpp      # Subprocess execution & shell argument escaping
│       └── FileUtils.hpp / .cpp    # Path tilde expansion & directory helpers
```

---

## Tech Stack & Dependencies

### C++ Libraries (Managed via CMake `FetchContent`)

- **FTXUI** (v5.0.0) — Functional Terminal User Interface framework.
- **nlohmann/json** (v3.11.3) — JSON parser and serializer.
- **fmt** (v10.2.1) — Safe, fast string formatting library.
- **spdlog** (v1.13.0) — Fast C++ logging library.

### External System Utilities

- **yt-dlp** — Extracting metadata, stream URLs, and downloading media.
- **mpv** — Native media player for video and audio playback.

---

## Prerequisites & Installation

### System Dependencies Installation

```bash
# Ubuntu / Debian
sudo apt update
sudo apt install build-essential cmake yt-dlp mpv

# Arch Linux
sudo pacman -S gcc cmake yt-dlp mpv

# Fedora
sudo dnf install gcc-c++ cmake yt-dlp mpv
```

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/izzulgod/media-cli.git
cd media-cli

# Configure and compile
cmake -B build -S .
cmake --build build -j$(nproc)
```

### Execution

```bash
./build/mediacli
```

---

## Keyboard Controls & Navigation

| Context | Action | Shortcut |
|---|---|---|
| **Global** | Return to Main Menu | `ESC` |
| **Main Menu** | Navigate Options | `Up` / `Down` |
| **Main Menu** | Select Option | `Enter` |
| **Main Menu** | Quick Exit | `q` / `ESC` |
| **Search Results** | Navigate Results List | `Up` / `Down` |
| **Search Results** | Open Media Action Menu | `Enter` |
| **Settings / Forms** | Move Between Form Input Fields | `Tab` / `Shift+Tab` |
| **Player (mpv)** | Pause / Play Media | `Space` |
| **Player (mpv)** | Seek Backward / Forward | `Left` / `Right` |
| **Player (mpv)** | Volume Control | `9` / `0` |
| **Player (mpv)** | Quit Playback | `q` |

---

## Configuration & Storage Formats

MediaCLI stores configuration and user data under `~/.config/mediacli/`:

### `config.json`
```json
{
    "backend": "youtube",
    "cookie_browser": "",
    "download_path": "~/Downloads/MediaCLI",
    "player": "mpv",
    "theme": "dark",
    "video_quality": "best"
}
```

### `history.json` & `favorites.json`
```json
[
    {
        "backend_name": "youtube",
        "channel": "Deddy Corbuzier",
        "duration": "1:20:49",
        "id": "WzW3hUqnaGI",
        "thumbnail_url": "https://i.ytimg.com/vi/WzW3hUqnaGI/hqdefault.jpg",
        "timestamp": 1784760292,
        "title": "Example Video Title",
        "url": "https://www.youtube.com/watch?v=WzW3hUqnaGI",
        "views": "3.8M"
    }
]
```

---

## Developer Guide: Adding New Backends

To add a new media backend (e.g., Spotify, SoundCloud, Podcasts):

1. Create a class implementing `IMediaBackend`:
   ```cpp
   #include "backend/IMediaBackend.hpp"

   namespace mediacli {

   class SpotifyBackend : public IMediaBackend {
   public:
       std::string name() const override { return "spotify"; }
       std::vector<MediaInfo> search(const std::string& query, int limit = 20) override;
       std::string getStreamUrl(const MediaInfo& media, VideoQuality quality) override;
       bool download(const MediaInfo& media, const std::string& downloadPath, VideoQuality quality) override;
       void setCookieBrowser(const std::string& browserName) override;
   };

   } // namespace mediacli
   ```
2. Register the backend in `BackendFactory::create()`:
   ```cpp
   if (name == "spotify") {
       return std::make_unique<SpotifyBackend>();
   }
   ```
3. Zero changes are needed in `Application.cpp` or any `ui/` components.

---

## Author & License

- **Author / Developer**: [izzulgod](https://github.com/izzulgod)
- **Repository**: [https://github.com/izzulgod/media-cli](https://github.com/izzulgod/media-cli)
- **License**: Distributed under the [MIT License](LICENSE).
