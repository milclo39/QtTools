#ifndef CLIMDRVCTRL_H
#define CLIMDRVCTRL_H

#include "ClUsbDriver/ClUsbDriver_global.h"
#include "ClUsbDriver/ClUsbDriver_linux.h"
#include "linux/ClDeviceInfo.h"
#include "ClUsbCmd.h"

//--------------------------------------------------------------------------------------
class ClSourceImageThread : public QThread
{
	Q_OBJECT

public:
	ClSourceImageThread(QObject *parent, ClImDrvCtrl *a_devctrl);
	void setFrate(qint32 lRate);					//set framerate
	qint32 getFrate();								//get framerate
	void setRectId(eKINDRECTID iKind);				//set Rect ID
	eKINDRECTID getRectId();						//get Rect ID

protected:
	bool					m_bTerminate;			//Terminate flag
	bool					m_bZeroImage;
	void run();										//main

	eKINDRECTID				m_iRectId;				//
	QVariantList			m_qQueue;				//
	ClImDrvCtrl				*m_pDevCtrl;			//control of device
	double					m_dSleepTime;			//adjust time for framerate
signals:
	void sigSourceImage(qint32 lProductId, qint32 lStatus, QVariantList queue);				//is sent when received picture
	void sigStartZeroImage(qint32 lProductId);		//is sent when received zero size picture
	void sigEndZeroImage(qint32 lProductId);		//is sent when terminate thread
	void sigTerminate(qint32 lProductId);			//is sent when terminate thread
public slots:
	void slotLoopTerminate();						//Signal is end of loop
};

//--------------------------------------------------------------------------------------
class ClJpegImageThread : public QThread
{
	Q_OBJECT

public:
	ClJpegImageThread(QObject *parent);
	void setFrate(qint32 lRate);					//set framerate
	qint32 getFrate();								//get framerate

protected:
	bool					m_bTerminate;			//Terminate flag
	void run();										//main

	bool					m_bUpdate;
	double					m_dSleepTime;			//adjust time for framerate
	qint32					m_lProductId;			//
	QByteArray				m_qJpegArray;			//
	qint32					m_lStatus;				//
signals:
	void sigJpegImage(qint32 lProductId, qint32 lStatus, QByteArray clAry);				//is sent when received picture
	void sigStartZeroImage(qint32 lProductId, eKINDSTREAM iStream);		//is sent when received zero size picture
	void sigEndZeroImage(qint32 lProductId, eKINDSTREAM iStream);		//is sent when terminate thread
	void sigTerminate(qint32 lProductId, eKINDSTREAM iStream);				//is sent when terminate thread
public slots:
	void slotLoopTerminate();						//Signal is end of loop
	void slotSourceImage(qint32 lProductId, qint32 lStatus, QVariantList queue);
	void slotStartZeroImage(qint32 lProductId);
	void slotEndZeroImage(qint32 lProductId);
};

//--------------------------------------------------------------------------------------
class ClRawImageThread : public QThread
{
	Q_OBJECT

public:
	ClRawImageThread(QObject *parent);
	void setFrate(qint32 rate);						//set framerate
	qint32 getFrate();								//get framerate

protected:
	bool					m_bTerminate;			//Terminate flag
	void run();										//main

	bool					m_bUpdate;
	QByteArray				m_qJpegArray;			//!< JPEGデータ格納用
	double					m_dSleepTime;			//adjust time for framerate
	qint32					m_lProductId;			//
	QImage					m_qImage;				//
	qint32					m_lStatus;				//
	QMutex					m_clMutex;				//!< JPEGデータ保護用Mutex

signals:
	void sigRawImage(qint32 lProductId, qint32 lStatus, QImage clImage);				//is sent when received picture
	void sigStartZeroImage(qint32 lProductId, eKINDSTREAM iStream);		//is sent when received zero size picture
	void sigEndZeroImage(qint32 lProductId, eKINDSTREAM iStream);		//is sent when terminate thread
	void sigTerminate(qint32 lProductId, eKINDSTREAM iStream);			//is sent when terminate thread
public slots:
	void slotLoopTerminate();						//Signal is end of loop
	void slotSourceImage(qint32 lProductId, qint32 lStatus, QVariantList queue);
	void slotStartZeroImage(qint32 lProductId);
	void slotEndZeroImage(qint32 lProductId);

};
//--------------------------------------------------------------------------------------
class ClImDrvCtrl : public QObject
{
	Q_OBJECT
public:
	ClImDrvCtrl();
	explicit ClImDrvCtrl(qint32 _productId, libusb_context *ctx,
						 QObject *parent = 0);
	virtual ~ClImDrvCtrl();

	virtual qint32 initialize();						//initialize
	virtual bool isInitialize();						//return whether is it already initialized
	virtual bool OpenDriver();
	virtual bool CloseDriver();
	virtual bool IsOpenDriver();
	virtual qint32 CommandPresenter(qint32 CmdID, QVariant &Parameter);

	virtual bool isStreaming(eKINDSTREAM iKind);		//return whether is already streamed
	virtual qint32 startStream(ClUsbDriver *pclUsbDriver,eKINDSTREAM iKind,qint32 iFrate);
	virtual qint32 stopStream(eKINDSTREAM iKind);

	virtual void setFrameRate(qint32 lRate, eKINDSTREAM iKind);
	virtual qint32 getFrameRate(eKINDSTREAM iKind);

	virtual void setDriverPath(QString strPath){ m_strDriverPath = strPath; }	//set driver path for myself
	virtual QString getDriverPath(){ return m_strDriverPath; }					//return my driver path

	virtual QString chkAllDeviceInfo();
	virtual void setContext(libusb_context *val){ m_ctx = val; }	//set context
	virtual libusb_context *getContext(){ return m_ctx; }			//get context

	virtual qint32 getProductId(){ return m_lProductId; }			//
protected:

	bool					m_bOpen;				//this flag shows an opened dvice
	qint32					m_lProductId;			//Number of Product ID
	ClSourceImageThread		*m_pclSrcThread;		//
	ClRawImageThread		*m_pclRawThread;		//picture receive thread
	ClJpegImageThread		*m_pclJpegThread;		//picture receive thread

	libusb_context			*m_ctx;					//Context for errors
	ClDeviceInfo			*m_pclUseDevice;		//Just now be using device
	QMutex					m_Mutex;				//
	QMutex					m_DelMutex;				//
	bool					m_bInitialized;
	QString					m_strDriverPath;		//my driver path

	virtual void processSleep(quint32 seconds);		//
public slots:
	void slotSourceTerminate(qint32 lProductId);						//
};
//--------------------------------------------------------------------------------------
class ClWebCamCtrl : public ClImDrvCtrl
{
	Q_OBJECT
public:
	ClWebCamCtrl();
	explicit ClWebCamCtrl(qint32 _productId, libusb_context *ctx,
						 QObject *parent = 0);
	virtual ~ClWebCamCtrl();

	virtual bool OpenDriver();
};
//--------------------------------------------------------------------------------------
#endif // CLIMDRVCTRL_H
