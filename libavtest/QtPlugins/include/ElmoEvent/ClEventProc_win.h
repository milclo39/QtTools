//--------------------------------------------------------------------------------//
/*!
	@file	ClEventProc_win.h
	@brief	イベント処理ヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLEVENTPROC_WIN_H
#define CLEVENTPROC_WIN_H
#include <QObject>
#include <QEvent>
#include <QWidget>
#include <QList>
#include <QPoint>
#include <QAbstractNativeEventFilter>
#include "ClElmoEvent.h"
#include "Windows.h"

class ClInput;


//--------------------------------------------------------------------------------//
/*!
	@defgroup	Qt5用イベント処理クラス
	@brief		Windowsイベントを処理する
*/
//--------------------------------------------------------------------------------//
class clWindowsEventFilter : public QAbstractNativeEventFilter
{
public:
	virtual bool nativeEventFilter(const QByteArray &eventType, void *pvMessage, long *plResult) Q_DECL_OVERRIDE;
};

//--------------------------------------------------------------------------------//
/*!
	@defgroup	groupWinEvent Windowsイベント処理
	@brief		Windowsイベントを処理する
*/
//--------------------------------------------------------------------------------//

bool EventFilter(void *pvMessage, long *result);	//イベント受信

//--------------------------------------------------------------------------------//
/*!
	@brief	Windowsイベント処理クラス
*/
//--------------------------------------------------------------------------------//
class ClEventProc : public QObject
{
	//------------------------------//
	/*!
		@brief	イベント通知するオブジェクト情報
	*/
	//------------------------------//
	typedef struct
	{
		QObject *pclObject;			//!< 通知するオブジェクト
		eElmoEventType eEventType;	//!< イベントの種類
	}stObjectData;

private:
	ClEventProc();		//コンストラクタ
	~ClEventProc();		//デストラクタ

protected:
	bool initialize();	//初期化
    bool hasObject(QObject *pclObject);	//オブジェクトを持っているか確認

public:
	virtual bool eventFilter(QObject *pclWatched, QEvent *pclEvent);	//イベントフィルタ
	bool isInitialized(){return m_bIsInitialize;}	//初期化済み確認
	static ClEventProc *getInstance();				//インスタンス生成
	void addObject(QObject *pclObject, eElmoEventType eEventType);		//オブジェクト追加
	void removeObject(QObject *pclObject, eElmoEventType eEventType);	//オブジェクト削除
	void clearObject();								//オブジェクトリスト削除
	bool eventProc(MSG *pstMsg, long *plResult);	//イベント処理
	void eventProc(QEvent *pclEvent);				//イベント処理

	/*!
		@brief	マウスイベント使用設定取得
		@retval	入力イベントをマウスイベントで処理する場合はtrue
	*/
	bool getUseMouseEvent(){return m_bIsUseMouseEvent;}
	void setUseMouseEvent(bool bIsUse);

	void setScreenInfoPri(QRect clRectScreen);				//プライマリモニタ情報設定
	void setScreenInfoSec(QRect clRectScreen);				//セカンダリモニタ情報設定
	void setSelectScreen(bool bIsSelectPri);				//プライマリモニタ選択判定フラグ
	bool getDetectElmoTablet(void);						//エルモタブレット検出状態取得

private:
	QObject *getTargetObject(eElmoEventType eEventType);	//イベントのターゲットオブジェクト取得

private:
	bool m_bIsInitialize;				//!< 初期化済みフラグ
	QList<stObjectData> m_clObjectList;	//!< イベント通知するオブジェクトリスト
	ClInput *m_pclInput;				//!< 入力処理クラス
	QObject *m_pclInputTarget;			//!< 入力イベント送信先
	bool m_bIsUseMouseEvent;			//!< マウスイベント使用フラグ
};
//--------------------------------------------------------------------------------//
#endif
