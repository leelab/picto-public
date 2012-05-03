TEMPLATE = app
TARGET = PictoDirector
QT = core gui xml network script sql
CONFIG += qt warn_on debug_and_release
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += $$(PICTO_TREE)/source/director/main.cpp
HEADERS += $$(PICTO_TREE)/source/director/Director.h
SOURCES += $$(PICTO_TREE)/source/director/Director.cpp
SOURCES += $$(PICTO_TREE)/source/director/HardwareSetup.cpp
HEADERS += $$(PICTO_TREE)/source/director/HardwareSetup.h
SOURCES += $$(PICTO_TREE)/source/director/EngineTest.cpp
HEADERS += $$(PICTO_TREE)/source/director/EngineTest.h

HEADERS += $$(PICTO_TREE)/source/director/network/DirectorStatusManager.h
SOURCES += $$(PICTO_TREE)/source/director/network/DirectorStatusManager.cpp

HEADERS += $$(PICTO_TREE)/source/director/protocol/DirectorEndSessionResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/director/protocol/DirectorEndSessionResponseHandler.cpp
HEADERS += $$(PICTO_TREE)/source/director/protocol/DirectorLoadExpResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/director/protocol/DirectorLoadExpResponseHandler.cpp
HEADERS += $$(PICTO_TREE)/source/director/protocol/DirectorNewSessionResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/director/protocol/DirectorNewSessionResponseHandler.cpp
HEADERS += $$(PICTO_TREE)/source/director/protocol/DirectorStartResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/director/protocol/DirectorStartResponseHandler.cpp
HEADERS += $$(PICTO_TREE)/source/director/protocol/DirectorStopResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/director/protocol/DirectorStopResponseHandler.cpp
HEADERS += $$(PICTO_TREE)/source/director/protocol/DirectorPauseResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/director/protocol/DirectorPauseResponseHandler.cpp
HEADERS += $$(PICTO_TREE)/source/director/protocol/DirectorResumeResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/director/protocol/DirectorResumeResponseHandler.cpp
HEADERS += $$(PICTO_TREE)/source/director/protocol/DirectorRewardResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/director/protocol/DirectorRewardResponseHandler.cpp
HEADERS += $$(PICTO_TREE)/source/director/protocol/DirectorErrorResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/director/protocol/DirectorErrorResponseHandler.cpp
HEADERS += $$(PICTO_TREE)/source/director/protocol/DirectorParameterResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/director/protocol/DirectorParameterResponseHandler.cpp
HEADERS += $$(PICTO_TREE)/source/director/protocol/DirectorClickResponseHandler.h
SOURCES += $$(PICTO_TREE)/source/director/protocol/DirectorClickResponseHandler.cpp


win32 {
SOURCES += $$(PICTO_TREE)/source/director/SignalChannelTest.cpp
HEADERS += $$(PICTO_TREE)/source/director/SignalChannelTest.h
HEADERS += $$(PICTO_TREE)/source/director/compositor/D3DCompositingSurface.h
SOURCES += $$(PICTO_TREE)/source/director/compositor/D3DCompositingSurface.cpp
HEADERS += $$(PICTO_TREE)/source/director/compositor/D3DVisualTarget.h
SOURCES += $$(PICTO_TREE)/source/director/compositor/D3DVisualTarget.cpp
INCLUDEPATH += "$$(DXSDK_DIR)/include"

LIBS += "$$(DXSDK_DIR)/Lib/x86/D3D9.lib"
LIBS += "$$(DXSDK_DIR)/Lib/x86/d3dx9.lib"

HEADERS += $$(PICTO_TREE)/source/director/iodevices/PictoBoxXPEventCodeGenerator.h
SOURCES += $$(PICTO_TREE)/source/director/iodevices/PictoBoxXPEventCodeGenerator.cpp
HEADERS += $$(PICTO_TREE)/source/director/iodevices/PictoBoxXPRewardController.h
SOURCES += $$(PICTO_TREE)/source/director/iodevices/PictoBoxXPRewardController.cpp
HEADERS += $$(PICTO_TREE)/source/director/iodevices/LegacySystemXPEventCodeGenerator.h
SOURCES += $$(PICTO_TREE)/source/director/iodevices/LegacySystemXPEventCodeGenerator.cpp
HEADERS += $$(PICTO_TREE)/source/director/iodevices/LegacySystemXPRewardController.h
SOURCES += $$(PICTO_TREE)/source/director/iodevices/LegacySystemXPRewardController.cpp
INCLUDEPATH +="$$(NIDAQmxSwitchDir)/../DAQmx ANSI C Dev/include"
LIBS += "$$(NIDAQmxSwitchDir)/../DAQmx ANSI C Dev/lib/msvc/NIDAQmx.lib"

HEADERS += $$(PICTO_TREE)/source/director/engine/PictoBoxXPAnalogInputPort.h
SOURCES += $$(PICTO_TREE)/source/director/engine/PictoBoxXPAnalogInputPort.cpp
HEADERS += $$(PICTO_TREE)/source/director/engine/LegacySystemXPAnalogInputPort.h
SOURCES += $$(PICTO_TREE)/source/director/engine/LegacySystemXPAnalogInputPort.cpp

}


wince* {
HEADERS += $$(PICTO_TREE)/source/director/compositor/DDrawCompositingSurface.h
SOURCES += $$(PICTO_TREE)/source/director/compositor/DDrawCompositingSurface.cpp
HEADERS += $$(PICTO_TREE)/source/director/compositor/DDrawVisualTarget.h
SOURCES += $$(PICTO_TREE)/source/director/compositor/DDrawVisualTarget.cpp
LIBS += ddraw.lib

HEADERS += $$(PICTO_TREE)/source/director/compositor/D3DMCompositingSurface.h
SOURCES += $$(PICTO_TREE)/source/director/compositor/D3DMCompositingSurface.cpp
HEADERS += $$(PICTO_TREE)/source/director/compositor/D3DMVisualTarget.h
SOURCES += $$(PICTO_TREE)/source/director/compositor/D3DMVisualTarget.cpp
LIBS += d3dm.lib
LIBS += d3dmx.lib

HEADERS += $$(PICTO_TREE)/source/director/iodevices/PictoBoxDaqBoard.h
HEADERS += $$(PICTO_TREE)/source/director/iodevices/PictoBoxDaqBoard_p.h
HEADERS += $$(PICTO_TREE)/source/director/iodevices/PictoBoxDaqBoardRegisterMap.h
SOURCES += $$(PICTO_TREE)/source/director/iodevices/PictoBoxDaqBoard.cpp
HEADERS += $$(PICTO_TREE)/source/director/iodevices/PictoBoxRewardControllerCE.h
SOURCES += $$(PICTO_TREE)/source/director/iodevices/PictoBoxRewardControllerCE.cpp

HEADERS += $$(PICTO_TREE)/source/director/engine/PictoBoxAnalogInputSignalChannel.h
SOURCES += $$(PICTO_TREE)/source/director/engine/PictoBoxAnalogInputSignalChannel.cpp
}

include($$(PICTO_TREE)/source/common/common.pri)



# Output
build_pass:CONFIG(debug, debug|release) {
  DESTDIR = $$(PICTO_TREE)/output/bin/debug
  OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/director/debug
}
build_pass:CONFIG(release, debug|release) {
  DESTDIR = $$(PICTO_TREE)/output/bin/release
  OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/director/release
}
RCC_DIR = $$(PICTO_TREE)/intermediates/resources/director
UI_DIR = $$(PICTO_TREE)/intermediates/ui/director
MOC_DIR = $$(PICTO_TREE)/intermediates/moc/director

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
