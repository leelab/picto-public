TEMPLATE = app
TARGET = EmbeddedFrontPanel
QT = core gui xml network
CONFIG += qt warn_on debug_and_release
CONFIG += console
DEPENDPATH += .
INCLUDEPATH += $$(PICTO_TREE)/3rdparty/include .

# Input
SOURCES += $$(PICTO_TREE)/source/embedded/frontpanel/main.cpp
HEADERS += $$(PICTO_TREE)/source/embedded/frontpanel/engineconnections.h
SOURCES += $$(PICTO_TREE)/source/embedded/frontpanel/engineconnections.cpp
HEADERS += $$(PICTO_TREE)/source/embedded/frontpanel/eventchannelthread.h
SOURCES += $$(PICTO_TREE)/source/embedded/frontpanel/eventchannelthread.cpp
HEADERS += $$(PICTO_TREE)/source/embedded/frontpanel/ENGEVENTCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/embedded/frontpanel/ENGEVENTCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/3rdparty/include/phidget21.h
HEADERS += $$(PICTO_TREE)/source/embedded/frontpanel/phidgets.h
SOURCES += $$(PICTO_TREE)/source/embedded/frontpanel/phidgets.cpp
HEADERS += $$(PICTO_TREE)/source/embedded/frontpanel/FrontPanelInfo.h
SOURCES += $$(PICTO_TREE)/source/embedded/frontpanel/FrontPanelInfo.cpp
HEADERS += $$(PICTO_TREE)/source/embedded/frontpanel/menu.h
SOURCES += $$(PICTO_TREE)/source/embedded/frontpanel/menu.cpp

# Output
build_pass:CONFIG(debug, debug|release) {
  DESTDIR = $$(PICTO_TREE)/output/bin/debug
  OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/embedded/frontpanel/debug
}
build_pass:CONFIG(release, debug|release) {
  DESTDIR = $$(PICTO_TREE)/output/bin/release
  OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/embedded/frontpanel/release
}
RCC_DIR = $$(PICTO_TREE)/intermediates/resources/embedded/frontpanel
UI_DIR = $$(PICTO_TREE)/intermediates/ui/embedded/frontpanel
MOC_DIR = $$(PICTO_TREE)/intermediates/moc/embedded/frontpanel

# Libraries
win32:LIBPATH += $$(PICTO_TREE)/3rdparty/lib
wince*:LIBS += phidget21_x86.lib
win32:!wince*:LIBS += phidget21.lib

build_pass:CONFIG(debug, debug|release) {
win32:LIBPATH += $$(PICTO_TREE)/intermediates/lib/debug
unix:!macx:LIBPATH += $$(PICTO_TREE)/output/bin/debug/shared
macx:LIBPATH += $$(PICTO_TREE)/intermediates/lib/debug
win32:LIBS += libPicto_debug.lib
unix:LIBS += -lPicto_debug
macx:PRIVATE_LIBRARIES.files = $$(PICTO_TREE)/intermediates/lib/debug/
}

build_pass:CONFIG(release, debug|release) {
win32:LIBPATH += $$(PICTO_TREE)/intermediates/lib/release
unix:!macx:LIBPATH += $$(PICTO_TREE)/output/bin/release/shared
macx:LIBPATH += $$(PICTO_TREE)/intermediates/lib/release
win32:LIBS += libPicto.lib
unix:LIBS += -lPicto
macx:PRIVATE_LIBRARIES.files = $$(PICTO_TREE)/intermediates/lib/release/
}

win32 {
#Perform compilation using multiple processes if using
#Microsoft compiler version 15 (Visual Studio 2008) or greater
#Version 14 (Visual Studio 2005) supported the necessary compiler
#switch, but had bugs which could lead to corrupt PDB files
for(compilerDefine, QMAKE_COMPILER_DEFINES) {
    compilerVersion = $$find(compilerDefine, "_MSC_VER=*")
    !isEmpty(compilerVersion) {
        compilerVersion = $$replace(compilerVersion, "_MSC_VER=", "")
        !contains(compilerVersion,1200) {
        !contains(compilerVersion,1300) {
        !contains(compilerVersion,1310) {
        !contains(compilerVersion,1400) {
            DEFINES += _BIND_TO_CURRENT_CRT_VERSION
            #Note: This switch has been moved to a custom mkspecs
            #file for win32-msvc2008
            #QMAKE_CXXFLAGS+=/MP
            visualStudioProject = $$find(TEMPLATE, "vc")
            build_pass:CONFIG(release, debug|release):!isEmpty(visualStudioProject) {
                message(Warnings regarding Compiler option: -MP can safely be ignored)
            }
        }}}}
    }
}
#Output PDB file to appropriate directory
QMAKE_CXXFLAGS+=/Fd$(IntDir)
}

build_pass:CONFIG(debug, debug|release) {
  TARGET 		= $$member(TARGET, 0)_debug
  OBJECTS_DIR	= $$member(OBJECTS_DIR, 0)/debug
}
build_pass:CONFIG(release, debug|release) {
  OBJECTS_DIR	= $$member(OBJECTS_DIR, 0)./release
}
