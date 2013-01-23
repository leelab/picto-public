TEMPLATE = app
TARGET = NetworkTest
QT = core gui xml network script
CONFIG += qt warn_on release console
CONFIG -= flat
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp
