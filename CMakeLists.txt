cmake_minimum_required(VERSION 3.0.0)
project(GameOfLife VERSION 1.0.0)

set(CMAKE_CXX_FLAGS "-std=c++17")

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
add_executable(GameOfLife main.cpp)
target_link_libraries(GameOfLife PRIVATE sfml-system sfml-network sfml-graphics sfml-window)