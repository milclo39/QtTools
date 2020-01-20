#ifndef TABLETPAINT_H
#define TABLETPAINT_H

#include <QWidget>
#include <QPixmap>
#include <QPoint>
#include <QTabletEvent>
#include <QColor>
#include <QBrush>
#include <QPen>
#include <QPoint>

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QString;
QT_END_NAMESPACE
class ClSelectedDispWidget;
class ClTabletPaint : public QWidget
{
	Q_OBJECT

public:
	enum{
		PENMODE_PEN = 0,
		PENMODE_DEL,
		PENMODE_SEL,
	};

public:
	explicit ClTabletPaint(QWidget* parent = 0);

protected:
	void tabletEvent(QTabletEvent *event) Q_DECL_OVERRIDE;
	void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
	void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

public:
	bool saveImage(const QString &file);
	bool loadImage(const QString &file);
	void setPenColor(int color);
	void setPenWidth(int width);
	void setPenTrans(int value);
	void setPenMode(int mode);

private:
	void initPixmap();

private:
	QPixmap m_clPixmap;		// 描画領域
	QPen m_clPen;			// 描画ペン
	bool m_bIsDrawing;		// 描画中フラグ
	QPoint m_clPolyLine[3];	// 直線の描画座標
	int m_nPenMode;			// ペンモード

	ClSelectedDispWidget* m_pclSelectedDispWidget;	// 選択Widget
	QPoint m_clStartPoint;	// 選択開始点
};

class ClSelectedDispWidget : public QWidget
{
	Q_OBJECT

private:
	QPen m_clPen;

public:
	ClSelectedDispWidget(QWidget *parent = 0);				//コンストラクタ
	virtual ~ClSelectedDispWidget();						//デストラクタ

protected:
	virtual void	paintEvent(QPaintEvent* in_pclEvent);	//描画イベント処理
};

#endif // TABLETPAINT_H
