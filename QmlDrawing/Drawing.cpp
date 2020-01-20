#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include "Drawing.h"
#include "ElmoEvent/ClElmoEvent.h"
#include "ElmoEvent/ClEventProc.h"

//#define USE_USER_EVENT	// eEVENT_INPUTイベントを使う

// *********************************************************************
ClDrawParts::ClDrawParts(QQuickItem *parent) : QQuickPaintedItem(parent)
{
}
void ClDrawParts::paint(QPainter *painter)
{
	painter->setPen(QColor("red"));
	painter->drawEllipse(QRect(0, 0, 99, 99));
}
// *********************************************************************
ClDrawFreeLine::ClDrawFreeLine(QQuickItem *parent) : QQuickPaintedItem(parent)
{
	setPath(QPoint(0, 0));
	setPath(QPoint(100, 100));
	setPath(QPoint(0, 100));
	setPath(QPoint(100, 0));
}
void ClDrawFreeLine::paint(QPainter *painter)
{
	painter->drawPixmap(0, 0, m_clPixmap);
}
void ClDrawFreeLine::setPath(QPoint point)
{
	m_clPolyLine.append(point);
}
// *********************************************************************
ClDrawImage::ClDrawImage(QQuickItem *parent) : QQuickPaintedItem(parent)
{
	setPath("C:\\images.png");
}
void ClDrawImage::paint(QPainter *painter)
{
	painter->drawPixmap(0, 0, m_clPixmap);
}
void ClDrawImage::setPath(QString path)
{
	m_clPixmap.load(path);
}
// *********************************************************************
Drawing::Drawing(QQuickItem *parent) : QQuickPaintedItem(parent)
{
#ifdef USE_USER_EVENT
	ClEventProc::getInstance()->setScreenInfoPri( QApplication::desktop()->screenGeometry(0) );
	ClEventProc::getInstance()->setScreenInfoSec( QApplication::desktop()->screenGeometry(1) );
	ClEventProc::getInstance()->setSelectScreen( true );
	ClEventProc::getInstance()->addObject(this, eEVENT_INPUT);
#endif
	this->setAcceptedMouseButtons(Qt::AllButtons);
//	this->setAcceptHoverEvents(true);
//	this->setAntialiasing(true);
//	this->setFiltersChildMouseEvents(true);
	m_clBlush = QBrush();
	m_clPen = QPen(m_clBlush, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
	setColor("red");
	setNum(0);
	initPixmap();
}

void Drawing::initPixmap()
{
	QPixmap newPixmap = QPixmap(1920, 1080);
	newPixmap.fill(Qt::transparent);
	QPainter painter(&newPixmap);
	if (!m_clPixmap.isNull()){
		painter.drawPixmap(0, 0, m_clPixmap);
	}
	painter.end();
	m_clPixmap = newPixmap;
	m_bIsLeftPress = false;
}

void Drawing::mousePressEvent(QMouseEvent *event)
{
	execLeftPress(event->pos());
}

void Drawing::mouseMoveEvent(QMouseEvent *event)
{
	execMoveInput(event->pos());
}

void Drawing::mouseReleaseEvent(QMouseEvent *event)
{
	execLeftRelease(event->pos());
}

void Drawing::execLeftPress(QPoint clPoint)
{
	m_clPolyLine.append(clPoint);
	m_clFreeLineRect = QRect(clPoint, clPoint);
}

void Drawing::execLeftRelease(QPoint )
{
	update();
	m_clPolyLine.clear();
	m_clLinePixmap = QPixmap(m_clFreeLineRect.size());
	QPainter clPainter(&m_clLinePixmap);
	clPainter.drawPixmap(-m_clFreeLineRect.x(), -m_clFreeLineRect.y(), m_clPixmap);
}

void Drawing::execMoveInput(QPoint clPoint)
{
	if(m_clPolyLine.isEmpty()){ return; }

	QPainter painter(&m_clPixmap);
	painter.setPen(m_clPen);
	painter.setRenderHints(QPainter::Antialiasing);
	painter.setCompositionMode(QPainter::CompositionMode_Source);
	painter.drawLine(m_clPolyLine.back(), clPoint);

	// 描いた線の領域に線の太さ分(32px)のマージンを加えた領域に対して再描画
	QRect clDrawRect = QRect(m_clPolyLine.back(), clPoint).normalized().adjusted(-32, -32, 32, 32);
	m_clPolyLine.append(clPoint);

	update(clDrawRect);

	// 自由線の領域計算
	if(m_clFreeLineRect.x() > clPoint.x()){
		m_clFreeLineRect.setLeft(clPoint.x());
	}
	if(m_clFreeLineRect.y() > clPoint.y()){
		m_clFreeLineRect.setTop(clPoint.y());
	}
	if(m_clFreeLineRect.right() < clPoint.x()){
		m_clFreeLineRect.setRight(clPoint.x());
	}
	if(m_clFreeLineRect.bottom() < clPoint.y()){
		m_clFreeLineRect.setBottom(clPoint.y());
	}
}

void Drawing::touchMoveEvent(QTouchEvent *event)
{
	// マルチタッチ判定
	if(event->touchPoints().count() > 1){
		int count = event->touchPoints().count();
		int x = 0,y = 0;
		for(int i = 0; i < event->touchPoints().count(); i++){
			QPoint clPoint = event->touchPoints().at(i).scenePos().toPoint();
			x += clPoint.x();
			y += clPoint.y();
		}
		QPoint clPoint = QPoint(x / count, y / count);	// 中点
	}
	else{	// 通常処理
		execMoveInput(event->touchPoints().first().scenePos().toPoint());
	}
}

void Drawing::setNum(const int &num)
{
	switch(num){
	case 0:
		m_clPen.setWidth(2);
		break;
	case 1:
		m_clPen.setWidth(4);
		break;
	case 2:
		m_clPen.setWidth(8);
		break;
	case 3:
		m_clPen.setWidth(16);
		break;
	case 4:
		m_clPen.setWidth(24);
		break;
	}

}

void Drawing::setColor(const QString &color)
{
	if(!color.isEmpty()){
		m_clPen.setColor(QColor(color));
	}
}

void Drawing::setMode(const int &mode)
{
	qDebug("setMode:%d", mode);
}

void Drawing::paint(QPainter *painter)
{
	painter->drawPixmap(0, 0, m_clPixmap);
}

bool Drawing::event(QEvent *event)
{
	switch(event->type())
	{
#ifdef USE_USER_EVENT	// DrawWidgetTransparentのイベント処理を強引にもってきた
		case eEVENT_INPUT:
			{
				ClInputEvent *pclInput = dynamic_cast<ClInputEvent*>(event);
				if(pclInput != NULL)
				{
					//カーソル位置更新
					QCursor::setPos( pclInput->getPos() );
					//筆圧を設定
					m_iPenPresser = pclInput->getPresser();
					//scene上の座標に変換
					QPoint clScenePos = pclInput->getPos();
//					clScenePos.setX(clScenePos.x() / (width() / sceneRect().width()));
//					clScenePos.setY(clScenePos.y() / (height() / sceneRect().height()));
					//自分が押されたかを確認
//					bool bIsPressOwn = (this == qApp->topLevelAt(pclInput->getPos()));		//自分が押されたか
					//状態取得
					bool bIsBtnPressLeft = pclInput->isBtnPress(ClInputEvent::eBTN_LEFT);
					bool bIsBtnReleaseLeft = pclInput->isBtnRelease(ClInputEvent::eBTN_LEFT);
					bool bIsBtnPressRight = pclInput->isBtnPress(ClInputEvent::eBTN_RIGHT);
					bool bIsBtnReleaseRight = pclInput->isBtnRelease(ClInputEvent::eBTN_RIGHT);
					bool bIsBtnPressMid = pclInput->isBtnPress(ClInputEvent::eBTN_MID);
					bool bIsBtnReleaseMid = pclInput->isBtnRelease(ClInputEvent::eBTN_MID);
					if(bIsBtnPressLeft){
						execLeftPress(clScenePos);
						m_bIsLeftPress = true;
					}
					else if(bIsBtnReleaseLeft){
						execLeftRelease(clScenePos);
						m_bIsLeftPress = false;
					}
					else if(m_bIsLeftPress){
						execMoveInput(clScenePos);
					}
				}
			}
			break;
#else
		case QEvent::MouseButtonPress:
			mousePressEvent(reinterpret_cast<QMouseEvent*>(event));
			break;
		case QEvent::MouseMove:
			mouseMoveEvent(reinterpret_cast<QMouseEvent*>(event));
			break;
		case QEvent::MouseButtonRelease:
			mouseReleaseEvent(reinterpret_cast<QMouseEvent*>(event));
			break;
#endif
		case QEvent::TouchBegin:
			execLeftPress(reinterpret_cast<QTouchEvent*>(event)->touchPoints().first().scenePos().toPoint());
			break;
		case QEvent::TouchUpdate:
			touchMoveEvent(reinterpret_cast<QTouchEvent*>(event));
			break;
		case QEvent::TouchEnd:
			execLeftRelease(reinterpret_cast<QTouchEvent*>(event)->touchPoints().first().scenePos().toPoint());
			break;
		default:
			qDebug() << "other event:" << event->type();
			break;
	}
	return true;
}
