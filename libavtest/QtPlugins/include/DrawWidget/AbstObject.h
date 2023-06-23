//--------------------------------------------------------------------------------//
/*!
	@file	AbstObject.h
	@brief	オブジェクト抽象クラス

	@author	株式会社エルモ社　映像技術第二部
*/
//--------------------------------------------------------------------------------//
#ifndef ABSTOBJECT_H
#define ABSTOBJECT_H

//#define USE_QPIXMAP		//描画にQPixmapを使用.このdefineがなかったらQImageを使用

#include <QGraphicsItem>


//--------------------------------------------------------------------------------//
/*!
	@class	AbstObject
	@brief	オブジェクト抽象クラス
*/
//--------------------------------------------------------------------------------//
class AbstObject : public QGraphicsItem
{
public:
	virtual QRectF draw(QPainter *pclPainter) = 0;	//生成中描画
	virtual void newPoint(QPointF clPoint) = 0;		//新しい座標通知(mousemove)
	virtual void endCreate() = 0;					//生成終了(mouserelease)
	virtual QRectF boundingRect() const{return m_clRect;}		//領域取得

protected:
	QRectF				m_clRect;			//領域
};
//--------------------------------------------------------------------------------//
#endif
