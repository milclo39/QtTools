#-------------------------------------------------
#
# Project created by QtCreator 2010-05-24T18:24:44
#
#-------------------------------------------------

TARGET = ClUsbDriver
CONFIG += staticlib

QT += multimedia

DEFINES += USBDRIVER_LIBRARY
CONFIG(debug, debug|release){
        DEFINES += __DEBUG #__USB_DEBUG
}
INCLUDEPATH	+=	. \
                                ../Common \
                                ./Common \
								../include \
                                ../RectWidget \
                                ../RollOverToolBtn \
                                ./ClUsbDriver


HEADERS +=	../Common/CmnDefine.h \
                        ClUsbDriver/ClUsbDriver_Struct.h

linux:{
        CONFIG(debug, debug|release){
                mac:{
                        LIBS += ./lib/macx/libusb-1.0.a
                }else{
                        LIBS += ./lib/linux32/libusb-1.0.a
                }
                DEFINES += LIBUSB_ERR_LEVEL=3
        }else{
                mac:{
                        LIBS += ./lib/macx/libusb-1.0.a
                }else{
                        LIBS += -L/usr/local/libusb-1.0.8/lib -lusb-1.0
                }
                DEFINES += LIBUSB_ERR_LEVEL=0
        }
        INCLUDEPATH += linux

        SOURCES += linux/ClUsbDriver_linux.cpp \
								linux/ClDeviceInfo.cpp \
                                linux/ClPresenterL1ex.cpp \
                                linux/ClPresenterL1n.cpp \
                                linux/ClImDrvCtrl.cpp

        HEADERS += 	linux/ClDeviceInfo.h \
                                linux/ClPresenterL1ex.h \
                                linux/ClPresenterL1n.h \
                                linux/ClImDrvCtrl.h \
                                ClUsbDriver/ClUsbDriver_linux.h
}

linux-icc|linux-g++{
        SOURCES += linux/ClPresenterWebcam.cpp \
                                linux/ClUevent.cpp
        HEADERS += linux/ClPresenterWebcam.h \
                                linux/ClUevent.h
}

win32:{
        INCLUDEPATH += ./win32 \
                                        $(QTPLUGINSROOT)/RollOverToolBtn/core \
                                        $(QTPLUGINSROOT)/RectWidget/core
        SOURCES += win32/ClUsbDriver_win.cpp \
                                win32/ClDeviceInfo_win.cpp \
                                win32/ClPresenterL12.cpp \
                                win32/ClPresenterL1ex.cpp \
                                win32/ClPresenterL1n.cpp \
                                win32/ClImDrvCtrl.cpp \
                                win32/UsbDevice.cpp \
                                win32/ClPresenterUVC.cpp

        HEADERS += 	win32/ClDeviceInfo_win.h \
                                win32/ClPresenterL12.h \
                                win32/ClPresenterL1ex.h \
                                win32/ClPresenterL1n.h \
                                win32/ClImDrvCtrl.h \
                                win32/UsbDevice.h \
                                win32/IoDevice.h \
                                win32/Ioctl.h \
                                ClUsbDriver/ClUsbDriver_win.h \
                                win32/ClPresenterUVC.h

        contains(PLATFORM , x64)|contains(PLATFORM , x64_111){
                INCLUDEPATH += "C:/Program Files (x86)/Microsoft SDKs/Windows/v6.0A/Include"
                QMAKE_LIBDIR += "C:/Program Files (x86)/Microsoft SDKs/Windows/v6.0A/Lib"
        }
        contains(PLATFORM , x86)|isEmpty(PLATFORM){
                INCLUDEPATH += "C:/Program Files/Microsoft SDKs/Windows/v6.0A/Include"
                QMAKE_LIBDIR += "C:/Program Files/Microsoft SDKs/Windows/v6.0A/Lib"

                SOURCES += win32/ClPresenterCO10.cpp \
                                        win32/ClPresenterMO.cpp

                HEADERS += win32/ClPresenterCO10.h \
                                win32/ClPresenterMO.h

                LIBS += Ole32.lib \
                                ./lib/win32/ELVDLib.lib

                DEFINES += OS_WIN_X32

                #not use  ELVDLib.dll
#				no_elvdlib{
                {
                        SOURCES -=	win32/ClPresenterUVC.cpp \
                                                win32/ClPresenterCO10.cpp \
                                                win32/ClPresenterMO.cpp

                        HEADERS -=	win32/ClPresenterUVC.cpp \
                                                win32/ClPresenterCO10.h \
                                                win32/ClPresenterMO.h

                        LIBS -=		Ole32.lib \
                                                ./lib/win32/ELVDLib.lib

                        DEFINES += _NO_USE_ELVDLIB
                }

        }

        LIBS += User32.lib \
                        setupapi.lib \
                        psapi.lib \
                        shell32.lib \
                        $(QTPLUGINSROOT)/lib/RectWidget.lib \
                        $(QTPLUGINSROOT)/lib/RollOverToolBtn.lib

        DEFINES += UNICODE
}

mac:{
		INCLUDEPATH += mac

		SOURCES += mac/ClUsbDriver_mac.cpp \
								mac/ClPresenterL1n.cpp \
								mac/ClPresenterL1ex.cpp \
								mac/ClPresenterL12.cpp \
								mac/ClDeviceInfo_mac.cpp \
								mac/UsbDevice.cpp

		OBJECTIVE_SOURCES +=	mac/ClImDrvCtrl.mm \

		HEADERS += ClUsbDriver/ClUsbDriver_mac.h \
								mac/ClImDrvCtrl.h \
								mac/ClPresenterL1n.h \
								mac/ClPresenterL1ex.h \
								mac/ClPresenterL12.h \
								mac/ClDeviceInfo_mac.h \
								mac/UsbDevice.h

#        OBJECTIVE_HEADERS +=	mac/ClPresenterMO1Wrap.hpp

		#CO-10(i-Pochette)はMO-1と接続したときに問題がでるので対応機種からはずす
		#DEFINES += _USE_CO10
		contains(DEFINES, _USE_CO10){
			OBJECTIVE_SOURCES += mac/ClPresenterCo10.mm
			HEADERS += mac/ClPresenterCo10.h
		}

		#MO-1/MO-1wのMac対応は途中なので現状は除外する
		#MO-1対応では、frameworkのAppKit, QuickTime, QuartzCore, QTKitが必要になる
		#DEFINES += _USE_MO1
		contains(DEFINES, _USE_MO1){
			SOURCES += mac/ClPresenterMO1.cpp
			OBJECTIVE_SOURCES += mac/ClPresenterMO1Wrap.mm
			HEADERS += mac/ClPresenterMO1.h
			OBJECTIVE_HEADERS += mac/ClPresenterMO1Wrap.hpp
		}

		#not use  ELVDLib.dll
#		no_elvdlib{
		{
				SOURCES -=		mac/ClPresenterMO1.cpp

				OBJECTIVE_SOURCES -= mac/ClPresenterMO1Wrap.mm

				HEADERS -=		mac/ClPresenterMO1.h \
								mac/ClPresenterCo10.h

				OBJECTIVE_HEADERS -= mac/ClPresenterMO1Wrap.hpp

				DEFINES += _NO_USE_ELVDLIB
		}
}

        SOURCES += Common/ClUsbCmd.cpp \
                                Common/ClUsbCmdV2.cpp

        HEADERS += Common/ClUsbCmd.h \
                                Common/ClUsbCmdV2.h \
                                Common/ElmoUsbCmd.h \
                                Common/ElmoUsbCmdV2.h \
                                Common/ElmoStruct.h \
                                Common/ElmoStructV2.h \
                                ClUsbDriver/ClUsbDriver_global.h \
                                ClUsbDriver/ClUsbDriver_Errors.h

include (../Flags.pri)

linux:{
        libusbheader.files += linux/libusb/libusb.h
        libusbheader.path += $$INSTALL_INCLUDEDIR/libusb
        INSTALLS += libusbheader
}

RESOURCES += Remocon.qrc

TRANSLATIONS += UsbDriver_en.ts \
                                UsbDriver_ja.ts \
                                UsbDriver_ru.ts \
                                UsbDriver_zh.ts \
                                UsbDriver_fr.ts \
                                UsbDriver_de.ts
