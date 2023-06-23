//--------------------------------------------------------------------------------//
/*!
	@file	ClDrawScene.h
	@brief	透過描画ウィジェット用シーンヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLDRAWSCENE_H
#define CLDRAWSCENE_H
#include <QGraphicsItem>
#include <QGraphicsScene>

class ClDrawUndo;
//--------------------------------------------------------------------------------//
/*!
	@brief		オブジェクトレイヤ
*/
//--------------------------------------------------------------------------------//
class ClObjectLayer : public QGraphicsItem
{
public:
	/*!
		@brief		コンストラクタ
		@param[in]	pclParent : 親アイテム
	*/
	ClObjectLayer(QGraphicsItem *pclParent = NULL) : QGraphicsItem(pclParent)
	{setFlag(QGraphicsItem::ItemHasNoContents); m_pclObjectFrameLayer = NULL;}

	/*!
		@brief	領域取得
		@retval	領域
	*/
	virtual QRectF boundingRect() const{return QRectF();}

	/*!
		@brief	描画
	*/
	virtual void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*){}

	/*!
		@brief	オブジェクト枠レイヤ取得
		@retval	オブジェクト枠レイヤ
	*/
	virtual ClObjectLayer *getObjectFrameLayer(){return m_pclObjectFrameLayer;}
	/*!
		@brief		オブジェクト枠レイヤ設定
		@param[in]	pclLayer ： オブジェクト枠レイヤ
	*/
	virtual void setObjectFrameLayer(ClObjectLayer *pclLayer){m_pclObjectFrameLayer = pclLayer;}

protected:
	ClObjectLayer *m_pclObjectFrameLayer;	//!< オブジェクト枠レイヤ
};
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		描画シーン
*/
//--------------------------------------------------------------------------------//
class ClDrawScene : public QGraphicsScene
{
public:
	ClDrawScene(qint32 lLayerCount, QObject *pclParent = NULL);	//コンストラクタ
	ClDrawScene(QObject *pclParent = NULL);	//コンストラクタ
	~ClDrawScene();	//デストラクタ

	void addLayer(qint32 lLayerCount = 1);	//レイヤ追加
	void removeLayer(qint32 lFrom, qint32 lTo = 0);	//レイヤ削除
	bool setLayer(qint32 lNumber);			//レイヤ設定

	ClObjectLayer *getCurrentObjectLayer();	//選択中オブジェクトレイヤ取得
	ClDrawUndo *getUndoStack();	//Undo管理クラス取得

protected:
	virtual void initialize(qint32 lLayerCount);	//初期化
	ClObjectLayer *createLayer();	//レイヤ生成

protected:
	QList<ClObjectLayer*> m_clObjectLayers;	//!< オブジェクトレイヤリスト
	QMap<ClObjectLayer*, ClDrawUndo*> m_clLayerUndoList;	//!< レイヤ,Undoマップ
	ClObjectLayer *m_pclCurrentObjectLayer;	//!< 現在のオブジェクトレイヤ
	ClObjectLayer *m_pclObjectLayerParent;	//!< オブジェクトレイヤの親ｓ
};
//--------------------------------------------------------------------------------//
#endif
