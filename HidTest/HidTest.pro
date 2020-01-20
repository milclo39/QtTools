#-------------------------------------------------
#
# Project created by QtCreator 2018-04-12T10:09:02
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HidTest
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS

LIBS += -lhidapi

HEADERS += widget.h\
		hidapi.h\
		ClHidThread.h

SOURCES += main.cpp\
		widget.cpp\
		ClHidThread.cpp
