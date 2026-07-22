#pragma once

#include <ftxui/component/component.hpp>
#include <functional>

namespace mediacli::ui {

using namespace ftxui;

class AboutView {
public:
    explicit AboutView(std::function<void()> onBack);
    Component getComponent();

private:
    Component container_;
    Component renderer_;
};

} // namespace mediacli::ui
