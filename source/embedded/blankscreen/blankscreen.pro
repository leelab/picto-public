TEMPLATE = app
TARGET = BlankScreen
QT = core gui xml
CONFIG += qt warn_on debug_and_release
DEPENDPATH += .
INCLUDEPATH += .

# Input
SOURCES += $$(PICTO_TREE)/source/embedded/blankscreen/main.cpp

# Output
build_pass:CONFIG(debug, debug|release) {
  DESTDIR = $$(PICTO_TREE)/output/bin/debug
  OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/embedded/blankscreen/debug
}
build_pass:CONFIG(release, debug|release) {
  DESTDIR = $$(PICTO_TREE)/output/bin/release
  OBJECTS_DIR	= $$(PICTO_TREE)/intermediates/obj/embedded/blankscreen/release
}
RCC_DIR = $$(PICTO_TREE)/intermediates/resources/embedded/blankscreen
UI_DIR = $$(PICTO_TREE)/intermediates/ui/embedded/blankscreen
MOC_DIR = $$(PICTO_TREE)/intermediates/moc/embedded/blankscreen

win32 {
#Perform compilation using multiple processes if using
#Microsoft compiler version 15 (Visual Studio 2008) or greater
#Version 14 (Visual Studio 2005) supported the necessary compiler
#switch, but had bugs which could lead to corrupt PDB files
for(compilerDefine, QMAKE_COMPILER_DEFINES) {
    compilerVersion = $$find(compilerDefine, "_MSC_VER=*")
    !isEmpty(compilerVersion) {
        compilerVersion = $$replace(compilerVersion, "_MSC_VER=", "")
        !contains(compilerVersion,1200) {
        !contains(compilerVersion,1300) {
        !contains(compilerVersion,1310) {
        !contains(compilerVersion,1400) {
            QMAKE_CXXFLAGS+=/MP
            visualStudioProject = $$find(TEMPLATE, "vc")
            build_pass:CONFIG(release, debug|release):!isEmpty(visualStudioProject) {
                message(Warnings regarding Compiler option: /MP can safely be ignored)
            }
        }}}}
    }
}
#Output PDB file to appropriate directory
QMAKE_CXXFLAGS+=/Fd$(IntDir)
}

build_pass:CONFIG(debug, debug|release) {
  TARGET 		= $$member(TARGET, 0)_debug
  OBJECTS_DIR	= $$member(OBJECTS_DIR, 0)/debug
}
build_pass:CONFIG(release, debug|release) {
  OBJECTS_DIR	= $$member(OBJECTS_DIR, 0)./release
}
