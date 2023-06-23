//--------------------------------------------------------------------------------//
/*!
	@file	ClPptControl.h
	@brief	パワーポイント操作クラスヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLPPTCONTROL_H
#define CLPPTCONTROL_H
#include <QObject>
#include "Common/CmnDefine.h"
#ifdef Q_OS_WIN
#include <QAxObject>
#endif //Q_OS_WIN

class ClPptEvent;
//--------------------------------------------------------------------------------//
/*!
	@brief	パワーポイント操作クラス
*/
//--------------------------------------------------------------------------------//
class ClPptControl : public QObject
{
	Q_OBJECT

public:
	static ClPptControl *getInstance();	//インスタンス取得
	static bool canUsePpt();	//パワーポイント使用できるか取得
	bool getCanUsePptSlideShow();	//スライドショーが使えるか取得

protected:
	ClPptControl();		//コンストラクタ
#ifdef Q_OS_WIN
	~ClPptControl();	//デストラクタ
	virtual bool initialize();	//初期化
	void initSlideshowSetting();	//スライドショー設定初期化
	QAxObject *getActiveSlideShow();	//アクティブなスライドショー取得
	void clearAxInstance();	//ActiveXインスタンス開放
#endif //Q_OS_WIN
public slots:
	void slotPptCommand(EL_CMD clCommand);	//パワーポイントコマンド受信
#ifdef Q_OS_WIN
private slots:
	void slotPptEvent(int iEvent, IDispatch *pDispatch);	//パワーポイントイベント通知
	void slotException(int iError, QString strSource, QString strDesc, QString strHelp);	//ActiveX例外通知
#endif //Q_OS_WIN
signals:
	void sigPptCommand(EL_CMD clCommand);	//パワーポイントコマンド通知
#ifdef Q_OS_WIN
protected:
	static bool ms_bCanUsePpt;	//!< パワーポイント使用可能フラグ
	static QAxObject *ms_pclApplication;	//!< アプリケーションクラス
	QList<QAxObject*> m_clPresentationList;	//!< スライドショー中のプレゼンテーションリスト
	QList<QAxObject*> m_clPresentationParentList;	//!< スライドショー中のプレゼンテーションの親リスト
	ClPptEvent *m_pclEvent;	//!< パワーポイントイベントクラス

	QAxObject *m_pclActivePresentation;	//!< 操作中のプレゼンテーション
	bool m_bIsSlideShowLoop;	//!< スライドショーループフラグ
	qint32 m_lRangeType;	//!< スライドショーレンジ(1:全スライド表示,2:区間指定,3:個別指定)
	qint32 m_lSlideBegin;	//!< 開始スライド番号
	qint32 m_lSlideEnd;	//!< 終了スライド番号
#endif //Q_OS_WIN
};
//--------------------------------------------------------------------------------//
#endif //CLPPTCONTROL_H
