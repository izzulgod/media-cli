cmake_minimum_required(VERSION 3.16)
project(MediaCLI VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# FetchContent module for external libraries
include(FetchContent)

# 1. nlohmann_json
FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)

# 2. fmt
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        10.2.1
)

# 3. spdlog
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.13.0
)

# 4. FTXUI
FetchContent_Declare(
    ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI.git
    GIT_TAG        v5.0.0
)

set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE)

message(STATUS "Fetching dependencies...")
FetchContent_MakeAvailable(nlohmann_json fmt spdlog ftxui)

# Source files
set(SOURCES
    src/main.cpp
    src/utils/Logger.cpp
    src/utils/Process.cpp
    src/utils/FileUtils.cpp
    src/config/ConfigManager.cpp
    src/backend/BackendFactory.cpp
    src/backend/youtube/YouTubeBackend.cpp
    src/player/PlayerFactory.cpp
    src/player/mpv/MpvPlayer.cpp
    src/history/HistoryManager.cpp
    src/favorite/FavoriteManager.cpp
    src/core/Application.cpp
    src/ui/App.cpp
    src/ui/MainMenu.cpp
    src/ui/SearchView.cpp
    src/ui/ResultsView.cpp
    src/ui/VideoMenu.cpp
    src/ui/HistoryView.cpp
    src/ui/FavoritesView.cpp
    src/ui/SettingsView.cpp
    src/ui/AboutView.cpp
)

add_executable(mediacli ${SOURCES})

target_include_directories(mediacli PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(mediacli PRIVATE
    nlohmann_json::nlohmann_json
    fmt::fmt
    spdlog::spdlog
    ftxui::component
    ftxui::dom
    ftxui::screen
)
