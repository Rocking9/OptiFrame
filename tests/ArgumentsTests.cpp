#include "optiframe/cli/Arguments.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(EXIT_FAILURE);
    }
}

} // namespace

int main() {
    std::string error;

    const auto defaults = optiframe::parseArguments(
        {"compress", "--input", "source.mov", "--output", "result.mp4"}, error);
    expect(defaults.has_value(), "valid minimum command should parse");
    expect(defaults->crf == 23, "default CRF should be 23");
    expect(defaults->preset == "medium", "default preset should be medium");
    expect(defaults->audioBitrateKbps == 128, "default audio bitrate should be 128 kbps");

    const auto custom = optiframe::parseArguments(
        {"compress", "--input", "in.mp4", "--output", "out.mp4",
         "--crf", "30", "--preset", "fast", "--audio-bitrate-kbps", "192"}, error);
    expect(custom.has_value(), "valid custom settings should parse");
    expect(custom->crf == 30, "custom CRF should be retained");
    expect(custom->preset == "fast", "custom preset should be retained");
    expect(custom->audioBitrateKbps == 192, "custom audio bitrate should be retained");

    const auto invalidCrf = optiframe::parseArguments(
        {"compress", "--input", "in.mp4", "--output", "out.mp4", "--crf", "99"}, error);
    expect(!invalidCrf.has_value(), "CRF above 51 should be rejected");

    const auto missingOutput = optiframe::parseArguments(
        {"compress", "--input", "in.mp4"}, error);
    expect(!missingOutput.has_value(), "missing output should be rejected");

    const auto help = optiframe::parseArguments({"--help"}, error);
    expect(help.has_value() && help->showHelp, "help should parse successfully");

    std::cout << "All argument parser tests passed.\n";
    return EXIT_SUCCESS;
}
