#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QTextEdit>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "Uvc/ClUvcManager.h"
//#include "widget/ClLiveWidget.h"

class Widget : public QWidget
{
	Q_OBJECT

public:
	Widget(QWidget *parent = 0);
	~Widget();
	void initialize();

signals:
	void sigComments(QString);
	void sigReceiveData(QByteArray);

public slots:
	QJsonObject getObject(QString);

	void slotTimerProc();
	void slotReceiveData(QByteArray);
	void slotChangedDevice(bool);
	void slotLiveClosed();

private:
	QTextEdit *m_pclEdtText;
	QLineEdit *m_pclEdtSend;
	QLineEdit *m_pclEdtPolling;
	QLabel *m_pclLblZoomPos;
	QPushButton *m_pclBtnClear;
	QPushButton *m_pclBtnSend;
	QPushButton *m_pclBtnMode;
	QPushButton *m_pclBtnLive;
	QComboBox *m_pclCmbSerial;
	QSlider *m_pclZoomLevel;

	ClUvcManager m_pclUvc;
	QTimer *m_pclActTimer;
	bool m_bStart;
	QString m_strMethod;
	qint32 m_iPreZoom;
	qint32 m_iJsonId;

//	ClLiveWidget *m_pclLiveWidget;

private slots:
	void slotSendClicked();
	void slotModeClicked();
	void slotClearClicked();
	void slotLiveClicked();
	void slotSliderMoved(qint32);
};

#endif // WIDGET_H
