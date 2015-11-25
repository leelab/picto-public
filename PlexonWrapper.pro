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
