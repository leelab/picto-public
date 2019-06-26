TEMPLATE = subdirs
CONFIG -= flat
CONFIG += ordered 
CONFIG += c++11

MACHINE_TYPE = $$(PICTO_MACHINE_TYPE)

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

addSubdirs(source/common)
addSubdirs(source/director)
addSubdirs(source/server)
addSubdirs(source/proxyserver)
addSubdirs(source/config)
addSubdirs(source/workstation)
addSubdirs(source/embedded)

addSubdirs(source/proxyplugins/plexonplugin)
addSubdirs(source/proxyplugins/tdtplugin)
addSubdirs(source/proxyplugins/RHD2000Control)
addSubdirs(source/proxyplugins/virtualdeviceplugin)

#SUBDIRS = source/common
#SUBDIRS += source/director
#!wince* {
#  SUBDIRS += source/server
#  SUBDIRS += source/proxyserver
#  SUBDIRS += source/config
#  SUBDIRS += source/workstation
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
#exists(C:/TDT){
#
#	addSubdirs(source/proxyplugins/tdtplugin)
#	message("Building TDT proxy server plugin")
#}

# Dependencies

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

!wince* {
SSLLIBS.files += $$(OPENSSLDIR)/out32dll/ssleay32.dll
SSLLIBS.files += $$(OPENSSLDIR)/out32dll/libeay32.dll
}
SSLLIBS.path = $$(PICTO_TREE)/output/bin/release
INSTALLS += SSLLIBS

!wince* {
SSLLIBS_DEBUG.files += $$(OPENSSLDIR)/out32dll/ssleay32.dll
SSLLIBS_DEBUG.files += $$(OPENSSLDIR)/out32dll/libeay32.dll
}
SSLLIBS_DEBUG.path = $$(PICTO_TREE)/output/bin/debug
INSTALLS += SSLLIBS_DEBUG

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



PICTOLIB_DEBUG.files += $$(PICTO_TREE)/intermediates/lib/debug/libPicto_debug.ilk
PICTOLIB_DEBUG.files += $$(PICTO_TREE)/intermediates/lib/debug/libPicto_debug.pdb
PICTOLIB_DEBUG.path += $$(PICTO_TREE)/output/bin/debug
INSTALLS += PICTOLIB_DEBUG

}