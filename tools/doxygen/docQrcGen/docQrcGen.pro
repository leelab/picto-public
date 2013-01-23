TEMPLATE = app
TARGET = docQrcGen
QT = core xml
CONFIG += qt warn_on release
win32:CONFIG += console
CONFIG -= flat
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS +=
SOURCES += $$(PICTO_TREE)/tools/doxygen/docQrcGen/main.cpp

# Output
DESTDIR = $$(PICTO_TREE)/tools/doxygen/docQrcGen/bin
OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/tools/docQrcGen/release
RCC_DIR = $$(PICTO_TREE)/intermediates/resources/tools/docQrcGen
UI_DIR = $$(PICTO_TREE)/intermediates/ui/tools/docQrcGen
MOC_DIR = $$(PICTO_TREE)/intermediates/moc/tools/docQrcGen

# Platform Specific Configuration

win32 {
QMAKE_CXXFLAGS=/Fd$(IntDir)\
}
win32:!wince*:CONFIG += embed_manifest_exe

# Machine Type

win32:QMAKE_LFLAGS += /MACHINE:$$(PICTO_MACHINE_TYPE)
