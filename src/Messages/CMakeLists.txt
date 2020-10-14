cmake_minimum_required(VERSION 3.18)

project(WatchMessages CXX)

set(CMAKE_AUTOMOC ON)

find_package(Qt5Core REQUIRED)

include_directories(${PROJECT_SOURCE_DIR})

set(SRCS
    hello.cpp
    message.cpp
    timestamp.cpp
    )

set(HEADERS
    constants.h
    hello.h
    message.h
    passwordconventions.h
    timestamp.h
    )

add_library(WatchMessages SHARED ${SRCS} ${HEADERS})
target_link_libraries(WatchMessages Qt5::Core)
