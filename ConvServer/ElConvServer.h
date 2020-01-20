#ifndef ELCONVSERVER_H
#define ELCONVSERVER_H

#include <QThread>
#include <QTcpSocket>
#include <QTcpServer>

class ElConvServer : public QObject
{
	Q_OBJECT
public:
	explicit ElConvServer();
	~ElConvServer();

signals:
	void sigRecv(QByteArray);
	void sigConnect(bool);

private slots:
	void procInit();
	void newConnection();
	void readyRead();
	void disconnected();
	void bytesWritten(qint64);
	void slotSend(QByteArray);
	void connected();
	void error();
	void ConnectTimeout();

private:
	QThread *m_ServerThread;
	QTcpServer *m_pclServer;
	QTcpSocket *m_pclSocket;
	bool m_bConnect;
};

#endif // ELCONVSERVER_H
