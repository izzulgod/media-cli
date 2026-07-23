#include "ui/HistoryView.hpp"
#include "ui/Theme.hpp"

namespace mediacli::ui {

HistoryView::HistoryView(std::function<void(const MediaInfo&)> onSelect,
                         std::function<void()> onClear,
                         std::function<void()> onBack)
    : results_view_(onSelect, onBack) {

    auto btn_clear = Button(" 󰩹 Clear History ", [onClear]() {
        if (onClear) onClear();
    });

    auto btn_back = Button(" ← Back ", [onBack]() {
        if (onBack) onBack();
    });

    container_ = Container::Vertical({
        results_view_.getComponent(),
        Container::Horizontal({
            btn_clear,
            btn_back
        })
    });

    renderer_ = Renderer(container_, [this, btn_clear, btn_back]() -> Element {
        auto content_box = vbox(Elements{
            results_view_.getComponent()->Render(),
            text(""),
            hbox(Elements{
                btn_clear->Render() | color(Color::Red),
                text("  "),
                btn_back->Render() | color(Color::Cyan),
            }) | center,
        }) | size(WIDTH, EQUAL, 140);

        return vbox(Elements{ content_box | center });
    });
}

void HistoryView::setHistory(const std::vector<MediaInfo>& history) {
    results_view_.setResults(history, "Watch History", "");
}

Component HistoryView::getComponent() {
    return renderer_;
}

} // namespace mediacli::ui
