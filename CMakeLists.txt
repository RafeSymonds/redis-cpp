cmake_minimum_required(VERSION 3.13)

project(redis-starter-cpp)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

set(CMAKE_CXX_STANDARD 23) # Enable the C++23 standard
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

include(FetchContent)
FetchContent_Declare(
  cxxopts
  GIT_REPOSITORY https://github.com/jarro2783/cxxopts.git
  GIT_TAG v3.3.1
  FIND_PACKAGE_ARGS CONFIG # use vcpkg's copy when available
)
FetchContent_MakeAvailable(cxxopts)

add_executable(redis ${SOURCE_FILES})

target_link_libraries(redis PRIVATE Threads::Threads cxxopts::cxxopts)

target_include_directories(redis PRIVATE include)
