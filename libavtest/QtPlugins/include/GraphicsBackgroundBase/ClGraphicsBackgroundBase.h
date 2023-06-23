//***************************************************************************//
/*!
 @file		ClGraphicsBackgroundBase.h
 @author		(S技1)須田
 @brief		特定背景描画ライブラリヘッダ
*/
//***************************************************************************//
#ifndef CLGRAPHICSBACKGROUNDBASE_H
#define CLGRAPHICSBACKGROUNDBASE_H

#include <QtGui>
#include <QtCore/qstate.h>
#include <QDebug>
//#include <QGuiApplication>
#include <QApplication>
#include <QDesktopWidget>
#include <QGraphicsPixmapItem>

//--------------------------------------------------//
/*!
	@ingroup	grpGraphics
	@brief		特定背景をPixmapに描画するクラス
	@attention	シーンにaddしても使えるが重たいのでbackgroundにセットすること

	@example
	{
	 QGraphicsScene scene(0,0,800,600);
	 ClGraphicsBackgroundBase  *pclBg = new ClGraphicsBackgroundBase(":/img/BackFloor");
	 pclBg->initialize();

	 View *qView = new View(&scene);

	 qView->setBackgroundBrush(pclBg->pixmap());

	 qView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	 qView->setCacheMode(QGraphicsView::CacheBackground);
	 qView->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	 qView->show();
	 //I've never seen easy example!
	}
*/
//--------------------------------------------------//
class ClGraphicsBackgroundBase :  public QObject, public QGraphicsPixmapItem
{
	Q_OBJECT
	//プロパティ群の登録
	Q_PROPERTY(QSize size READ getSize WRITE setSize)
	Q_PROPERTY(QColor bgColor READ getBgColor WRITE setBgColor)
	Q_PROPERTY(QString bgFilename READ getBgFilename WRITE setBgFilename)
	Q_PROPERTY(QGradient bgGrad READ getBgGradient WRITE setBgGradient)
public:

	explicit ClGraphicsBackgroundBase(QGraphicsItem *parent = 0);				//バックグラウンドクラス
	explicit ClGraphicsBackgroundBase(QString BgFilename, QColor bgColor = Qt::white, QGradient qGradient = QGradient(), QGraphicsItem *parent = 0);				//バックグラウンドクラス
	virtual ~ClGraphicsBackgroundBase(){}
	virtual bool initialize();										//初期化
	virtual QPixmap pixmap();												//オーバーライド

	virtual void setSize(QSize size);
	virtual QSize getSize(){ return m_qSize; }

	virtual bool setBgColor(QColor bgColor);						//背景色の設定
	virtual QColor getBgColor(){ return m_qBgColor; }				//背景色の取得

	virtual bool setBgFilename(QString bgFilename);					//背景画像設定
	virtual QString getBgFilename(){ return m_strBgFilename; }		//背景用ファイル名取得

	virtual bool setBgGradient(QGradient qGrad);					//グラデーション更新
	virtual QGradient getBgGradient(){ return m_qGradient; }		//グラデーション取得

protected:
	bool m_bInit;													//初期化済みフラグ
	bool m_bUseBgColorByGrad;										//グラデーションで背景色使うかどうかのスイッチ
	QString m_strBgFilename;										//背景のファイル名
	QGradient m_qGradient;											//背景用グラデーション
	QColor m_qBgColor;												//背景色
	QSize m_qSize;													//このオブジェクトのサイズ

};
//--------------------------------------------------//


#endif // CLGRAPHICSBACKGROUNDBASE_H
