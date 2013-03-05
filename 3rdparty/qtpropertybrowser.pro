TEMPLATE=lib
TARGET = QtPropertyBrowser
QT = core gui xml widgets
CONFIG += qt warn_on debug_and_release build_all static ordered

DEPENDPATH += .
INCLUDEPATH += .

include(qtpropertybrowser.pri)

# Output
build_pass:CONFIG(debug, debug|release) {
  DESTDIR = ./lib
  OBJECTS_DIR	= ./intermediates/obj/debug
}
build_pass:CONFIG(release, debug|release) {
  DESTDIR = ./lib
  OBJECTS_DIR	= ./intermediates/obj/release
}
RCC_DIR = ./intermediates/resources/
UI_DIR = ./intermediates/ui/
MOC_DIR = ./intermediates/moc/

# Debug Naming
build_pass:CONFIG(debug, debug|release) {
  TARGET 		= $$member(TARGET, 0)d
}

