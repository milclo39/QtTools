//--------------------------------------------------------------------------------//
/*!
	@file	ClLiveWidget.cpp
    @brief	ウィンドウ
    @author	内田(FFD)
*/
//--------------------------------------------------------------------------------//
#include "ClLiveWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QTimer>

//--------------------------------------------------------------------------------//
/*!
	@brief	コンストラクタ
*/
//--------------------------------------------------------------------------------//
ClLiveWidget::ClLiveWidget(QWidget *parent) : QWidget(parent)
{
	m_pDecoder = NULL;

	initialize();
	this->setMinimumSize(QSize(300, 180));
	ClStreamDecoder::avStreamInit();
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief	デストラクタ
*/
//--------------------------------------------------------------------------------//
ClLiveWidget::~ClLiveWidget()
{
	if(NULL != m_pDecoder)
	{
		delete m_pDecoder;
		m_pDecoder = NULL;
	}
	ClStreamDecoder::avStreamDeinit();
}
//--------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------//
/*!
    @brief	初期化
*/
//--------------------------------------------------------------------------------//
void ClLiveWidget::initialize()
{
	m_pWidgetImage = new ClWidgetDrawImage();
	QVBoxLayout *layout = new QVBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(m_pWidgetImage);

	this->setLayout(layout);
}
//--------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------//
/*!
	@brief	stream開始
*/
//--------------------------------------------------------------------------------//
void ClLiveWidget::slotStreamStart(QString strUrl)
{
	if(NULL == m_pDecoder)
	{
		m_pDecoder = new ClStreamDecoder(this);
		connect(m_pDecoder, SIGNAL(sigImage(ClAvImage)), SLOT(slotImage(ClAvImage)));
		m_pDecoder->setStream(strUrl);
	}
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief	stream停止
*/
//--------------------------------------------------------------------------------//
void ClLiveWidget::slotStreamStop()
{
	if(NULL != m_pDecoder)
	{
		disconnect(m_pDecoder, SIGNAL(sigImage(ClAvImage)), this, SLOT(slotImage(ClAvImage)));
		m_pDecoder->stop();
		QTimer::singleShot(0, this, SLOT(slotStreamEnd()));
	}
}
//--------------------------------------------------------------------------------//

void ClLiveWidget::setRectSize(QRect iRect)
{
	m_pWidgetImage->setRectSize(iRect);
}

//--------------------------------------------------------------------------------//
/*!
	@brief	stream停止のThread終了待ち
*/
//--------------------------------------------------------------------------------//
void ClLiveWidget::slotStreamEnd()
{
	m_pDecoder->wait();
	delete m_pDecoder;
	m_pDecoder = NULL;
}
//--------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------//
/*!
	@brief		画像更新
	@param[in]	img : 画像
*/
//--------------------------------------------------------------------------------//
void ClLiveWidget::slotImage(ClAvImage img)
{
	QImage imgNew = img.getImage();
	m_pWidgetImage->updateImage(imgNew);
}
//--------------------------------------------------------------------------------//

void ClLiveWidget::closeEvent(QCloseEvent *)
{
	emit sigClosed();
}
