CONFIG += c++17

linux {
    # Some linker flags needed on Linux (with g++ at least)
    LIBS += -lboost_system -lrt
}

QT += core gui widgets

TARGET = dataslinger
TEMPLATE = app

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000 # Disables all the APIs deprecated before Qt 6.0.0

# Include boost
win32:APPVEYOR {
    INCLUDEPATH += C:/Libraries/boost_1_77_0
    DEPENDPATH += C:/Libraries/boost_1_77_0
    LIBS += "-LC:/Libraries/boost_1_77_0/lib64-msvc-14.2"
} else {
    INCLUDEPATH += C:/boost_1_78_0/
    DEPENDPATH += C:/boost_1_78_0/
    LIBS += "-LC:/boost_1_78_0/stage/x64/lib/"
}

include($$PWD/lib/dataslinger-lib/dataslinger.pri)

# Include sources
include($$PWD/src/sources.pri)
