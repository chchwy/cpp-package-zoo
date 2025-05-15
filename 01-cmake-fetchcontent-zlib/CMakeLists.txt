# FetchContent example for zlib, fmt, spdlog, and googletest
# This CMakeLists.txt file demonstrates how to use FetchContent to download and build

cmake_minimum_required(VERSION 3.24)
project(P01_FetchContent_zlib LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch zlib
FetchContent_Declare(
    zlib
    URL https://zlib.net/zlib-1.3.1.tar.gz
)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        11.2.0
    GIT_SHALLOW 1
)

# Fetch spdlog
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.15.2
    GIT_SHALLOW 1
)

# Fetch googletest
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz
)

set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "Use external fmt library")
set(BUILD_GMOCK OFF CACHE BOOL "Disable the GoogleMock subproject" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "Disable installation of googletest" FORCE)

FetchContent_MakeAvailable(zlib)
FetchContent_MakeAvailable(fmt)
FetchContent_MakeAvailable(spdlog)
FetchContent_MakeAvailable(googletest)

# Setup minizip manually from zlib's contrib/minizip
set(MINIZIP_DIR ${zlib_SOURCE_DIR}/contrib/minizip)

add_library(minizip STATIC
    ${MINIZIP_DIR}/ioapi.c
    ${MINIZIP_DIR}/mztools.c
    ${MINIZIP_DIR}/unzip.c
    ${MINIZIP_DIR}/zip.c
)

if (WIN32)
    target_sources(minizip PRIVATE ${MINIZIP_DIR}/iowin32.c) # Only needed on Windows
endif()

target_include_directories(minizip PUBLIC
    ${MINIZIP_DIR}
    ${zlib_SOURCE_DIR}
)

target_link_libraries(minizip PUBLIC zlibstatic)

message(STATUS "TEST ${CMAKE_CURRENT_SOURCE_DIR}")

# Add our main executable
add_executable(zipit
    src/zipit.h
    src/zipit.cpp
    src/main.cpp
)

target_link_libraries(zipit PRIVATE
    zlibstatic
    minizip
    fmt::fmt
    spdlog::spdlog)

# On Windows, enable Unicode properly
if (WIN32)
    target_compile_options(zipit PUBLIC /utf-8) # Set UTF-8 encoding for source files
    target_compile_definitions(zipit PRIVATE UNICODE _UNICODE)
endif()

# Enable CTest (already included in your file)
include(CTest)
enable_testing()

# Add test executable
add_executable(zipit_tests
    src/zipit.h
    src/zipit.cpp
    tests/zipit_tests.cpp  # Create this file for your tests
)

target_link_libraries(zipit_tests PRIVATE
    zlibstatic
    minizip
    fmt::fmt
    spdlog::spdlog
    gtest_main  # This includes the main() function
)

if (WIN32)
    target_compile_options(zipit_tests PUBLIC /utf-8) # Set UTF-8 encoding for source files
    target_compile_definitions(zipit_tests PRIVATE UNICODE _UNICODE)
endif()

# Register tests with CTest
include(GoogleTest)
gtest_discover_tests(zipit_tests)
