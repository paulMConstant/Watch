cmake_minimum_required(VERSION 3.18)

project(WatchServer CXX)

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../Messages/build)

set(SRCS
    main.cpp
    password.cpp
    server.cpp
    Logger/logger.cpp
    )

set(HEADERS
    password.h
    server.h
    Logger/logger.h
    )


add_executable(WatchServer ${SRCS} ${HEADERS} Resources/resources.qrc)
target_link_libraries(WatchServer WatchMessages Qt5::Core Qt5::Network)
