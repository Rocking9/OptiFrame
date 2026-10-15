#include "optiframe/cli/Arguments.hpp"
#include "optiframe/core/CompressionRunner.hpp"

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    std::vector<std::string> arguments;
    arguments.reserve(argc > 1 ? static_cast<std::size_t>(argc - 1) : 0);
    for (int i = 1; i < argc; ++i) {
        arguments.emplace_back(argv[i]);
    }

    std::string errorMessage;
    const auto parsedArguments = optiframe::parseArguments(arguments, errorMessage);
    if (!parsedArguments) {
        std::cerr << "Error: " << errorMessage << "\n\n";
        optiframe::printHelp();
        return 2;
    }

    if (parsedArguments->showHelp) {
        optiframe::printHelp();
        return 0;
    }

    return optiframe::runCompression(*parsedArguments);
}
