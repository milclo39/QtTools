#include "ClStreamView.h"
#include <QTimer>

ClStreamView::ClStreamView(QQuickItem *parent) : QQuickPaintedItem(parent)
{
	setAcceptedMouseButtons(Qt::AllButtons);
	m_pDecoder = NULL;

	ClStreamDecoder::avStreamInit();
}

ClStreamView::~ClStreamView()
{
	ClStreamDecoder::avStreamDeinit();
}

void ClStreamView::slotStreamStart(QString strUrl)
{
	if(NULL == m_pDecoder)
	{
		m_pDecoder = new ClStreamDecoder(this);
		connect(m_pDecoder, SIGNAL(sigImage(ClAvImage)), SLOT(slotImage(ClAvImage)));
		m_pDecoder->setStream(strUrl);
	}
}

void ClStreamView::slotStreamStop()
{
	if(NULL != m_pDecoder)
	{
		disconnect(m_pDecoder, SIGNAL(sigImage(ClAvImage)), this, SLOT(slotImage(ClAvImage)));
		m_pDecoder->stop();
		QTimer::singleShot(0, this, SLOT(slotStreamEnd()));
	}
}

void ClStreamView::slotStreamEnd()
{
	m_pDecoder->wait();
	delete m_pDecoder;
	m_pDecoder = NULL;
}

void ClStreamView::slotImage(ClAvImage img)
{
	m_imgSource = img.getImage();
	m_imgDraw = m_imgSource;

	QPainter painterRectLine(&m_imgDraw);
	painterRectLine.setPen(QPen(QColor(255, 0, 0, 128), 4));
	painterRectLine.drawRect(QRect());
	update();
}

void ClStreamView::paint(QPainter *painter)
{
	if(false == m_imgDraw.isNull()){
		QSize sizeDraw = m_imgDraw.size().scaled(size().toSize(), Qt::KeepAspectRatio);
		int iX = (width() - sizeDraw.width()) / 2;
		int iY = (height() - sizeDraw.height()) / 2;
		QRect rectDraw(QPoint(iX, iY), sizeDraw);
		painter->drawImage(rectDraw, m_imgDraw);
	}
}
