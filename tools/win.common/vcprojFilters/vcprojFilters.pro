TEMPLATE = app
TARGET = vcprojFilters
QT = core xml
CONFIG += qt warn_on release
win32:CONFIG += console
DEPENDPATH += .
INCLUDEPATH += .

# Input
HEADERS +=
SOURCES += $$(PICTO_TREE)/tools/win.common/vcprojFilters/main.cpp

# Output
DESTDIR = $$(PICTO_TREE)/tools/win.common/vcprojFilters/bin
OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/tools/vcprojFilters/release
RCC_DIR = $$(PICTO_TREE)/intermediates/resources/tools/vcprojFilters
UI_DIR = $$(PICTO_TREE)/intermediates/ui/tools/vcprojFilters
MOC_DIR = $$(PICTO_TREE)/intermediates/moc/tools/vcprojFilters

# Platform Specific Configuration

win32 {
QMAKE_CXXFLAGS=/Fd$(IntDir)\
}
win32:!wince*:CONFIG += embed_manifest_exe

# Machine Type

win32:QMAKE_LFLAGS += /MACHINE:$$(PICTO_MACHINE_TYPE)
