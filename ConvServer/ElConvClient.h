#ifndef ELCONVCLIENT_H
#define ELCONVCLIENT_H

#include <QThread>
#include <QTcpSocket>
#include <QQueue>
#include <QMutex>

#include "openssl/ssl.h"
#include "curl/curl.h"

class ElConvClient : public QThread
{
	Q_OBJECT
public:
	explicit ElConvClient();
	~ElConvClient();

	enum CURL_STATUS{
		STATE_DISCONNECTED = 0,
		STATE_CONNECTING,
		STATE_CONNECTED,
		STATE_DISCONNECTING,
	};

signals:
	void sigRecv(QByteArray);
	void sigError();

public:
	void setUrl(QString, QString);
	int getStatus(void);

private slots:
	void slotSend(QByteArray);
	void slotConnect(bool);

protected:
	void run();

private:
	// func
	void procInit();
	void procSend(QByteArray);
	int procRecv();

	// param
	CURL *m_pclCurl;
	int m_isCurlConnected;
	char *m_pcRecv;
	QQueue<QByteArray> m_reqQueue;
	QMutex m_mutex;
	QString m_strUrl;
};

#endif // ELCONVCLIENT_H
