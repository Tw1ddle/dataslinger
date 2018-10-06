QT += core gui widgets

TARGET = dataslinger
TEMPLATE = app

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000 # Disables all the APIs deprecated before Qt 6.0.0

# Include boost
INCLUDEPATH += C:/Development/boost_1_68_0/
DEPENDPATH += C:/Development/boost_1_68_0/

# Include libs
LIBS += "-LC:/Development/boost_1_68_0/stage/x64/lib/"

include($$PWD/lib/dataslinger/dataslinger/dataslinger.pri)

# Include sources
include($$PWD/src/sources.pri)
