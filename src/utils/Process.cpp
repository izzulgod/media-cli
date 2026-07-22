#include "utils/Process.hpp"
#include "utils/Logger.hpp"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>

namespace mediacli {

static std::string escapeShellArg(const std::string& arg) {
    std::string escaped = "'";
    for (char c : arg) {
        if (c == '\'') {
            escaped += "'\\''";
        } else {
            escaped += c;
        }
    }
    escaped += "'";
    return escaped;
}

ProcessResult Process::execute(const std::string& command, bool merge_stderr) {
    ProcessResult res;
    LOG_TRACE("Executing command: {}", command);

    std::array<char, 512> buffer;
    std::string full_cmd = command;
    if (merge_stderr) {
        full_cmd += " 2>&1";
    } else {
        full_cmd += " 2>/dev/null";
    }

    FILE* pipe = popen(full_cmd.c_str(), "r");
    if (!pipe) {
        LOG_ERROR("popen() failed for command: {}", command);
        res.exit_code = -1;
        return res;
    }

    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
        res.stdout_output += buffer.data();
    }

    int status = pclose(pipe);
    if (WIFEXITED(status)) {
        res.exit_code = WEXITSTATUS(status);
    } else {
        res.exit_code = status;
    }

    return res;
}

ProcessResult Process::execute(const std::vector<std::string>& args, bool merge_stderr) {
    std::ostringstream cmd;
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) cmd << " ";
        cmd << escapeShellArg(args[i]);
    }
    return execute(cmd.str(), merge_stderr);
}

bool Process::isCommandAvailable(const std::string& commandName) {
    std::string checkCmd = "which " + escapeShellArg(commandName) + " > /dev/null 2>&1";
    int res = std::system(checkCmd.c_str());
    return (res == 0);
}

int Process::runInteractive(const std::string& command) {
    LOG_INFO("Running interactive command: {}", command);
    int res = std::system(command.c_str());
    if (WIFEXITED(res)) {
        return WEXITSTATUS(res);
    }
    return res;
}

} // namespace mediacli
