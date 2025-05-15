# FetchCotnet example for boost
# This is the official document for Using boost with FetchContent
# https://github.com/boostorg/cmake#using-boost-with-fetchcontent

cmake_minimum_required(VERSION 3.24)
project(P02_FetchContent_boost LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

# Fetch boost
set(BOOST_INCLUDE_LIBRARIES program_options asio)
set(BOOST_ENABLE_CMAKE ON)

FetchContent_Declare(
    Boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.88.0/boost-1.88.0-cmake.tar.xz
    URL_HASH SHA256=f48b48390380cfb94a629872346e3a81370dc498896f16019ade727ab72eb1ec
    DOWNLOAD_EXTRACT_TIMESTAMP ON
    USES_TERMINAL_DOWNLOAD ON
)

FetchContent_MakeAvailable(Boost)

add_executable(boost_example main.cpp)
target_link_libraries(boost_example PRIVATE
    Boost::program_options
    Boost::asio
)