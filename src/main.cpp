#include "utils/Logger.hpp"
#include "core/Application.hpp"
#include "ui/App.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        mediacli::Logger::init();
        LOG_INFO("Starting MediaCLI v1.0.0...");

        mediacli::Application app;
        mediacli::ui::AppUI app_ui(app);
        
        app_ui.run();

        LOG_INFO("MediaCLI exiting cleanly.");
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        LOG_CRITICAL("Fatal error: {}", e.what());
        return 1;
    }
    return 0;
}
