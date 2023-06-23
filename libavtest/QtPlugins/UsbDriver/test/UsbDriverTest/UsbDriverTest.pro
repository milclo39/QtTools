# -------------------------------------------------
# Project created by QtCreator 2010-06-09T13:39:56
# -------------------------------------------------
TARGET = UsbDriverTest
TEMPLATE = app
QT += network \
	multimedia
DEFINES += USBDRIVER_LIBRARY
linux-g++|linux-icc { 
	INCLUDEPATH += ../../core/linux
	LIBS += ../../../lib/libClUsbDriver.a \
		../../lib/linux32/libusb-1.0.a \
		../../../lib/libQtSingleApplication.a
}
mac: {
	CONFIG += x86
	LIBS += ../../../lib/libClUsbDriver.a \
			../../../lib/libQtSingleApplication.a \
			../../../lib/libQtdxutTimer.a \
			../../../lib/libRollOverToolBtn.a \
			../../../lib/libRectWidget.a
	LIBS += -framework IOKit \
		-framework CoreFoundation \
#		-framework VisualPresenter \
		-framework Cocoa \
		-framework QTKit \
		-framework QuartzCore
	LIBS += -framework MOCameraLib
}
win32: { 
	CONFIG(debug, debug|release):LIBS += ../../../lib/ClUsbDriverd.lib \
		../../../lib/QtSingleApplicationd.lib \
		../../../lib/QtdxutTimerd.lib \
		../../../lib/RollOverToolBtnd.lib \
		../../../lib/RectWidgetd.lib
	else:LIBS += ../../../lib/ClUsbDriver.lib \
		../../../lib/QtSingleApplication.lib \
		../../../lib/QtdxutTimer.lib \
		../../../lib/RollOverToolBtn.lib \
		../../../lib/RectWidget.lib
	LIBS += User32.lib \
		setupapi.lib \
		psapi.lib
	CONFIG(x64):{
		INCLUDEPATH += "C:/Program Files (x86)/Microsoft SDKs/Windows/v6.0A/Include"
		QMAKE_LIBDIR += "C:/Program Files (x86)/Microsoft SDKs/Windows/v6.0A/Lib"
	}else{
		INCLUDEPATH += "C:/Program Files/Microsoft SDKs/Windows/v6.0A/Include"
		QMAKE_LIBDIR += "C:/Program Files/Microsoft SDKs/Windows/v6.0A/Lib"
		LIBS += Ole32.lib \
				../../lib/win32/ELVDLib.lib
		DEFINES += OS_WIN_X32
	}
}
INCLUDEPATH += ../../core \
	../../../Common \
	../../../SingleApplication/src
SOURCES += main.cpp \
	mainwindow.cpp \
	livedialog.cpp
HEADERS += mainwindow.h \
	livedialog.h
FORMS += mainwindow.ui \
	livedialog.ui
