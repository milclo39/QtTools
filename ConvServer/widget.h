#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QThread>
#include "ElConvServer.h"
#include "ElConvClient.h"

class Widget : public QWidget
{
	Q_OBJECT

public:
	Widget(QWidget *parent = 0);
	~Widget();

private:
	QLineEdit *m_pclEdtServer;
	QLineEdit *m_pclEdtPort;
	QLineEdit *m_pclEdtApplication;
	QLineEdit *m_pclEdtStreamName;
	QPushButton *m_pclBtnConnect;

	ElConvServer *m_pclConvServer;
	ElConvClient *m_pclConvClient;
	QThread *m_pclClientThread;

private slots:
	void slotConnectClicked();
};

#endif // WIDGET_H
