QT += core gui qml quick axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Drawing
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += main.cpp \
		Drawing.cpp
HEADERS += Drawing.h
DISTFILES += qml/*.qml

LIBS += -lDrawObjectd -lImageManagerd -lBackgroundd -lQtdxutTimerd \
		-lElmoEventd -lsetupapi
QMAKE_LIBDIR += $(QTPLUGINSROOT)/lib
INCLUDEPATH += $(QTPLUGINSROOT)/include
RESOURCES += Drawing.qrc
