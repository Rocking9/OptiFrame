#pragma once

#include <string>
#include <vector>

namespace optiframe {

// Runs a program without passing arguments through a shell.
// Returns the process exit code, or -1 if process creation itself failed.
int runProcess(const std::vector<std::string>& command);

} // namespace optiframe
