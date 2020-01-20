QT += qml quick widgets gui quickwidgets

include(AvLib/StreamDecoder.pri)
SOURCES += main.cpp \
    ClStreamView.cpp
HEADERS += \
    ClStreamView.h
DISTFILES += main.qml

win32:{
    CONFIG(debug, debug|release){
        LIBS *= -L$(QTPLUGINSROOT)/lib -L$$PWD/lib
    }else{
        LIBS *= -L$(QTPLUGINSROOT)/lib -L$$PWD/lib
    }
}
win32-msvc*{
    QMAKE_CXXFLAGS += /wd4819
}
