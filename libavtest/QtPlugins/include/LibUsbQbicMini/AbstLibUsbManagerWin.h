//--------------------------------------------------------------------------------//
/*!
	@file	AbstLibUsbManagerWin.h
	@brief	Windows用libusb通信管理クラスヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef ABSTLIBUSBMANAGERWIN_H
#define ABSTLIBUSBMANAGERWIN_H
#include "CmnDefine.h"
#include <QThread>
#include <QQueue>
#include <QPair>
#include <QEventLoop>
#include <QTimer>
#include <QMutex>

class ClLibUsbController;
class ClWidgetReceiveNotifyEvent;

//--------------------------------------------------------------------------------//
/*!
	@brief	Windows用libusb通信管理クラス
*/
//--------------------------------------------------------------------------------//
class AbstLibUsbManager : public QThread
{
	Q_OBJECT

public:
	AbstLibUsbManager(QObject *pParent = NULL);	//コンストラクタ
	virtual ~AbstLibUsbManager();	//デストラクタ

	virtual bool initialize();	//初期化
	virtual bool isInitialized();	//初期化済みか取得
	virtual bool canCommunication();	//通信可能か取得
	void addCommand(EL_CMD cmd);	//コマンド追加
	virtual QList<ClLibUsbController*> getControllerList(){return m_listController;}	//!< 通信クラスリスト取得

protected:
	virtual void run();	//メイン処理
	virtual QVector<QPair<quint16, quint16> > getSupportVendorProduct() = 0;	//!< 対象ベンダ・プロダクトリスト取得
	virtual int getConnectWaitTime(){return 0;}	//!< 接続検出から処理開始までの待ち時間(msec)
	virtual void closeDevice();	//デバイスクローズ
	virtual int execPrivateCmd(QUuid idCmd);	//内部コマンド実行
	virtual void sendOpen(ClLibUsbController *pController);	//オープン通知
	ClLibUsbController *getController(EL_CMD cmd);	//通信クラス取得

	/*!
		@brief		コマンド追加確認
		@param[in]	cmd : 追加しようとしているコマンド
		@retval		追加するならばtrue
	*/
	virtual bool isAddCommand(EL_CMD cmd) = 0;
	/*!
		@brief		コマンド処理
		@param[in]	cmd : コマンド
	*/
	virtual void procCommand(EL_CMD cmd) = 0;

private:
	bool initializePrivate();	//初期化

protected slots:
	void slotRemoveController(ClLibUsbController *pController);	//通信クラス削除
	void slotOpenDevice();	//デバイスオープン
	virtual void slotConnectWaitEnd();	//接続待ち終了

signals:
	/*!
		@brief		コマンド通知
		@param[in]	cmd : コマンド
	*/
	void sigCommand(EL_CMD cmd);

protected:
	bool m_bIsInitialized;	//!<　初期化済みフラグ
	ClWidgetReceiveNotifyEvent *m_pWidgetReceiveNotify;	//!< デバイス変更通知受信ウィジェット

	QList<ClLibUsbController*> m_listController;	//!< USB通信クラスリスト
	QQueue<QPair<EL_CMD, QEventLoop*> > m_listCmdIn;	//!< 処理待ち内部コマンド
	QQueue<EL_CMD> m_listCmd;	//!< 処理待ちコマンド
	QMap<QTimer*, ClLibUsbController*> m_listConnectWait;	//!<　接続通知待ちリスト
	QMutex m_clCmdListMutex;	//!< コマンドリストアクセス排他用
};
//--------------------------------------------------------------------------------//
#endif //ABSTLIBUSBMANAGERWIN_H
