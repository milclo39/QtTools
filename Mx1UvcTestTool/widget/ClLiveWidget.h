//--------------------------------------------------------------------------------//
/*!
	@file	ClLiveWidget.h
	@brief	ウィンドウヘッダ
	@author	内田(FFD)
*/
//--------------------------------------------------------------------------------//
#ifndef CLLIVEWIDGET_H
#define CLLIVEWIDGET_H

#include "../../QtAvLibraries/StreamDecoder/ClStreamDecoder.h"
#include "ClWidgetDrawImage.h"

//--------------------------------------------------------------------------------//
/*!
	@brief	ウィンドウ
*/
//--------------------------------------------------------------------------------//
class ClLiveWidget : public QWidget
{
	Q_OBJECT
public:
	ClLiveWidget(QWidget *parent = NULL);
	~ClLiveWidget();
	virtual void closeEvent(QCloseEvent*);

public:
	void slotStreamStart(QString strUrl);
	void slotStreamStop();
	void setRectSize(QRect iRect);

signals:
	void sigClosed();

protected slots:
	void slotImage(ClAvImage clImage);
	void slotStreamEnd();

private:
	void initialize();
	ClStreamDecoder *m_pDecoder;
	ClWidgetDrawImage *m_pWidgetImage;
};

#endif // CLLIVEWIDGET_H
