cmake_minimum_required(VERSION 3.25)

# The vcpkg toolchain is set in CMakePresets.json, so it is in place
# before project() runs. Setting it after project() has no effect.
project(itch_feed_handler
    VERSION 0.1.0
    DESCRIPTION "NASDAQ TotalView-ITCH 5.0 feed handler and order book reconstruction"
    LANGUAGES CXX)

# Fail loudly if this is configured as a subproject; the presets assume top level.
if(NOT PROJECT_IS_TOP_LEVEL)
    message(FATAL_ERROR "itch_feed_handler must be built as a top-level project")
endif()

# C++20, no GNU extensions (-std=c++20, not -std=gnu++20).
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# compile_commands.json for clang-tidy and editors (ITCH-004).
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -----------------------------------------------------------------------------
# Warnings are attached per target through this INTERFACE target instead of
# CMAKE_CXX_FLAGS, so they apply only to our code and never to third-party
# headers or libraries pulled in through vcpkg.
# -----------------------------------------------------------------------------
add_library(itch_warnings INTERFACE)
add_library(itch::warnings ALIAS itch_warnings)
target_compile_options(itch_warnings INTERFACE
    $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Werror>
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>)

# -----------------------------------------------------------------------------
# Core library. Header-only for now because src/ has no sources yet.
# ITCH-011 turns this into a STATIC library when the first .cpp lands in src/.
# -----------------------------------------------------------------------------
add_library(itch INTERFACE)
add_library(itch::itch ALIAS itch)
target_include_directories(itch INTERFACE
    ${PROJECT_SOURCE_DIR}/include)
target_compile_features(itch INTERFACE cxx_std_20)

# apps/itch_replay is added in ITCH-015, once it has a main().

# -----------------------------------------------------------------------------
# Tests
# -----------------------------------------------------------------------------
include(CTest)   # defines BUILD_TESTING (ON by default) and calls enable_testing()
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
