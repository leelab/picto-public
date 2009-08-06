TEMPLATE = app
TARGET = DDrawTest
QT = core gui xml network script
CONFIG += qt warn_on release
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp
HEADERS += ddrawobject.h
SOURCES += ddrawobject.cpp
HEADERS += UpdateListener.h
SOURCES += UpdateListener.cpp

wince*:LIBS += ddraw.lib
