#pragma once

#include <string>
#include <vector>

namespace mediacli {

struct ProcessResult {
    int exit_code{-1};
    std::string stdout_output;
    std::string stderr_output;
};

class Process {
public:
    static ProcessResult execute(const std::string& command, bool merge_stderr = false);
    static ProcessResult execute(const std::vector<std::string>& args, bool merge_stderr = false);
    static bool isCommandAvailable(const std::string& commandName);
    static int runInteractive(const std::string& command);
};

} // namespace mediacli
