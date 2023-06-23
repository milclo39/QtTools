CONFIG += staticlib

TARGET = RectWidget

INCLUDEPATH = .

HEADERS += RectWidget/ClRectWidget.h
SOURCES += ClRectWidget.cpp \
			ClRectWidget2.cpp

include (../Flags.pri)
