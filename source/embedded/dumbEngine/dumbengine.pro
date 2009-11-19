TEMPLATE = app
TARGET = EmbeddedDumbEngine
QT = core gui xml network
CONFIG += qt warn_on debug_and_release
CONFIG += console
DEPENDPATH += .
INCLUDEPATH += $$(PICTO_TREE)/3rdparty/include .

# Input
SOURCES += $$(PICTO_TREE)/source/embedded/dumbengine/main.cpp
HEADERS += $$(PICTO_TREE)/source/embedded/dumbengine/engine.h
SOURCES += $$(PICTO_TREE)/source/embedded/dumbengine/engine.cpp

HEADERS += $$(PICTO_TREE)/source/embedded/dumbengine/protocol/FPGETCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/embedded/dumbengine/protocol/FPGETCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/source/embedded/dumbengine/protocol/FPPUTCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/embedded/dumbengine/protocol/FPPUTCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/source/embedded/dumbengine/protocol/FPREWARDCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/embedded/dumbengine/protocol/FPREWARDCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/source/embedded/dumbengine/protocol/FPSTARTFLUSHCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/embedded/dumbengine/protocol/FPSTARTFLUSHCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/source/embedded/dumbengine/protocol/FPSTOPFLUSHCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/embedded/dumbengine/protocol/FPSTOPFLUSHCommandHandler.cpp

HEADERS += $$(PICTO_TREE)/source/embedded/dumbengine/daqboard/DaqBoard.h
HEADERS += $$(PICTO_TREE)/source/embedded/dumbengine/daqboard/DaqBoard_p.h
HEADERS += $$(PICTO_TREE)/source/embedded/dumbengine/daqboard/DAQ_Driver.h
HEADERS += $$(PICTO_TREE)/source/embedded/dumbengine/daqboard/registerMap.h
SOURCES += $$(PICTO_TREE)/source/embedded/dumbengine/daqboard/DaqBoard.cpp


# Output
build_pass:CONFIG(debug, debug|release) {
  DESTDIR = $$(PICTO_TREE)/output/bin/debug
  OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/embedded/dumbengine/debug
}
build_pass:CONFIG(release, debug|release) {
  DESTDIR = $$(PICTO_TREE)/output/bin/release
  OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/embedded/dumbengine/release
}
RCC_DIR = $$(PICTO_TREE)/intermediates/resources/embedded/dumbengine
UI_DIR = $$(PICTO_TREE)/intermediates/ui/embedded/dumbengine
MOC_DIR = $$(PICTO_TREE)/intermediates/moc/embedded/dumbengine

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
            QMAKE_CXXFLAGS+=/MP
            visualStudioProject = $$find(TEMPLATE, "vc")
            build_pass:CONFIG(release, debug|release):!isEmpty(visualStudioProject) {
                message(Warnings regarding Compiler option: /MP can safely be ignored)
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
