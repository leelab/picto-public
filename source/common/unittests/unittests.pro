TEMPLATE = app
TARGET = PictoLibraryUnitTests
QT = core gui network testlib script scripttools sql widgets
CONFIG += qt warn_on debug_and_release
CONFIG -= flat
DEPENDPATH += .
INCLUDEPATH += .

# Input

SOURCES += $$(PICTO_TREE)/source/common/unittests/main.cpp
SOURCES += $$(PICTO_TREE)/source/common/unittests/TestDataStore.cpp
HEADERS += $$(PICTO_TREE)/source/common/unittests/TestDataStore.h
SOURCES += $$(PICTO_TREE)/source/common/unittests/TestParameters.cpp
HEADERS += $$(PICTO_TREE)/source/common/unittests/TestParameters.h
SOURCES += $$(PICTO_TREE)/source/common/unittests/TestPlayback.cpp
HEADERS += $$(PICTO_TREE)/source/common/unittests/TestPlayback.h
SOURCES += $$(PICTO_TREE)/source/common/unittests/TestStateMachine.cpp
HEADERS += $$(PICTO_TREE)/source/common/unittests/TestStateMachine.h
SOURCES += $$(PICTO_TREE)/source/common/unittests/TestScripts.cpp
HEADERS += $$(PICTO_TREE)/source/common/unittests/TestScripts.h


include($$(PICTO_TREE)/source/common/common.pri)

# Output
build_pass:CONFIG(debug, debug|release) {
  DESTDIR = $$(PICTO_TREE)/output/tests/bin/debug
  OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/common/tests/debug
}
build_pass:CONFIG(release, debug|release) {
  DESTDIR = $$(PICTO_TREE)/output/tests/bin/release
  OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/common/tests/release
}
RCC_DIR = $$(PICTO_TREE)/intermediates/resources/common/tests
UI_DIR = $$(PICTO_TREE)/intermediates/ui/common/tests
MOC_DIR = $$(PICTO_TREE)/intermediates/moc/common/tests

# Libraries
build_pass:CONFIG(debug, debug|release) {
win32:QMAKE_LIBDIR += $$(PICTO_TREE)/intermediates/lib/debug
unix:!macx:QMAKE_LIBDIR += $$(PICTO_TREE)/output/bin/debug/shared
macx:QMAKE_LIBDIR += $$(PICTO_TREE)/intermediates/lib/debug
win32:LIBS += libPicto_debug.lib
unix:LIBS += -lPicto_debug
macx:PRIVATE_LIBRARIES.files = $$(PICTO_TREE)/intermediates/lib/debug/
}

build_pass:CONFIG(release, debug|release) {
win32:QMAKE_LIBDIR += $$(PICTO_TREE)/intermediates/lib/release
unix:!macx:QMAKE_LIBDIR += $$(PICTO_TREE)/output/bin/release/shared
macx:QMAKE_LIBDIR += $$(PICTO_TREE)/intermediates/lib/release
win32:LIBS += libPicto.lib
unix:LIBS += -lPicto
macx:PRIVATE_LIBRARIES.files = $$(PICTO_TREE)/intermediates/lib/release/
}

# Platform Specific Configuration

win32 {
#Perform compilation using multiple processes if using
#Microsoft compiler version 15 (Visual Studio 2008) or greater
#Version 14 (Visual Studio 2005) supported the necessary compiler
#switch, but had bugs which could lead to corrupt PDB files
#
#Also, Visual Studio 2008 changed the default behavior for service
#packs so that the RTM version of the CRT, ATL, MFC, and OPENMP
#libraries are specified in the manifest by default.  Since we're
#likely to use the CRT as a private assembly, we need to specify
#that the compiler should bind to the current version.  This should
#be done using the _BIND_TO_CURRENT_VCLIBS_VERSION preprocessor
#definition (which specifies to use the latest version of each of
#the CRT, ATL, MFC, and OPENMP); however, the Visual Studio 2008
#feature pack has a bug with this definition, so instead we'll use
#_BIND_TO_CURRENT_CRT_VERSION since we only use the CRT anyway
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
win32:!wince*:CONFIG += embed_manifest_exe

unix {
DEFINES += BUILD_UNIX
}
unix:!macx {
QMAKE_RPATHDIR =

QMAKE_LFLAGS += -Wl,-rpath,\'\$\$ORIGIN/shared\'
}

macx{
CONFIG += x86 ppc
PRIVATE_LIBRARIES.path = Contents/Libraries/picto
QMAKE_BUNDLE_DATA += PRIVATE_LIBRARIES
QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.4u.sdk
}

# Machine Type

win32:QMAKE_LFLAGS += /MACHINE:$$(PICTO_MACHINE_TYPE)

# Debug Naming

build_pass:CONFIG(debug, debug|release) {
  TARGET 		= $$member(TARGET, 0)_debug
}
