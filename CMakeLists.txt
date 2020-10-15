cmake_minimum_required(VERSION 3.18)

project(Watch CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src/Messages)
add_subdirectory(src/Client)
add_subdirectory(src/Server)
