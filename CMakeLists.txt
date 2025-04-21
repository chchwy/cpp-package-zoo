cmake_minimum_required(VERSION 3.21.0)
project(zip-dir VERSION 0.1.0 LANGUAGES C CXX)


include(FetchContent)

FetchContent_Declare(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG        v1.3.1
)
FetchContent_MakeAvailable(zlib)

add_executable(zip-dir main.cpp)
set_target_properties(zip-dir PROPERTIES CXX_STANDARD 20)
set_target_properties(zip-dir PROPERTIES CXX_STANDARD_REQUIRED ON)
set_target_properties(zip-dir PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(zip-dir PRIVATE zlib)

include(CTest)
enable_testing()

