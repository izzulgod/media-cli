#pragma once

#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <memory>
#include <thread>
#include <atomic>
#include "core/Application.hpp"
#include "core/Types.hpp"

#include "ui/MainMenu.hpp"
#include "ui/SearchView.hpp"
#include "ui/ResultsView.hpp"
#include "ui/VideoMenu.hpp"
#include "ui/HistoryView.hpp"
#include "ui/FavoritesView.hpp"
#include "ui/SettingsView.hpp"
#include "ui/AboutView.hpp"

namespace mediacli::ui {

using namespace ftxui;

class AppUI {
public:
    explicit AppUI(Application& app);
    ~AppUI();
    void run();

private:
    Application& app_;
    ScreenInteractive screen_{ScreenInteractive::Fullscreen()};
    ViewState current_state_{ViewState::MainMenu};
    ViewState previous_state_{ViewState::MainMenu};

    std::unique_ptr<MainMenu> main_menu_;
    std::unique_ptr<SearchView> search_view_;
    std::unique_ptr<ResultsView> results_view_;
    std::unique_ptr<VideoMenu> video_menu_;
    std::unique_ptr<HistoryView> history_view_;
    std::unique_ptr<FavoritesView> favorites_view_;
    std::unique_ptr<SettingsView> settings_view_;
    std::unique_ptr<AboutView> about_view_;

    Component main_container_;
    Component modal_component_;
    bool show_modal_{false};

    std::thread worker_thread_;
    std::thread ticker_thread_;
    std::atomic<bool> is_worker_active_{false};

    void setupViews();
    void setupModal();
    void navigateTo(ViewState newState);
    void startAsyncSearch(const std::string& query);
    void startAsyncPlay(const MediaInfo& media);
};

} // namespace mediacli::ui
