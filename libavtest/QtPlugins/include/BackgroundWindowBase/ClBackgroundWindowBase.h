//--------------------------------------------------------------------------------//
/*!
	@file	ClBackgroundWindowBase.h
	@brief	背景ウィンドウヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLBACKGROUNDWINDOWBASE_H
#define CLBACKGROUNDWINDOWBASE_H
#include "Common/CmnDefine.h"
#include <QGraphicsWidget>

//--------------------------------------------------------------------------------//
/*!
	@brief	背景ウィンドウ
*/
//--------------------------------------------------------------------------------//
class ClBackgroundWindowBase : public QGraphicsWidget
{
public:
	ClBackgroundWindowBase(QGraphicsItem *pclParent = NULL);	//コンストラクタ

	virtual void paint(QPainter *pclPainter, const QStyleOptionGraphicsItem*, QWidget*);	//描画

	void setBackground(QPixmap clPixmap);	//背景画像設定
	/*!
		@brief	背景画像取得
		@retval	背景画像
	*/
	QPixmap getBackground(){return m_clBackgroundPixmap;}

	void setChildItem(QGraphicsObject *pclChild);	//子アイテム設定
	void setChildItems(QList<QGraphicsObject*> clChildList);	//子アイテム設定
	QList<QGraphicsObject*> takeChildItems();				//子アイテムの親子関係を切って取得

protected:
	QPixmap m_clBackgroundPixmap;	//!< 背景画像

	QMap<QGraphicsObject*, QTransform> m_clChildList;	//!< 子アイテムリスト
};
//--------------------------------------------------------------------------------//
Q_DECLARE_METATYPE(ClBackgroundWindowBase*)
#endif
