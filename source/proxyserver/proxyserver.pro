TEMPLATE = app
TARGET = PictoProxyServer
QT = core gui xml network sql
CONFIG += qt warn_on debug_and_release
# We elect to be a console application on Windows so that we will have our
# standard output attached to the invoking terminal, if one is present; if
# none is present, then we can still create a GUI
#win32:CONFIG += console
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += $$(PICTO_TREE)/3rdparty/include

# Input
macx {
SOURCES += $$(PICTO_TREE)/source/proxyserver/processinfo/GetPID.c
HEADERS += $$(PICTO_TREE)/source/proxyserver/processinfo/GetPID.h
}
win32 {
SOURCES += $$(PICTO_TREE)/source/proxyserver/processinfo/WinGetPID.cpp
HEADERS += $$(PICTO_TREE)/source/proxyserver/processinfo/WinGetPID.h
}
HEADERS += $$(PICTO_TREE)/source/proxyserver/interfaces.h
SOURCES += $$(PICTO_TREE)/source/proxyserver/main.cpp
HEADERS += $$(PICTO_TREE)/source/proxyserver/mainwindow.h
SOURCES += $$(PICTO_TREE)/source/proxyserver/mainwindow.cpp
HEADERS += $$(PICTO_TREE)/source/proxyserver/InteractiveSTDIOHandler.h
SOURCES += $$(PICTO_TREE)/source/proxyserver/InteractiveSTDIOHandler.cpp
HEADERS += $$(PICTO_TREE)/source/proxyserver/network/server.h
SOURCES += $$(PICTO_TREE)/source/proxyserver/network/server.cpp
HEADERS += $$(PICTO_TREE)/source/proxyserver/network/serverthread.h
SOURCES += $$(PICTO_TREE)/source/proxyserver/network/serverthread.cpp
HEADERS += $$(PICTO_TREE)/source/proxyserver/network/broadcast.h
SOURCES += $$(PICTO_TREE)/source/proxyserver/network/broadcast.cpp
HEADERS += $$(PICTO_TREE)/source/proxyserver/protocol/ACQGetCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/proxyserver/protocol/ACQGetCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/source/proxyserver/protocol/HTTPGetCommandHandler.h
SOURCES += $$(PICTO_TREE)/source/proxyserver/protocol/HTTPGetCommandHandler.cpp
HEADERS += $$(PICTO_TREE)/source/proxyserver/protocol/ServerAcqProtocol.h
SOURCES += $$(PICTO_TREE)/source/proxyserver/protocol/ServerAcqProtocol.cpp
HEADERS += $$(PICTO_TREE)/source/proxyserver/protocol/ServerHTTPProtocol.h
SOURCES += $$(PICTO_TREE)/source/proxyserver/protocol/ServerHTTPProtocol.cpp
HEADERS += $$(PICTO_TREE)/source/proxyserver/protocol/ServerProtocols.h
SOURCES += $$(PICTO_TREE)/source/proxyserver/protocol/ServerProtocols.cpp

include($$(PICTO_TREE)/source/common/common.pri)

# Resources
RESOURCES += $$(PICTO_TREE)/source/proxyserver/proxyserver.qrc

# Translations
TRANSLATIONS += $$(PICTO_TREE)/source/proxyserver/translations/proxyserver_ko.ts

# Output
build_pass:CONFIG(debug, debug|release) {
  DESTDIR = $$(PICTO_TREE)/output/bin/debug
  OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/proxyserver/debug
}
build_pass:CONFIG(release, debug|release) {
  DESTDIR = $$(PICTO_TREE)/output/bin/release
  OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/proxyserver/release
}
RCC_DIR = $$(PICTO_TREE)/intermediates/resources/proxyserver
UI_DIR = $$(PICTO_TREE)/intermediates/ui/proxyserver
MOC_DIR = $$(PICTO_TREE)/intermediates/moc/proxyserver

# Application Icon
win32 {
RC_FILE = $$(PICTO_TREE)/source/proxyserver/proxyserver.rc
}
macx {
ICON = $$(PICTO_TREE)/source/proxyserver/images/scope.icns
}

# Libraries
build_pass:CONFIG(debug, debug|release) {
win32:LIBPATH += $$(PICTO_TREE)/intermediates/lib/debug
unix:!macx:LIBPATH += $$(PICTO_TREE)/output/bin/debug/shared
macx:LIBPATH += $$(PICTO_TREE)/intermediates/lib/debug
win32:LIBS += libPicto_debug.lib
unix:LIBS += -lPicto_debug
macx:PRIVATE_LIBRARIES.files = $$(PICTO_TREE)/intermediates/lib/debug/
win32:LIBPATH += $$(PICTO_TREE)/3rdparty/lib
win32:LIBS += PlexClient.lib
}

build_pass:CONFIG(release, debug|release) {
win32:LIBPATH += $$(PICTO_TREE)/intermediates/lib/release
unix:!macx:LIBPATH += $$(PICTO_TREE)/output/bin/release/shared
macx:LIBPATH += $$(PICTO_TREE)/intermediates/lib/release
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
#Output PDB file to appropriate directory
QMAKE_CXXFLAGS+=/Fd$(IntDir)
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
