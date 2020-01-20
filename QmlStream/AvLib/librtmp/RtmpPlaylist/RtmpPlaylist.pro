TEMPLATE = lib
DESTDIR = $$PWD/../../lib
SOURCES = ClRtmpPlaylist.cpp
HEADERS = ClRtmpPlaylist.h
win32{
	INCLUDEPATH += $$PWD/../librtmp-2.4
	CONFIG += static
	win32-msvc2008 | win32-msvc2010 | win32-msvc2012{
		QMAKE_CXXFLAGS += /wd4819
	}
}
android{
	INCLUDEPATH += $$PWD/../librtmp-2.3
	LIBS += -L../libAndroid -lrtmp
}
