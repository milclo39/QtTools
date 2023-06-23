#ifndef CLIMDRVCTRL_H
#define CLIMDRVCTRL_H

#include "ClUsbDriver_global.h"
#include "ClUsbDriver/ClUsbDriver_win.h"
#include "ClDeviceInfo_win.h"
#include "ClUsbCmd.h"
#include "ClUsbCmdV2.h"
#include "ClUsbDriver/ClUsbDriver_Struct.h"

#define		ELUSB_WT_USBPACKET_LENGTH	(512)		//512バイト境界
#define		ELUSB_WT_SENDLENGTH_ADJUST	(32)		//512バイト境界 対策

#if 1		//ストリーム未使用の場合は有効にする
#define		STREAM_CMD_SPLT							//従来通りキャプチャで画像取得する場合は有効にする
#endif

#if !defined(STREAM_CMD_SPLT)
#include <QAudioOutput>
#include <QSemaphore>
#include "Common/ElmoStructV2.h"
#define		AUDIO_INFO_FORMAT_QT	"audio/pcm"		//
#define		TIME_SLEEP_FOR_RETRY_SOUND	(1)			//音声データ書き込みリトライ時のsleep時間(msec)
#endif

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
	void setQuality(qint32 lQuality);				//set image quality
	qint32 getQuality();							//get image quality
	void setRectId(eKINDRECTID iKind);				//set Rect ID
	eKINDRECTID getRectId();						//get Rect ID
	void setStreamParam(qint32 lType, qint32 lCompType, qint32 lFrate);
	qint32 getStreamSoundType();
	qint32 getStreamImageType();
	qint32 getStreamFrate();

	qint32 m_lStreamSoundType;
	qint32 m_lStreamImageType;
	qint32 m_lStreamFRate;

protected:
	bool					m_bTerminate;			//Terminate flag
	bool					m_bZeroImage;
	void run();										//main

	eKINDRECTID				m_iRectId;				//
	QVariantList			m_qQueue;				//
	ClImDrvCtrl				*m_pDevCtrl;			//control of device
	double					m_dSleepTime;			//adjust time for framerate
	qint32					m_lImgQuality;			//画像品質

#if !defined(STREAM_CMD_SPLT)
	//音声出力
	QAudioOutput		*m_pclAudioOutput;			//	音声出力
	QIODevice			*m_pclAudioStream;			//	音声データ
	QSemaphore			m_clSemSleep;				//	sleep動作用セマフォ

protected:
	void				sleep( quint32  ulMsec );	//	セマフォsleep
#endif

public slots:
	qint32 slotGetStreamImg(QVariant& qParameter);	//ストリームで取得した画像を送る
signals:
	void sigSourceImage(qint32 lProductId, qint32 lStatus, QVariantList queue);	//is sent when received picture
	void sigStartZeroImage(qint32 lProductId);									//is sent when received zero size picture
	void sigEndZeroImage(qint32 lProductId);									//is sent when terminate thread
	void sigTerminate(qint32 lProductId);										//is sent when terminate thread
	void sigGetStreamFail(qint32 lProductId);									//get stream fail
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
	void sigJpegImage(qint32 lProductId, qint32 lStatus, QByteArray clAry);	//is sent when received picture
	void sigStartZeroImage(qint32 lProductId, eKINDSTREAM iStream);			//is sent when received zero size picture
	void sigEndZeroImage(qint32 lProductId, eKINDSTREAM iStream);			//is sent when terminate thread
	void sigTerminate(qint32 lProductId, eKINDSTREAM iStream);				//is sent when terminate thread
	void sigGetStreamFail(qint32 lProductId, eKINDSTREAM iStream);			//get stream faill
public slots:
	void slotLoopTerminate();						//Signal is end of loop
	void slotSourceImage(qint32 lProductId, qint32 lStatus, QVariantList queue);
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
	void setFrate(qint32 lRate);					//set framerate
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
	void sigRawImage(qint32 lProductId, qint32 lStatus, QImage clImage);	//is sent when received picture
	void sigStartZeroImage(qint32 lProductId, eKINDSTREAM iStream);			//is sent when received zero size picture
	void sigEndZeroImage(qint32 lProductId, eKINDSTREAM iStream);			//is sent when terminate thread
	void sigTerminate(qint32 lProductId, eKINDSTREAM iStream);				//is sent when terminate thread
	void sigGetStreamFail(qint32 lProductId, eKINDSTREAM iStream);			//get stream fail

public slots:
	void slotLoopTerminate();						//Signal is end of loop
	void slotSourceImage(qint32 lProductId, qint32 lStatus, QVariantList queue);
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
	virtual bool OpenDriver(QString strDevPath = QString());
	virtual bool CloseDriver();
	virtual bool IsOpenDriver();
	virtual qint32 CommandPresenter(qint32 CmdID, QVariant &Parameter);

	virtual bool isStreaming(eKINDSTREAM iKind);		//return whether is already streamed
	virtual qint32 startStream(ClUsbDriver *pclUsbDriver,eKINDSTREAM iKind,qint32 iFrate, qint32 iQuality);
	virtual qint32 stopStream(eKINDSTREAM iKind);

	virtual qint32 startInputStream();
	virtual qint32 stopInputStream();

	virtual void setFrameRate(qint32 lRate, eKINDSTREAM iKind);
	virtual qint32 getFrameRate(eKINDSTREAM iKind);

	virtual void setQuality(qint32 lQuality, eKINDSTREAM iKind);
	virtual qint32 getQuality(eKINDSTREAM iKind);

	virtual void setDriverPath(QString strPath){ m_strDriverPath = strPath; }	//set driver path for myself
	virtual QString getDriverPath(){ return m_strDriverPath; }					//return my driver path

	virtual QString chkAllDeviceInfo();

	virtual qint32 getProductId(){ return m_lProductId; }						//

	virtual qint32 getStreamImg(QVariant& qParameter);							//ストリーム中の場合にストリームで取得した画像をとりに行く関数 V1

	virtual qint32 startSendFile(ClUsbDriver *pclUsbDriver, QVariant &qParam);	//ファイル送信スレッド

	virtual void setStreamParam(eKINDSTREAM iKind, qint32 lType, qint32 lCompType, qint32 lFrate);
	virtual void getStreamParam(eKINDSTREAM iKind, qint32 &lType, qint32 &lCompType, qint32 &lFrate);

	virtual QWidget*		getRemoconWidget();
	virtual void			setTranslator();

	virtual QStringList		getSupportedResolutions();
	virtual QString			getCurrentResolution();
	virtual qint32			setCurrentResolution(QString strResolution);

	virtual qint32			setCurrentFrameRate(bool bIsFrameRateMax);

	virtual void			reopen();

protected:
	bool					m_bOpen;				//this flag shows an opened dvice
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

	qint32					m_lStreamSoundType;
	qint32					m_lStreamImageType;
	qint32					m_lStreamFRate;

	virtual void processSleep(quint32 seconds);		//

signals:
	void sigFwUpdateProgress(qint32 lParam, qint32 lStatus);			//Fwアップデートの進捗状態を伝えるシグナル

public slots:
	void slotSourceTerminate(qint32 lProductId);						//
};
#endif // CLIMDRVCTRL_H
