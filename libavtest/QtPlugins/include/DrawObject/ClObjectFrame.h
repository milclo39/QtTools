//--------------------------------------------------------------------------------//
/*!
	@file	ClObjectFrame.h
	@brief	オブジェクト枠ヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLOBJECTFRAME_H
#define CLOBJECTFRAME_H
#include "IDrawObjectCreator.h"
#include "ClObjectKnob.h"
#include <QGraphicsItem>

//--------------------------------------------------------------------------------//
/*!
	@brief	オブジェクト枠(サイズ変更、回転)
*/
//--------------------------------------------------------------------------------//
class ClObjectFrame : public QGraphicsItem
{
public:
	ClObjectFrame(AbstDrawObject *pclParent);	//コンストラクタ

	QList<ClObjectKnob*> getKnobs();	//つまみリスト取得

	/*!
		@brief	領域取得
		@retval	子の領域
	*/
	virtual QRectF boundingRect() const{return childrenBoundingRect();}
	virtual void paint(QPainter *pclPainter, const QStyleOptionGraphicsItem*, QWidget*);	//描画

	virtual void moveItem();	//アイテム移動
	virtual void resizeItem(ClObjectKnob *pclKnob, QPointF clDistance);	//サイズ変更
	virtual void rotateItem(qreal dRad);		//アイテム回転
	QPointF getObjectCenterPoint();	//オブジェクトの中心座標取得
	QPointF getTransformPoint(QPointF clPoint);	//変形座標取得

protected:
	virtual void adjustKnobs();	//つまみ位置調整
	void adjustKnobRect(ClObjectKnob *pclKnob, QPointF clPointCenter);	//つまみ領域調整

protected:
	QList<ClObjectKnob*> m_clKnobs;	//!< つまみリスト
	AbstDrawObject *m_pclObject;	//!< オブジェクト
	QTransform m_clRotateTransform;	//!< 回転変形
};
//--------------------------------------------------------------------------------//
#endif //CLOBJECTFRAME_H
