#ifndef CLIMDRVCTRL_H
#define CLIMDRVCTRL_H

#include "ClDeviceInfo_mac.h"
#include "ClUsbDriver/ClUsbDriver_mac.h"
#include "ClUsbDriver_global.h"
#include "ClUsbCmd.h"
#include "ClUsbCmdV2.h"

#define		ELUSB_WT_USBPACKET_LENGTH	(512)		//512バイト境界
#define		ELUSB_WT_SENDLENGTH_ADJUST	(32)		//512バイト境界 対策

#if 1
#define		SEND_SAME_IMGDATA						//同じ画像データっぽくても送る場合は有効にする
#endif

//--------------------------------------------------------------------------------------
class ClImDrvCtrl;
class ClSourceImageThread : public QThread
{
	Q_OBJECT

public:
	ClSourceImageThread(QObject *parent, ClImDrvCtrl *a_devctrl);
	void setFrate(qint32 lRate);					//set framerate
	qint32 getFrate();								//get framerate
	void setQuality(qint32 lQuality){m_lImgQuality = lQuality;}
	qint32 getQuality(){return m_lImgQuality;}
	void setRectId(eKINDRECTID iKind);				//set Rect ID
	eKINDRECTID getRectId();						//get Rect ID
	qint32 getStreamingImage(QVariant &clParameter);

protected:
	bool					m_bTerminate;			//Terminate flag
	bool					m_bZeroImage;
    void run();										//main

	eKINDRECTID				m_iRectId;				//
	QByteArray				m_qData;				//
	ClImDrvCtrl				*m_pDevCtrl;			//control of device
	double					m_dSleepTime;			//adjust time for framerate
	qint32					m_lImgQuality;			//画質

	qint32					m_lLoopCount;			//画像取得失敗ループカウンタ

signals:
	void sigSourceImage(qint32 lProductId, qint32 lStatus, QByteArray data);				//is sent when received picture
	void sigStartZeroImage(qint32 lProductId);		//is sent when received zero size picture
	void sigEndZeroImage(qint32 lProductId);		//is sent when terminate thread
	void sigTerminate(qint32 lProductId);			//is sent when terminate thread
	void sigGetStreamFail(qint32 lProductId);
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
	void sigGetStreamFail(qint32 lProductId, eKINDSTREAM eKind);
public slots:
	void slotLoopTerminate();						//Signal is end of loop
	void slotSourceImage(qint32 lProductId, qint32 lStatus, QByteArray data);
	void slotStartZeroImage(qint32 lProductId);
	void slotEndZeroImage(qint32 lProductId);
	void slotGetStreamFail(qint32 lProductId);
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

#ifndef SEND_SAME_IMGDATA
	bool					m_bIsUpdateImg;			//新しい画像データが格納されたかを確認するフラグ
#endif

signals:
	void sigRawImage(qint32 lProductId, qint32 lStatus, QImage clImage);				//is sent when received picture
	void sigStartZeroImage(qint32 lProductId, eKINDSTREAM iStream);		//is sent when received zero size picture
	void sigEndZeroImage(qint32 lProductId, eKINDSTREAM iStream);		//is sent when terminate thread
	void sigTerminate(qint32 lProductId, eKINDSTREAM iStream);			//is sent when terminate thread
	void sigGetStreamFail(qint32 lProductId, eKINDSTREAM eKind);
public slots:
	void slotLoopTerminate();						//Signal is end of loop
	void slotSourceImage(qint32 lProductId, qint32 lStatus, QByteArray data);
	void slotStartZeroImage(qint32 lProductId);
	void slotEndZeroImage(qint32 lProductId);
	void slotGetStreamFail(qint32 lProductId);
};
//--------------------------------------------------------------------------------------
class ClSendFileThread : public QThread
{
	Q_OBJECT

public:
	ClSendFileThread(QObject *parent, ClImDrvCtrl *a_devctrl);
	void setFileName(QString qStrFileName);
	void stop();
protected:
	void run();
signals:
	void sigFwUpdateProgress(qint32 lParam, qint32 lStatus);					//Fwアップデートの進捗状態を伝えるシグナル
protected:
	ClImDrvCtrl				*m_pDevCtrl;			//control of device
	bool					m_bStopFlg;
	qint32					m_lImgLength;
	QByteArray				m_qArrayFile;
};
//--------------------------------------------------------------------------------------
class ClImDrvCtrl : public QObject
{
	Q_OBJECT
public:
	ClImDrvCtrl();
	explicit ClImDrvCtrl(qint32 _productId, QObject *parent = 0);
	virtual ~ClImDrvCtrl();

	virtual qint32 initialize();						//initialize
	virtual bool isInitialize();						//return whether is it already initialized
	virtual bool OpenDriver();
	virtual bool CloseDriver();
	virtual bool IsOpenDriver();
	virtual qint32 CommandPresenter(qint32 CmdID, QVariant &Parameter);

	virtual bool isStreaming(eKINDSTREAM iKind);		//return whether is already streamed
	virtual qint32 startStream(ClUsbDriver *pclUsbDriver,eKINDSTREAM iKind,qint32 iFrate, qint32 lQuality);
	virtual qint32 stopStream(eKINDSTREAM iKind);

	virtual void setFrameRate(qint32 lRate, eKINDSTREAM iKind);
	virtual qint32 getFrameRate(eKINDSTREAM iKind);

	virtual void setQuality(qint32 lQuality, eKINDSTREAM eKind);
	virtual qint32 getQuality(eKINDSTREAM eKind);

	virtual void setDriverPath(QString strPath){ m_strDriverPath = strPath; }	//set driver path for myself
	virtual QString getDriverPath(){ return m_strDriverPath; }					//return my driver path

	virtual QString chkAllDeviceInfo();

	virtual qint32 getProductId(){ return m_lProductId; }			//

	virtual qint32 getStreamingImage(QVariant &clParam);

	virtual qint32 startSendFile(ClUsbDriver *pclUsbDriver, QVariant &qParam);

	// ----------------------------------------------------------- //
	//								UVC
	// ------------------------------------------------------------//
	virtual QWidget*		getRemoconWidget();
	virtual void			setTranslator();
	virtual QStringList		getSupportedResolutions();
	virtual QString			getCurrentResolution();
	virtual qint32			setCurrentResolution(QString strResolution);

protected:
	virtual void processSleep(quint32 seconds);		//

public slots:
	void slotSourceTerminate(qint32 lProductId);						//

protected:
	qint32					m_lProductId;			//Number of Product ID
	ClSourceImageThread		*m_pclSrcThread;		//
	ClRawImageThread		*m_pclRawThread;		//picture receive thread
	ClJpegImageThread		*m_pclJpegThread;		//picture receive thread

	ClSendFileThread		*m_pclSendFileThread;	//send file thread

	ClDeviceInfo			*m_pclUseDevice;		//Just now be using device
	QMutex					m_Mutex;				//
	QMutex					m_DelMutex;				//
	bool					m_bInitialized;
	QString					m_strDriverPath;		//my driver path
};
#endif // CLIMDRVCTRL_H
