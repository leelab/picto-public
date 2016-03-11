TEMPLATE = app
TARGET = PlexonWrapper
QT = core
CONFIG += qt warn_on debug_and_release
CONFIG -= flat
CONFIG += c++11
CONFIG += console
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += $$(PICTO_TREE)/3rdparty/include

MACHINE_TYPE = X86

# Input
SOURCES += $$(PICTO_TREE)/source/plexonwrapper/main.cpp
HEADERS += $$(PICTO_TREE)/source/plexonwrapper/Task.h
SOURCES += $$(PICTO_TREE)/source/plexonwrapper/Task.cpp
HEADERS += $$(PICTO_TREE)/source/plexonwrapper/PlexonWrapperDefs.h

# Output
build_pass:CONFIG(debug, debug|release) {
  DESTDIR = $$(PICTO_TREE)/output/bin/debug/wrapper
  OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/plexonwrapper/debug
}
build_pass:CONFIG(release, debug|release) {
  DESTDIR = $$(PICTO_TREE)/output/bin/release/wrapper
  OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/plexonwrapper/release
}
RCC_DIR = $$(PICTO_TREE)/intermediates/resources/plexonwrapper
UI_DIR = $$(PICTO_TREE)/intermediates/ui/plexonwrapper
MOC_DIR = $$(PICTO_TREE)/intermediates/moc/plexonwrapper

# Libraries
build_pass:CONFIG(debug, debug|release) {
win32:QMAKE_LIBDIR += $$(PICTO_TREE)/3rdparty/lib
win32:LIBS += PlexClient.lib
}

build_pass:CONFIG(release, debug|release) {
win32:QMAKE_LIBDIR += $$(PICTO_TREE)/3rdparty/lib
win32:LIBS += PlexClient.lib
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

build_pass:CONFIG(debug, debug|release) {
	#Output PDB file to appropriate directory
	QMAKE_CXXFLAGS+=/Fd$(IntDir)
}
}
win32:!wince*:CONFIG += embed_manifest_exe

# Machine Type

win32:QMAKE_LFLAGS += /MACHINE:$$(PICTO_MACHINE_TYPE)

# Debug Naming

build_pass:CONFIG(debug, debug|release) {
  TARGET 		= $$member(TARGET, 0)_debug
}
