#-------------------------------------------------
#
# Project created by QtCreator 2011-11-04T08:55:46
#
#-------------------------------------------------


QT			+= core gui

TARGET		= LibTest
TEMPLATE	= app

include (../../RicohFlags.pri)

SOURCES		+= main.cpp\
			ClMainWindow.cpp \
    ClAdaptRicohDeviceCtl.cpp

HEADERS		+= ClMainWindow.h \
    lib/5U87xCtl.h \
    ClAdaptRicohDeviceCtl.h

FORMS		+= ClMainWindow.ui

INCLUDEPATH	+= lib

win32:{
	LIBS		+=	lib/5U87xCtlx32.lib
}
