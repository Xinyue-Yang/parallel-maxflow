cmake_minimum_required(VERSION 3.20)
project(parallel_maxflow LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-Wall -Wextra -Wpedantic)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -O3)
endif()

add_executable(main main.cc
    src/network.cc src/utils.cc
    src/sequential/edmonds_karp.cc
    src/parallel1a/edmonds_karp.cc
    src/parallel1b/edmonds_karp.cc
    src/parallel1c/edmonds_karp.cc
    src/parallel1d/edmonds_karp.cc
    src/parallel2a/edmonds_karp.cc
    src/parallel2b/edmonds_karp.cc
    src/sequential/dinics.cc
    src/parallel1a/dinics.cc
    src/parallel1b/dinics.cc
    src/parallel1c/dinics.cc
    src/parallel1d/dinics.cc
    src/parallel2a/dinics.cc
    src/parallel2b/dinics.cc)
target_include_directories(main PRIVATE include)
target_link_libraries(main PRIVATE OpenMP::OpenMP_CXX)
