//--------------------------------------------------------------------------------//
/*!
	@file	ClSQbicQr.h
	@brief	docomoQBiC用QRコード処理クラスヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef ClSQBICQR_H
#define ClSQBICQR_H
#include <QImage>

#define QBIC_QR_MAXBYTE		130		//エンコードできる最大バイト数(マジックコード分引く)

//--------------------------------------------------------------------------------//
/*!
	@brief	QBiC用QRコード処理クラス
*/
//--------------------------------------------------------------------------------//
class ClSQbicQr
{
public:
	//録画開始トリガ
	typedef enum
	{
		eRecStartRemote,	//遠隔
		eRecStartCamera		//本体
	}eRecTimeSetting;
	//動体検知モード
	typedef enum
	{
		eMotionDetectionCaution_DoorPhone,	//警戒、ドアホンモード
		eMotionDetectionWatch				//見守りモード
	}eMotionDetectionMode;

public:
	ClSQbicQr();	//コンストラクタ

	int getBufferSize(){return m_dataEncode.length();}	//!< エンコード元データサイズ取得

	//エンコード
	bool setTimeZone(int iValue);	//タイムゾーン設定
	bool setAutoPowerOff(int iSecond);	//自動電源OFF設定
	bool setLedStart(bool bValue);		//起動時LED設定
	bool setContrastOptimizer(bool bValue);	//明るさオプティマイザ使用設定
	bool setCameraName(QString strName);	//カメラ名設定
	bool setScheduleMode(int iValue);	//スケジュールモード設定
	bool setLockStart(bool bValue);	//起動時操作ロック設定
	bool setRecTimeMax(eRecTimeSetting eMode, int iMinute);	//最大録画時間設定
	bool setAlermTime(int iSecond);	//プレアラーム時間設定
	bool setMotionDetectionArea(const quint8 *pucData);	//動体検知エリア設定
	bool setMotionDetectionQuality(eMotionDetectionMode eMode, int iValue);	//動体検知感度設定
	bool setVolume(int iValue);	//音量設定
	bool setShutterSound(int iMode, int iValue);	//シャッター音設定
	bool setKillTimeQrMode(int iMinute);	//QRモード無操作終了時間設定
	bool setWatchInterval(int iMinute);	//見守りモードの見守り間隔設定
	bool setMotionDetectionInterval(int iSecond);	//ドアホンモード動体検知間隔設定
	bool setUploadUse(bool bValue);	//アップロード可否設定
	bool setUploadedFileDelete(bool bValue);	//アップロード後ファイル消す設定
	bool setRotateImage(bool bValue);	//180度回転設定
	bool setBatteryAlerm(bool bValue);	//バッテリーアラーム設定

	//wifi設定
	bool setWifiCountry(int iValue);	//wifi対応国設定
	bool setWifiStart(int iValue);		//起動時wifi設定
	bool setHttpPort(quint16 usPort);	//HTTPポート設定
	bool setAudioPort(quint16 usPort);	//音声上りポート設定
	bool setRtspPort(quint16 usPort);	//RTSPポート設定
	bool setPortFowarding(bool bValue);	//port fowarding使用設定
	bool setPortFowardingAudioPort(quint16 usPort);	//port fowarding用音声上りポート
	bool setInfraSsid(QString strSsid);	//インフラストラクチャSSID設定
	bool setInfraKey(QString strKey);	//インフラストラクチャ暗号キー設定
	bool setInfraDhcp(bool bValue);		//インフラストラクチャDHCP使用設定
	bool setInfraAddress(QString strAddress);	//インフラストラクチャアドレス設定
	bool setInfraSubnetmask(QString strMask);	//インフラストラクチャサブネットマスク設定
	bool setInfraDefaultGateway(QString strGateway);	//インフラストラクチャデフォルトゲートウェイ設定
	bool setAuthorization(bool bValue);	//認証使用設定
	bool setAuthorizationPassword(QString strPass);	//認証パスワード設定
	bool setDns1Address(QString strAddress);	//プライマリDNSサーバアドレス
	bool setDns2Address(QString strAddress);	//セカンダリDNSサーバアドレス
	bool setWifiMode(int iValue);

	//動画
	bool setQuarityPreset(int iValue);	//品質プリセット
	//録画
	bool setRecResolution(QSize size);	//録画解像度設定
	bool setRecFps(QSize sizeRec, int iValue);	//録画フレームレート設定
	bool setRecBps(QSize sizeRec, int iValue);	//録画ビットレート設定
	//配信
	bool setLiveResolution(QSize size);	//配信解像度設定
	bool setLiveFps(QSize sizeLive, int iValue);	//配信フレームレート設定
	bool setLiveBps(QSize sizeLive, int iValue);	//配信ビットレート設定

	//スケジュール周期
	bool setScheduleCycleMode(int iValue);	//周期モード設定
	bool setScheduleCycleCycle(int iMinute);	//撮影周期設定
	bool setScheduleCycleRecTime(int iSecond);	//撮影時間設定
	bool setScheduleCycleLed(bool bValue);	//LED設定
	//スケジュール時刻指定
	bool setScheduleTimeValid(int iNumber, bool bValue);	//有効設定
	bool setScheduleTime(int iNumber, int iMode, int iDay, QTime timeStart, QTime timeEnd, bool bLed);	//スケジュール設定

	QImage encode(int iPixelPerCel = 1);	//エンコード

protected:
	bool addEncodeData(quint8 ucKind, QByteArray data);	//エンコードデータ追加
	quint8 getScheduleTimeKind(int iNumber, int iKind);	//時間スケジュールの種別取得
	QByteArray convertBool(bool bValue);	//boolを設定値に変換
	int convertRecSize(QSize size);		//録画サイズを識別値に変換
	int convertLiveSize(QSize size);	//配信サイズを識別値に変換

protected:
	QByteArray m_dataEncode;	//!< エンコード元データ
};
//--------------------------------------------------------------------------------//
#endif //ClSQBICQR_H
