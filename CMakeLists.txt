cmake_minimum_required(VERSION 3.21.0)
project(zip-dir VERSION 0.1.0 LANGUAGES C CXX)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)

include(FetchContent)

FetchContent_Declare(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG        v1.3.1
)
set(ZLIB_BUILD_EXAMPLES OFF)
FetchContent_MakeAvailable(zlib)

# Add minizip source files
set(MINIZIP_SOURCES
    ${zlib_SOURCE_DIR}/contrib/minizip/zip.c
    ${zlib_SOURCE_DIR}/contrib/minizip/unzip.c
    ${zlib_SOURCE_DIR}/contrib/minizip/ioapi.c
    ${zlib_SOURCE_DIR}/contrib/minizip/iowin32.c
)

add_executable(zip-dir main.cpp ${MINIZIP_SOURCES})
set_target_properties(zip-dir PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_link_libraries(zip-dir PRIVATE zlibstatic)

include(CTest)
enable_testing()

