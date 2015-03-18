TEMPLATE = app
TARGET = PictoWorkstation
QT = core gui xml network script scripttools sql svg multimedia concurrent 
CONFIG += qt warn_on debug_and_release 
CONFIG -= flat
CONFIG += c++11
DEPENDPATH += .
INCLUDEPATH += .
INCLUDEPATH += $$(PICTO_TREE)/3rdparty/include
MACHINE_TYPE = $$(PICTO_MACHINE_TYPE)


win32 {
QT += winextras
}

# Input
SOURCES += $$(PICTO_TREE)/source/workstation/main.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/mainwindow.h
SOURCES += $$(PICTO_TREE)/source/workstation/mainwindow.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/ErrorList.h
SOURCES += $$(PICTO_TREE)/source/workstation/ErrorList.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/viewer.h
SOURCES += $$(PICTO_TREE)/source/workstation/viewer.cpp

HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/stateeditviewer.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/stateeditviewer.cpp

HEADERS += $$(PICTO_TREE)/source/workstation/testviewer/testviewer.h
SOURCES += $$(PICTO_TREE)/source/workstation/testviewer/testviewer.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/testviewer/TestPlaybackController.h
SOURCES += $$(PICTO_TREE)/source/workstation/testviewer/TestPlaybackController.cpp

HEADERS += $$(PICTO_TREE)/source/workstation/remoteviewer/RemoteStateUpdater.h
SOURCES += $$(PICTO_TREE)/source/workstation/remoteviewer/RemoteStateUpdater.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/remoteviewer/remoteviewer.h
SOURCES += $$(PICTO_TREE)/source/workstation/remoteviewer/remoteviewer.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/remoteviewer/neuralDataViewer.h
SOURCES += $$(PICTO_TREE)/source/workstation/remoteviewer/neuralDataViewer.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/remoteviewer/TaskRunViewer.h
SOURCES += $$(PICTO_TREE)/source/workstation/remoteviewer/TaskRunViewer.cpp

HEADERS += $$(PICTO_TREE)/source/workstation/replayviewer/ReplayViewer.h
SOURCES += $$(PICTO_TREE)/source/workstation/replayviewer/ReplayViewer.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/replayviewer/PlaybackStateUpdater.h
SOURCES += $$(PICTO_TREE)/source/workstation/replayviewer/PlaybackStateUpdater.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/replayviewer/PlaybackController.h
SOURCES += $$(PICTO_TREE)/source/workstation/replayviewer/PlaybackController.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/replayviewer/PlaybackThread.h
SOURCES += $$(PICTO_TREE)/source/workstation/replayviewer/PlaybackThread.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/replayviewer/SaveOutputDialog.h
SOURCES += $$(PICTO_TREE)/source/workstation/replayviewer/SaveOutputDialog.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/replayviewer/SpeedWidget.h
SOURCES += $$(PICTO_TREE)/source/workstation/replayviewer/SpeedWidget.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/replayviewer/ProgressWidget.h
SOURCES += $$(PICTO_TREE)/source/workstation/replayviewer/ProgressWidget.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/replayviewer/OutputWidgetHolder.h
SOURCES += $$(PICTO_TREE)/source/workstation/replayviewer/OutputWidgetHolder.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/replayviewer/RecordingVisualTargetHost.h
SOURCES += $$(PICTO_TREE)/source/workstation/replayviewer/RecordingVisualTargetHost.cpp

#HEADERS += $$(PICTO_TREE)/source/workstation/commands.h
#SOURCES += $$(PICTO_TREE)/source/workstation/commands.cpp
#HEADERS += $$(PICTO_TREE)/source/workstation/document.h
#SOURCES += $$(PICTO_TREE)/source/workstation/document.cpp

RESOURCES += $$(PICTO_TREE)/source/workstation/workstation.qrc

#FORMS += $$(PICTO_TREE)/source/workstation/mainwindow.ui

include($$(PICTO_TREE)/source/common/common.pri)

#include($$(PROPBROWSDIR)/qtpropertybrowser.pri)
macx:QTSOLUTIONS_PROPERTYBROWSER.files += $$QTPROPERTYBROWSER_LIBDIR/

# Application Icon
win32 {
RC_FILE = $$(PICTO_TREE)/source/workstation/workstation.rc
}
macx {
ICON = $$(PICTO_TREE)/source/workstation/icons/workstation.icns
}

# Output
build_pass:CONFIG(debug, debug|release) {
  DESTDIR = $$(PICTO_TREE)/output/bin/debug
  OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/workstation/debug
}
build_pass:CONFIG(release, debug|release) {
  DESTDIR = $$(PICTO_TREE)/output/bin/release
  OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/workstation/release
}
RCC_DIR = $$(PICTO_TREE)/intermediates/resources/workstation
UI_DIR = $$(PICTO_TREE)/intermediates/ui/workstation
MOC_DIR = $$(PICTO_TREE)/intermediates/moc/workstation

# Libraries
build_pass:CONFIG(debug, debug|release) {
win32:QMAKE_LIBDIR += $$(PICTO_TREE)/intermediates/lib/debug
unix:!macx:QMAKE_LIBDIR += $$(PICTO_TREE)/output/bin/debug/shared
macx:QMAKE_LIBDIR += $$(PICTO_TREE)/intermediates/lib/debug
win32:LIBS += libPicto_debug.lib
unix:LIBS += -lPicto_debug
win32:QMAKE_LIBDIR += $$(PICTO_TREE)/3rdparty/lib
macx:PRIVATE_LIBRARIES.files = $$(PICTO_TREE)/intermediates/lib/debug/
win32:LIBS += qwtd.lib
}

build_pass:CONFIG(release, debug|release) {
win32:QMAKE_LIBDIR += $$(PICTO_TREE)/intermediates/lib/release
unix:!macx:QMAKE_LIBDIR += $$(PICTO_TREE)/output/bin/release/shared
macx:QMAKE_LIBDIR += $$(PICTO_TREE)/intermediates/lib/release
win32:LIBS += libPicto.lib
unix:LIBS += -lPicto
win32:QMAKE_LIBDIR += $$(PICTO_TREE)/3rdparty/lib
macx:PRIVATE_LIBRARIES.files = $$(PICTO_TREE)/intermediates/lib/release/
win32:LIBS += qwt.lib
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
QTSOLUTIONS_PROPERTYBROWSER.path = Contents/Libraries/qtpropertybrowser
QMAKE_BUNDLE_DATA += QTSOLUTIONS_PROPERTYBROWSER
QMAKE_MAC_SDK = /Developer/SDKs/MacOSX10.4u.sdk
}

# Machine Type

win32:QMAKE_LFLAGS += /MACHINE:$$(PICTO_MACHINE_TYPE)

# Debug Naming

build_pass:CONFIG(debug, debug|release) {
  TARGET 		= $$member(TARGET, 0)_debug
}



# Section below was copied directly from qtffmpegwrapper's config.pro with modified paths in "Modify here" section.
# Include the configuration file below in the QT .pro file, and modify the path accordingly.




# ##############################################################################
# ##############################################################################
# FFMPEG: START OF CONFIGURATION BELOW ->
# Copy these lines into your own project
# Make sure to set the path variables for:
# 1) QTFFmpegWrapper sources (i.e. where the QVideoEncoder.cpp and QVideoDecoder.cpp lie),
# 2) FFMPEG include path (i.e. where the directories libavcodec, libavutil, etc. lie),
# 3) the binary FFMPEG libraries (that must be compiled separately).
# Under Linux path 2 and 3 may not need to be set as these are usually in the standard include and lib path.
# Under Windows, path 2 and 3 must be set to the location where you placed the FFMPEG includes and compiled binaries
# Note that the FFMPEG dynamic librairies (i.e. the .dll files) must be in the PATH
# ##############################################################################
# ##############################################################################

# ##############################################################################
# Modify here: set FFMPEG_LIBRARY_PATH and FFMPEG_INCLUDE_PATH
# ##############################################################################

# Set QTFFMPEGWRAPPER_SOURCE_PATH to point to the directory containing the QTFFmpegWrapper sources
QTFFMPEGWRAPPER_SOURCE_PATH = $$(PICTO_TREE)/3rdparty/QTFFmpegWrapper/QTFFmpegWrapper

# Set FFMPEG_LIBRARY_PATH to point to the directory containing the FFmpeg import libraries (if needed - typically for Windows), i.e. the dll.a files

contains(MACHINE_TYPE,X86) {
	FFMPEG_LIBRARY_PATH = $$(PICTO_TREE)/3rdparty/QTFFmpegWrapper/ffmpeg_lib_win32
	}
contains(MACHINE_TYPE,X64) {
	FFMPEG_LIBRARY_PATH = $$(PICTO_TREE)/3rdparty/QTFFmpegWrapper/ffmpeg_lib_win64
	}

# Set FFMPEG_INCLUDE_PATH to point to the directory containing the FFMPEG includes (if needed - typically for Windows)
FFMPEG_INCLUDE_PATH = $$(PICTO_TREE)/3rdparty/QTFFmpegWrapper/QTFFmpegWrapper

# ##############################################################################
# Do not modify: FFMPEG default settings
# ##############################################################################
# Sources for QT wrapper
SOURCES += $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoEncoder.cpp \
    $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoDecoder.cpp
HEADERS += $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoEncoder.h \
    $$QTFFMPEGWRAPPER_SOURCE_PATH/QVideoDecoder.h

# Set list of required FFmpeg libraries
LIBS += -lavutil \
    -lavcodec \
    -lavformat \
    -lswscale

# Add the path
LIBS += -L$$FFMPEG_LIBRARY_PATH
INCLUDEPATH += QVideoEncoder
INCLUDEPATH += $$FFMPEG_INCLUDE_PATH

# Requied for some C99 defines
DEFINES += __STDC_CONSTANT_MACROS

# ##############################################################################
# FFMPEG: END OF CONFIGURATION
# ##############################################################################
