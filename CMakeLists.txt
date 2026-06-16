cmake_minimum_required(VERSION 3.16)
project(n_body)

find_package(raylib REQUIRED)

add_library(arena STATIC arena-cpp/arena.cpp)
target_include_directories(arena PUBLIC arena-cpp)

add_executable(main main.cpp)

target_include_directories(main PRIVATE clmath)
target_link_libraries(main PRIVATE arena)
target_link_libraries(main PRIVATE raylib)
