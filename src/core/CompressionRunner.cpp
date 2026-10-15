#include "optiframe/core/CompressionRunner.hpp"

#include "optiframe/platform/Process.hpp"

#include <algorithm>
#include <chrono>
#include <cctype>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include <system_error>
#include <vector>

namespace optiframe {
namespace fs = std::filesystem;
namespace {

std::string lowercase(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

std::string formatMegabytes(std::uintmax_t bytes) {
    std::ostringstream output;
    output << std::fixed << std::setprecision(2)
           << (static_cast<double>(bytes) / (1024.0 * 1024.0)) << " MiB";
    return output.str();
}

} // namespace

int runCompression(const Arguments& arguments) {
    try {
        const fs::path inputPath = fs::absolute(fs::path(arguments.inputPath)).lexically_normal();
        const fs::path outputPath = fs::absolute(fs::path(arguments.outputPath)).lexically_normal();

        if (!fs::exists(inputPath)) {
            std::cerr << "Error: input file does not exist: " << inputPath.string() << '\n';
            return 2;
        }
        if (!fs::is_regular_file(inputPath)) {
            std::cerr << "Error: input path is not a regular file: " << inputPath.string() << '\n';
            return 2;
        }
        if (lowercase(outputPath.extension().string()) != ".mp4") {
            std::cerr << "Error: output file must use the .mp4 extension in this version.\n";
            return 2;
        }
        if (fs::exists(outputPath)) {
            std::cerr << "Error: output file already exists; refusing to overwrite it: "
                      << outputPath.string() << '\n';
            return 2;
        }

        const fs::path outputParent = outputPath.parent_path();
        if (!outputParent.empty()) {
            std::error_code directoryError;
            fs::create_directories(outputParent, directoryError);
            if (directoryError) {
                std::cerr << "Error: could not create output directory: "
                          << directoryError.message() << '\n';
                return 2;
            }
        }

        const std::uintmax_t inputBytes = fs::file_size(inputPath);
        std::vector<std::string> command = {
            "ffmpeg",
            "-hide_banner",
            "-nostdin",
            "-i", inputPath.string(),
            "-map", "0:v:0",
            "-map", "0:a?",
            "-c:v", "libx264",
            "-preset", arguments.preset,
            "-crf", std::to_string(arguments.crf),
            "-c:a", "aac",
            "-b:a", std::to_string(arguments.audioBitrateKbps) + "k",
            "-movflags", "+faststart",
            outputPath.string()
        };

        std::cout << "OptiFrame v0.1.0\n"
                  << "Input:  " << inputPath.string() << '\n'
                  << "Output: " << outputPath.string() << '\n'
                  << "Settings: H.264 / CRF " << arguments.crf
                  << " / preset " << arguments.preset
                  << " / audio " << arguments.audioBitrateKbps << " kbps\n\n";

        const auto startTime = std::chrono::steady_clock::now();
        const int exitCode = runProcess(command);
        const auto endTime = std::chrono::steady_clock::now();
        const double elapsedSeconds =
            std::chrono::duration<double>(endTime - startTime).count();

        if (exitCode != 0) {
            std::cerr << "Error: FFmpeg failed with exit code " << exitCode << ".\n";
            return exitCode == -1 ? 1 : exitCode;
        }

        if (!fs::exists(outputPath) || !fs::is_regular_file(outputPath)) {
            std::cerr << "Error: FFmpeg finished but no output file was found.\n";
            return 1;
        }

        const std::uintmax_t outputBytes = fs::file_size(outputPath);
        std::cout << "\nCompression complete.\n"
                  << "Original size:   " << formatMegabytes(inputBytes) << '\n'
                  << "Compressed size: " << formatMegabytes(outputBytes) << '\n';

        if (inputBytes > 0) {
            const double changePercent =
                (1.0 - static_cast<double>(outputBytes) / static_cast<double>(inputBytes)) * 100.0;
            if (changePercent >= 0.0) {
                std::cout << "Size reduction:  " << std::fixed << std::setprecision(2)
                          << changePercent << "%\n";
            } else {
                std::cout << "Size increase:   " << std::fixed << std::setprecision(2)
                          << -changePercent << "%\n";
            }
        }

        std::cout << "Elapsed time:    " << std::fixed << std::setprecision(2)
                  << elapsedSeconds << " seconds\n"
                  << "Note: file size is measured; perceptual quality is not yet scored.\n";
        return 0;
    } catch (const fs::filesystem_error& error) {
        std::cerr << "Filesystem error: " << error.what() << '\n';
        return 1;
    } catch (const std::exception& error) {
        std::cerr << "Unexpected error: " << error.what() << '\n';
        return 1;
    }
}

} // namespace optiframe
