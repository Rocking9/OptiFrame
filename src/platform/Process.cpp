#include "optiframe/platform/Process.hpp"

#include <cerrno>
#include <cstdio>
#include <cstring>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace optiframe {

#ifdef _WIN32
namespace {

// Apply the Windows command-line escaping rules for one argument.
std::string quoteWindowsArgument(const std::string& argument) {
    std::string quoted = "\"";
    std::size_t backslashes = 0;

    for (const char character : argument) {
        if (character == '\\') {
            ++backslashes;
            continue;
        }

        if (character == '"') {
            quoted.append(backslashes * 2 + 1, '\\');
            quoted.push_back('"');
        } else {
            quoted.append(backslashes, '\\');
            quoted.push_back(character);
        }
        backslashes = 0;
    }

    // Backslashes immediately before the closing quote must be doubled.
    quoted.append(backslashes * 2, '\\');
    quoted.push_back('"');
    return quoted;
}

} // namespace
#endif

int runProcess(const std::vector<std::string>& command) {
    if (command.empty()) {
        std::fprintf(stderr, "OptiFrame: cannot run an empty command.\n");
        return -1;
    }

#ifdef _WIN32
    std::string commandLine;
    for (const auto& argument : command) {
        if (!commandLine.empty()) {
            commandLine.push_back(' ');
        }
        commandLine += quoteWindowsArgument(argument);
    }

    // CreateProcess may modify its command-line buffer, so it must be writable.
    std::vector<char> mutableCommandLine(commandLine.begin(), commandLine.end());
    mutableCommandLine.push_back('\0');

    STARTUPINFOA startupInfo{};
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags = STARTF_USESTDHANDLES;
    startupInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
    startupInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    startupInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);

    PROCESS_INFORMATION processInfo{};
    const BOOL created = CreateProcessA(
        nullptr,
        mutableCommandLine.data(),
        nullptr,
        nullptr,
        TRUE,
        0,
        nullptr,
        nullptr,
        &startupInfo,
        &processInfo);

    if (!created) {
        std::fprintf(stderr,
                     "OptiFrame: could not start '%s' (Windows error %lu). "
                     "Check that FFmpeg is installed and available on PATH.\n",
                     command[0].c_str(),
                     static_cast<unsigned long>(GetLastError()));
        return -1;
    }

    WaitForSingleObject(processInfo.hProcess, INFINITE);
    DWORD exitCode = 1;
    if (!GetExitCodeProcess(processInfo.hProcess, &exitCode)) {
        std::fprintf(stderr, "OptiFrame: unable to read child process exit code.\n");
        exitCode = 1;
    }

    CloseHandle(processInfo.hThread);
    CloseHandle(processInfo.hProcess);
    return static_cast<int>(exitCode);
#else
    const pid_t child = fork();
    if (child < 0) {
        std::fprintf(stderr, "OptiFrame: fork failed: %s\n", std::strerror(errno));
        return -1;
    }

    if (child == 0) {
        std::vector<char*> childArguments;
        childArguments.reserve(command.size() + 1);
        for (const auto& argument : command) {
            childArguments.push_back(const_cast<char*>(argument.c_str()));
        }
        childArguments.push_back(nullptr);

        execvp(childArguments[0], childArguments.data());
        std::fprintf(stderr,
                     "OptiFrame: could not start '%s': %s\n",
                     command[0].c_str(),
                     std::strerror(errno));
        _exit(127);
    }

    int status = 0;
    while (waitpid(child, &status, 0) < 0) {
        if (errno == EINTR) {
            continue;
        }
        std::fprintf(stderr, "OptiFrame: waitpid failed: %s\n", std::strerror(errno));
        return -1;
    }

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    if (WIFSIGNALED(status)) {
        return 128 + WTERMSIG(status);
    }
    return -1;
#endif
}

} // namespace optiframe
