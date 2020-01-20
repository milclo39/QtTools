#include <QQuickPaintedItem>
#include <QColor>
#include <QPen>
#include <QPainter>
#include <QRect>
#include <QFont>
#include <QThread>

//*******************************************************************
// 図形
class ClDrawParts : public QQuickPaintedItem
{
	Q_OBJECT
public:
	explicit ClDrawParts(QQuickItem *parent = 0);

protected:
	bool event(QEvent *event){qDebug() << event->type();return true;}
	void paint(QPainter *painter);
};
//*******************************************************************
// 自由線
class ClDrawFreeLine : public QQuickPaintedItem
{
	Q_OBJECT
	Q_PROPERTY(QPixmap linePixmap READ linePixmap WRITE setLinePixmap)
public:
	explicit ClDrawFreeLine(QQuickItem *parent = 0);
	void setPath(QPoint point);
	QPixmap linePixmap(){return m_clPixmap;}
	void setLinePixmap(QPixmap path){m_clPixmap = path;}

protected:
	bool event(QEvent *event){qDebug() << event->type();return true;}
	void paint(QPainter *painter);

private:
	QVector<QPoint> m_clPolyLine;
	QPixmap m_clPixmap;
};
//*******************************************************************
// 画像
class ClDrawImage : public QQuickPaintedItem
{
	Q_OBJECT
public:
	explicit ClDrawImage(QQuickItem *parent = 0);
	void setPath(QString strPath);

protected:
	bool event(QEvent *event){qDebug() << event->type();return true;}
	void paint(QPainter *painter);

private:
	QPixmap m_clPixmap;
};
//*******************************************************************
class Drawing : public QQuickPaintedItem
{
	Q_OBJECT
	Q_DISABLE_COPY(Drawing)
	Q_PROPERTY(QString imagePath READ imagePath WRITE setImagePath)
	Q_PROPERTY(QPixmap linePixmap READ linePixmap WRITE setLinePixmap)

public:
	explicit Drawing(QQuickItem *parent = 0);
	Q_INVOKABLE void setNum(const int &num);
	Q_INVOKABLE void setColor(const QString &color);
	Q_INVOKABLE void setMode(const int &mode);

	QString imagePath(){return m_strImagePath;}
	void setImagePath(QString path){m_strImagePath = path;}
	QPixmap linePixmap(){return m_clLinePixmap;}
	void setLinePixmap(QPixmap path){m_clLinePixmap = path;}

protected:
	void initPixmap();
	bool event(QEvent *event);
	void paint(QPainter *painter);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void execLeftPress(QPoint point);
	void execLeftRelease(QPoint point);
	void execMoveInput(QPoint point);
	void touchMoveEvent(QTouchEvent *event);

private:
	QPixmap m_clPixmap;
	QVector<QPoint> m_clPolyLine;	// 自由線座標記録
	QPen m_clPen;
	QBrush m_clBlush;
	int m_iPenPresser;		// 筆圧
	bool m_bIsLeftPress;	// 左クリックされたか
	QRect m_clFreeLineRect;	// 自由線矩形
	QString m_strImagePath;
	QPixmap m_clLinePixmap;
};
