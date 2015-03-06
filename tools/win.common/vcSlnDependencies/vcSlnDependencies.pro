TEMPLATE = app
TARGET = vcSlnDependencies
QT = core xml
CONFIG += qt warn_on release
CONFIG -= flat
win32:CONFIG += console
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS +=
SOURCES += $$(PICTO_TREE)/main.cpp

# Output
DESTDIR = $$(PICTO_TREE)/$$(PICTO_MACHINE_TYPE)/bin/
OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/
RCC_DIR = $$(PICTO_TREE)/intermediates/resources/
UI_DIR = $$(PICTO_TREE)/intermediates/ui/
MOC_DIR = $$(PICTO_TREE)/intermediates/moc/

# Platform Specific Configuration

win32 {
QMAKE_CXXFLAGS=/Fd$(IntDir)\
}
win32:!wince*:CONFIG += embed_manifest_exe

# Machine Type

win32:QMAKE_LFLAGS += /MACHINE:$$(PICTO_MACHINE_TYPE)
