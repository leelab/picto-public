TEMPLATE = subdirs
CONFIG -= flat
CONFIG += ordered 
CONFIG += c++11

MACHINE_TYPE = X86

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

addSubdirs(source/plexonwrapper)

# Deployment

win32 {

QTLIBS.files += $$[QT_INSTALL_PREFIX]/lib/Qt5Core.dll
QTLIBS.path = $$(PICTO_TREE)/output/bin/release/wrapper
INSTALLS += QTLIBS

QTLIBS_DEBUG.files += $$[QT_INSTALL_PREFIX]/lib/Qt5Cored.dll
QTLIBS_DEBUG.path = $$(PICTO_TREE)/output/bin/debug/wrapper
INSTALLS += QTLIBS_DEBUG

CRUNTIMEPRIVATEASSEMBLY.extra = xcopy /E /I /Y \"$$(DevEnvDir)/../../VC/redist/x86/Microsoft.VC90.CRT\" "$$(PICTO_TREE)/output/bin/release/wrapper/Microsoft.VC90.CRT"
CRUNTIMEPRIVATEASSEMBLY_IMAGEFORMATS.extra = xcopy /E /I /Y \"$$(DevEnvDir)/../../VC/redist/x86/Microsoft.VC90.CRT\" $$(PICTO_TREE)/output/bin/release/wrapper/imageformats/Microsoft.VC90.CRT

CRUNTIMEPRIVATEASSEMBLY.path = $$(PICTO_TREE)/output/bin/release/wrapper
INSTALLS += CRUNTIMEPRIVATEASSEMBLY


#The C runtime files for debug are not licensed for redistribution, We need to make sure that 
#we don't accidentally release them.  However, that seems like a low risk, given that we won't be 
#releasing a debug build of Picto.  The benefit of copying them here is that the output folder can
#be copied directly to other machines.

#Also, OpenSSL requires the non-debug c runtime, so we copy that here as well


CRUNTIMEPRIVATEASSEMBLY_DEBUG.extra = xcopy /E /I /Y \"$$(DevEnvDir)/../../VC/redist/Debug_NonRedist/x86/Microsoft.VC90.DebugCRT\" $$(PICTO_TREE)/output/bin/debug/wrapper/Microsoft.VC90.DebugCRT
CRUNTIMEPRIVATEASSEMBLY_DEBUG_RELEASE_CRT.extra = xcopy /E /I /Y \"$$(DevEnvDir)/../../VC/redist/x86/Microsoft.VC90.CRT\" $$(PICTO_TREE)/output/bin/debug/wrapper/Microsoft.VC90.CRT
 
CRUNTIMEPRIVATEASSEMBLY_DEBUG.path = $$(PICTO_TREE)/output/bin/debug/wrapper
INSTALLS += CRUNTIMEPRIVATEASSEMBLY_DEBUG

CRUNTIMEPRIVATEASSEMBLY_DEBUG_RELEASE_CRT.path = $$(PICTO_TREE)/output/bin/debug/wrapper
INSTALLS += CRUNTIMEPRIVATEASSEMBLY_DEBUG_RELEASE_CRT

# Resources
RESOURCES += $$(PICTO_TREE)/source/common/common.qrc

#Files for running the ProxyServer with the various nerual acquisition devices

#The PlexClient.dll only exists for x86 releases.
NEURALACQ_DLLS.files += $$(PICTO_TREE)/3rdparty/bin/PlexClient.dll
NEURALACQ_DLLS.path = $$(PICTO_TREE)/output/bin/release/wrapper
INSTALLS += NEURALACQ_DLLS


NEURALACQ_DLLS_DEBUG.files += $$(PICTO_TREE)/3rdparty/bin/PlexClient.dll
NEURALACQ_DLLS_DEBUG.path = $$(PICTO_TREE)/output/bin/debug/wrapper
INSTALLS += NEURALACQ_DLLS_DEBUG

}
