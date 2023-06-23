#ifndef USBDRIVER_H
#define USBDRIVER_H

#include "ClUsbDriver/ClUsbDriver_global.h"

#if defined(USBDRIVER_LIBRARY)
#  define USBDRIVERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define USBDRIVERSHARED_EXPORT Q_DECL_IMPORT
#endif


class ClDeviceInfo;
class ClImDrvCtrl;
class ClUevent;

//--------------------------------------------------------------------------------------
class USBDRIVERSHARED_EXPORT ClUsbDriver : public QWidget
{
Q_OBJECT

public:
	ClUsbDriver(QWidget *parent);
	virtual ~ClUsbDriver();

	virtual qint32 initialize();										//initialize

	virtual void setErrorLevel(qint32 val){ m_lErrLevel = val; }		//set error level
	virtual qint32 getErrorLevel(){ return m_lErrLevel; }				//get error level

protected:
	virtual bool isInitialize();										//return whether is it already initialized
	virtual bool isOpened(qint32 lProductId = 0);							//return whether is already opened
	virtual bool isStreaming(qint32 lProductId, eKINDSTREAM iKind);	//return whether is already streamed
	virtual qint32 openDevice(qint32 lProductId = 0);					//open for a device
	virtual void closeDevice(qint32 lProductId = 0);					//close for a device
	virtual qint32 startStream(qint32 lProductId, eKINDSTREAM iKind,qint32 iFrate);
	virtual qint32 stopStream(qint32 lProductId, eKINDSTREAM iKind);
	virtual qint32 getPictureRect(qint32 lProductId, QSize &qSize);				//get width and height
	virtual qint32 getRectId(qint32 lProductId, eKINDRECTID &lRectId);				//get width and height
	virtual qint32 getPictureSize(qint32 lProductId, qint32 &lSizeId);			//get SizeId
	virtual qint32 setRectId(qint32 lProductId, eKINDRECTID lRectId);				//set SizeId
	virtual qint32 execCameraControl(qint32 lProductId, qint32 command);		//execute camera control
	virtual qint32 captureJpeg(qint32 lProductId, QByteArray &qOutputJpgData, eKINDRECTID lRectId);	//capture Jpeg
	virtual qint32 captureRaw(qint32 lProductId, QImage &qOutputRawData, eKINDRECTID lRectId);		//capture Rawdata

	virtual QString chkAllDeviceInfo();							//
	//PnP functions for Linux
	virtual void chkConnectedDevice();							//
	virtual void chkConnectedDeviceWithSysfs();					//
#ifdef Q_OS_LINUX
	virtual void chkConnectedDeviceWithSysfs4video();			//
	virtual void chkAddVideoDevice(QString strMesg);				//
#endif
	virtual void chkRemoveVideoDevice(QString strMesg);				//
	virtual void chkAddConnectedDevice(QString strMesg);		//
	virtual void chkRemoveConnectedDevice(QString strMesg);		//
	virtual bool chkElmoProduction(qint32 lProductId);			//
	virtual bool setDevPermission(QString strMesg);				//
protected:
#define SYSFS_USB_PATH		"/sys/bus/usb/drivers/usb"
#define SYSFS_VIDEO_PATH	"/sys/class/video4linux"
#define DEV_USB_PATH		"/dev/bus/usb"
#define MAGIC_CORD			"mierumon"
#define PROCESS_TIMEOUT		3000
	bool					m_bZeroImage;
	bool					m_bInitialized;
	qint32					m_lPresenterId;						//using Presenter ID

	qint32					m_lErrLevel;						//level of the error
	libusb_context			*m_ctx;								//Context for errors
	QList<qint32>			m_clProductList;					//list of ELMO product ID
	QMap<qint32, QObject*>	m_ImDrvCtrlList;					//list of ClImDrvCtrl,
	ClUevent				*m_pclUevent;						//for Uevent receive thread

signals:
	//error
	void sigErrorFatal(qint32 lError);								//is sent when detect fatal error (wanna exit for this application)
	void sigErrorNormal(qint32 lError);								//is sent when detect normal error
	//openclose
	void sigOpenDevice(qint32 lProductId);							//is sent when device open
	void sigCloseDevice(qint32 lProductId);							//is sent when device close
	//stream
	void sigStartZeroImage(qint32 lProductId, eKINDSTREAM iKind);	//is sent when received zero size picture
	void sigEndZeroImage(qint32 lProductId, eKINDSTREAM iKind);		//is sent when received zero size picture
	void sigTerminate(qint32 lProductId, eKINDSTREAM iKind);		//is sent when terminate thread
	//get picture
	void sigRawImage(qint32 lProductId, qint32 lStatus, QImage clImage);			//is sent when received picture
	void sigJpegImage(qint32 lProductId, qint32 lStatus, QByteArray clAry);			//is sent when received picture
	void sigZeroCapture(qint32 lProductId);							//is sent when received slotCapture and cannot capture

	//for Uevent signal
	void sigUeventTerminate();												//this terminate signal is for Uevent thread

protected slots:
	//openclose
	void slotOpenDevice(qint32 /*lProductId*/){}
	void slotCloseDevice(qint32 /*lProductId*/){}
	//get picture
	void slotStartStream(qint32 lProductId, eKINDSTREAM kind, qint32 lFrate);	//is received when wanna start picture stream
	void slotStopStream(qint32 lProductId, eKINDSTREAM kind);				//is received when wanna stop picture stream
	void slotGetFRate(qint32 lProductId, eKINDSTREAM kind, qint32 &lFrate);	//is received when wanna get frame rate of stream
	void slotSetFRate(qint32 lProductId, eKINDSTREAM kind, qint32 lFrate);	//is received when wanna set frame rate of stream
	void slotCapture(qint32 lProductId, eKINDSTREAM kind, qint32 &lStatus, QVariant &qParam, eKINDRECTID lRectId);		//is received when wanna capture picture
	//presenter info
	void slotGetPresenterName(qint32 lProductId, QString &qName);			//is received when wanna get camera name
	void slotGetPresenterVersion(qint32 lProductId, QStringList &qVerlist);		//is received when wanna get camera version
	void slotGetPresenterList(QVariantList &qSendlist);								//is received when wanna get connect camera list
	//picture info
	void slotGetPictureRect(qint32 lProductId, QSize &qSize);				//is received when wanna get width and height of picture
	void slotGetPictureSize(qint32 lProductId, qint32 &lPictSize);			//is received when wanna get sizeID of picture
	void slotGetRectId(qint32 lProductId, eKINDRECTID &lRectId);						//is received when wanna get width and height of picture
	void slotSetRectId(qint32 lProductId, eKINDRECTID lRectId);					//is received when wanna set sizeID of picture
	//camera controll
	void slotZoom(qint32 lProductId, qint32 command);						//is received when wanna control zoom
	void slotFocus(qint32 lProductId, qint32 command);						//is received when wanna control focus
	void slotIris(qint32 lProductId, qint32 command);						//is received when wanna control iris
	//extend(L-1ex,SDC330)
	void slotRotate(qint32 lProductId, qint32 command);						//is received when wanna roatate picture
	void slotPosiNega(qint32 lProductId, qint32 command);					//is received when wanna set posi or nega
	void slotColorBw(qint32 lProductId, qint32 command);					//is received when wanna set color or bw
	void slotSlowShutter(qint32 lProductId, qint32 command);				//is received when wanna set slowshutter
	void slotImageMode(qint32 lProductId, qint32 command);					//is received when wanna set imagemode
	void slotMicroscopeMode(qint32 lProductId, qint32 command);				//is received when wanna set microscopemode

	void slotChkAllDevice();

	//for some Uevent slots
	void slotUeventFatal();							//receive fatal error from uevent thread
	void slotUeventTerminated();					//receive terminate event from uevent thread
	void slotUeventString(QString);					//receive kernel message from uevent thread

private slots:
	void slotStartZeroImage(qint32 lProductId, eKINDSTREAM kind);								//
	void slotEndZeroImage(qint32 lProductId, eKINDSTREAM kind);								//

};
//--------------------------------------------------------------------------------------

#endif // USBDRIVER_H
