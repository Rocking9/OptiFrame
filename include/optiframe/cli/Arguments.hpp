#pragma once

#include <optional>
#include <string>
#include <vector>

namespace optiframe {

struct Arguments {
    bool showHelp = false;
    std::string command;
    std::string inputPath;
    std::string outputPath;
    int crf = 23;
    std::string preset = "medium";
    int audioBitrateKbps = 128;
};

// args excludes the executable name (argv[0]).
std::optional<Arguments> parseArguments(
    const std::vector<std::string>& args,
    std::string& errorMessage);

void printHelp();

} // namespace optiframe
