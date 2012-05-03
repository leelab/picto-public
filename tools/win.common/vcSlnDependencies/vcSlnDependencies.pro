TEMPLATE = app
TARGET = vcSlnDependencies
QT = core xml
CONFIG += qt warn_on release
win32:CONFIG += console
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS +=
SOURCES += $$(PICTO_TREE)/tools/win.common/vcSlnDependencies/main.cpp

# Output
DESTDIR = $$(PICTO_TREE)/tools/win.common/vcSlnDependencies/bin
OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/tools/vcSlnDependencies/release
RCC_DIR = $$(PICTO_TREE)/intermediates/resources/tools/vcSlnDependencies
UI_DIR = $$(PICTO_TREE)/intermediates/ui/tools/vcSlnDependencies
MOC_DIR = $$(PICTO_TREE)/intermediates/moc/tools/vcSlnDependencies

# Platform Specific Configuration

win32 {
QMAKE_CXXFLAGS=/Fd$(IntDir)\
}
win32:!wince*:CONFIG += embed_manifest_exe

# Machine Type

win32:QMAKE_LFLAGS += /MACHINE:$$(PICTO_MACHINE_TYPE)
