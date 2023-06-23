#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include "CmnDefine.h"
#include "ClUsbDriver/ClUsbDriver.h"
#include "ClUsbDriver/Command_Presenter.h"

#include <QtGui>
#include "Common/ElmoStruct.h"
#include "Common/ElmoStructV2.h"

#include "livedialog.h"

#include <QAudioOutput>

#define AUDIO_INFO_FORMAT_QT	"audio/pcm"	//!< Qtでのフォーマット宣言


//--------------------------------------------------------------------------------//
/*!
	@brief		メイン画面
*/
//--------------------------------------------------------------------------------//

namespace Ui {
    class MainWindow;
}
class ClAudioThread;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);
	ClUsbDriver *usb;
	qint32 m_lProductId;
private:
    Ui::MainWindow *ui;

private:
	 //matsubara add.
	 QTime			m_clTimerStreamRaw;
	 QTime			m_clTimerStreamJpg;
	 qint32			m_lCnt;

	 QByteArray		m_qArraySound;

	 QAudioOutput	*m_pclAudioOutput;
	 QIODevice		*m_pclAudioStream;

	 LiveDialog		*m_pclLiveDialog;

public slots:
	//button slot
	//line 1
	void slotPushStartStreamRaw(){
		m_lCnt = 0;
		m_clTimerStreamRaw.restart();
		emit sigStartStream(m_lProductId, RAW_STREAM, 30, 80);
	}
	void slotPushStopStreamRaw(){emit sigStopStream(m_lProductId, RAW_STREAM);}
	void slotPushStartStreamJpeg(){
		m_lCnt = 0;
		m_clTimerStreamJpg.restart();
		emit sigStartStream(m_lProductId, JPEG_STREAM, 30, 80);									//ストリームスレッド1 開始シグナル
	}
	void slotPushStopStreamJpeg(){
		qDebug("slotPushStopStreamJpeg");
		emit sigStopStream(m_lProductId, JPEG_STREAM);										//ストリームスレッド1 終了シグナル
		QFile file("sound");																//ファイル保存
		if(file.open(QFile::ReadWrite))
		{
			file.write(m_qArraySound);
			file.close();
		}
		m_qArraySound.clear();
	}
	void slotPushGetFrameRateRaw()
	{
		qint32 lFrate = 0;
		emit sigGetFrameRate(m_lProductId, RAW_STREAM, lFrate);
		if(lFrate != 0){
			qDebug() << lFrate;
		}else{
			qDebug() << "sigGetFrameRate RAW error";
		}
	}
	void slotPushSetFrameRateRaw(){emit sigSetFrameRate(m_lProductId, RAW_STREAM, 2);}
	void slotPushGetFrameRateJpeg()
	{
		qint32 lFrate = 0;
		emit sigGetFrameRate(m_lProductId, JPEG_STREAM, lFrate);
		if(lFrate != 0){
			qDebug() << lFrate;
		}else{
			qDebug() << "sigGetFrameRate Jpeg error";
		}
	}
	void slotPushSetFrameRateJpeg(){emit sigSetFrameRate(m_lProductId, JPEG_STREAM, 2);}
	void slotPushCaptureRaw()
	{
		QVariant qParam;
		qint32 lStatus = 0;
		emit sigCapture(m_lProductId, RAW_STREAM, lStatus, qParam, RECTID_PICTURE_DEFAULT);
		if(lStatus == IM_ERROR_SUCCESS){
			if(qParam.type() == QVariant::Image){
				qDebug() << "get to the Raw picture";
			}else{
				qDebug() << "sigCapture RAW error";
			}
		}else if(lStatus == IM_ERROR_CANNOT_OUTPUT_IMAGE){
			qDebug() << "sigCapture SD MODE NOW!";
		}
	}
	void slotPushCaptureJpeg()
	{
		QVariant qParam;
		qint32 lStatus = 0;
		emit sigCapture(m_lProductId, JPEG_STREAM, lStatus, qParam, RECTID_PICTURE_DEFAULT);
		if(lStatus == IM_ERROR_SUCCESS){
			if(qParam.type() == QVariant::ByteArray){
				qDebug() << "get to the Jpeg picture";

				//save jpg capture image
				QByteArray qArrayJpg(qParam.toByteArray().constData(), qParam.toByteArray().length());
				QFile file("testV1.jpg");
				if(file.open(QFile::WriteOnly))
				{
					file.write(qArrayJpg);
					file.close();
				}
			}else{
				qDebug() << "sigCapture Jpeg error";
			}
		}else if(lStatus == IM_ERROR_CANNOT_OUTPUT_IMAGE){
			qDebug() << "sigCapture SD MODE NOW!";
		}
	}
	//line 2
	void slotPushModelName()
	{
		QString strName;
		emit sigPresenterName(m_lProductId, strName);
		if(strName.isEmpty() == false){
			qDebug() << strName;
		}else{
			qDebug() << "sigPresenterName error";
		}
	}
	void slotPushModelVersion()
	{
		QStringList qVerlist;
		emit sigPresenterVersion(m_lProductId, qVerlist);
		if(qVerlist.count() != 0){
			for(qint32 i = 0; i < qVerlist.count(); i++ ){
				qDebug() << qVerlist[i];
			}
		}else{
			qDebug() << "didn't get some version strings";
		}
	}
	void slotPushGetPresenterList()
	{
		QVariantList qSendlist;
		emit sigPresenterList(qSendlist);
		if(qSendlist.count() != 0){
			for(qint32 i = 0; i < qSendlist.count(); i++ ){
				qDebug() << qSendlist[i];
			}
		}else{
			qDebug() << "didn't get some version strings";
		}
	}
	void slotPushGetRect()
	{
		QSize qSize;
		emit sigGetRect(m_lProductId, qSize);
		qDebug() << qSize;
	}
	void slotPushGetSize()
	{
		qint32 lSize;
		emit sigGetSize(m_lProductId, lSize);
		qDebug() << lSize;
	}
	void slotPushSetRect();

	//line 3
	void slotPushZoomTele(){emit sigZoom(m_lProductId, ZOOM_TELE);}
	void slotPushZoomWide(){emit sigZoom(m_lProductId, ZOOM_WIDE);}
	void slotPushZoomStop(){emit sigZoom(m_lProductId, ZOOM_STOP);}
	void slotPushIrisBright(){emit sigIris(m_lProductId, IRIS_OPEN);}
	void slotPushIrisDark(){emit sigIris(m_lProductId, IRIS_CLOSE);}
	void slotPushIrisStop(){emit sigIris(m_lProductId, IRIS_STOP);}
	void slotPushIrisNormal(){emit sigIris(m_lProductId, IRIS_INITIALIZE);}
	void slotPushFocusNear(){emit sigFocus(m_lProductId, FOCUS_NEAR);}
	void slotPushFocusFar(){emit sigFocus(m_lProductId, FOCUS_FAR);}
	void slotPushFocusStop(){emit sigFocus(m_lProductId, FOCUS_STOP);}
	void slotPushFocusAuto(){emit sigFocus(m_lProductId, FOCUS_AUTO);}

	//line 4
	void slotPushRotateOn(){emit sigRotate(m_lProductId, PARAM_ROTATION_ON);}
	void slotPushRotateOff(){emit sigRotate(m_lProductId, PARAM_ROTATION_OFF);}
	void slotPushPosi(){emit sigPosiNega(m_lProductId, PARAM_POSI);}
	void slotPushNega(){emit sigPosiNega(m_lProductId, PARAM_NEGA);}
	void slotPushColor(){emit sigColorBw(m_lProductId, PARAM_COLOR);}
	void slotPushBw(){emit sigColorBw(m_lProductId, PARAM_BW);}
	void slotPushSHOn(){emit sigSlowShutter(m_lProductId, PARAM_SLOW_SHUTTER_ON);}
	void slotPushSHOff(){emit sigSlowShutter(m_lProductId, PARAM_SLOW_SHUTTER_OFF);}
	void slotPushImageMode();
	void slotPushMSOff(){emit sigMicroscopeMode(m_lProductId, PARAM_MICROSCOPE_OFF);}
	void slotPushMSOn(){emit sigMicroscopeMode(m_lProductId, PARAM_MICROSCOPE_ON);}

	//openclose
	void slotOpenDevice(qint32 lProductId);
	void slotCloseDevice(qint32 lProductId);

	//stream
	void slotRawImage(qint32 lProductId, qint32 lStatus, QImage qImage);
	void slotJpegImage(qint32 lProductId, qint32 lStatus, QByteArray Ary);

	//matsubara add. ------------------------------------------------------------------------------
	void slotPushCaptureRawV2();
	void slotPushCaptureJpegV2();
	void slotPushStartStreamRawV2();
	void slotPushStopStreamRawV2();
	void slotPushStartStreamJpegV2();
	void slotPushStopStreamJpegV2();
	void slotRefUploadFileV2();
	void slotPushSendFileInfoV2();
	void slotPushUploadJpgV2();

	void slotPushMyCmdV1();
	void slotPushMyCmdV2();
	void slotPushSetSysTime();

	void slotPushClearBtn();
	//--------------------------------------------------------------------------------------------

	void slotPushGetV2();

signals:
	//stream
	void sigStartStream(qint32 lProductId, eKINDSTREAM kind, qint32 lFrate, qint32 lQuality);
	void sigStopStream(qint32 lProductId, eKINDSTREAM kind);
	void sigSetFrameRate(qint32 lProductId, eKINDSTREAM kind, qint32 lFrate);
	void sigGetFrameRate(qint32 lProductId, eKINDSTREAM kind, qint32 &lFrate);
	void sigCapture(qint32 lProductId, eKINDSTREAM kind, qint32 &lStatus, QVariant &qParam, eKINDRECTID lRectId);

	//info
	void sigPresenterName(qint32 lProductId, QString &qName);
	void sigPresenterVersion(qint32 lProductId, QStringList &qVerlist);
	void sigPresenterList(QVariantList &qSendlist);
	void sigGetRect(qint32 lProductId, QSize &qSize);
	void sigGetSize(qint32 lProductId, qint32 &lPictSize);
	void sigGetRectId(qint32 lProductId, eKINDRECTID &lRectId);
	void sigSetRect(qint32 lProductId, eKINDRECTID lRectId);

	//camera control
	 void sigZoom(qint32 lProductId, qint32 command);
	 void sigIris(qint32 lProductId, qint32 command);
	 void sigFocus(qint32 lProductId, qint32 command);
	 void sigRotate(qint32 lProductId, qint32 command);
	 void sigPosiNega(qint32 lProductId, qint32 command);
	 void sigColorBw(qint32 lProductId, qint32 command);
	 void sigSlowShutter(qint32 lProductId, qint32 command);
	 void sigImageMode(qint32 lProductId, qint32 command);
	 void sigMicroscopeMode(qint32 lProductId, qint32 command);

	 //matsubara add.
	 void sigSendUploadFileInfoV2(qint32 lProductId, qint32 command, QVariant &qParam);
	 void sigUploadJpgFileV2(qint32 lProductId, qint32 command, QVariant &qParam);
	 void sigExternalInputCmd(qint32 lProductId, QVariant qVariantReq,QVariant &qVariantRes);
	 void sigExternalInputCmdV2(qint32 lProductId, QVariant qVariantReq,QVariant &qVariantRes);
	 void sigSetSystemTimeV2(qint32 m_lProductId, QDateTime &qDateTime);

	 void sigStreamImg(QByteArray qArray);

	 void sigGetV2(qint32 lProductId, qint32 &lParam);
};

#endif // MAINWINDOW_H
