include($$PWD/Paths.pri)
greaterThan(QT_MAJOR_VERSION, 4):QT += widgets
CONFIG += warn_on
TEMPLATE = lib
UI_DIR = ./$$TARGET
isEmpty(DESTDIR){
	DESTDIR = $$PWD/lib
}else{
	DESTDIR = $$PWD/$$DESTDIR
}

#Xcode6.3以降でビルドを通すのに必要 2015.04.22 matsubara
ios: {
	QMAKE_MAC_SDK.iphoneos.path = /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS8.3.sdk
	QMAKE_MAC_SDK.iphonesimulator.path = /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator8.3.sdk
}

#mac: {
macx: {
	ARCH = $$system(uname -m)
	DEFINES += LINUX32 OSX32
	contains(ARCH , x86_64){
		DEFINES += LINUX64 OSX64 WITH_IPP
	}
	contains(APP, EIT_Q){
		DEFINES -= WITH_IPP
	}
	contains(ARCH , i386){
		DEFINES += WITH_IPP
	}
	contains(ARCH , Power Macintosh){
		CONFIG += ppc
	}
	QMAKE_CXXFLAGS_DEBUG	= -g3 -Wall
	#QMAKE_LFLAGS_DEBUG		=

	QMAKE_CXXFLAGS_RELEASE	= -O2 -Wall
	#QMAKE_LFLAGS_RELEASE	=
	CONFIG += lib_bundle
}
ios: {
#	QMAKE_IOS_DEVICE_ARCHS=arm64
#	QMAKE_IOS_SIMULATOR_ARCHS=x86_64

	QMAKE_CXXFLAGS_DEBUG	= -g3 -Wall
	#QMAKE_LFLAGS_DEBUG		=

	QMAKE_CXXFLAGS_RELEASE	= -O2 -Wall
	#QMAKE_LFLAGS_RELEASE	=
}
linux-g++ {
	QMAKE_CXXFLAGS_DEBUG	= -g3 -Wall -pg
	#QMAKE_LFLAGS_DEBUG		=

	QMAKE_CXXFLAGS_RELEASE	= -O2 -Wall -pg
	#QMAKE_LFLAGS_RELEASE	=
}

linux-icc {
	QMAKE_CXXFLAGS_DEBUG	= -g -Wall
	#QMAKE_LFLAGS_DEBUG		=

	QMAKE_CXXFLAGS_RELEASE	= -O2 -Wall -openmp
	#QMAKE_LFLAGS_RELEASE	=
}

win32{
	DEFINES += WIN32 _UNICODE UNICODE _LIB WITH_IPP
	greaterThan(QT_MAJOR_VERSION, 4):DEFINES *= NOMINMAX
	contains(APP, QBICMONITOR){
		DEFINES -= WITH_IPP
	}
    contains(APP, EIT_Q){
        DEFINES -= WITH_IPP
    }
}

win32-msvc2008 | win32-msvc2010 | win32-msvc2012 | win32-msvc2013 | win32-msvc2015 | win32-msvc2017 {
	QMAKE_CXXFLAGS += /wd4819
	#QMAKE_CXXFLAGS_RELEASE	+= /O2 /Ot /GA
	#QMAKE_LFLAGS_RELEASE	+=
	CONFIG += SSE2
}

win32-icc {
	QMAKE_CXXFLAGS_RELEASE += /O2 /MD /Zc:wchar_t
	QMAKE_CXXFLAGS_RELEASE += /Ob2 /Oi /Ot /Qipo /EHa /MD /GS- /fp:precise
	QMAKE_CXXFLAGS_RELEASE += /Qparallel /Qopenmp /MP
}

CONFIG(release, debug|release){
	DEFINES += QT_NO_DEBUG_STREAM
	DEFINES += NODEBUG
}

INCLUDEPATH += $$PWD
HEADERS += $$PWD/Common/CmnDefine.h

!plugin{
	headers.files += ./$$TARGET/*.h
	headers.path += $$INSTALL_INCLUDEDIR/$$TARGET
	INSTALLS += headers
	CONFIG += staticlib

	CONFIG(debug, debug|release){
		TARGET = $$join(TARGET,,,d)
	}
}
