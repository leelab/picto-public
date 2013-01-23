TEMPLATE = subdirs
CONFIG -= flat
CONFIG += ordered 

# addSubdirs(subdirs,deps): Adds directories to the project that depend on
# other directories
defineTest(addSubdirs) {
    for(subdirs, 1) {
        entries = $$files($$subdirs)
        for(entry, entries) {
            name = $$replace(entry, [/\\\\], _)
            SUBDIRS += $$name
            eval ($${name}.subdir = $$entry)
            for(dep, 2):eval ($${name}.depends += $$replace(dep, [/\\\\], _))
            export ($${name}.subdir)
            export ($${name}.depends)
        }
    }
    export (SUBDIRS)
}

addSubdirs(source/common, source/common/unittests)
addSubdirs(source/director)
addSubdirs(source/server, source/server/unittests)
addSubdirs(source/proxyserver)
addSubdirs(source/config)
addSubdirs(source/workstation)
addSubdirs(source/documentation/developersguide)
addSubdirs(source/embedded)
addSubdirs(source/proxyplugins/plexonplugin)
addSubdirs(source/proxyplugins/virtualdeviceplugin)

#SUBDIRS = source/common				source/common/unittests
#SUBDIRS += source/director
#!wince* {
#  SUBDIRS += source/server          source/server/unittests
#  SUBDIRS += source/proxyserver
#  SUBDIRS += source/config
#  SUBDIRS += source/workstation
#  SUBDIRS += source/documentation/developersguide
#}
#win* {
#  SUBDIRS += source/embedded
#}

#proxy server plugins
#win32:!wince*:  SUBDIRS += source/proxyplugins/plexonplugin
#win32:!wince*:  SUBDIRS += source/proxyplugins/virtualdeviceplugin

#We can't build the TDT plugin unless we have the TDT SDK installed.
#The TDT software has a hard time running under 64-bit Vista, so I always
#used a 32-bit XP virtual machine to build this.
exists(C:/TDT){

	addSubdirs(source/proxyplugins/tdtplugin)
	message("Building TDT proxy server plugin")
}

# Dependencies

#unittests.depends = common
#director.depends = common
#server.depends = common
#proxyserver.depends = common
#config.depends = common
#workstation.depends = common
#developersguide.depends = common
#embedded.depends = common
#plexonplugin.depends = proxyserver
#virtualdeviceplugin.depends = proxyserver
#tdtplugin.depends = proxyserver

# Deployment

include($$(PROPBROWSDIR)/qtpropertybrowser.pri)

win32 {
IMAGEFORMATPLUGINS.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qgif.dll
IMAGEFORMATPLUGINS.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qico.dll
IMAGEFORMATPLUGINS.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qjpeg.dll
IMAGEFORMATPLUGINS.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qmng.dll
IMAGEFORMATPLUGINS.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qsvg.dll
IMAGEFORMATPLUGINS.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qtiff.dll
IMAGEFORMATPLUGINS.path = $$(PICTO_TREE)/output/bin/release/imageformats
INSTALLS += IMAGEFORMATPLUGINS

IMAGEFORMATPLUGINS_DEBUG.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qgifd.dll
IMAGEFORMATPLUGINS_DEBUG.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qicod.dll
IMAGEFORMATPLUGINS_DEBUG.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qjpegd.dll
IMAGEFORMATPLUGINS_DEBUG.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qmngd.dll
IMAGEFORMATPLUGINS_DEBUG.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qsvgd.dll
IMAGEFORMATPLUGINS_DEBUG.files += $$[QT_INSTALL_PREFIX]/plugins/imageformats/qtiffd.dll
IMAGEFORMATPLUGINS_DEBUG.path = $$(PICTO_TREE)/output/bin/debug/imageformats
INSTALLS += IMAGEFORMATPLUGINS_DEBUG

!wince*:QTSOLUTIONS_PROPERTYBROWSER.files += $$(PROPBROWSDIR)/bin/release/QtPropertyBrowser.dll
wince*:QTSOLUTIONS_PROPERTYBROWSER.files += $$(PROPBROWSDIR)/bin/release/QtPropertyBrowser.dll
QTSOLUTIONS_PROPERTYBROWSER.path = $$(PICTO_TREE)/output/bin/release
INSTALLS += QTSOLUTIONS_PROPERTYBROWSER

!wince*:QTSOLUTIONS_PROPERTYBROWSER_DEBUG.files += $$(PROPBROWSDIR)/bin/debug/QtPropertyBrowserd.dll
wince*:QTSOLUTIONS_PROPERTYBROWSER_DEBUG.files += $$(PROPBROWSDIR)/bin/debug/QtPropertyBrowserd.dll
QTSOLUTIONS_PROPERTYBROWSER_DEBUG.path = $$(PICTO_TREE)/output/bin/debug
INSTALLS += QTSOLUTIONS_PROPERTYBROWSER_DEBUG

QTLIBS.files += $$[QT_INSTALL_PREFIX]/lib/Qt5Core.dll
QTLIBS.files += $$[QT_INSTALL_PREFIX]/lib/Qt5Gui.dll
QTLIBS.files += $$[QT_INSTALL_PREFIX]/lib/Qt5Network.dll
QTLIBS.files += $$[QT_INSTALL_PREFIX]/lib/Qt5Script.dll
QTLIBS.files += $$[QT_INSTALL_PREFIX]/lib/Q5tScriptTools.dll
QTLIBS.files += $$[QT_INSTALL_PREFIX]/lib/Qt5Sql.dll
QTLIBS.files += $$[QT_INSTALL_PREFIX]/lib/Qt5Xml.dll
QTLIBS.files += $$[QT_INSTALL_PREFIX]/lib/Qt5Widgets.dll
QTLIBS.path = $$(PICTO_TREE)/output/bin/release
INSTALLS += QTLIBS

QTLIBS_DEBUG.files += $$[QT_INSTALL_PREFIX]/lib/Qt5Cored.dll
QTLIBS_DEBUG.files += $$[QT_INSTALL_PREFIX]/lib/Qt5Guid.dll
QTLIBS_DEBUG.files += $$[QT_INSTALL_PREFIX]/lib/Qt5Networkd.dll
QTLIBS_DEBUG.files += $$[QT_INSTALL_PREFIX]/lib/Qt5Scriptd.dll
QTLIBS_DEBUG.files += $$[QT_INSTALL_PREFIX]/lib/Qt5ScriptToolsd.dll
QTLIBS_DEBUG.files += $$[QT_INSTALL_PREFIX]/lib/Qt5Sqld.dll
QTLIBS_DEBUG.files += $$[QT_INSTALL_PREFIX]/lib/Qt5Xmld.dll
QTLIBS_DEBUG.files += $$[QT_INSTALL_PREFIX]/lib/Qt5Widgetsd.dll
QTLIBS_DEBUG.path = $$(PICTO_TREE)/output/bin/debug
INSTALLS += QTLIBS_DEBUG

wince* {
SSLLIBS.files += $$(PICTO_THIRD_PARTY)/openssl-0.9.8k/out32dll_X86/ssleay32.dll
SSLLIBS.files += $$(PICTO_THIRD_PARTY)/openssl-0.9.8k/out32dll_X86/libeay32.dll
}
!wince* {
SSLLIBS.files += $$(PICTO_THIRD_PARTY)/openssl/bin/ssleay32.dll
SSLLIBS.files += $$(PICTO_THIRD_PARTY)/openssl/bin/libeay32.dll
}
SSLLIBS.path = $$(PICTO_TREE)/output/bin/release
INSTALLS += SSLLIBS

wince* {
SSLLIBS_DEBUG.files += $$(PICTO_THIRD_PARTY)/openssl/out32dll_X86/ssleay32.dll
SSLLIBS_DEBUG.files += $$(PICTO_THIRD_PARTY)/openssl/out32dll_X86/libeay32.dll
}
!wince* {
SSLLIBS_DEBUG.files += $$(PICTO_THIRD_PARTY)/openssl/bin/ssleay32.dll
SSLLIBS_DEBUG.files += $$(PICTO_THIRD_PARTY)/openssl/bin/libeay32.dll
}
SSLLIBS_DEBUG.path = $$(PICTO_TREE)/output/bin/debug
INSTALLS += SSLLIBS_DEBUG

MACHINE_TYPE = $$(PICTO_MACHINE_TYPE)
!wince* {
    contains(MACHINE_TYPE,X86) {
    CRUNTIMEPRIVATEASSEMBLY.extra = xcopy /E /I /Y \"$$(DevEnvDir)/../../VC/redist/x86/Microsoft.VC90.CRT\" "$$(PICTO_TREE)/output/bin/release/Microsoft.VC90.CRT"
    CRUNTIMEPRIVATEASSEMBLY_IMAGEFORMATS.extra = xcopy /E /I /Y \"$$(DevEnvDir)/../../VC/redist/x86/Microsoft.VC90.CRT\" $$(PICTO_TREE)/output/bin/release/imageformats/Microsoft.VC90.CRT
    }
    contains(MACHINE_TYPE,X64) {
    CRUNTIMEPRIVATEASSEMBLY.extra = xcopy /E /I /Y \"$$(DevEnvDir)/../../VC/redist/amd64/Microsoft.VC90.CRT\" $$(PICTO_TREE)/output/bin/release/Microsoft.VC90.CRT
    CRUNTIMEPRIVATEASSEMBLY_IMAGEFORMATS.extra = xcopy /E /I /Y \"$$(DevEnvDir)/../../VC/redist/amd64/Microsoft.VC90.CRT\" $$(PICTO_TREE)/output/bin/release/imageformats/Microsoft.VC90.CRT
    }
}
wince* {
    CRUNTIMEPRIVATEASSEMBLY.extra = copy \"$$(DevEnvDir)/../../VC/ce/dll/x86/msvcr??.dll\" $$(PICTO_TREE)/output/bin/release
}
CRUNTIMEPRIVATEASSEMBLY.path = $$(PICTO_TREE)/output/bin/release
INSTALLS += CRUNTIMEPRIVATEASSEMBLY
CRUNTIMEPRIVATEASSEMBLY_IMAGEFORMATS.path = $$(PICTO_TREE)/output/bin/release/imageformats
INSTALLS += CRUNTIMEPRIVATEASSEMBLY_IMAGEFORMATS




#The C runtime files for debug are not licensed for redistribution, We need to make sure that 
#we don't accidentally release them.  However, that seems like a low risk, given that we won't be 
#releasing a debug build of Picto.  The benefit of copying them here is that the output folder can
#be copied directly to other machines.

#Also, OpenSSL requires the non-debug c runtime, so we copy that here as well

!wince* {
    contains(MACHINE_TYPE,X86) {
    CRUNTIMEPRIVATEASSEMBLY_DEBUG.extra = xcopy /E /I /Y \"$$(DevEnvDir)/../../VC/redist/Debug_NonRedist/x86/Microsoft.VC90.DebugCRT\" $$(PICTO_TREE)/output/bin/debug/Microsoft.VC90.DebugCRT
    CRUNTIMEPRIVATEASSEMBLY_DEBUG_RELEASE_CRT.extra = xcopy /E /I /Y \"$$(DevEnvDir)/../../VC/redist/x86/Microsoft.VC90.CRT\" $$(PICTO_TREE)/output/bin/debug/Microsoft.VC90.CRT
    CRUNTIMEPRIVATEASSEMBLY_IMAGEFORMATS_DEBUG.extra = xcopy /E /I /Y \"$$(DevEnvDir)/../../VC/redist/Debug_NonRedist/x86/Microsoft.VC90.DebugCRT\" $$(PICTO_TREE)/output/bin/debug/imageformats/Microsoft.VC90.DebugCRT
    }
    contains(MACHINE_TYPE,X64) {
    CRUNTIMEPRIVATEASSEMBLY_DEBUG.extra = xcopy /E /I /Y \"$$(DevEnvDir)/../../VC/redist/Debug_NonRedist/amd64/Microsoft.VC90.DebugCRT\" $$(PICTO_TREE)/output/bin/debug/Microsoft.VC90.DebugCRT
    CRUNTIMEPRIVATEASSEMBLY_DEBUG_RELEASE_CRT.extra = xcopy /E /I /Y \"$$(DevEnvDir)/../../VC/redist/amd64/Microsoft.VC90.CRT\" $$(PICTO_TREE)/output/bin/debug/Microsoft.VC90.CRT
    CRUNTIMEPRIVATEASSEMBLY_IMAGEFORMATS_DEBUG.extra = xcopy /E /I /Y \"$$(DevEnvDir)/../../VC/redist/Debug_NonRedist/amd64/Microsoft.VC90.DebugCRT\" $$(PICTO_TREE)/output/bin/debug/imageformats/Microsoft.VC90.DebugCRT
    }
}
wince* {
	# !!! WARNING !!!
	# I set this up without looking at an actual CE dev environment.  This path may be incorrect.
    CRUNTIMEPRIVATEASSEMBLY_DEBUG.extra = copy \"$$(DevEnvDir)/../../VC/ce/dll/x86/msvcr??d.dll\" $$(PICTO_TREE)/output/bin/release
}
CRUNTIMEPRIVATEASSEMBLY_DEBUG.path = $$(PICTO_TREE)/output/bin/debug
INSTALLS += CRUNTIMEPRIVATEASSEMBLY_DEBUG
CRUNTIMEPRIVATEASSEMBLY_IMAGEFORMATS_DEBUG.path = $$(PICTO_TREE)/output/bin/debug/imageformats
INSTALLS += CRUNTIMEPRIVATEASSEMBLY_IMAGEFORMATS_DEBUG
CRUNTIMEPRIVATEASSEMBLY_DEBUG_RELEASE_CRT.path = $$(PICTO_TREE)/output/bin/debug
INSTALLS += CRUNTIMEPRIVATEASSEMBLY_DEBUG_RELEASE_CRT





TESTS.extra = copy $$(PICTO_TREE)/output/bin/release/*.dll $$(PICTO_TREE)/output/tests/bin/release
TESTS.files = $$[QT_INSTALL_PREFIX]/lib/QtTest4.dll
TESTS.path = $$(PICTO_TREE)/output/tests/bin/release
INSTALLS += TESTS

PICTOLIB_DEBUG.files += $$(PICTO_TREE)/intermediates/lib/debug/libPicto_debug.ilk
PICTOLIB_DEBUG.files += $$(PICTO_TREE)/intermediates/lib/debug/libPicto_debug.pdb
PICTOLIB_DEBUG.path += $$(PICTO_TREE)/output/bin/debug
INSTALLS += PICTOLIB_DEBUG

!wince* {
    contains(MACHINE_TYPE,X86) {
    CRUNTIMETESTSPRIVATEASSEMBLY.extra = xcopy /E /I /Y \"$$(DevEnvDir)/../../VC/redist/x86/Microsoft.VC90.CRT\" $$(PICTO_TREE)/output/tests/bin/release/Microsoft.VC90.CRT
    }
    contains(MACHINE_TYPE,X64) {
    CRUNTIMETESTSPRIVATEASSEMBLY.extra = xcopy /E /I /Y \"$$(DevEnvDir)/../../VC/redist/amd64/Microsoft.VC90.CRT\" $$(PICTO_TREE)/output/tests/bin/release/Microsoft.VC90.CRT
    }
}
wince* {
    CRUNTIMETESTSPRIVATEASSEMBLY.extra = copy \"$$(DevEnvDir)/../../VC/ce/dll/x86/msvcr??.dll\" $$(PICTO_TREE)/output/bin/release
}
CRUNTIMETESTSPRIVATEASSEMBLY.path = $$(PICTO_TREE)/output/tests/bin/release
INSTALLS += CRUNTIMETESTSPRIVATEASSEMBLY

# As noted above, we are copying C runtime assemblies that aren't licensed for redistribution.  
# Don't redistribute the debug builds.
!wince* {
    contains(MACHINE_TYPE,X86) {
    CRUNTIMETESTSPRIVATEASSEMBLY_DEBUG.extra = xcopy /E /I /Y \"$$(DevEnvDir)/../../VC/redist/Debug_NonRedist/x86/Microsoft.VC90.DebugCRT\" $$(PICTO_TREE)/output/tests/bin/debug/Microsoft.VC90.DebugCRT
    CRUNTIMETESTPRIVATEASSEMBLY_DEBUG_RELEASE_CRT.extra = xcopy /E /I /Y \"$$(DevEnvDir)/../../VC/redist/x86/Microsoft.VC90.CRT\" $$(PICTO_TREE)/output/tests/bin/debug/Microsoft.VC90.CRT
    }
    contains(MACHINE_TYPE,X64) {
    CRUNTIMETESTSPRIVATEASSEMBLY_DEBUG.extra = xcopy /E /I /Y \"$$(DevEnvDir)/../../VC/redist/Debug_NonRedist/amd64/Microsoft.VC90.DebugCRT\" $$(PICTO_TREE)/output/tests/bin/debug/Microsoft.VC90.DebugCRT
     CRUNTIMETESTPRIVATEASSEMBLY_DEBUG_RELEASE_CRT.extra = xcopy /E /I /Y \"$$(DevEnvDir)/../../VC/redist/amd64/Microsoft.VC90.CRT\" $$(PICTO_TREE)/output/tests/bin/debug/Microsoft.VC90.CRT
   }
}
wince* {
    CRUNTIMETESTSPRIVATEASSEMBLY_DEBUG.extra = copy \"$$(DevEnvDir)/../../VC/ce/dll/x86/msvcr??d.dll\" $$(PICTO_TREE)/output/bin/debug
}
CRUNTIMETESTSPRIVATEASSEMBLY_DEBUG.path = $$(PICTO_TREE)/output/tests/bin/debug/
INSTALLS += CRUNTIMETESTSPRIVATEASSEMBLY_DEBUG

TESTS_DEBUG.extra = copy $$(PICTO_TREE)/output/bin/debug/*.dll $$(PICTO_TREE)/output/tests/bin/debug
TESTS_DEBUG.files = $$[QT_INSTALL_PREFIX]/lib/QtTestd4.dll
TESTS_DEBUG.path = $$(PICTO_TREE)/output/tests/bin/debug
INSTALLS += TESTS_DEBUG


#Files for running the ProxyServer with the various nerual acquisition devices
NEURALACQ_DLLS.files += $$(PICTO_TREE)/3rdparty/bin/PlexClient.dll
NEURALACQ_DLLS.path = $$(PICTO_TREE)/output/bin/release
INSTALLS += NEURALACQ_DLLS

NEURALACQ_DLLS_DEBUG.files += $$(PICTO_TREE)/3rdparty/bin/PlexClient.dll
NEURALACQ_DLLS_DEBUG.path = $$(PICTO_TREE)/output/bin/debug
INSTALLS += NEURALACQ_DLLS_DEBUG

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

#QTSOLUTIONS_PROPERTYBROWSER.extra = cp -dp $$QTPROPERTYBROWSER_LIBDIR/* $$(PICTO_TREE)/output/bin/release/shared
#QTSOLUTIONS_PROPERTYBROWSER.path = $$(PICTO_TREE)/output/bin/release/shared
#INSTALLS += QTSOLUTIONS_PROPERTYBROWSER

#QTSOLUTIONS_PROPERTYBROWSER_DEBUG.extra = cp -dp $$QTPROPERTYBROWSER_LIBDIR/* $$(PICTO_TREE)/output/bin/debug/shared
#QTSOLUTIONS_PROPERTYBROWSER_DEBUG.path = $$(PICTO_TREE)/output/bin/debug/shared
#INSTALLS += QTSOLUTIONS_PROPERTYBROWSER_DEBUG

TESTS.extra  =   cp -dp $$(PICTO_TREE)/output/bin/release/shared/* $$(PICTO_TREE)/output/tests/bin/release/shared
TESTS.extra += ; cp -dp $$[QT_INSTALL_PREFIX]/lib/libQtTest.so* $$(PICTO_TREE)/output/tests/bin/release/shared
TESTS.path = $$(PICTO_TREE)/output/tests/bin/release/shared/
INSTALLS += TESTS

TESTS_DEBUG.extra  =   cp -dp $$(PICTO_TREE)/output/bin/debug/shared/* $$(PICTO_TREE)/output/tests/bin/debug/shared
TESTS_DEBUG.extra += ; cp -dp $$[QT_INSTALL_PREFIX]/lib/libQtTest.so* $$(PICTO_TREE)/output/tests/bin/debug/shared
TESTS_DEBUG.path = $$(PICTO_TREE)/output/tests/bin/debug/shared/
INSTALLS += TESTS_DEBUG

}

