TEMPLATE = lib
DESTDIR = $$PWD/lib
android:QMAKE_LIBDIR *= $$PWD/libav/libs/Android_arm
INCLUDEPATH *= $$PWD/Common $$PWD/libav/include
CONFIG(release, debug|release){
	DEFINES += QT_NO_DEBUG_STREAM
	MOC_DIR = .int
	OBJECTS_DIR = .int
	RCC_DIR = .int
	UI_DIR = .int
}
else{
	MOC_DIR = .intd
	OBJECTS_DIR = .intd
	RCC_DIR = .intd
	UI_DIR = .intd
}
win32:CONFIG *= static
win32-msvc2008 | win32-msvc2010 | win32-msvc2012 | win32-msvc2013{
	QMAKE_CXXFLAGS += /wd4819
	INCLUDEPATH *= $$PWD/libav/Source/inttypes
}
