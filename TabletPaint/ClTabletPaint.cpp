#include <QtWidgets>
#include "ClTabletPaint.h"

#define DEF_WIDTH 2

ClTabletPaint::ClTabletPaint(QWidget* parent) : QWidget(parent)
{
	QBrush brush = QBrush(QColor(0, 0, 0, 255), Qt::SolidPattern);
	m_clPen = QPen(brush, DEF_WIDTH, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	initPixmap();
	setAutoFillBackground(true);
	m_bIsDrawing = false;
	m_nPenMode = 0;

	this->m_pclSelectedDispWidget = new ClSelectedDispWidget(this);
	this->m_pclSelectedDispWidget->setVisible(false);
}

void ClTabletPaint::initPixmap()
{
	QPixmap newPixmap = QPixmap(width(), height());
	newPixmap.fill(Qt::transparent);
	QPainter painter(&newPixmap);
	if (!m_clPixmap.isNull()){
		painter.drawPixmap(0, 0, m_clPixmap);
	}
	painter.end();
	m_clPixmap = newPixmap;
}

bool ClTabletPaint::saveImage(const QString &file)
{
	return m_clPixmap.save(file);
}

bool ClTabletPaint::loadImage(const QString &file)
{
	bool success = m_clPixmap.load(file);
	if(success){
		initPixmap();
		update();
		return true;
	}
	return false;
}

void ClTabletPaint::mousePressEvent(QMouseEvent *event)
{
	if (!m_bIsDrawing) {
		m_bIsDrawing = true;
		m_clPolyLine[0] = m_clPolyLine[1] = m_clPolyLine[2] = event->pos();
	}
	m_clStartPoint = event->pos();
	qDebug("mousePressEvent x=%d,y=%d", event->x(), event->y());
}

void ClTabletPaint::mouseMoveEvent(QMouseEvent *event)
{
	if(m_nPenMode == PENMODE_PEN){
		m_clPolyLine[2] = m_clPolyLine[1];
		m_clPolyLine[1] = m_clPolyLine[0];
		m_clPolyLine[0] = event->pos();

		QPainter painter(&m_clPixmap);
		painter.setRenderHint(QPainter::Antialiasing);
		//painter.setCompositionMode(QPainter::CompositionMode_Source);
		painter.setPen(m_clPen);
		painter.drawLine(m_clPolyLine[1], m_clPolyLine[0]);
#if 1
		update();
#else
		update(QRect(m_clPolyLine[1], event->pos()).normalized().adjusted(-32, -32, 32, 32));
#endif
	}
	else if(m_nPenMode == PENMODE_SEL){
		if(m_pclSelectedDispWidget){
			m_pclSelectedDispWidget->setGeometry(QRect(m_clStartPoint, event->pos()).normalized());
			if(false == m_pclSelectedDispWidget->isVisible()){
				m_pclSelectedDispWidget->setVisible(true);
			}
		}
	}
	else if(m_nPenMode == PENMODE_DEL){
	}
//	qDebug("mouseMoveEvent x=%d,y=%d", event->x(), event->y());
}

void ClTabletPaint::mouseReleaseEvent(QMouseEvent *event)
{
	if (m_bIsDrawing){
		update();
		m_bIsDrawing = false;
	}
	if(m_pclSelectedDispWidget){
		m_pclSelectedDispWidget->setVisible(false);
	}
	qDebug("mouseReleaseEvent x=%d,y=%d", event->x(), event->y());
}

void ClTabletPaint::tabletEvent(QTabletEvent *event)
{
	switch (event->type()) {
		case QEvent::TabletPress:
			break;
		case QEvent::TabletRelease:
			break;
		case QEvent::TabletMove:
			if (m_bIsDrawing) {
				m_clPen.setWidthF(event->pressure() * 5 + 2);
			}
			break;
		default:
			break;
	}
}

void ClTabletPaint::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.drawPixmap(0, 0, m_clPixmap);
}

void ClTabletPaint::resizeEvent(QResizeEvent *)
{
	initPixmap();
	m_clPolyLine[0] = m_clPolyLine[1] = m_clPolyLine[2] = QPoint();
}

void ClTabletPaint::setPenWidth(int value)
{
	m_clPen.setWidth(value);
}

void ClTabletPaint::setPenColor(int value)
{
	m_clPen.setColor(value);
}

void ClTabletPaint::setPenTrans(int value)
{
	QColor color = m_clPen.color();
	m_clPen.setColor(QColor(color.red(), color.green(), color.blue(), value));
}

void ClTabletPaint::setPenMode(int mode)
{
	m_nPenMode = mode;
}

ClSelectedDispWidget::ClSelectedDispWidget(QWidget *parent) : QWidget(parent)
{
	this->m_clPen.setWidth(4);
	this->m_clPen.setColor(QColor(0, 0, 255, 255));
}

ClSelectedDispWidget::~ClSelectedDispWidget()
{
}

void ClSelectedDispWidget::paintEvent(QPaintEvent *in_pclEvent)
{
	QPainter	l_clPainter(this);								//描画用クラスを準備
	l_clPainter.setClipping(true);								//一応クリッピングを有効化
	l_clPainter.setClipRect(in_pclEvent->rect());				//引数から更新が必要な領域を取得し、クリップ領域として設定

	l_clPainter.setPen(this->m_clPen);							//設定された色と太さのペンを描画クラスに設定
	l_clPainter.fillRect(this->rect(), QColor(255, 255, 255, 64));	//まず内側を塗り潰す
	l_clPainter.drawRect(this->rect());							//その後外周をペンで描画する

	QWidget::paintEvent(in_pclEvent);							//継承元の処理を行う
}
