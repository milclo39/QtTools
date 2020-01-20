#include "ClWidgetDrawImage.h"
#include <QPainter>
#include <QDebug>

//--------------------------------------------------------------------------------//
/*!
	@brief	コンストラクタ
*/
//--------------------------------------------------------------------------------//
ClWidgetDrawImage::ClWidgetDrawImage() : QWidget()
{
//	setMinimumSize(640, 360);
//	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		画像更新
	@param[in]	img : 新しい画像
*/
//--------------------------------------------------------------------------------//
void ClWidgetDrawImage::updateImage(QImage img)
{
	setMaximumHeight(img.height());

	m_imgSource = img;
	m_imgDraw = m_imgSource;

	if(true == m_bIsRectLine){
		QPainter painterRectLine(&m_imgDraw);
		painterRectLine.setPen(QPen(QColor(255, 0, 0, 128), 4));
		painterRectLine.drawRect(m_iRectSize);
	}

	update();
}
//--------------------------------------------------------------------------------//


void ClWidgetDrawImage::setRectSize(QRect cRect){
	m_bIsRectLine = true;
	m_iRectSize = cRect;
}

//--------------------------------------------------------------------------------//
/*!
	@brief		描画イベント
*/
//--------------------------------------------------------------------------------//
void ClWidgetDrawImage::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	if(false == m_imgDraw.isNull())
	{
		QSize sizeDraw = m_imgDraw.size().scaled(size(), Qt::KeepAspectRatio);
		if(sizeDraw.width() > m_imgDraw.width())
		{
//			sizeDraw = m_imgDraw.size();
		}
		int iX = (width() - sizeDraw.width()) / 2;
		int iY = (height() - sizeDraw.height()) / 2;
		QRect rectDraw(QPoint(iX, iY), sizeDraw);
		painter.drawImage(rectDraw, m_imgDraw);
	}
}
//--------------------------------------------------------------------------------//
