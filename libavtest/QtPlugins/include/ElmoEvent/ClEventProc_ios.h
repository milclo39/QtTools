//--------------------------------------------------------------------------------//
/*!
	@file	ClEventProc_ios.h
	@brief	イベント処理ヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLEVENTPROC_IOS_H
#define CLEVENTPROC_IOS_H
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
	bool getUseMouseEvent(){return true;}
	void setUseMouseEvent(bool bIsUse);	//マウスイベント使用設定 (iOSでは設定無効)

protected:
	virtual bool initialize();	//初期化
	bool hasObject(QObject *pclObject);	//オブジェクトを持っているか確認

protected:
	bool m_bIsInitialized;				//!< 初期化フラグ
	QList<stObjectData> m_clObjectList;	//!< イベント通知するオブジェクトリスト
};
//--------------------------------------------------------------------------------//
#endif //CLEVENTPROC_IOS_H
