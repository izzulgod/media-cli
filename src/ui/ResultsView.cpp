#include "ui/ResultsView.hpp"
#include "ui/Theme.hpp"
#include "utils/Logger.hpp"

namespace mediacli::ui {

ResultsView::ResultsView(std::function<void(const MediaInfo&)> onSelect, std::function<void()> onBack) {
    MenuOption option;
    option.on_enter = [this, onSelect]() {
        if (selected_ >= 0 && selected_ < static_cast<int>(items_.size()) && onSelect) {
            onSelect(items_[selected_]);
        }
    };

    menu_component_ = Menu(&display_entries_, &selected_, option);
    
    btn_back_ = Button(&back_label_, [onBack]() {
        if (onBack) onBack();
    });

    container_ = Container::Vertical({
        menu_component_,
        btn_back_
    });

    renderer_ = Renderer(container_, [this]() -> Element {
        Element back_element = emptyElement();
        if (!back_label_.empty()) {
            back_element = btn_back_->Render() | color(Color::Red);
        }

        if (items_.empty()) {
            return vbox(Elements{
                applyHeader(title_header_),
                text(""),
                text(" ⚠️  No media found.") | color(Color::Yellow),
                text(""),
                back_element,
                text(""),
                text(" [ESC] Main Menu") | dim | align_right,
            }) | size(WIDTH, LESS_THAN, 140);
        }

        return vbox(Elements{
            applyHeader(title_header_ + fmt::format(" ({})", items_.size())),
            text(""),
            menu_component_->Render() | vscroll_indicator | frame | size(HEIGHT, LESS_THAN, 20) | border | color(Color::Cyan),
            text(""),
            hbox(Elements{
                back_element,
                filler(),
                text(!back_label_.empty() ? " [↑/↓] Navigate Items   [Tab] Select Back Button   [Enter] Select" : " [↑/↓] Navigate Items   [Enter] Select") | dim,
            }),
        }) | size(WIDTH, LESS_THAN, 140);
    });

    renderer_ = CatchEvent(renderer_, [onBack](Event event) {
        if (event == Event::Escape) {
            if (onBack) onBack();
            return true;
        }
        return false;
    });
}

void ResultsView::setResults(const std::vector<MediaInfo>& items, const std::string& titleHeader, const std::string& backLabel) {
    items_ = items;
    title_header_ = titleHeader;
    back_label_ = backLabel;
    selected_ = 0;
    updateEntries();
}

void ResultsView::updateEntries() {
    display_entries_.clear();
    for (size_t i = 0; i < items_.size(); ++i) {
        const auto& item = items_[i];
        std::string titleStr = item.title.length() > 75 ? item.title.substr(0, 72) + "..." : item.title;
        std::string channelStr = item.channel.length() > 25 ? item.channel.substr(0, 22) + "..." : item.channel;

        std::string line = fmt::format("{:2d}. {:<75} | {:<25} | ⏱ {:<7} | 👁 {}",
            i + 1,
            titleStr,
            channelStr,
            item.duration,
            item.views);
        display_entries_.push_back(line);
    }
}

Component ResultsView::getComponent() {
    return renderer_;
}

} // namespace mediacli::ui
