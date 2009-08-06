TEMPLATE = subdirs

SUBDIRS = source/common
SUBDIRS += source/director
!wince* {
  SUBDIRS += source/server          source/server/unittests
  SUBDIRS += source/config
  SUBDIRS += source/workstation
  SUBDIRS += source/documentation/developersguide
}
wince* {
  SUBDIRS += source/embedded
}

# Deployment

include($$(PICTO_THIRD_PARTY)/qtiocompressor-2.3-opensource/src/qtiocompressor.pri)
include($$(PICTO_THIRD_PARTY)/qtpropertybrowser-2.5-opensource/src/qtpropertybrowser.pri)

win32 {
IMAGEFORMATPLUGINS.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qgif4.dll
IMAGEFORMATPLUGINS.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qico4.dll
IMAGEFORMATPLUGINS.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qjpeg4.dll
IMAGEFORMATPLUGINS.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qmng4.dll
IMAGEFORMATPLUGINS.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qsvg4.dll
IMAGEFORMATPLUGINS.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qtiff4.dll
IMAGEFORMATPLUGINS.path = $$(PICTO_TREE)/output/bin/release/imageformats
INSTALLS += IMAGEFORMATPLUGINS

IMAGEFORMATPLUGINS_DEBUG.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qgifd4.dll
IMAGEFORMATPLUGINS_DEBUG.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qicod4.dll
IMAGEFORMATPLUGINS_DEBUG.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qjpegd4.dll
IMAGEFORMATPLUGINS_DEBUG.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qmngd4.dll
IMAGEFORMATPLUGINS_DEBUG.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qsvgd4.dll
IMAGEFORMATPLUGINS_DEBUG.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qtiffd4.dll
IMAGEFORMATPLUGINS_DEBUG.path = $$(PICTO_TREE)/output/bin/debug/imageformats
INSTALLS += IMAGEFORMATPLUGINS_DEBUG

!wince*:QTSOLUTIONS_IOCOMPRESSOR.files += $$QTIOCOMPRESSOR_LIBDIR/../release/QtSolutions_IOCompressor-2.3.dll
wince*:QTSOLUTIONS_IOCOMPRESSOR.files += $$QTIOCOMPRESSOR_LIBDIR/../release/IOCompressor-2.3.dll
QTSOLUTIONS_IOCOMPRESSOR.path = $$(PICTO_TREE)/output/bin/release
INSTALLS += QTSOLUTIONS_IOCOMPRESSOR

!wince*:QTSOLUTIONS_IOCOMPRESSOR_DEBUG.files += $$QTIOCOMPRESSOR_LIBDIR/../debug/QtSolutions_IOCompressor-2.3d.dll
wince*:QTSOLUTIONS_IOCOMPRESSOR_DEBUG.files += $$QTIOCOMPRESSOR_LIBDIR/../debug/IOCompressor-2.3d.dll
QTSOLUTIONS_IOCOMPRESSOR_DEBUG.path = $$(PICTO_TREE)/output/bin/debug
INSTALLS += QTSOLUTIONS_IOCOMPRESSOR_DEBUG

!wince*:QTSOLUTIONS_PROPERTYBROWSER.files += $$QTPROPERTYBROWSER_LIBDIR/../release/QtSolutions_PropertyBrowser-2.5.dll
wince*:QTSOLUTIONS_PROPERTYBROWSER.files += $$QTPROPERTYBROWSER_LIBDIR/../release/PropertyBrowser-2.5.dll
QTSOLUTIONS_PROPERTYBROWSER.path = $$(PICTO_TREE)/output/bin/release
INSTALLS += QTSOLUTIONS_PROPERTYBROWSER

!wince*:QTSOLUTIONS_PROPERTYBROWSER_DEBUG.files += $$QTPROPERTYBROWSER_LIBDIR/../debug/PropertyBrowser-2.5d.dll
wince*:QTSOLUTIONS_PROPERTYBROWSER_DEBUG.files += $$QTPROPERTYBROWSER_LIBDIR/../debug/PropertyBrowser-2.5d.dll
QTSOLUTIONS_PROPERTYBROWSER_DEBUG.path = $$(PICTO_TREE)/output/bin/debug
INSTALLS += QTSOLUTIONS_PROPERTYBROWSER_DEBUG

QTLIBS.files += $$[QT_INSTALL_PREFIX]/lib/QtCore4.dll
QTLIBS.files += $$[QT_INSTALL_PREFIX]/lib/QtGui4.dll
QTLIBS.files += $$[QT_INSTALL_PREFIX]/lib/QtNetwork4.dll
QTLIBS.files += $$[QT_INSTALL_PREFIX]/lib/phonon4.dll
QTLIBS.files += $$[QT_INSTALL_PREFIX]/lib/QtScript4.dll
QTLIBS.files += $$[QT_INSTALL_PREFIX]/lib/QtScriptTools4.dll
QTLIBS.files += $$[QT_INSTALL_PREFIX]/lib/QtSql4.dll
QTLIBS.files += $$[QT_INSTALL_PREFIX]/lib/QtXml4.dll
QTLIBS.files += $$[QT_INSTALL_PREFIX]/lib/QtWebKit4.dll
QTLIBS.path = $$(PICTO_TREE)/output/bin/release
INSTALLS += QTLIBS

QTLIBS_DEBUG.files += $$[QT_INSTALL_PREFIX]/lib/QtCored4.dll
QTLIBS_DEBUG.files += $$[QT_INSTALL_PREFIX]/lib/QtGuid4.dll
QTLIBS_DEBUG.files += $$[QT_INSTALL_PREFIX]/lib/QtNetworkd4.dll
QTLIBS_DEBUG.files += $$[QT_INSTALL_PREFIX]/lib/phonond4.dll
QTLIBS_DEBUG.files += $$[QT_INSTALL_PREFIX]/lib/QtScriptd4.dll
QTLIBS_DEBUG.files += $$[QT_INSTALL_PREFIX]/lib/QtScriptToolsd4.dll
QTLIBS_DEBUG.files += $$[QT_INSTALL_PREFIX]/lib/QtSqld4.dll
QTLIBS_DEBUG.files += $$[QT_INSTALL_PREFIX]/lib/QtXmld4.dll
QTLIBS_DEBUG.files += $$[QT_INSTALL_PREFIX]/lib/QtWebKitd4.dll
QTLIBS_DEBUG.path = $$(PICTO_TREE)/output/bin/debug
INSTALLS += QTLIBS_DEBUG

wince* {
SSLLIBS.files += $$(PICTO_THIRD_PARTY)/openssl-0.9.8k/out32dll_X86/ssleay32.dll
SSLLIBS.files += $$(PICTO_THIRD_PARTY)/openssl-0.9.8k/out32dll_X86/libeay32.dll
}
!wince* {
SSLLIBS.files += $$(PICTO_THIRD_PARTY)/openssl-0.9.8k/bin/ssleay32.dll
SSLLIBS.files += $$(PICTO_THIRD_PARTY)/openssl-0.9.8k/bin/libeay32.dll
}
SSLLIBS.path = $$(PICTO_TREE)/output/bin/release
INSTALLS += SSLLIBS

wince* {
SSLLIBS_DEBUG.files += $$(PICTO_THIRD_PARTY)/openssl-0.9.8k/out32dll_X86/ssleay32.dll
SSLLIBS_DEBUG.files += $$(PICTO_THIRD_PARTY)/openssl-0.9.8k/out32dll_X86/libeay32.dll
}
!wince* {
SSLLIBS_DEBUG.files += $$(PICTO_THIRD_PARTY)/openssl-0.9.8k/bin/ssleay32.dll
SSLLIBS_DEBUG.files += $$(PICTO_THIRD_PARTY)/openssl-0.9.8k/bin/libeay32.dll
}
SSLLIBS_DEBUG.path = $$(PICTO_TREE)/output/bin/debug
INSTALLS += SSLLIBS_DEBUG

MACHINE_TYPE = $$(PICTO_MACHINE_TYPE)
!wince* {
    contains(MACHINE_TYPE,X86) {
    CRUNTIMEPRIVATEASSEMBLY.extra = xcopy /E /I \"$$(DevEnvDir)\..\..\VC\redist\x86\Microsoft.VC90.CRT\" $$(PICTO_TREE)\output\bin\release\Microsoft.VC90.CRT
    CRUNTIMEPRIVATEASSEMBLY_IMAGEFORMATS.extra = xcopy /E /I \"$$(DevEnvDir)\..\..\VC\redist\x86\Microsoft.VC90.CRT\" $$(PICTO_TREE)\output\bin\release\imageformats\Microsoft.VC90.CRT
    }
    contains(MACHINE_TYPE,X64) {
    CRUNTIMEPRIVATEASSEMBLY.extra = xcopy /E /I \"$$(DevEnvDir)\..\..\VC\redist\amd64\Microsoft.VC90.CRT\" $$(PICTO_TREE)\output\bin\release\Microsoft.VC90.CRT
    CRUNTIMEPRIVATEASSEMBLY_IMAGEFORMATS.extra = xcopy /E /I \"$$(DevEnvDir)\..\..\VC\redist\amd64\Microsoft.VC90.CRT\" $$(PICTO_TREE)\output\bin\release\imageformats\Microsoft.VC90.CRT
    }
}
wince* {
    CRUNTIMEPRIVATEASSEMBLY.extra = copy \"$$(DevEnvDir)\..\..\VC\ce\dll\x86\msvcr90.dll\" $$(PICTO_TREE)\output\bin\release
}
CRUNTIMEPRIVATEASSEMBLY.path = $$(PICTO_TREE)/output/bin/release
INSTALLS += CRUNTIMEPRIVATEASSEMBLY
CRUNTIMEPRIVATEASSEMBLY_IMAGEFORMATS.path = $$(PICTO_TREE)/output/bin/release/imageformats
INSTALLS += CRUNTIMEPRIVATEASSEMBLY_IMAGEFORMATS

#The C runtime files for debug are not licensed for redistribution, and are thus not copied here

TESTS.extra = copy $$(PICTO_TREE)\output\bin\release\*.dll $$(PICTO_TREE)\output\tests\bin\release
TESTS.files = $$[QT_INSTALL_PREFIX]/lib/QtTest4.dll
TESTS.path = $$(PICTO_TREE)/output/tests/bin/release
INSTALLS += TESTS

!wince* {
    contains(MACHINE_TYPE,X86) {
    CRUNTIMETESTSPRIVATEASSEMBLY.extra = xcopy /E /I \"$$(DevEnvDir)\..\..\VC\redist\x86\Microsoft.VC90.CRT\" $$(PICTO_TREE)\output\tests\bin\release\Microsoft.VC90.CRT
    }
    contains(MACHINE_TYPE,X64) {
    CRUNTIMETESTSPRIVATEASSEMBLY.extra = xcopy /E /I \"$$(DevEnvDir)\..\..\VC\redist\amd64\Microsoft.VC90.CRT\" $$(PICTO_TREE)\output\tests\bin\release\Microsoft.VC90.CRT
    }
}
wince* {
    CRUNTIMETESTSPRIVATEASSEMBLY.extra = copy \"$$(DevEnvDir)\..\..\VC\ce\dll\x86\msvcr90.dll\" $$(PICTO_TREE)\output\bin\release
}
CRUNTIMETESTSPRIVATEASSEMBLY.path = $$(PICTO_TREE)/output/tests/bin/release
INSTALLS += CRUNTIMETESTSPRIVATEASSEMBLY

#The C runtime files for debug are not licensed for redistribution, and are thus not copied here

TESTS_DEBUG.extra = copy $$(PICTO_TREE)\output\bin\debug\*.dll $$(PICTO_TREE)\output\tests\bin\debug
TESTS_DEBUG.files = $$[QT_INSTALL_PREFIX]/lib/QtTestd4.dll
TESTS_DEBUG.path = $$(PICTO_TREE)/output/tests/bin/debug
INSTALLS += TESTS_DEBUG
}

unix:!macx {
IMAGEFORMATPLUGINS.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/libqgif.so
IMAGEFORMATPLUGINS.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/libqico.so
IMAGEFORMATPLUGINS.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/libqjpeg.so
IMAGEFORMATPLUGINS.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/libqmng.so
IMAGEFORMATPLUGINS.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/libqsvg.so
IMAGEFORMATPLUGINS.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/libqtiff.so
IMAGEFORMATPLUGINS.path = $$(PICTO_TREE)/output/bin/release/imageformats
INSTALLS += IMAGEFORMATPLUGINS

IMAGEFORMATPLUGINS_DEBUG.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/libqgif.so
IMAGEFORMATPLUGINS_DEBUG.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/libqico.so
IMAGEFORMATPLUGINS_DEBUG.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/libqjpeg.so
IMAGEFORMATPLUGINS_DEBUG.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/libqmng.so
IMAGEFORMATPLUGINS_DEBUG.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/libqsvg.so
IMAGEFORMATPLUGINS_DEBUG.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/libqtiff.so
IMAGEFORMATPLUGINS_DEBUG.path = $$(PICTO_TREE)/output/bin/debug/imageformats
INSTALLS += IMAGEFORMATPLUGINS_DEBUG

QTLIBS.extra  =   cp -dp $$[QT_INSTALL_PREFIX]/lib/libQtCore.so* $$(PICTO_TREE)/output/bin/release/shared
QTLIBS.extra += ; cp -dp $$[QT_INSTALL_PREFIX]/lib/libQtGui.so* $$(PICTO_TREE)/output/bin/release/shared
QTLIBS.extra += ; cp -dp $$[QT_INSTALL_PREFIX]/lib/libQtNetwork.so* $$(PICTO_TREE)/output/bin/release/shared
QTLIBS.extra += ; cp -dp $$[QT_INSTALL_PREFIX]/lib/libphonon.so* $$(PICTO_TREE)/output/bin/release/shared
QTLIBS.extra += ; cp -dp $$[QT_INSTALL_PREFIX]/lib/libQtScript.so* $$(PICTO_TREE)/output/bin/release/shared
QTLIBS.extra += ; cp -dp $$[QT_INSTALL_PREFIX]/lib/libQtScriptTools.so* $$(PICTO_TREE)/output/bin/release/shared
QTLIBS.extra += ; cp -dp $$[QT_INSTALL_PREFIX]/lib/libQtSql.so* $$(PICTO_TREE)/output/bin/release/shared
QTLIBS.extra += ; cp -dp $$[QT_INSTALL_PREFIX]/lib/libQtXml.so* $$(PICTO_TREE)/output/bin/release/shared
QTLIBS.extra += ; cp -dp $$[QT_INSTALL_PREFIX]/lib/libQtWebKit.so* $$(PICTO_TREE)/output/bin/release/shared
QTLIBS.path = $$(PICTO_TREE)/output/bin/release/shared
INSTALLS += QTLIBS

QTLIBS_DEBUG.extra  =   cp -dp $$[QT_INSTALL_PREFIX]/lib/libQtCore.so* $$(PICTO_TREE)/output/bin/debug/shared
QTLIBS_DEBUG.extra += ; cp -dp $$[QT_INSTALL_PREFIX]/lib/libQtGui.so* $$(PICTO_TREE)/output/bin/debug/shared
QTLIBS_DEBUG.extra += ; cp -dp $$[QT_INSTALL_PREFIX]/lib/libQtNetwork.so* $$(PICTO_TREE)/output/bin/debug/shared
QTLIBS_DEBUG.extra += ; cp -dp $$[QT_INSTALL_PREFIX]/lib/libphonon.so* $$(PICTO_TREE)/output/bin/debug/shared
QTLIBS_DEBUG.extra += ; cp -dp $$[QT_INSTALL_PREFIX]/lib/libQtScript.so* $$(PICTO_TREE)/output/bin/debug/shared
QTLIBS_DEBUG.extra += ; cp -dp $$[QT_INSTALL_PREFIX]/lib/libQtScriptTools.so* $$(PICTO_TREE)/output/bin/debug/shared
QTLIBS_DEBUG.extra += ; cp -dp $$[QT_INSTALL_PREFIX]/lib/libQtSql.so* $$(PICTO_TREE)/output/bin/debug/shared
QTLIBS_DEBUG.extra += ; cp -dp $$[QT_INSTALL_PREFIX]/lib/libQtXml.so* $$(PICTO_TREE)/output/bin/debug/shared
QTLIBS_DEBUG.extra += ; cp -dp $$[QT_INSTALL_PREFIX]/lib/libQtWebKit.so* $$(PICTO_TREE)/output/bin/debug/shared
QTLIBS_DEBUG.path = $$(PICTO_TREE)/output/bin/debug/shared
INSTALLS += QTLIBS_DEBUG

QTSOLUTIONS_IOCOMPRESSOR.extra = cp -dp $$QTIOCOMPRESSOR_LIBDIR/* $$(PICTO_TREE)/output/bin/release/shared
QTSOLUTIONS_IOCOMPRESSOR.path = $$(PICTO_TREE)/output/bin/release/shared
INSTALLS += QTSOLUTIONS_IOCOMPRESSOR

QTSOLUTIONS_IOCOMPRESSOR_DEBUG.extra = cp -dp $$QTIOCOMPRESSOR_LIBDIR/* $$(PICTO_TREE)/output/bin/debug/shared
QTSOLUTIONS_IOCOMPRESSOR_DEBUG.path = $$(PICTO_TREE)/output/bin/debug/shared
INSTALLS += QTSOLUTIONS_IOCOMPRESSOR_DEBUG

QTSOLUTIONS_PROPERTYBROWSER.extra = cp -dp $$QTPROPERTYBROWSER_LIBDIR/* $$(PICTO_TREE)/output/bin/release/shared
QTSOLUTIONS_PROPERTYBROWSER.path = $$(PICTO_TREE)/output/bin/release/shared
INSTALLS += QTSOLUTIONS_PROPERTYBROWSER

QTSOLUTIONS_PROPERTYBROWSER_DEBUG.extra = cp -dp $$QTPROPERTYBROWSER_LIBDIR/* $$(PICTO_TREE)/output/bin/debug/shared
QTSOLUTIONS_PROPERTYBROWSER_DEBUG.path = $$(PICTO_TREE)/output/bin/debug/shared
INSTALLS += QTSOLUTIONS_PROPERTYBROWSER_DEBUG

TESTS.extra  =   cp -dp $$(PICTO_TREE)/output/bin/release/shared/* $$(PICTO_TREE)/output/tests/bin/release/shared
TESTS.extra += ; cp -dp $$[QT_INSTALL_PREFIX]/lib/libQtTest.so* $$(PICTO_TREE)/output/tests/bin/release/shared
TESTS.path = $$(PICTO_TREE)/output/tests/bin/release/shared/
INSTALLS += TESTS

TESTS_DEBUG.extra  =   cp -dp $$(PICTO_TREE)/output/bin/debug/shared/* $$(PICTO_TREE)/output/tests/bin/debug/shared
TESTS_DEBUG.extra += ; cp -dp $$[QT_INSTALL_PREFIX]/lib/libQtTest.so* $$(PICTO_TREE)/output/tests/bin/debug/shared
TESTS_DEBUG.path = $$(PICTO_TREE)/output/tests/bin/debug/shared/
INSTALLS += TESTS_DEBUG
}
