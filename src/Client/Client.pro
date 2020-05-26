#-------------------------------------------------
#
# Project created by QtCreator 2020-05-22T14:01:27
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Client
TEMPLATE = app
CONFIG += c++1z

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    main.cpp \
    HMI/mainwindow.cpp \
    HMI/Player/player.cpp \
    HMI/networkdisplay.cpp \
    Network/client.cpp \
    Logger/logger.cpp \
    HMI/Player/customsignalsmediaplayer.cpp \
    HMI/Convenience/discreetdock.cpp

HEADERS += \
    HMI/mainwindow.h \
    HMI/Player/player.h \
    HMI/networkdisplay.h \
    Network/client.h \
    Logger/logger.h \
    HMI/Player/customsignalsmediaplayer.h \
    HMI/Convenience/discreetdock.h

FORMS += \
    HMI/Player/player.ui \
    HMI/networkdisplay.ui \
    HMI/mainwindow.ui

LIBS += -L$$PWD/../../../build-Watch-Desktop-Release/src/Messages -lMessages
INCLUDEPATH += $$PWD/../

LIBS        += -lVLCQtCore -lVLCQtWidgets
DESTDIR = bin

unix:{
    QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
}

RESOURCES += \
    Resources/resources.qrc

