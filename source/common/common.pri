#include($$(PROPBROWSDIR)/qtpropertybrowser.pri)
#macx {
#QTSOLUTIONS_PROPERTYBROWSER.files += $$QTPROPERTYBROWSER_LIBDIR/
#QTSOLUTIONS_PROPERTYBROWSER.path = Contents/Libraries/qtpropertybrowser
#QMAKE_BUNDLE_DATA += QTSOLUTIONS_PROPERTYBROWSER
#}
#win32 {
#contains(DEFINES, QT_QTPROPERTYBROWSER_EXPORT):DEFINES -= QT_QTPROPERTYBROWSER_EXPORT
#!contains(DEFINES, QT_QTPROPERTYBROWSER_IMPORT):DEFINES += QT_QTPROPERTYBROWSER_IMPORT
#}

# Include QtPropertyBrowsers headers and libs
build_pass:CONFIG(debug, debug|release) {
  win32:LIBS += $$(PROPBROWSDIR)/lib/QtPropertyBrowserd.lib
}
build_pass:CONFIG(release, debug|release) {
  win32:LIBS += $$(PROPBROWSDIR)/lib/QtPropertyBrowser.lib
}

INCLUDEPATH += $$(PROPBROWSDIR)/
win32:QMAKE_LIBDIR += $$(PROPBROWSDIR)/lib

# Include QWT header's and libs
INCLUDEPATH += $$(PICTO_THIRD_PARTY)/qwt/src
win32:QMAKE_LIBDIR += $$(PICTO_THIRD_PARTY)/qwt/lib

# Tell everyone that we're running Windows 7+
win32:!wince*:DEFINES += _WIN32_WINNT=0x0601
win32:!wince*:DEFINES += _WINVER=0x0601

