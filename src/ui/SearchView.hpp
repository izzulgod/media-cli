#pragma once

#include <ftxui/component/component.hpp>
#include <functional>
#include <string>
#include <atomic>

namespace mediacli::ui {

using namespace ftxui;

class SearchView {
public:
    SearchView(std::function<void(const std::string&)> onSearch, std::function<void()> onBack);
    Component getComponent();
    void reset();

    void setSearching(bool searching, const std::string& query = "");
    void tickSpinner();
    bool isSearching() const { return is_searching_; }

private:
    std::string query_;
    std::string active_query_;
    std::atomic<bool> is_searching_{false};
    int spinner_step_{0};

    Component input_component_;
    Component container_;
    Component renderer_;
};

} // namespace mediacli::ui
