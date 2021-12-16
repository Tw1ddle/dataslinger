CONFIG += c++17
QT += core gui widgets
TARGET = dataslinger
TEMPLATE = app

linux {
    LIBS += -lboost_system -lrt # Some linker flags needed on Linux (with g++ at least)
}

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000 # Disables all the APIs deprecated before Qt 6.0.0

win32:APPVEYOR {
    INCLUDEPATH += $$(DATASLINGER_BOOST_PATH)
    LIBS += "-L$$(DATASLINGER_BOOST_PATH)/lib64-msvc-14.2"
} else {
    INCLUDEPATH += $$(DATASLINGER_BOOST_PATH)
    LIBS += "-L$$(DATASLINGER_BOOST_PATH)/stage/x64/lib/"
}

include($$PWD/lib/dataslinger-lib/dataslinger.pri)
include($$PWD/src/sources.pri)
