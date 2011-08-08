TEMPLATE = app
TARGET = PictoWorkstation
QT = core gui xml network script scripttools sql svg
CONFIG += qt warn_on debug_and_release
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += $$(PICTO_TREE)/source/workstation/main.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/mainwindow.h
SOURCES += $$(PICTO_TREE)/source/workstation/mainwindow.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/StartSessionDialog.h
SOURCES += $$(PICTO_TREE)/source/workstation/StartSessionDialog.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/RunTaskDialog.h
SOURCES += $$(PICTO_TREE)/source/workstation/RunTaskDialog.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/ErrorList.h
SOURCES += $$(PICTO_TREE)/source/workstation/ErrorList.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/viewer.h
SOURCES += $$(PICTO_TREE)/source/workstation/viewer.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/PictoData.h
SOURCES += $$(PICTO_TREE)/source/workstation/PictoData.cpp

HEADERS += $$(PICTO_TREE)/source/workstation/texteditor/textviewer.h
SOURCES += $$(PICTO_TREE)/source/workstation/texteditor/textviewer.cpp
SOURCES += $$(PICTO_TREE)/source/workstation/texteditor/xmleditor.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/texteditor/xmleditor.h
SOURCES += $$(PICTO_TREE)/source/workstation/texteditor/syntaxhighlighter.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/texteditor/syntaxhighlighter.h

HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/EditorState.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/EditorState.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/arrow.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/arrow.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/diagramitem.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/diagramitem.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/ArrowPortItem.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/ArrowPortItem.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/ArrowSourceItem.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/ArrowSourceItem.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/ArrowDestinationItem.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/ArrowDestinationItem.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/DiagramItemFactory.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/DiagramItemFactory.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/WireableItem.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/WireableItem.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/AssetItem.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/AssetItem.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/ResultItem.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/ResultItem.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/StateMachineElementItem.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/StateMachineElementItem.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/ControlElementItem.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/ControlElementItem.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/diagramscene.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/diagramscene.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/diagramtextitem.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/diagramtextitem.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/stateeditviewer.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/stateeditviewer.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/Toolbox.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/Toolbox.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/ToolGroup.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/ToolGroup.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/AssetToolGroup.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/AssetToolGroup.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/PropertyToolGroup.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/PropertyToolGroup.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/BackgroundToolGroup.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/BackgroundToolGroup.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/PropertyEditorFactory.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/PropertyEditorFactory.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/PropertyBrowser.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/PropertyBrowser.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/AssetInfoBox.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/AssetInfoBox.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/ScriptWidget.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/ScriptWidget.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/DeletableWidget.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/DeletableWidget.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/ViewerWindow.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/ViewerWindow.cpp
HEADERS += $$(PICTO_TREE)/source/workstation/statemachineeditor/StateMachineEditorData.h
SOURCES += $$(PICTO_TREE)/source/workstation/statemachineeditor/StateMachineEditorData.cpp

HEADERS += $$(PICTO_TREE)/source/workstation/testviewer/testviewer.h
SOURCES += $$(PICTO_TREE)/source/workstation/testviewer/testviewer.cpp

HEADERS += $$(PICTO_TREE)/source/workstation/remoteviewer/remoteviewer.h
SOURCES += $$(PICTO_TREE)/source/workstation/remoteviewer/remoteviewer.cpp

#HEADERS += $$(PICTO_TREE)/source/workstation/commands.h
#SOURCES += $$(PICTO_TREE)/source/workstation/commands.cpp
#HEADERS += $$(PICTO_TREE)/source/workstation/document.h
#SOURCES += $$(PICTO_TREE)/source/workstation/document.cpp

RESOURCES += $$(PICTO_TREE)/source/workstation/workstation.qrc

#FORMS += $$(PICTO_TREE)/source/workstation/mainwindow.ui

include($$(PICTO_TREE)/source/common/common.pri)

include($$(PICTO_THIRD_PARTY)/qtpropertybrowser-2.5-opensource/src/qtpropertybrowser.pri)
macx:QTSOLUTIONS_PROPERTYBROWSER.files += $$QTPROPERTYBROWSER_LIBDIR/

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
