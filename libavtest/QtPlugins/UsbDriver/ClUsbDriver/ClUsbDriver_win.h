#ifndef USBDRIVER_WIN32_H
#define USBDRIVER_WIN32_H

#include "ClUsbDriver/ClUsbDriver_global.h"
#include "ClUsbDriver/ClUsbDriver_Struct.h"

#if defined(USBDRIVER_LIBRARY)
#  define USBDRIVERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define USBDRIVERSHARED_EXPORT Q_DECL_IMPORT
#endif

class ClDeviceInfo;
class ClImDrvCtrl;

Q_DECLARE_METATYPE(ulong);

//--------------------------------------------------------------------------------------
class ClThreadDeviceOpen : public QThread
{
	Q_OBJECT

public:
	ClThreadDeviceOpen(QWidget *parent, QMap<qint32, QString> map);
	virtual ~ClThreadDeviceOpen();
	virtual void run();

	virtual void stop(){ m_bStop = true; }

	virtual bool isSend(){ return m_bSend; }

signals:
	void sigOpenDevice(qint32 lProductId);

private slots:
	void slotReceive(qint32 lParam){ m_lReceive = lParam; }

protected:
	bool m_bStop;
	bool m_bSend;
	QMap<qint32, QString> m_mapTarget;
	qint32 m_lReceive;
};
//--------------------------------------------------------------------------------------
class ClUsbDriver : public QWidget
{
	Q_OBJECT

public:
	ClUsbDriver(QWidget *parent);
	virtual ~ClUsbDriver();

	virtual qint32 initialize();											//initialize

	virtual void setErrorLevel(qint32 val){ m_lErrLevel = val; }			//set error level
	virtual qint32 getErrorLevel(){ return m_lErrLevel; }					//get error level

	virtual void setEnableOtherCamera(bool bIsEnable){ m_bIsEnableOtherCamera = bIsEnable; }

protected:
	virtual bool isInitialize();											//return whether is it already initialized
	virtual bool isOpened(qint32 lProductId = 0);							//return whether is already opened
	virtual bool isStreaming(qint32 lProductId, eKINDSTREAM iKind);			//return whether is already streamed
	virtual qint32 openDevice(qint32 lProductId,
								QString  strDevPath);						//open for a device
	virtual void closeDevice(qint32 lProductId = 0);						//close for a device
	virtual qint32 startStream(qint32 lProductId, eKINDSTREAM iKind,qint32 iFrate, qint32 iQuality);
	virtual qint32 stopStream(qint32 lProductId, eKINDSTREAM iKind);
	virtual qint32 getPictureRect(qint32 lProductId, QSize &qSize);			//get width and height
	virtual qint32 getPictureSize(qint32 lProductId, qint32 &lSizeId);		//get lSizeId
	virtual qint32 getRectId(qint32 lProductId, eKINDRECTID &lRectId);		//get lRectId
	virtual qint32 setRectId(qint32 lProductId, eKINDRECTID lRectId);		//set lSizeId
	virtual qint32 execCameraControl(qint32 lProductId, qint32 command);	//execute camera control
	virtual qint32 captureJpeg(qint32 lProductId, QByteArray &qOutputJpgData, QVariant &qParam, eKINDRECTID lRectId);	//capture Jpeg
	virtual qint32 captureRaw(qint32 lProductId, QImage &qOutputRawData, QVariant &qParam, eKINDRECTID lRectId);		//capture Rawdata

	//matsubara add.
	virtual qint32 sendImgFileInfoV2(qint32 lProductId, QVariant &qParam);	//send image file info
	virtual qint32 sendFwFileInfoV2(qint32 lProductId, QVariant &qParam);	//send fw file info
	virtual qint32 uploadFileV2(qint32 lProductId, QVariant &qParam);		//upload file

	virtual QString chkAllDeviceInfo();
	virtual bool winEvent(MSG *message, long *result);						//receive window message
	virtual bool checkDevPath( QString  strDevPath, QUuid  clGuid );

	virtual bool checkDevPathOther( QString  strDevPath, QUuid  clGuid );
	virtual void cretaeThreadDeviceOpen();

protected:
	bool					m_bZeroImage;
	bool					m_bInitialized;
	qint32					m_lErrLevel;							//level of the error

	QMap<qint32, QUuid>		m_clProductList;						//list of ELMO product ID
	QMap<qint32, QObject*>	m_ImDrvCtrlList;						//list of ClImDrvCtrl,

	bool					m_bStreamState;							//0 : stop stream, 1 : start stream

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
    void sigGetStreamFail(qint32 lProductId, eKINDSTREAM iKind);	//get stream fail
	//get picture
	void sigRawImage(qint32 lProductId, qint32 lStatus, QImage clImage);		//is sent when received picture
	void sigJpegImage(qint32 lProductId, qint32 lStatus, QByteArray clAry);		//is sent when received picture
	void sigZeroCapture(qint32 lProductId);							//is sent when received slotCapture and cannot capture
	//firmware
	void sigFwUpdateProgress(qint32 lParam, qint32 lStatus);		//Fwアップデートの進捗状態を伝えるシグナル

	//uvc
	void sigSwitchCamera();											//カメラ切替を要求するシグナル

protected slots:
	//openclose
	void slotOpenDevice(qint32 lProductId);
	void slotCloseDevice(qint32 lProductId);
	//get picture
	void slotStartStream(qint32 lProductId, eKINDSTREAM kind, qint32 lFrate, qint32 lQuality);				//is received when wanna start picture stream
	void slotStopStream(qint32 lProductId, eKINDSTREAM kind);				//is received when wanna stop picture stream
	void slotGetFRate(qint32 lProductId, eKINDSTREAM kind, qint32 &lFrate);	//is received when wanna get frame rate of stream
	void slotSetFRate(qint32 lProductId, eKINDSTREAM kind, qint32 lFrate);	//is received when wanna set frame rate of stream
	void slotGetQuality(qint32 lProductId, eKINDSTREAM kind, qint32 &lQuality);	//is received when wanna get image quality of stream
	void slotSetQuality(qint32 lProductId, eKINDSTREAM kind, qint32 lQuality);	//is received when wanna set image quality of stream
	void slotGetStreamParam(qint32 lProductId, eKINDSTREAM iKind, qint32 &lType, qint32 &lCompType, qint32 &lFrate);
	void slotSetStreamParam(qint32 lProductId, eKINDSTREAM iKind, qint32 lType, qint32 lCompType, qint32 lFrate);
	void slotCapture(qint32 lProductId, eKINDSTREAM kind, qint32 &lStatus, QVariant &qParam, eKINDRECTID lRectId);		//is received when wanna capture picture
	//picture info
	void slotGetPictureRect(qint32 lProductId, QSize &qSize);				//is received when wanna get width and height of picture
	void slotGetPictureSize(qint32 lProductId, qint32 &lPictSize);			//is received when wanna get sizeID of picture
	void slotGetRectId(qint32 lProductId, eKINDRECTID &lRectId);			//is received when wanna get width and height of picture
	void slotSetRectId(qint32 lProductId, eKINDRECTID lRectId);				//is received when wanna set sizeID of picture

	//V1 Command
	//presenter info
	void slotGetPresenterName(qint32 lProductId, QString &qName);			//is received when wanna get camera name
	void slotGetPresenterVersion(qint32 lProductId, QStringList &qVerlist);		//is received when wanna get camera version
	void slotGetPresenterList(QVariantList &qSendlist);								//is received when wanna get connect camera list
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

	//V2 Command
	//camera control
	void slotZoomV2(qint32 lProductId, qint32 command);						//is received when wanna control zoom
	void slotSetVariantZoomV2(qint32 lProductId, qint32 lParam);			//is received when wanna control zoom set zoom pos
	void slotGetZoomStatusV2(qint32 lProductId, qint32 &lParam);			//is received when wanna control zoom get zoom pos
	void slotFocusV2(qint32 lProductId, qint32 command);					//is received when wanna control focus
	void slotSetVariantFocusV2(qint32 lProductId, qint32 lParam);			//is received when wanna control focus set focus
	void slotGetFocusStatusV2(qint32 lProductId, qint32 &lParam);			//is received when wanna control focus get foces
	void slotSetAFModeV2(qint32 lProductId, qint32 command);				//is received when wanna control focus set af mofe
	void slotGetAFModeStatusV2(qint32 lProductId, qint32 &lParam);			//is received when wanna control focus get af mode
	void slotIrisV2(qint32 lProductId, qint32 command);						//is received when wanna control iris
	void slotSetVariantIrisV2(qint32 lProductId, qint32 lParam);			//is received when wanna control iris set bright
	void slotGetIrisStatusV2(qint32 lProductId, qint32 &lParam);			//is received when wanna control iris get bright
	void slotSetBRTModeV2(qint32 lProductId, qint32 command);				//is received when wanna control iris set bright mode
	void slotGetBRTModeStatusV2(qint32 lProductId, qint32 &lParam);			//is received when wanna control iris get bright mode
	//file upload
	void slotSendImgFileInfoV2(qint32 lProductId,qint32 command,QVariant &qParam);
	void slotSendFwFileInfoV2(qint32 lProductId,qint32 command,QVariant &qParam);
	void slotUploadFileV2(qint32 lProductId,qint32 command,QVariant &qParam);
	//presenter info
	void slotGetPresenterNameV2(qint32 lProductId, QString &qName);							//is received when wanna get camera name for V2
	void slotGetPresenterVersionV2(qint32 lProductId, QStringList &qVerlist);				//is received when wanna get sh version for V2
	void slotGetPresenterSHVersionV2(qint32 lProductId, QString &qVersion);					//is received when wanna get sh version for V2
	void slotGetPresenterLensVersionV2(qint32 lProductId, QString &qVersion);				//is received when wanna get lens version for V2
	void slotGetPresenterFPGAVersionV2(qint32 lProductId, QString &qVersion);				//is received when wanna get fpga version for V2
	void slotGetProductIdV2(qint32 lProductId, QByteArray &lParam);							//is received when wanna get product id
	void slotGetCurrentModeV2(qint32 lProductId, QStringList &lParam);						//is received when wanna get current mode
	//extend
	void slotGetResSwV2(qint32 lProductId, qint32 &lParam);
	void slotGetImgOutSwV2(qint32 lProductId, qint32 &lParam);
	void slotGetCapPushFlgV2(qint32 lProductId, qint32 &lParam);
	void slotSetSystemTimeV2(qint32 lProductId, QDateTime &lParam);							//is received when wanna set system time
	void slotSetModelNameV2(qint32 lProductId, QByteArray lParam);							//is received when wanna set model name
	void slotSetProductCodeV2(qint32 lProductId, QByteArray lParam);						//is received when wanna set product code
	void slotWhiteBalanceV2(qint32 lProductId, qint32 command);
	void slotGetWhiteStatusV2(qint32 lProductId, qint32 &lParam);
	void slotFlickerV2(qint32 lProductId, qint32 command);
	void slotGetFlickerStatusV2(qint32 lProductId, qint32 &lParam);
	void slotImageModeV2(qint32 lProductId, qint32 command);
	void slotGetImageModeStatusV2(qint32 lProductId, qint32 &lParam);
	void slotEdgeV2(qint32 lProductId, qint32 command);
	void slotGetEdgeStatusV2(qint32 lProductId, qint32 &lParam);
	void slotGammaV2(qint32 lProductId, qint32 command);
	void slotGetGammaStatusV2(qint32 lProductId, qint32 &lParam);
	void slotFreezeV2(qint32 lProductId, qint32 command);
	void slotGetFreezeStatusV2(qint32 lProductId, qint32 &lParam);
	void slotRotationV2(qint32 lProductId, qint32 command);
	void slotGetRotationStatusV2(qint32 lProductId, qint32 &lParam);
	void slotNegaPosiV2(qint32 lProductId, qint32 command);
	void slotGetNegaPosiStatusV2(qint32 lProductId, qint32 &lParam);
	void slotColorMonoV2(qint32 lProductId, qint32 command);
	void slotGetColorMonoStatusV2(qint32 lProductId, qint32 &lParam);
	void slotScopeV2(qint32 lProductId, qint32 command);
	void slotGetScopeStatusV2(qint32 lProductId, qint32 &lParam);
	void slotSetSpeakerStatusV2(qint32 lProductId, qint32 lParam);
	void slotGetSpeakerStatusV2(qint32 lProductId, QStringList &lParam);
	void slotVideoV2(qint32 lProductId, qint32 command);
	void slotGetVideoStatusV2(qint32 lProductId, qint32 &lParam);
	void slotLanguageV2(qint32 lProductId, qint32 command);
	void slotGetLanguageStatusV2(qint32 lProductId, qint32 &lParam);
	void slotGuideV2(qint32 lProductId, qint32 command);
	void slotGetGuideStatusV2(qint32 lProductId, qint32 &lParam);
	void slotSaveSettingV2(qint32 lProductId, qint32 lParam);
	void slotLoadSettingV2(qint32 lProductId, qint32 lParam);

	// -------------------------- //
	//
	//		For UVC
	//
	// -------------------------- //

	// リモコンのゲッタ
	qint32 slotGetRemoconWidget(qint32 lProductId, QWidget** pclDestRemoconWidget);

	// 解像度のゲッタ・セッタ
	qint32	slotGetSupportedResolutions(qint32 lProductId, QStringList&	clDestResolutions);
	qint32	slotGetCurrentResolution(qint32 lProductId, QString&		clResolution);
	qint32	slotSetCurrentResolution(qint32 lProductId, QString			clResolution);

	// フレームレートのゲッタ・セッタ
	qint32	slotSetCurrentFrameRate(qint32 lProductId, bool				bIsFrameRateMax);

	//External Input Command
	void slotExternalInputCmd(qint32 lProductId, QVariant qVariantReq,  QVariant &qVariantRes);
	void slotExternalInputCmdV2(qint32 lProductId, QVariant qVariantReq,  QVariant &qVariantRes);

	//Bus reset
	void slotBusReset(qint32 lProductId, qint32 command);

	void slotChkAllDevice();

	qint32 slotSetTranslator(qint32 lProductId);

	qint32 slotStartInputStream(qint32 lProductId);
	qint32 slotStopInputStream(qint32 lProductId);

	void slotReopenDevice(qint32 lProductId);

private slots:
	void slotStartZeroImage(qint32 lProductId, eKINDSTREAM kind);								//
	void slotEndZeroImage(qint32 lProductId, eKINDSTREAM kind);									//
    void slotGetStreamFail(qint32 lProductId, eKINDSTREAM kind);								//
	void slotTerminate( qint32  lProductId, eKINDSTREAM  eKind );

protected:
	QString		m_qStrSendFilePath;																//送信ファイルのパス記憶用
	QMap<qint32, QString>	m_mapUvcPathV;														//!<	検知したUVC系機体のDevicePath(映像)
	QMap<qint32, QString>	m_mapUvcPathA;														//!<	検知したUVC系機体のDevicePath(音声)

	QMap<qint32, QString>	m_mapConnectProduct;												//!<	検出した製品リスト(未接続)
	QMap<qint32, QString>	m_mapOpenProduct;													//!<	検出した製品リスト(接続済)
	qint32					m_lProductIdOther;													//!<	他社製品プロダクトID
	ClThreadDeviceOpen*		m_pclThreadDeviceOpen;												//!<	接続スレッド

	bool					m_bIsEnableOtherCamera;												//!<	他社カメラ対応の有効/無効
};
//--------------------------------------------------------------------------------------

#endif // USBDRIVER_WIN32_H
