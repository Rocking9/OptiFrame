cmake_minimum_required(VERSION 3.16)

project(OptiFrame VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(optiframe_core
    src/cli/Arguments.cpp
    src/core/CompressionRunner.cpp
    src/platform/Process.cpp
)

target_include_directories(optiframe_core PUBLIC include)

if(MSVC)
    target_compile_options(optiframe_core PRIVATE /W4 /permissive-)
else()
target_compile_options(optiframe_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(optiframe src/main.cpp)
target_link_libraries(optiframe PRIVATE optiframe_core)

include(CTest)
if(BUILD_TESTING)
    add_executable(optiframe_tests tests/ArgumentsTests.cpp)
    target_link_libraries(optiframe_tests PRIVATE optiframe_core)
    add_test(NAME arguments_tests COMMAND optiframe_tests)
endif()
