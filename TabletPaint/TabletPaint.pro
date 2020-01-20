QT += core gui axcontainer multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += . \
		$(QTPLUGINSROOT)/include \
		$(QTPLUGINSROOT)/Common \
		IMACommand.h

HEADERS += widget.h \
		ClComponentRec.h \
    ClTabletPaint.h

SOURCES += main.cpp\
		widget.cpp \
		ClComponentRec.cpp \
    ClTabletPaint.cpp

win32:{
	QMAKE_LIBDIR += $(QTPLUGINSROOT)/lib
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
	LIBS += -lsetupapi -lPdh -lwinmm
	QMAKE_LIBDIR += $(QTPLUGINSROOT)/ippLib/win32/x86
}
