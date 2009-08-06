include($$(PICTO_THIRD_PARTY)/qtiocompressor-2.3-opensource/src/qtiocompressor.pri)
macx {
QTSOLUTIONS_IOCOMPRESSOR.files += $$QTIOCOMPRESSOR_LIBDIR/
QTSOLUTIONS_IOCOMPRESSOR.path = Contents/Libraries/qtiocompressor
QMAKE_BUNDLE_DATA += QTSOLUTIONS_IOCOMPRESSOR
}
win32 {
# The QtIOCompressor project include tries to intelligently set the import/export
# compiler definitions on Windows based on the including project being an application
# versus a library.  However, since this project is a library, it incorrectly sets
# the QT_QTIOCOMPRESSOR_EXPORT instead of QT_QTIOCOMPRESSOR_IMPORT, so we have to
# undo that here
contains(DEFINES, QT_QTIOCOMPRESSOR_EXPORT):DEFINES -= QT_QTIOCOMPRESSOR_EXPORT
!contains(DEFINES, QT_QTIOCOMPRESSOR_IMPORT):DEFINES += QT_QTIOCOMPRESSOR_IMPORT
}
