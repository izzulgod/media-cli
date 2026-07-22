#include "utils/Logger.hpp"
#include <spdlog/sinks/basic_file_sink.h>
#include <filesystem>

namespace mediacli {

std::shared_ptr<spdlog::logger> Logger::s_logger;

void Logger::init() {
    std::filesystem::create_directories("logs");

    // Only write logs to error.log file to avoid corrupting FTXUI terminal rendering
    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/error.log", true);
    file_sink->set_level(spdlog::level::trace);
    file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%s:%#] %v");

    s_logger = std::make_shared<spdlog::logger>("MediaCLI", file_sink);
    s_logger->set_level(spdlog::level::trace);
    s_logger->flush_on(spdlog::level::warn);

    spdlog::set_default_logger(s_logger);
}

std::shared_ptr<spdlog::logger>& Logger::get() {
    if (!s_logger) {
        init();
    }
    return s_logger;
}

} // namespace mediacli
