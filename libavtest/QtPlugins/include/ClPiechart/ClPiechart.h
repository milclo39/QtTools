//***************************************************************************//
/*!
 @file		ClPiechart.h
 @author		(S技1)須田
 @brief		円グラフ生成クラスヘッダ
*/
//***************************************************************************//
#ifndef CLPIECHART_H
#define CLPIECHART_H

#include <QObject>
#include <QPainter>
#include <QVector>
#include <QDebug>
#include <QLinearGradient>
#include <QBitmap>
#include <QRegion>


#define	_USE_MATH_DEFINES				//M_PIを使うために必要
#include <qmath.h>


//--------------------------------------------------//
/*!
	@ingroup	grpclpiechart
	@brief		円グラフの１要素を表すクラス
*/
//--------------------------------------------------//
class ClPiece : public QObject
{
	Q_OBJECT
public:
	explicit ClPiece(QObject *parent = 0, qreal dPercentage = 0, QColor qColor = QColor(Qt::gray), QString strName = QString());
	void setName(QString strName){ m_strName = strName; }
	QString getName() const { return m_strName; }
	void setColor(Qt::GlobalColor eColor){ m_RgbColor = eColor; }
	void setColor(QColor qColor){ m_RgbColor = qColor; }
	QColor getColor() const { return m_RgbColor; }
	void setPercentage(qreal dPercentage){ m_dPercentage = dPercentage; }
	qreal getPercentage() const { return m_dPercentage; }

	void backupParam(qreal dCircleX, qreal dCircleY, qreal dCircleWidth, qreal dCircleHeight, qreal dStartAngle, qreal dEndAngle);

	QRegion clipRegion() const;					//Pieのリージョンを得る (backupParamで値を取得してから有効)

private:
	QString m_strName;
	QColor m_RgbColor;
	qreal m_dPercentage;

	//自分をDrawした時のデータ保存用
	qreal m_dCircleX;						//円の表示位置のX
	qreal m_dCircleY;						//円の表示位置のY
	qreal m_dCircleWidth;					//円の横幅
	qreal m_dCircleHeight;					//円の縦幅
	qreal m_dStartAngle;					//Circle開始角度
	qreal m_dEndAngle;						//Circle終了角度


};

//--------------------------------------------------//
/*!
	@ingroup	grpclpiechart
	@brief		円グラフ全体を構成するクラス
*/
//--------------------------------------------------//
class ClPiechart : public QObject
{
	Q_OBJECT
public:
	explicit ClPiechart(QObject *parent = 0);
	virtual ~ClPiechart();

	virtual QVector<ClPiece*> getPieceVector() const{ return m_vPieces; }
	virtual void addPiece(ClPiece *pqPiece);
	virtual void addPiece(QString strName, QColor qColor, qreal qPercentage);
	virtual void removePiece(QString strName);
	virtual void removePiece(ClPiece *pqPiece);
	virtual void clearPiece(){ m_vPieces.clear(); }
	virtual bool getShadows(){ return m_bShadow; }
	virtual void setShadows(bool bShadow = true){ m_bShadow = bShadow; }
	virtual bool getClockwise(){ return m_bClockwise; }
	virtual void setClockwise(bool bClockwise){ m_bClockwise = bClockwise; }
	virtual qreal getStartAngle(){ return m_dStartAngle; }
	virtual void setStartAngle(qreal dStartAngle){ m_dStartAngle = dStartAngle; }
	virtual qreal getSlopeDegree(){ return m_dSlopeDegree; }
	virtual void setSlopeDegree(qreal degree){ m_dSlopeDegree = degree; }
	virtual qreal get3dHeight(){return m_d3dHeight;}
	virtual void set3dHeight(qreal dHeight){m_d3dHeight = dHeight;}
	virtual qint32 drawNormalChart(QPainter *qPainter);
	virtual qint32 draw3DChart(QPainter *qPainter);

	virtual QPointF getPointXY(){ QPointF qPoint(m_dCircleX,m_dCircleY); return qPoint; }
	virtual void setPointXY(QPointF qPoint){ m_dCircleX = qPoint.x(); m_dCircleY = qPoint.y(); }
	virtual void setPointXY(qreal x, qreal y){ m_dCircleX = x; m_dCircleY = y; }
	virtual qreal getWidth(){ return m_dCircleWidth; }
	virtual void setWidth(qreal width){ m_dCircleWidth = width; }
	virtual qreal getHeight(){ return m_dCircleHeight; }
	virtual void setHeight(qreal height){ m_dCircleHeight = height; }
	virtual QRectF getRect(bool bIs3d = false);

	virtual void doSortLessThan();
	virtual void doSortGreaterThan();

protected:
	qreal m_dCircleX;						//円の表示位置のX
	qreal m_dCircleY;						//円の表示位置のY
	qreal m_dCircleWidth;					//円の横幅
	qreal m_dCircleHeight;					//円の縦幅
	qreal m_dShadowMargin;					//グラフと影のズレ幅
	qreal m_dSlopeDegree;					//傾きの角度(0～90)
	qreal m_dStartAngle;					//Circle開始角度
	qreal m_d3dHeight;						//3次元グラフの高さ

	qint32 m_lPiece;
	QVector<ClPiece *> m_vPieces;			//Piecesの管理用Vector
	bool m_bShadow;							//影有無設定
	bool m_bClockwise;						//True=右回り

	virtual QPointF getPointF(qreal dAngle, qreal dR1 = 0, qreal dR2 = 0);
	virtual qint32 getQuater(qreal dAngle);
	virtual qreal getAngle360(qreal dAngle);
	virtual qreal getSlope();

};

#endif // CLPIECHART_H
