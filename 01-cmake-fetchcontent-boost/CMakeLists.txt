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
set(BOOST_INCLUDE_LIBRARIES program_options)
set(BOOST_ENABLE_CMAKE ON)

FetchContent_Declare(
    Boost
    URL https://github.com/boostorg/boost/releases/download/boost-1.84.0/boost-1.84.0.tar.xz
    URL_MD5 893b5203b862eb9bbd08553e24ff146a
    DOWNLOAD_EXTRACT_TIMESTAMP ON
    USES_TERMINAL_DOWNLOAD TRUE
    DOWNLOAD_NO_EXTRACT FALSE
)

FetchContent_MakeAvailable(Boost)

add_executable(boost_example main.cpp)
target_link_libraries(boost_example PRIVATE
    Boost::program_options
)