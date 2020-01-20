include(../libs.pri)
QT += multimedia
SOURCES = ClStreamDecoder.cpp ../Common/CommonFunction.cpp ../Common/ClPlayAudioThread.cpp ../Common/AvCommon.cpp
HEADERS = ClStreamDecoder.h ../Common/CommonFunction.h ../Common/ClPlayAudioThread.h ../AvCommon.h
LIBS *= -lavutil -lavcodec -lavformat -lswscale -lavresample
