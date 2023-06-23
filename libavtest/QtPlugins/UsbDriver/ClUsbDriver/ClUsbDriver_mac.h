//--------------------------------------------------------------------------------//
/*!
	@file		ClUsbDriver_mac.h
	@brief		mac用USBドライバヘッダ
	@author		大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLUSBDRIVER_MAC_H
#define CLUSBDRIVER_MAC_H

#include <QWidget>
#include <QList>
#include <QVariant>

#include "ClUsbDriver/ClUsbDriver_global.h"

#include "IOKit/IOTypes.h"

#if 1 //elmo add
#include "UsbDriver/mac/libusb/libusb.h"
#endif

Q_DECLARE_METATYPE(ulong);

//--------------------------------------------------------------------------------//
/*!
	@brief		mac用USBドライバ
*/
//--------------------------------------------------------------------------------//
class ClUsbDriver : public QWidget
{
	Q_OBJECT

public:
	ClUsbDriver(QWidget *pclParent = NULL);
	virtual ~ClUsbDriver();

	virtual qint32 initialize();

	//内部用
	virtual void open(qint32 lProductId);
	virtual void close(qint32 lProductId);
	virtual void checkRestart();

#if 1 //elmo add
    qint32 checkDeviceAll();
#endif

protected:
	virtual bool isInitialize();
	virtual bool isOpened(qint32 lProductId = 0);
	virtual bool isStreaming(qint32 lProductId, eKINDSTREAM iKind);
	virtual qint32 openDevice(qint32 lProductId);
	virtual void closeDevice(qint32 lProductId = 0);
	virtual qint32 startStream(qint32 lProductId, eKINDSTREAM iKind, qint32 lFrate, qint32 lQuality);
	virtual qint32 stopStream(qint32 lProductId, eKINDSTREAM iKind);
	virtual qint32 captureJpeg(qint32 lProductId, QByteArray &clData, eKINDSTREAM eKind,
							   QVariant clQuality, eKINDRECTID eRectId = RECTID_PICTURE_DEFAULT);
	virtual qint32 captureRaw(qint32 lProductId, QImage &clData, eKINDSTREAM eKind,
							  QVariant clQuality, eKINDRECTID eRectId = RECTID_PICTURE_DEFAULT);
	virtual qint32 execCameraControl(qint32 lProductId, qint32 command);

private:
	virtual qint32 chkInitOpen(qint32 lProductId = 0);

signals:
	//エラー
	void sigErrorFatal(qint32 lError);
	void sigErrorNormal(qint32 lError);
	//再起動確認
	void sigCheckRestartCo10();
	//オープンクローズ
	void sigOpenDevice(qint32 lProductId);
	void sigCloseDevice(qint32 lProductId);
	//ストリーム
	void sigStartZeroImage(qint32 lProductId, eKINDSTREAM iKind);
	void sigEndZeroImage(qint32 lProductId, eKINDSTREAM iKind);
	void sigTerminate(qint32 lProductId, eKINDSTREAM iKind);
	//画像取得
	void sigRawImage(qint32 lProductId, qint32 lStatus, QImage clImage);
	void sigJpegImage(qint32 lProductId, qint32 lStatus, QByteArray clAry);
	void sigZeroCapture(qint32 lProductId);
	void sigGetStreamFail(qint32 lProductId, eKINDSTREAM eKind);

protected slots:
	//画像取得
	void slotStartStream(qint32 lProductId, eKINDSTREAM kind, qint32 lFrate, qint32 lQuality);	//is received when wanna start picture stream
	void slotStopStream(qint32 lProductId, eKINDSTREAM kind);				//is received when wanna stop picture stream
	void slotGetFRate(qint32 lProductId, eKINDSTREAM kind, qint32 &lFrate);	//is received when wanna get frame rate of stream
	void slotSetFRate(qint32 lProductId, eKINDSTREAM kind, qint32 lFrate);//is received when wanna set frame rate of stream
	void slotGetQuality(qint32 lProductId, eKINDSTREAM kind, qint32 &lQuality);
	void slotSetQuality(qint32 lProductId, eKINDSTREAM kind, qint32 lQuality);
	void slotCapture(qint32 lProductId, eKINDSTREAM kind, qint32 &lStatus, QVariant &qParam, eKINDRECTID lRectId);		//is received when wanna capture picture
	//カメラ情報
	void slotGetPresenterName(qint32 lProductId, QString &qName);			//is received when wanna get camera name
	void slotGetPresenterVersion(qint32 lProductId, QStringList &qVerlist);		//is received when wanna get camera version
	void slotGetPresenterList(QVariantList &qSendlist);								//is received when wanna get connect camera list
	//画像情報
	void slotGetPictureRect(qint32 lProductId, QSize &qSize);				//is received when wanna get width and height of picture
	void slotGetPictureSize(qint32 lProductId, qint32 &lPictSize);			//is received when wanna get sizeID of picture
	void slotGetRectId(qint32 lProductId, eKINDRECTID &lRectId);						//is received when wanna get width and height of picture
	void slotSetRectId(qint32 lProductId, eKINDRECTID lRectId);					//is received when wanna set sizeID of picture
	//カメラ操作
	void slotZoom(qint32 lProductId, qint32 command);						//is received when wanna control zoom
	void slotFocus(qint32 lProductId, qint32 command);						//is received when wanna control focus
	void slotIris(qint32 lProductId, qint32 command);						//is received when wanna control iris
	//拡張(L-1ex,SDC330)
	void slotRotate(qint32 lProductId, qint32 command);						//is received when wanna roatate picture
	void slotPosiNega(qint32 lProductId, qint32 command);					//is received when wanna set posi or nega
	void slotColorBw(qint32 lProductId, qint32 command);					//is received when wanna set color or bw
	void slotSlowShutter(qint32 lProductId, qint32 command);				//is received when wanna set slowshutter
	void slotImageMode(qint32 lProductId, qint32 command);					//is received when wanna set imagemode
	void slotMicroscopeMode(qint32 lProductId, qint32 command);				//is received when wanna set microscopemode
	//画像取得不可
	void slotStartZeroImage(qint32 lProductId, eKINDSTREAM eKind);
	void slotEndZeroImage(qint32 lProductId, eKINDSTREAM eKind);

	//V2
	//カメラ操作
	void slotZoomV2(qint32 lProductId, qint32 command){slotZoom(lProductId, command);}
	void slotSetVariantZoomV2(qint32 lProductId, qint32 lParam);
	void slotGetZoomStatusV2(qint32 lProductId, qint32 &lParam);
	void slotFocusV2(qint32 lProductId, qint32 command){slotFocus(lProductId, command);}

	//ファイルアップロード
	qint32 slotSendImgFileInfoV2(qint32 lProductId, qint32 command, QVariant &clParam);
	qint32 slotSendFwFileInfoV2(qint32 lProductId, qint32 command, QVariant &clParam);
	qint32 slotUploadFileV2(qint32 lProductId, qint32 command, QVariant &clParam);

#if 0
	void slotSetVariantFocusV2(qint32 lProductId, qint32 lParam){}
	void slotGetFocusStatusV2(qint32 lProductId, qint32 &lParam){}
	void slotSetAFModeV2(qint32 lProductId, qint32 lParam){}
	void slotGetAFModeStatusV2(qint32 lProductId, qint32 &lParam){}
	void slotIrisV2(qint32 lProductId, qint32 command){slotIris(lProductId, command);}
	void slotSetVariantIrisV2(qint32 lProductId, qint32 lParam){}
	void slotGetIrisStatus(qint32 lProductId, qint32 &lParam){}
	void slotSetBRTModeV2(qint32 lProductId, qint32 lParam){}
	void slotGetBRTModeV2(qint32 lProductId, qint32 &lParam){}
	//カメラ情報
	void slotGetPresenterNameV2(qint32 lProductId, QString &strName){}
	void slotGetPresenterVersionV2(qint32 lProductId, QStringList &clVerList){}
	void slotGetPresenterSHVersionV2(qint32 lProductId, QString &strVersion){}
	void slotGetPresenterLensVersionV2(qint32 lProductId, QString &strVersion){}
	void slotGetPresenterFPGAVersionV2(qint32 lProductId, QString &strVersion){}
	void slotGetProductIdV2(qint32 lProductId, QByteArray &clParam){}
	void slotGetCurrentModeV2(qint32 lProductId, QByteArray &clParam){}
	//拡張
	void slotGetResSWV2(qint32 lProductId, qint32 &lParam){}
	void slotGetImgOutSwV2(qint32 lProductId, qint32 &lParam){}
	void slotGetCapPushFlgV2(qint32 lProductId, qint32 &lParam){}
	void slotSetSystemTimeV2(qint32 lProductId, QDateTime &clParam){}
	void slotSetModelNameV2(qint32 lProductId, QByteArray clParam){}
	void slotSetProductCodeV2(qint32 lProductId, QByteArray clParam){}
	void slotWhiteBalanceV2(qint32 lProductId, qint32 command){}
	void slotGetWhiteStatusV2(qint32 lProductId, qint32 &lParam){}
	void slotFlickerV2(qint32 lProductId, qint32 command){}
	void slotGetFlickerStatusV2(qint32 lProductId, qint32 &lParam){}
	void slotImageModeV2(qint32 lProductId, qint32 command){}
	void slotGetImageModeStatusV2(qint32 lProductId, qint32 &lParam){}
	void slotEdgeV2(qint32 lProductId, qint32 command){}
	void slotGetEdgeStatusV2(qint32 lProductId, qint32 &lParam){}
	void slotGammaV2(qint32 lProductId, qint32 command){}
	void slotGetGammaStatusV2(qint32 lProductId, qint32 &lParam){}
	void slotFreezeV2(qint32 lProductId, qint32 command){}
	void slotGetFreezeStatusV2(qint32 lProductId, qint32 &lParam){}
	void slotRotationV2(qint32 lProductId, qint32 command){}
	void slotGetRotationStatusV2(qint32 lProductId, qint32 &lParam){}
	void slotNegaPosiV2(qint32 lProductId, qint32 command){}
	void slotGetNegaPosiStatusV2(qint32 lProductId, qint32 &lParam){}
	void slotColorMonoV2(qint32 lProductId, qint32 command){}
	void slotGetColorMonoStatusV2(qint32 lProductId, qint32 &lParam){}
	void slotScopeV2(qint32 lProductId, qint32 command){}
	void slotGetScopeStatusV2(qint32 lProductId, qint32 &lParam){}
	void slotSetSpeakerStatusV2(qint32 lProductId, qint32 command){}
	void slotGetSpeakerStatusV2(qint32 lProductId, QVariant &clParam){}
	void slotVideoV2(qint32 lProductId, qint32 command){}
	void slotGetVideoStatusV2(qint32 lProductId, qint32 &lParam){}
	void slotLanguageV2(qint32 lProductId, qint32 command){}
	void slotGetLanguageStatusV2(qint32 lProductId, qint32 &lParam){}
	void slotGuideV2(qint32 lProductId, qint32 command){}
	void slotGuideStatusV2(qint32 lProductId, qint32 &lParam){}
	void slotSaveSettingV2(qint32 lProductId, QByteArray clParam){}
	void slotLoadSettingV2(qint32 lProductId, QByteArray &clParam){}
#endif

	// ----------------------------------------------------------- //
	//								UVC
	// ------------------------------------------------------------//
	// リモコン
	qint32 slotGetRemoconWidget(qint32 lProductId, QWidget** pclDestRemoconWidget);
	//解像度
	qint32	slotGetSupportedResolutions(qint32 lProductId, QStringList&	clDestResolutions);
	qint32	slotGetCurrentResolution(qint32 lProductId, QString& clResolution);
	qint32	slotSetCurrentResolution(qint32 lProductId, QString clResolution);

private:
	qint32 setValueCommand(qint32 lProductId, qint32 lCommand, QVariant clParam);
	qint32 getValueCommand(qint32 lProductId, qint32 lCommand, QVariant &clParam);

protected:
	bool m_bIsInitialized;			//!< 初期化済みフラグ
	bool m_bZeroImage;				//!< 画像取得不可フラグ
	QList<qint32> m_clProductList;	//!< 接続可能なプロダクトIDリスト
	QMap<qint32, QObject*> m_ImDrvCtrlList;	//!< ClImDrvCtrlリスト

	QString m_qStrSendFilePath;
};
//--------------------------------------------------------------------------------//
#endif
