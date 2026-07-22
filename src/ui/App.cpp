#include "ui/App.hpp"
#include "utils/Logger.hpp"
#include <chrono>

namespace mediacli::ui {

AppUI::AppUI(Application& app) : app_(app) {
    setupViews();
}

AppUI::~AppUI() {
    is_worker_active_ = false;
    if (ticker_thread_.joinable()) {
        ticker_thread_.join();
    }
    if (worker_thread_.joinable()) {
        worker_thread_.join();
    }
}

void AppUI::setupViews() {
    // 1. Main Menu
    main_menu_ = std::make_unique<MainMenu>([this](int option) {
        switch (option) {
            case 0: navigateTo(ViewState::SearchInput); break;
            case 1: 
                history_view_->setHistory(app_.getHistory());
                navigateTo(ViewState::History); 
                break;
            case 2:
                favorites_view_->setFavorites(app_.getFavorites());
                navigateTo(ViewState::Favorites);
                break;
            case 3:
                settings_view_->loadFromConfig();
                navigateTo(ViewState::Settings);
                break;
            case 4: navigateTo(ViewState::About); break;
            case 5: screen_.ExitLoopClosure()(); break;
        }
    });

    // 2. Search Input View (Asynchronous non-blocking)
    search_view_ = std::make_unique<SearchView>(
        [this](const std::string& query) {
            startAsyncSearch(query);
        },
        [this]() {
            navigateTo(ViewState::MainMenu);
        }
    );

    // 3. Search Results View
    results_view_ = std::make_unique<ResultsView>(
        [this](const MediaInfo& media) {
            app_.setSelectedMedia(media);
            video_menu_->setMedia(media, app_.isFavorite(media));
            previous_state_ = ViewState::SearchResults;
            navigateTo(ViewState::VideoMenu);
        },
        [this]() {
            navigateTo(ViewState::SearchInput);
        }
    );

    // 4. Video Action Menu (Asynchronous stream resolution)
    video_menu_ = std::make_unique<VideoMenu>(
        [this](VideoMenuAction action) {
            const auto& media = app_.getSelectedMedia();
            switch (action) {
                case VideoMenuAction::Play: {
                    startAsyncPlay(media);
                    break;
                }
                case VideoMenuAction::Download: {
                    screen_.WithRestoredIO([this, &media]() {
                        app_.downloadMedia(media);
                    })();
                    break;
                }
                case VideoMenuAction::OpenBrowser: {
                    app_.openInBrowser(media);
                    break;
                }
                case VideoMenuAction::Favorite: {
                    app_.toggleFavorite(media);
                    video_menu_->setMedia(media, app_.isFavorite(media));
                    break;
                }
                case VideoMenuAction::Back: {
                    navigateTo(previous_state_);
                    break;
                }
            }
        }
    );

    // 5. History View
    history_view_ = std::make_unique<HistoryView>(
        [this](const MediaInfo& media) {
            app_.setSelectedMedia(media);
            video_menu_->setMedia(media, app_.isFavorite(media));
            previous_state_ = ViewState::History;
            navigateTo(ViewState::VideoMenu);
        },
        [this]() {
            app_.clearHistory();
            history_view_->setHistory({});
        },
        [this]() {
            navigateTo(ViewState::MainMenu);
        }
    );

    // 6. Favorites View
    favorites_view_ = std::make_unique<FavoritesView>(
        [this](const MediaInfo& media) {
            app_.setSelectedMedia(media);
            video_menu_->setMedia(media, app_.isFavorite(media));
            previous_state_ = ViewState::Favorites;
            navigateTo(ViewState::VideoMenu);
        },
        [this]() {
            navigateTo(ViewState::MainMenu);
        }
    );

    // 7. Settings View
    settings_view_ = std::make_unique<SettingsView>(
        app_.getConfig(),
        [this]() {
            app_.reloadBackendAndPlayer();
            navigateTo(ViewState::MainMenu);
        },
        [this]() {
            navigateTo(ViewState::MainMenu);
        }
    );

    // 8. About View
    about_view_ = std::make_unique<AboutView>(
        [this]() {
            navigateTo(ViewState::MainMenu);
        }
    );
}

void AppUI::startAsyncSearch(const std::string& query) {
    if (is_worker_active_) return;

    if (ticker_thread_.joinable()) ticker_thread_.join();
    if (worker_thread_.joinable()) worker_thread_.join();

    is_worker_active_ = true;
    search_view_->setSearching(true, query);

    // 1. Ticker thread for smooth spinner animation
    ticker_thread_ = std::thread([this]() {
        while (is_worker_active_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(80));
            if (!is_worker_active_) break;
            search_view_->tickSpinner();
            screen_.PostEvent(Event::Custom);
        }
    });

    // 2. Worker thread for running backend search
    worker_thread_ = std::thread([this, query]() {
        auto results = app_.search(query);
        is_worker_active_ = false;

        screen_.Post([this, results, query]() {
            search_view_->setSearching(false);
            results_view_->setResults(results, "Search Results for: " + query);
            navigateTo(ViewState::SearchResults);
        });
    });
}

void AppUI::startAsyncPlay(const MediaInfo& media) {
    if (is_worker_active_) return;

    if (ticker_thread_.joinable()) ticker_thread_.join();
    if (worker_thread_.joinable()) worker_thread_.join();

    is_worker_active_ = true;
    video_menu_->setLoading(true, "Fetching stream URL with yt-dlp...");

    // 1. Ticker thread for smooth spinner animation
    ticker_thread_ = std::thread([this]() {
        while (is_worker_active_) {
            std::this_thread::sleep_for(std::chrono::milliseconds(80));
            if (!is_worker_active_) break;
            video_menu_->tickSpinner();
            screen_.PostEvent(Event::Custom);
        }
    });

    // 2. Worker thread for resolving stream URL
    worker_thread_ = std::thread([this, media]() {
        LOG_INFO("Fetching stream URL asynchronously for: {}", media.title);
        // Execute playMedia in restored IO once stream URL is ready
        is_worker_active_ = false;

        screen_.Post([this, media]() {
            video_menu_->setLoading(false);
            screen_.WithRestoredIO([this, &media]() {
                app_.playMedia(media);
            })();
        });
    });
}

void AppUI::navigateTo(ViewState newState) {
    current_state_ = newState;
    screen_.PostEvent(Event::Custom);
}

void AppUI::run() {
    auto tab_container = Container::Tab(
        {
            main_menu_->getComponent(),      // 0: MainMenu
            search_view_->getComponent(),    // 1: SearchInput
            results_view_->getComponent(),   // 2: SearchResults
            video_menu_->getComponent(),     // 3: VideoMenu
            history_view_->getComponent(),   // 4: History
            favorites_view_->getComponent(), // 5: Favorites
            settings_view_->getComponent(),  // 6: Settings
            about_view_->getComponent(),     // 7: About
        },
        reinterpret_cast<int*>(&current_state_)
    );

    // Global ESC key fallback handler ensuring ESC returns to Main Menu from any screen
    auto root_component = CatchEvent(tab_container, [this](Event event) {
        if (event == Event::Escape) {
            if (current_state_ != ViewState::MainMenu && !is_worker_active_) {
                navigateTo(ViewState::MainMenu);
                return true;
            }
        }
        return false;
    });

    screen_.Loop(root_component);
}

} // namespace mediacli::ui
