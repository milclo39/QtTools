QT *= multimedia
INCLUDEPATH += $$PWD/StreamDecoder $$PWD/Common $$PWD/libav
CONFIG(debug, debug|release){
LIBS += -L$$PWD/lib -lStreamDecoderd
}
else{
LIBS += -L$$PWD/lib -lStreamDecoder
}
LIBS += -L$$PWD/libav/libs/Win_vc2010 -lavutil -lavcodec -lavformat -lswscale -lavresample
