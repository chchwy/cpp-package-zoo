cmake_minimum_required(VERSION 3.24)
project(ZipFolder LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch zlib
FetchContent_Declare(
    zlib
    URL https://zlib.net/zlib-1.3.1.tar.gz
)

FetchContent_MakeAvailable(zlib)

# Setup minizip manually from zlib's contrib/minizip
set(MINIZIP_DIR ${zlib_SOURCE_DIR}/contrib/minizip)

add_library(minizip STATIC
    ${MINIZIP_DIR}/ioapi.c
    ${MINIZIP_DIR}/iowin32.c # Only needed on Windows
    ${MINIZIP_DIR}/mztools.c
    ${MINIZIP_DIR}/unzip.c
    ${MINIZIP_DIR}/zip.c
)

target_include_directories(minizip PUBLIC
    ${MINIZIP_DIR}
    ${zlib_SOURCE_DIR}
)

target_link_libraries(minizip PUBLIC zlibstatic)

# Add our main executable
add_executable(zipit
    main.cpp
)
target_compile_options(zipit PUBLIC /utf-8) # Set UTF-8 encoding for source files
target_link_libraries(zipit PRIVATE zlibstatic minizip)

# On Windows, enable Unicode properly
if (WIN32)
    target_compile_definitions(zipit PRIVATE UNICODE _UNICODE)
endif()
