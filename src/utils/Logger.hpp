#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace mediacli {

class Logger {
public:
    static void init();
    static std::shared_ptr<spdlog::logger>& get();

private:
    static std::shared_ptr<spdlog::logger> s_logger;
};

#define LOG_TRACE(...)    ::mediacli::Logger::get()->trace(__VA_ARGS__)
#define LOG_DEBUG(...)    ::mediacli::Logger::get()->debug(__VA_ARGS__)
#define LOG_INFO(...)     ::mediacli::Logger::get()->info(__VA_ARGS__)
#define LOG_WARN(...)     ::mediacli::Logger::get()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    ::mediacli::Logger::get()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::mediacli::Logger::get()->critical(__VA_ARGS__)

} // namespace mediacli
