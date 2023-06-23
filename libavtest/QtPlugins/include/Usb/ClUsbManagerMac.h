//--------------------------------------------------------------------------------//
/*!
	@file	ClUsbManagerMac.h
	@brief	Mac用USB通信クラスヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLUSBMANAGERMAC_H
#define CLUSBMANAGERMAC_H
#include "Command.h"
#include "CmnDefine.h"
#include "UsbCommon.h"
#include <QThread>
#include <QTimer>
#include <QMutex>
#include <QMap>

struct libusb_context;
struct libusb_device;
struct libusb_device_handle;
struct libusb_device_descriptor;
struct libusb_config_descriptor;
struct libusb_interface_descriptor;
struct libusb_endpoint_descriptor;

class ClUsbController;
class ClQbicCommand;
class ClQbicCommandDataReceive;
class ClQbicCommandDataSend;

#define QBIC_COM_TIMEOUT		1000	//通信タイムアウト時間

//--------------------------------------------------------------------------------//
/*!
	@brief	Mac用QBiCUSB通信クラス
*/
//--------------------------------------------------------------------------------//
class ClUsbManager : public QThread
{
	Q_OBJECT

public:
	ClUsbManager(QObject *pclParent = NULL);	//コンストラクタ
	virtual ~ClUsbManager();	//デストラクタ

	qint32 initialize(QList<stVendorProduct> clList);	//初期化
	bool isInitialized();	//初期化されたか取得
	bool canCommunication();	//通信可能か取得

	void setTimeoutDef(qint32 lTime){m_lTimeoutDef = lTime;}	//!< デフォ通信タイムアウト設定

protected:
	quint16 execQbicCommand(ClQbicCommand &clCommand, ClUsbController *pclTarget, qint32 lReadWaitTime);	//QBiCコマンド実行
	quint16 execQbicDataRead(ClQbicCommand &clCommand, ClQbicCommandDataReceive &clCommandReceive, ClUsbController *pclTarget);	//QBiCデータ読み込み実行
	quint16 execQbicDataWrite(ClQbicCommand &clCommand, ClQbicCommandDataSend &clCommandSend, ClUsbController *pclTarget);		//QBicデータ書き込み実行
	quint16 execQbicGetWifiInfo(ClUsbController *pclTarget, stWifiInfo *pstInfo);		//wi-fi情報取得
	quint16 execQbicGetAccesspointList(ClUsbController *pclTarget, QList<stAccesspointInfo> *pclList);	//アクセスポイントリスト取得
	quint16 execQbicGetScheduleSettings(ClUsbController *pclTarget, ScheduleSettings *pclList);	//スケジュール設定取得
	void sendCommunicationResult(void *pclController, QUuid clCommand, quint16 usError);	//通信結果通知

	bool openDevice();	//デバイスオープン
	void closeDevice();	//デバイスクローズ

	void sendDeviceInfo(libusb_device *pstDevice);	//デバイス情報送信
	QString getDeviceDescriptorInfo(libusb_device_descriptor *pstDescriptor);	//デバイスデスクリプタ情報文字列取得
	QString getConfigDescriptorInfo(libusb_config_descriptor *pstDescriptor, qint32 lNumber = -1);	//コンフィグデスクリプタ情報文字列取得
	QString getInterfaceDescriptorInfo(libusb_interface_descriptor *pstDescriptor, qint32 lNumber = -1);	//インターフェースデスクリプタ情報文字列取得
	QString getEndpointDescriptorInfo(libusb_endpoint_descriptor *pstDescriptor, qint32 lNumber = -1);	//エンドポイントデスクリプタ情報文字列取得

protected slots:
	void slotRemoveController(ClUsbController *pclController);	//コントローラ削除
	void slotReceiveCommand(EL_CMD clCommand);	//コマンド受信
	void slotDeviceSearch();	//デバイス検出
	void slotConnectWaitEnd();	//接続待ち終了

signals:
	/*!
		@brief		コマンド送信
		@param[in]	clCommand : コマンド
	*/
	void sigSendCommand(EL_CMD clCommand);

protected:
	QTimer *m_pclPollingTimer;	//!< ポーリング用タイマ
	bool m_bIsInitialized;	//!< 初期化済みフラグ
	QList<stVendorProduct> m_clVendorProductList;	//!< 使用ベンダ・プロダクトリスト
	QMutex m_clComMutex;	//!< 通信中検出用ミューテックス
	bool m_bIsEnd;	//!< 終了フラグ
	bool m_bIsWaitSendOpen;	//!< 接続通知待ち
	qint32 m_lTimeoutDef;	//!< デフォ通信タイムアウト

	libusb_context *m_pstUsbContext;				//!< libusbのコンテキスト
	libusb_device **m_ppstUsbDeviceList;			//!< デバイスリスト
	QList<ClUsbController*> m_clUsbControllerList;	//!< USB操作クラスリスト
	QMap<QTimer*, ClUsbController*> m_clConnectWaitList;	//!< 接続待ちリスト
};
//--------------------------------------------------------------------------------//
#endif //CLUSBMANAGERMAC_H
