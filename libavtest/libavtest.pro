QT += quick multimedia xml core gui qml
greaterThan(QT_MAJOR_VERSION, 4):QT += widgets
CONFIG += c++11
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += $$PWD/QtPlugins/include
INCLUDEPATH += $$PWD/QtAvLibraries

QMAKE_LIBDIR +=	$$PWD/QtPlugins/lib
QMAKE_LIBDIR += $$PWD/QtAvLibraries/StreamDecoder/lib
#QMAKE_LIBDIR += $$PWD/QtAvLibraries/libav/libs/Win_vc2013/x86_64
QMAKE_LIBDIR += $$PWD/QtAvLibraries/libav/libs/Win_vc2019

CONFIG(debug, debug|release){
    QMAKE_LIBDIR += $$PWD/QtAvLibraries/lib/x86/debug
#    LIBS += -lStrmiids -lUvcDriverd -lUvcDriverMx1d -lUvcDriverPx1d -lUvcDriverMo1d
    LIBS += -lClUsbDriverd -lQtdxutTimerd
}
else{
    QMAKE_LIBDIR += $$PWD/QtAvLibraries/lib/x86/release
#    LIBS += -lStrmiids -lUvcDriver -lUvcDriverMx1 -lUvcDriverPx1 -lUvcDriverMo1
    LIBS += -lClUsbDriver -lQtdxutTimer
}

LIBS += -lStreamDecoder
LIBS += -lavutil -lavcodec -lavformat -lswscale -lavresample
LIBS += -lsetupapi -lPdh -lwinmm -ladvapi32 -lOle32 -lOleAut32 -lUser32

HEADERS += \
    ClL12Presenter.h \
    ClStreamView.h \
    ClCamDevice.h \
    ClLogger.h \
    ClVisualPresenter.h \
    ClUsbCamScanner.h \
    ClCamFilter.h \
    ClCamScanner.h \
    AbsCamScanner.h \
    ClCamScannerFactory.h \
    ClDocCamFilter.h \
    CmnValue.h

SOURCES += \
    ClL12Presenter.cpp \
    ClStreamView.cpp \
    ClCamDevice.cpp \
    ClVisualPresenter.cpp \
    ClUsbCamScanner.cpp \
    ClCamFilter.cpp \
    ClCamScanner.cpp \
    AbsCamScanner.cpp \
    ClCamScannerFactory.cpp \
    ClDocCamFilter.cpp \
    main.cpp

RESOURCES += qml.qrc

win32{
    DEFINES += WIN32
    LIBS += -lWs2_32 -lShell32
    QMAKE_CXXFLAGS += /wd4819
}

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
