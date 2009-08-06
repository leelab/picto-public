TEMPLATE = app
TARGET = changeqt
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += main.cpp ../shared/shared.cpp
CONFIG += qt warn_on release  create_prl link_prl
OBJECTS_DIR = .obj/release-shared
MOC_DIR = .moc/release-shared
CONFIG -= app_bundle