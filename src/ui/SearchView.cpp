#include "ui/SearchView.hpp"
#include "ui/Theme.hpp"
#include <fmt/format.h>
#include <array>

namespace mediacli::ui {

static const std::array<std::string, 10> kSpinnerFrames = {
    "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"
};

SearchView::SearchView(std::function<void(const std::string&)> onSearch, std::function<void()> onBack) {
    InputOption option;
    option.on_enter = [this, onSearch]() {
        if (!query_.empty() && !is_searching_ && onSearch) {
            onSearch(query_);
        }
    };

    input_component_ = Input(&query_, "Enter keyword, artist, song, or video title...", option);

    auto btn_search = Button("  Search ", [this, onSearch]() {
        if (!query_.empty() && !is_searching_ && onSearch) {
            onSearch(query_);
        }
    });

    auto btn_back = Button(" ← Back ", [this, onBack]() {
        if (!is_searching_ && onBack) onBack();
    });

    container_ = Container::Vertical({
        input_component_,
        Container::Horizontal({
            btn_search,
            btn_back
        })
    });

    renderer_ = Renderer(container_, [this, btn_search, btn_back]() -> Element {
        Element search_status = emptyElement();
        if (is_searching_) {
            std::string spinner = kSpinnerFrames[spinner_step_ % kSpinnerFrames.size()];
            std::string msg = fmt::format(" {} Searching for '{}'... Please wait...", spinner, active_query_);
            search_status = text(msg) | color(Color::Yellow) | bold | border;
        }

        auto box = vbox(Elements{
            applyHeader("Search"),
            text(""),
            text(" Enter query below and press Enter to search:") | color(Color::GrayLight),
            text(""),
            input_component_->Render() | border | color(is_searching_ ? Color::GrayDark : Color::Cyan),
            search_status,
            text(""),
            hbox(Elements{
                btn_search->Render() | color(is_searching_ ? Color::GrayDark : Color::Green),
                text("  "),
                btn_back->Render() | color(is_searching_ ? Color::GrayDark : Color::Red),
            }),
            text(""),
            text(" [Enter] Search   [ESC] Back to Menu") | dim | align_right,
        }) | size(WIDTH, EQUAL, 84);

        return vbox(Elements{ box | center });
    });

    renderer_ = CatchEvent(renderer_, [this, onBack](Event event) {
        if (event == Event::Escape) {
            if (!is_searching_ && onBack) {
                onBack();
                return true;
            }
        }
        return false;
    });
}

void SearchView::setSearching(bool searching, const std::string& query) {
    is_searching_ = searching;
    active_query_ = query;
    if (searching) {
        spinner_step_ = 0;
    }
}

void SearchView::tickSpinner() {
    spinner_step_++;
}

Component SearchView::getComponent() {
    return renderer_;
}

void SearchView::reset() {
    query_.clear();
    active_query_.clear();
    is_searching_ = false;
    spinner_step_ = 0;
}

} // namespace mediacli::ui
