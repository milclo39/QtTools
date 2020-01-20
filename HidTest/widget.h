#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include "ClHidThread.h"

class Widget : public QWidget
{
	Q_OBJECT

public:
	Widget(QWidget *parent = 0);
	~Widget();

public slots:

private:
	QLineEdit *m_pclEdtText;
	QLineEdit *m_pclEdtPos;
	QLineEdit *m_pclEdtPrs;
	QPushButton *m_pclBtnOpen;
	QPushButton *m_pclBtnSend;
	QComboBox *m_pclCmbSerial;

	ClHidThread *m_pclHidThread;

private slots:
	void slotOpenClicked();
	void slotSendClicked();
	void slotRecv(QByteArray);
};

#endif // WIDGET_H
