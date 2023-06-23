include(../libs.pri)
QT += multimedia
INCLUDEPATH += inc

SOURCES = ClStreamDecoder.cpp ../Common/CommonFunction.cpp ../Common/AvCommon.cpp
HEADERS = ClStreamDecoder.h ../Common/CommonFunction.h ../AvCommon.h
LIBS *= -lavutil -lavcodec -lavformat -lswscale -lavresample
