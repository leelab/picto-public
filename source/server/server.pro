TEMPLATE = app
TARGET = PictoServer
QT = core gui xml network sql concurrent widgets
CONFIG += qt warn_on debug_and_release
CONFIG -= flat
CONFIG += c++11
# We elect to be a console application on Windows so that we will have our
# standard output attached to the invoking terminal, if one is present; if
# none is present, then we can still create a GUI
win32:CONFIG += console
DEPENDPATH += .
INCLUDEPATH += .
MACHINE_TYPE = $$(PICTO_MACHINE_TYPE)

# Input
macx {
SOURCES += $$(PICTO_TREE)/source/server/processinfo/GetPID.c
HEADERS += $$(PICTO_TREE)/source/server/processinfo/GetPID.h
}
win32 {
SOURCES += $$(PICTO_TREE)/source/server/processinfo/WinGetPID.cpp
HEADERS += $$(PICTO_TREE)/source/server/processinfo/WinGetPID.h
}
SOURCES += $$(PICTO_TREE)/source/server/main.cpp
HEADERS += $$(PICTO_TREE)/source/server/dialog.h
SOURCES += $$(PICTO_TREE)/source/server/dialog.cpp

HEADERS += $$(PICTO_TREE)/source/server/connections/AlignSessionData.h
SOURCES += $$(PICTO_TREE)/source/server/connections/AlignSessionData.cpp
HEADERS += $$(PICTO_TREE)/source/server/connections/ConnectionManager.h
SOURCES += $$(PICTO_TREE)/source/server/connections/ConnectionManager.cpp
HEADERS += $$(PICTO_TREE)/source/server/connections/SessionData.h
SOURCES += $$(PICTO_TREE)/source/server/connections/SessionData.cpp
HEADERS += $$(PICTO_TREE)/source/server/connections/SessionInfoDefines.h
HEADERS += $$(PICTO_TREE)/source/server/connections/SessionInfo.h
SOURCES += $$(PICTO_TREE)/source/server/connections/SessionInfo.cpp
HEADERS += $$(PICTO_TREE)/source/server/connections/CurrentSessionData.h
SOURCES += $$(PICTO_TREE)/source/server/connections/CurrentSessionData.cpp
HEADERS += $$(PICTO_TREE)/source/server/connections/CachedSessionData.h
SOURCES += $$(PICTO_TREE)/source/server/connections/CachedSessionData.cpp
HEADERS += $$(PICTO_TREE)/source/server/connections/StoredSessionData.h
SOURCES += $$(PICTO_TREE)/source/server/connections/StoredSessionData.cpp
HEADERS += $$(PICTO_TREE)/source/server/connections/SerialSessionData.h
SOURCES += $$(PICTO_TREE)/source/server/connections/SerialSessionData.cpp
HEADERS += $$(PICTO_TREE)/source/server/connections/ComponentInfo.h
SOURCES += $$(PICTO_TREE)/source/server/connections/ComponentInfo.cpp
HEADERS += $$(PICTO_TREE)/source/server/connections/ServerConfig.h
SOURCES += $$(PICTO_TREE)/source/server/connections/ServerConfig.cpp

HEADERS += $$(PICTO_TREE)/source/server/datacollection/alignmenttool.h
SOURCES += $$(PICTO_TREE)/source/server/datacollection/alignmenttool.cpp

HEADERS += $$(PICTO_TREE)/source/server/network/server.h
SOURCES += $$(PICTO_TREE)/source/server/network/server.cpp
HEADERS += $$(PICTO_TREE)/source/server/network/serverthread.h
SOURCES += $$(PICTO_TREE)/source/server/network/serverthread.cpp

HEADERS += $$(PICTO_TREE)/source/server/protocol/ServerProtocols.h
SOURCES += $$(PICTO_TREE)/source/server/protocol/ServerProtocols.cpp
HEADERS += $$(PICTO_TREE)/source/server/protocol/ServerPictoProtocol.h
SOURCES += $$(PICTO_TREE)/source/server/protocol/ServerPictoProtocol.cpp
HEADERS += $$(PICTO_TREE)/source/server/protocol/ServerHTTPProtocol.h
SOURCES += $$(PICTO_TREE)/source/server/protocol/ServerHTTPProtocol.cpp
HEADERS += $$(PICTO_TREE)/source/server/protocol/ServerAppUpdateProtocol.h
SOURCES += $$(PICTO_TREE)/source/server/protocol/ServerAppUpdateProtocol.cpp
HEADERS += $$(PICTO_TREE)/source/server/protocol/GetFileCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/server/protocol/GetFileCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/source/server/protocol/HTTPGetCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/server/protocol/HTTPGetCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/source/server/protocol/TestCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/server/protocol/TestCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/source/server/protocol/ComponentUpdateCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/server/protocol/ComponentUpdateCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/source/server/protocol/PutDataCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/server/protocol/PutDataCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/source/server/protocol/GetDataCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/server/protocol/GetDataCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/source/server/protocol/DirectorListCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/server/protocol/DirectorListCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/source/server/protocol/ProxyListCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/server/protocol/ProxyListCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/source/server/protocol/StartsessionCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/server/protocol/StartsessionCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/source/server/protocol/JoinsessionCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/server/protocol/JoinsessionCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/source/server/protocol/EndsessionCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/server/protocol/EndsessionCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/source/server/protocol/TaskCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/server/protocol/TaskCommandHandler.cpp

HEADERS += $$(PICTO_TREE)/source/server/service/systemservice.h
SOURCES += $$(PICTO_TREE)/source/server/service/systemservice.cpp


include($$(PICTO_TREE)/source/common/common.pri)

# Resources
RESOURCES += $$(PICTO_TREE)/source/server/server.qrc

# Translations
TRANSLATIONS += $$(PICTO_TREE)/source/server/translations/server_ko.ts

# Output
build_pass:CONFIG(debug, debug|release) {
  DESTDIR = $$(PICTO_TREE)/output/bin/debug
  OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/server/debug
}
build_pass:CONFIG(release, debug|release) {
  DESTDIR = $$(PICTO_TREE)/output/bin/release
  OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/server/release
}
RCC_DIR = $$(PICTO_TREE)/intermediates/resources/server
UI_DIR = $$(PICTO_TREE)/intermediates/ui/server
MOC_DIR = $$(PICTO_TREE)/intermediates/moc/server

# Application Icon
win32 {
RC_FILE = $$(PICTO_TREE)/source/server/server.rc
}
macx {
ICON = $$(PICTO_TREE)/source/server/images/scope.icns
}

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

win32:!wince*:LIBS += advapi32.lib user32.lib psapi.lib

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

unix {
DEFINES += BUILD_UNIX
}
unix:!macx:QMAKE_LFLAGS += -Wl,-rpath,\'\$\$ORIGIN/shared\'

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
