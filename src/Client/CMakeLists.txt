cmake_minimum_required(VERSION 3.18)

project(Watch CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5Widgets REQUIRED)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include/vlc-qt)

set(SRCS
    Global/global.cpp Global/global.cpp
    HMI/mainwindow.cpp
    HMI/Convenience/discreetdock.cpp
    HMI/Player/customsignalsmediaplayer.cpp
    HMI/Player/player.cpp
    HMI/networkdisplay.cpp
    Helpers/helpers.cpp
    Logger/logger.cpp
    Network/client.cpp
    Resources/resources.qrc
    main.cpp
    )

set(HEADERS
    Global/global.cpp Global/global.h
    HMI/mainwindow.h
    HMI/Convenience/discreetdock.h
    HMI/Player/customsignalsmediaplayer.h
    HMI/Player/player.cpp
    HMI/networkdisplay.h 
    Helpers/helpers.h
    Logger/logger.h
    Network/client.h
    )

set(UIS
    HMI/mainwindow.ui
    HMI/Player/player.ui
    HMI/networkdisplay.ui
    )

add_executable(Watch ${SRCS} ${HEADERS} ${UIS})
target_link_libraries(Watch WatchMessages Qt5::Gui Qt5::Widgets Qt5::Core Qt5::Multimedia Qt5::Network VLCQtCore VLCQtWidgets)
