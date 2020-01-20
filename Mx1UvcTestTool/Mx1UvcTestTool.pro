#QT += core gui widgets
QT += core gui widgets multimedia axcontainer

#ARCH=Win_vc2010
#include($(QTAVLIBROOT)/StreamDecoder.pri)
#include(Avlib/StreamDecoder.pri)
TARGET = Mx1UvcTestTool
INCLUDEPATH += . ../../ inc/
INCLUDEPATH += $(QTPLUGINSROOT)/include

SOURCES += main.cpp \
		widget.cpp \
#		widget/ClLiveWidget.cpp \
#		widget/ClWidgetDrawImage.cpp \
		uvc/ClUvcManager.cpp

HEADERS  += widget.h \
#		widget/ClLiveWidget.h \
#		widget/ClWidgetDrawImage.h \
		inc/XuControlLib.h \
		uvc/ClUvcManager.h

win32:{
	CONFIG(debug, debug|release){
		LIBS *= -L$(QTPLUGINSROOT)/lib -L$$PWD/lib -lstrmbasd -lxuctrld -lole32 -luser32
		LIBS += -L$(QTPLUGINSROOT)/lib -L$$PWD/lib -lStrmiids -lUvcDriverd -lUvcDriverTxLx1d -lUvcDriverMx1d
	}else{
		LIBS *= -L$(QTPLUGINSROOT)/lib -L$$PWD/lib -lstrmbase -lxuctrl -lole32 -luser32
		LIBS += -L$(QTPLUGINSROOT)/lib -L$$PWD/lib -lStrmiids -lUvcDriver -lUvcDriverTxLx1 -lUvcDriverMx1
	}
}
win32-msvc*{
	QMAKE_CXXFLAGS += /wd4819
}
