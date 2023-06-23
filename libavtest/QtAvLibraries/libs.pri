TEMPLATE = lib
isEmpty(ARCH){
	ARCH = x86
	CONFIG += x86
}
android:QMAKE_LIBDIR *= $$PWD/libav/libs/Android_arm
INCLUDEPATH *= $$PWD/Common $$PWD/libav/include $(QTPLUGINSROOT)/include
QMAKE_LIBDIR *= $(QTPLUGINSROOT)/lib
CONFIG(release, debug|release){
	DEFINES += QT_NO_DEBUG_STREAM
	MOC_DIR = .int
	OBJECTS_DIR = .int
	RCC_DIR = .int
	UI_DIR = .int
	DESTDIR = $$PWD/lib/$$ARCH/release
}
else{
	MOC_DIR = .intd
	OBJECTS_DIR = .intd
	RCC_DIR = .intd
	UI_DIR = .intd
	DESTDIR = $$PWD/lib/$$ARCH/debug
}
win32{
	CONFIG += static
	win32-msvc2008 | win32-msvc2010 | win32-msvc2012 | win32-msvc2013{
		QMAKE_CXXFLAGS += /wd4819
		INCLUDEPATH *= $$PWD/libav/Source/inttypes
	}
        win32-msvc2010|win32-msvc2012:QMAKE_LIBDIR += $$PWD/libav/libs/Win_vc2010
        win32-msvc2013:QMAKE_LIBDIR += $$PWD/libav/libs/Win_vc2013/$$ARCH
}
macx{
	CONFIG *= static
}
