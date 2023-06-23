INSTALL_INCLUDEDIR = $$PWD/include
INSTALL_LIBDIR = $$PWD/lib
MOC_DIR = .moc
UI_SOURCES_DIR = .ui
CONFIG(debug, debug|release){
	OBJECTS_DIR = .objdebug
}
else{
	OBJECTS_DIR = .objrelease
}

UI_HEADERS_DIR = $$UI_DIR
INCLUDEPATH += $$UI_HEADERS_DIR
