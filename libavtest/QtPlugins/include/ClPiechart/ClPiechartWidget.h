//***************************************************************************//
/*!
 @file		ClPiechartWidget.h
 @author		(S技1)須田
 @brief		円グラフ表示用Widgetヘッダ
*/
//***************************************************************************//
#ifndef CLPIECHARTWIDGET_H
#define CLPIECHARTWIDGET_H

#include <QWidget>
#include "ClPiechart.h"

//--------------------------------------------------//
/*!
	@ingroup	grpclpiechart
	@brief		円グラフウィジット化するクラス
*/
//--------------------------------------------------//
class ClPiechartWidget : public QWidget
{
	Q_OBJECT
	//プロパティ群の登録
	Q_PROPERTY(bool shadow READ getShadows WRITE setShadows)
	Q_PROPERTY(qreal startAngle READ getStartAngle WRITE setStartAngle)
	Q_PROPERTY(qreal slopeDegree READ getSlopeDegree WRITE setSlopeDegree)
	Q_PROPERTY(emDRAWMODE drawMode READ getDrawMode WRITE setDrawMode)

	Q_PROPERTY(QPointF pointXY READ getPointXY WRITE setPointXY)

public:
	/** Draw方法 */
	typedef enum{
		DrawNormal,
		Draw3D
	}emDRAWMODE;


	explicit ClPiechartWidget(QWidget *parent = 0);
	virtual ~ClPiechartWidget(){}

	virtual QVector<ClPiece *> getPieceVector() const{ return m_qPiechart.getPieceVector(); }
	virtual void addPiece(ClPiece *pqPiece){ m_qPiechart.addPiece(pqPiece); }
	virtual void addPiece(QString strName,QColor qColor, qreal qPercentage);
	virtual void removePiece(QString strName){ m_qPiechart.removePiece(strName); update(); }
	virtual void clearPiece(){ m_qPiechart.clearPiece(); update(); }
	virtual bool getShadows(){ return m_qPiechart.getShadows(); }
	virtual void setShadows(bool bShadow = true){ m_qPiechart.setShadows(bShadow); update(); }
	virtual bool getClockwise(){ return m_qPiechart.getClockwise(); }
	virtual void setClockwise(bool bClockwise){ m_qPiechart.setClockwise(bClockwise); }
	virtual qreal getStartAngle(){ return m_qPiechart.getStartAngle(); }
	virtual void setStartAngle(qreal dStartAngle){ m_qPiechart.setStartAngle(dStartAngle); update(); }
	virtual qreal getSlopeDegree(){ return m_qPiechart.getSlopeDegree(); }
	virtual void setSlopeDegree(qreal degree){ m_qPiechart.setSlopeDegree(degree); update(); }
	virtual qreal get3dHeight(){return m_qPiechart.get3dHeight();}
	virtual void set3dHeight(qreal dHeight){m_qPiechart.set3dHeight(dHeight);update();}
	virtual emDRAWMODE getDrawMode(){ return m_lDrawMode; }
	virtual void setDrawMode(emDRAWMODE mode){ m_lDrawMode = mode; }

	virtual QPointF getPointXY(){ return m_qPiechart.getPointXY(); }
	virtual void setPointXY(QPointF qPoint){ m_qPiechart.setPointXY(qPoint); update(); }
	virtual void setPointXY(qreal x, qreal y){ m_qPiechart.setPointXY(x,y); update(); }

	virtual void setRenderHints(QPainter::RenderHint eValue){m_eRenderHint = eValue;}

protected:
	virtual void paintEvent(QPaintEvent *event);	//描画のオーバーライド
	virtual void resizeEvent(QResizeEvent *);		//サイズ変更

protected:
	emDRAWMODE m_lDrawMode;				//描画方法指定

	mutable ClPiechart m_qPiechart;				//円グラフクラスの実体
	QPainter::RenderHints m_eRenderHint;	//円グラフ描画オプション

};

#endif // CLPIECHARTWIDGET_H
