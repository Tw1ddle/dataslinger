CONFIG += c++17

TRAVIS|linux {
    # Some linker flags needed on Linux (with g++ at least)
    LIBS += -lboost_system -lrt
}

QT += core gui widgets

TARGET = dataslinger
TEMPLATE = app

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000 # Disables all the APIs deprecated before Qt 6.0.0

# Include boost
win32:APPVEYOR {
    INCLUDEPATH += C:/Libraries/boost_1_67_0
    DEPENDPATH += C:/Libraries/boost_1_67_0
    LIBS += "-LC:/Libraries/boost_1_67_0/lib64-msvc-14.1"
} else {
    INCLUDEPATH += C:/Development/boost_1_68_0/
    DEPENDPATH += C:/Development/boost_1_68_0/
    LIBS += "-LC:/Development/boost_1_68_0/stage/x64/lib/"
}

include($$PWD/lib/dataslinger-lib/dataslinger.pri)

# Include sources
include($$PWD/src/sources.pri)
