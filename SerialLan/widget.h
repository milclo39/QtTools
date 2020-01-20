#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QComboBox>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTcpSocket>
#include <QHostAddress>
#include <QCheckBox>

class Widget : public QWidget
{
	Q_OBJECT

public:
	Widget(QWidget *parent = 0);
	~Widget();

public slots:
	void slotReadyRead();

	void slotReadyReadLan();
	void tcpConnect();
	void tcpDisconnect();
	void error();

private:
	QSerialPort *m_pclSerial;
	QList<QSerialPortInfo> m_pclPortInfo;
	QTextEdit *m_pclEdtText;
	QPushButton *m_pclBtnOpen;
	QComboBox *m_pclCmbSerial;
	QLineEdit *m_pclEdtComPath;
	QPushButton *m_pclBtnComRefer;
	QPushButton *m_pclBtnComSend;
	QLineEdit *m_pclEdtComSendData;

	QTcpSocket *m_pclSocket;
	QTextEdit *m_pclEdtLanText;
	QLineEdit *m_pclEdtAddr;
	QLineEdit *m_pclEdtPort;
	QPushButton *m_pclBtnConnect;
	QLineEdit *m_pclEdtLanPath;
	QPushButton *m_pclBtnLanRefer;
	QPushButton *m_pclBtnLanSend;
	QCheckBox *m_pclChkHex;
	QLineEdit *m_pclEdtLanSendData;

private slots:
	void slotOpenClicked();
	void slotComReferClicked();
	void slotComSendClicked();
	void slotComClearClicked();
	void slotConnectClicked();
	void slotLanReferClicked();
	void slotLanSendClicked();
	void slotLanClearClicked();
};

#endif // WIDGET_H
