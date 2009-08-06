infile(config.pri, SOLUTIONS_LIBRARY, yes): CONFIG += qtpropertybrowser-uselib
macx:CONFIG += x86 ppc
TEMPLATE += fakelib
QTPROPERTYBROWSER_LIBNAME = $$qtLibraryTarget(QtSolutions_PropertyBrowser-2.5)
#For some reason, implicitly linked DLLs won't load properly on
#Windows CE if the filename sans extension is ~28 characters long.
#Both longer and shorter names work, so it is unclear why this
#occurs.  As the generated DLL would have an incorrect length, we
#must rename it here.
wince*:QTPROPERTYBROWSER_LIBNAME = $$qtLibraryTarget(PropertyBrowser-2.5)
CONFIG(debug, debug|release) {
win32:!wince*:QTPROPERTYBROWSER_LIBNAME = QtSolutions_PropertyBrowser-2.5d
win32:wince*:QTPROPERTYBROWSER_LIBNAME = PropertyBrowser-2.5d
QTPROPERTYBROWSER_LIBDIR = $$PWD/lib/debug
}
CONFIG(release, debug|release) {
QTPROPERTYBROWSER_LIBDIR = $$PWD/lib/release
}
TEMPLATE -= fakelib
unix:!macx {
QTPROPERTYBROWSER_LIBDIR = $$PWD/lib
QMAKE_RPATHDIR =
QMAKE_LFLAGS += -Wl,-rpath,\'\$\$ORIGIN\'
}
macx {
QMAKE_LFLAGS_SONAME = -install_name$${LITERAL_WHITESPACE}@executable_path/../Libraries/qtpropertybrowser/
CONFIG -= absolute_library_soname
}
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
        }}}}
    }
}
#Output PDB file to appropriate directory
QMAKE_CXXFLAGS+=/Fd$(IntDir)
}
