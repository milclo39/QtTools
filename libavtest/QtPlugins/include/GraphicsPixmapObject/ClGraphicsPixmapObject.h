/*--------------------------------------------------------------------------------*/
/*!
	@file	ClGraphicsPixmapObject.h
	@brief	プロパティを持つ画像描画クラス
*/
/*--------------------------------------------------------------------------------*/
#ifndef CLGRAPHICSPIXMAPOBJECT_H
#define CLGRAPHICSPIXMAPOBJECT_H

#include <QGraphicsObject>




/*================================================================================*/
/*--------------------------------------------------------------------------------*/
/*!
	@class	ClGraphicsPixmapObject
	@brief	プロパティを持つ画像描画クラス
	@note	QGraphicsObject+QGraphicsPixmapItem、プロパティで透過度とかいじりたいから自分で作った
			ほらアニメーションとかする時に不便だし？
*/
/*--------------------------------------------------------------------------------*/
class ClGraphicsPixmapObject : public QGraphicsObject
{
	Q_OBJECT


	/*----------------------------------------*/
	/*
		メンバー変数
	*/
	/*----------------------------------------*/
private:
	bool	m_bIsSmoothRendering;	//!< 綺麗に描画するかどうか
	QPixmap	m_clPixmap;				//!< 表示用画像
	QImage	m_clImage;				//!< 表示用画像。Pixmapに有効なのが設定されてたらこっちは基本的に無視される


	/*----------------------------------------*/
	/*
		プロパティ
	*/
	/*----------------------------------------*/
	//綺麗に描画するかどうか。trueで綺麗に描画。デフォルトはfalse
	Q_PROPERTY(bool smoothRendering READ smoothRendering WRITE setSmoothRendering)
public:
	bool	smoothRendering(){ return this->m_bIsSmoothRendering; }
	void	setSmoothRendering(bool in_bIsSmoothRendering){ this->m_bIsSmoothRendering = in_bIsSmoothRendering; }

	//表示用画像(QPixmap版)
	Q_PROPERTY(QPixmap pixmap READ pixmap WRITE setPixmap)
public:
	QPixmap	pixmap(){ return m_clPixmap; }
	void	setPixmap(const QPixmap in_clPixmap){ m_clPixmap = in_clPixmap; update(); }

	//表示用画像(QImage版)
	Q_PROPERTY(QImage image READ image WRITE setImage)
public:
	QImage	image(){ return this->m_clImage; }
	void	setImage(const QImage in_clImage){ this->m_clImage = in_clImage;  update(); }

	/*----------------------------------------*/
	/*
		メンバー関数
	*/
	/*----------------------------------------*/
public:
	ClGraphicsPixmapObject(QGraphicsItem* in_pclParent);	//コンストラクタ
	virtual ~ClGraphicsPixmapObject();						//デストラクタ

	QRectF	boundingRect() const;							//自身の範囲を返す
	void	paint(QPainter *in_pclPainter, const QStyleOptionGraphicsItem *option, QWidget *widget);	//描画イベント処理
};
/*================================================================================*/

#endif // CLGRAPHICSPIXMAPOBJECT_H
