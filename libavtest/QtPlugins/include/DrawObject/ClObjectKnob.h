//--------------------------------------------------------------------------------//
/*!
	@file	ClObjectKnob.h
	@brief	オブジェクトつまみヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLOBJECTKNOB_H
#define CLOBJECTKNOB_H
#include "IDrawObjectCreator.h"
#include <QGraphicsEllipseItem>

//マクロ定義	ClObjectKnob.cppより移動　（IMA2など応用アプリで参照するため）　20140116梅村
#define DRAWOBJECT_KNOB_SIZE	50	//!< つまみのあたり判定領域
#define DRAWOBJECT_KNOB_SCALE	0.3	//!< つまみのあたり判定領域に対する表示領域の倍率

#define DRAWOBJECT_KNOB_RESIZE_COLOR1	QColor(207, 63, 13)
#define DRAWOBJECT_KNOB_RESIZE_COLOR2	QColor(255, 241, 204)
#define DRAWOBJECT_KNOB_RESIZE_COLOR3	QColor(239, 137, 93)

#define DRAWOBJECT_KNOB_ROTATE_COLOR1	QColor(0, 107, 89)
#define DRAWOBJECT_KNOB_ROTATE_COLOR2	QColor(181, 250, 213)
#define DRAWOBJECT_KNOB_ROTATE_COLOR3	QColor(28, 179, 142)
//マクロ定義	ClObjectKnob.cppより移動ここまで

class ClObjectFrame;

//--------------------------------------------------------------------------------//
/*!
	@brief	オブジェクトつまみ
*/
//--------------------------------------------------------------------------------//
class ClObjectKnob : public QGraphicsEllipseItem
{
public:
	//つまみのタイプ
	typedef enum
	{
		eTYPE_TOPLEFT = 0,
		eTYPE_TOP,
		eTYPE_TOPRIGHT,
		eTYPE_LEFT,
		eTYPE_RIGHT,
		eTYPE_BOTTOMLEFT,
		eTYPE_BOTTOM,
		eTYPE_BOTTOMRIGHT,
		eTYPE_ROTATE,
		eTYPE_UNKNOWN
	}eKnobType;

public:
	ClObjectKnob(ClObjectFrame *pclParent, eKnobType eType);	//コンストラクタ

	/*!
		@brief	つまみタイプ取得
		@retval	つまみタイプ
	*/
	eKnobType getKnobType(){return m_eType;}

	virtual void mousePress(QPointF clPoint);	//マウス押下
	virtual void mouseMove(QPointF clPoint);	//マウス移動
	virtual void mouseRelease();				//マウス押下終了

	virtual QRectF boundingRect() const{return rect();}

protected:
	virtual void paint(QPainter *pclPainter, const QStyleOptionGraphicsItem*, QWidget*);	//描画

protected:
	QImage *m_pclImageKnob;	//!< つまみ画像
	eKnobType m_eType;		//!< つまみのタイプ
	QPointF m_clPressPoint;	//!< マウス押下座標
	ClObjectFrame *m_pclFrame;	//!< 枠
};
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief	サイズ変更用つまみ
*/
//--------------------------------------------------------------------------------//
class ClObjectKnobResize : public ClObjectKnob
{
public:
	ClObjectKnobResize(ClObjectFrame *pclParent, eKnobType eType);	//コンストラクタ

	void mouseMove(QPointF clPoint);	//マウス移動
};
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief	回転用つまみ
*/
//--------------------------------------------------------------------------------//
class ClObjectKnobRotate : public ClObjectKnob
{
public:
	ClObjectKnobRotate(ClObjectFrame *pclParent);	//コンストラクタ

	void mousePress(QPointF clPoint);	//マウス押下
	void mouseMove(QPointF clPoint);	//マウス移動
	void mouseRelease();	//マウス押下終了

protected:
	qreal m_dStartRad;	//!< 回転開始基準角度
};
//--------------------------------------------------------------------------------//
#endif //CLOBJECTKNOB_h
