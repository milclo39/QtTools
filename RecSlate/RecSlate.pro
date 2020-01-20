#-------------------------------------------------
#
# Project created by QtCreator 2015-07-23T13:15:01
#
#-------------------------------------------------

QT += core gui axcontainer multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RecSlate
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS
CONFIG += QAxContainer
INCLUDEPATH += . \
		$(QTPLUGINSROOT)/include \
		$(QTPLUGINSROOT)/Common \
		IMACommand.h

HEADERS += widget.h \
		cltabletcanvas.h \
		ClComponentRec.h

SOURCES += main.cpp\
		widget.cpp \
		cltabletcanvas.cpp \
		ClComponentRec.cpp

win32:{
	CONFIG(debug, debug|release){
		LIBS *= -L$(QTPLUGINSROOT)/lib -L$$PWD/lib
	}else{
		LIBS *= -L$(QTPLUGINSROOT)/lib -L$$PWD/lib
	}
	QMAKE_LIBDIR *= $(QTPLUGINSROOT)/lib
	CONFIG(debug, debug|release){
		LIBS += -lMP4Decoderd -lH264Encoderd -lQtdxutTimerd
	}
	else{
		LIBS += -lMP4Decoder -lH264Encoder -lQtdxutTimer
	}
	LIBS += -lumc_io -lmpeg4_mux -lvm -lvm_plus -lumc -lmedia_buffers -lmpeg4_spl \
			-laac_enc -laac_dec -lcolor_space_converter -lh264_enc -lh264_dec -lcommon -lspl_common
	LIBS += -lippvm_l -lippvc_l -lippsc_l -lippr_l -lippm_l -lippj_l -lippdi_l \
			-lippcv_l -lippch_l -lippcc_l -lippac_l -lippdc_l -lippi_l -lipps_l -lippcore_l
	win32-msvc2008 {
		QMAKE_LIBDIR += $(QTPLUGINSROOT)/ippLib/win32/msvc2008/x86
	}
	win32-msvc2010|win32-msvc2013 {
		QMAKE_LIBDIR += $(QTPLUGINSROOT)/ippLib/win32/msvc2010/x86
	}
	QMAKE_LIBDIR += $(QTPLUGINSROOT)/ippLib/win32/x86
}
