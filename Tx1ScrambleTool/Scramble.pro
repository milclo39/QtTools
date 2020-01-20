#-------------------------------------------------
#
# Project created by QtCreator 2015-07-23T13:15:01
#
#-------------------------------------------------

QT += core widgets sql

TARGET = Scramble

INCLUDEPATH *= . \
				$(QTPLUGINSROOT)/include \
				$(QTPLUGINSROOT)/Common \
				../..

SOURCES += main.cpp\
		widget.cpp \
		Parts/ClProcessWidgetBase.cpp \
		Parts/ClProgressIndicator.cpp

HEADERS  += widget.h \
		KeyDefine.h \
		Parts/ClProcessWidgetBase.h \
		Parts/ClProgressIndicator.h
QMAKE_LIBDIR *= $(QTPLUGINSROOT)/lib
CONFIG(debug, debug|release){
	LIBS += -lAppCheckd
}
else{
	LIBS += -lAppCheck
}

win32{
	LIBS += -lUser32 -lShell32 -ladvapi32
}

DEFINES += WIN32 _UNICODE UNICODE _LIB WITH_IPP
LIBS += -L$$PWD/openssl-1.0.2/lib -llibeay32 User32.lib \
		setupapi.lib \
		psapi.lib \
		shell32.lib
INCLUDEPATH += $$PWD/openssl-1.0.2/include/windows $$PWD/openssl-1.0.2/include
QMAKE_CXXFLAGS += /wd4819

RESOURCES += \
	resource/lang.qrc \
	resource/resource.qrc

TRANSLATIONS = \
	resource/lang/en.ts	\
	resource/lang/ja.ts
