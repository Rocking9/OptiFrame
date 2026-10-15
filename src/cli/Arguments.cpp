#include "optiframe/cli/Arguments.hpp"

#include <charconv>
#include <iostream>
#include <string_view>

namespace optiframe {
namespace {

bool parseInteger(const std::string& value, int& result) {
    if (value.empty()) {
        return false;
    }

    const char* begin = value.data();
    const char* end = begin + value.size();
    const auto parsed = std::from_chars(begin, end, result);
    return parsed.ec == std::errc{} && parsed.ptr == end;
}

bool isSupportedPreset(const std::string& preset) {
    return preset == "ultrafast" || preset == "superfast" ||
           preset == "veryfast" || preset == "faster" ||
           preset == "fast" || preset == "medium" ||
           preset == "slow" || preset == "slower" ||
           preset == "veryslow";
}

} // namespace

std::optional<Arguments> parseArguments(
    const std::vector<std::string>& args,
    std::string& errorMessage) {
    errorMessage.clear();

    if (args.empty() || args[0] == "--help" || args[0] == "-h" ||
        args[0] == "help") {
        Arguments result;
        result.showHelp = true;
        return result;
    }

    if (args[0] != "compress") {
        errorMessage = "Unknown command: " + args[0];
        return std::nullopt;
    }

    Arguments result;
    result.command = args[0];

    for (std::size_t i = 1; i < args.size(); ++i) {
        const std::string& option = args[i];

        if (option == "--help" || option == "-h") {
            result.showHelp = true;
            return result;
        }

        if (i + 1 >= args.size()) {
            errorMessage = "Missing value for option: " + option;
            return std::nullopt;
        }

        const std::string& value = args[++i];
        if (option == "--input") {
            result.inputPath = value;
        } else if (option == "--output") {
            result.outputPath = value;
        } else if (option == "--crf") {
            if (!parseInteger(value, result.crf) || result.crf < 0 || result.crf > 51) {
                errorMessage = "--crf must be an integer from 0 to 51.";
                return std::nullopt;
            }
        } else if (option == "--preset") {
            if (!isSupportedPreset(value)) {
                errorMessage = "Unsupported preset: " + value;
                return std::nullopt;
            }
            result.preset = value;
        } else if (option == "--audio-bitrate-kbps") {
            if (!parseInteger(value, result.audioBitrateKbps) ||
                result.audioBitrateKbps < 32 || result.audioBitrateKbps > 512) {
                errorMessage = "--audio-bitrate-kbps must be an integer from 32 to 512.";
                return std::nullopt;
            }
        } else {
            errorMessage = "Unknown option: " + option;
            return std::nullopt;
        }
    }

    if (result.inputPath.empty()) {
        errorMessage = "Missing required option: --input";
        return std::nullopt;
    }
    if (result.outputPath.empty()) {
        errorMessage = "Missing required option: --output";
        return std::nullopt;
    }

    return result;
}

void printHelp() {
    std::cout
        << "OptiFrame v0.1.0 - measurable video compression\n\n"
        << "Usage:\n"
        << "  optiframe compress --input <video> --output <file.mp4> [options]\n\n"
        << "Options:\n"
        << "  --input <path>              Source video (required)\n"
        << "  --output <path.mp4>         Output MP4 path (required; must not exist)\n"
        << "  --crf <0-51>                H.264 quality setting (default: 23)\n"
        << "  --preset <name>             ultrafast, superfast, veryfast, faster,\n"
        << "                              fast, medium, slow, slower, veryslow\n"
        << "                              (default: medium)\n"
        << "  --audio-bitrate-kbps <n>    AAC audio bitrate, 32-512 (default: 128)\n"
        << "  --help, -h                  Show this help\n\n"
        << "Notes:\n"
        << "  - Requires FFmpeg installed and available on PATH.\n"
        << "  - Lower CRF generally means higher quality and a larger file.\n"
        << "  - This initial version uses libx264 and writes MP4 output.\n";
}

} // namespace optiframe
