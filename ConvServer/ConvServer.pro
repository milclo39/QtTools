QT += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ConvServer
TEMPLATE = app

#DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += include
HEADERS += \
	include/curl/curl.h

SOURCES += main.cpp\
		widget.cpp \
		ElConvServer.cpp \
		ElConvClient.cpp

HEADERS  += widget.h \
		ElConvServer.h \
		ElConvClient.h

#CONFIG += mobility
MOBILITY = 

QMAKE_LIBDIR += $$PWD/lib
android{
#	ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android-sources
	LIBS = \
		$$PWD/lib/libcurl.a \
		$$PWD/lib/libssl.a \
		$$PWD/lib/libcrypto.a
#	LIBS += -lcurl -lcrypto -lssl
}

#contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
#	ANDROID_EXTRA_LIBS = \
#		$$PWD/lib/libcurl.so \
#		$$PWD/lib/libssl.so \
#		$$PWD/lib/libcrypto.so
#}
