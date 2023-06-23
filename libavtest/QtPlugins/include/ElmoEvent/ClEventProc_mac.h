//--------------------------------------------------------------------------------//
/*!
	@file	ClEventProc_mac.h
	@brief	イベント処理ヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLEVENTPROC_MAC_H
#define CLEVENTPROC_MAC_H
#include "ClElmoEvent.h"
#include <QObject>
#include <QEvent>
#include <QRect>

class ClInput;

//--------------------------------------------------------------------------------//
/*!
	@brief	イベント処理クラス
*/
//--------------------------------------------------------------------------------//
class ClEventProc : public QObject
{
	Q_OBJECT

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
	ClEventProc();	//コンストラクタ
	~ClEventProc();	//デストラクタ

public:
	static ClEventProc *getInstance();	//インスタンス生成
	virtual bool eventFilter(QObject *pclObject, QEvent *pclEvent);		//イベントフィルタ
	void addObject(QObject *pclObject, eElmoEventType eEventType);		//オブジェクト追加
	void removeObject(QObject *pclObject, eElmoEventType eEventType);	//オブジェクト削除
	void clearObject();								//オブジェクトリスト削除

	/*!
		@brief	マウスイベント使用設定取得
		@retval	入力イベントをマウスイベントで処理する場合はtrue
	*/
	bool getUseMouseEvent(){return m_bIsUseMouseEvent;}
	void setUseMouseEvent(bool bIsUse);	//マウスイベント使用設定

protected:
	virtual bool initialize();	//初期化
	bool hasObject(QObject *pclObject);	//オブジェクトを持っているか確認

protected slots:
	void slotEventProc(QEvent *pclEvent);//内部イベント処理

protected:
	bool m_bIsInitialized;				//!< 初期化フラグ
	QList<stObjectData> m_clObjectList;	//!< イベント通知するオブジェクトリスト
	ClInput *m_pclInput;				//!< 入力処理クラス
	bool m_bIsUseMouseEvent;			//!< マウスイベントで入力処理フラグ
};
//--------------------------------------------------------------------------------//
#endif //CLEVENTPROC_MAC_H
