#include "ElConvServer.h"
#include <QDebug>
#include <QTimer>

#define LISTEN_PORT 55555
#define MAX_READBUF 8192

ElConvServer::ElConvServer()
{
	m_pclSocket = NULL;
	m_bConnect = false;
	m_ServerThread = new QThread;
	moveToThread(m_ServerThread);
	connect(m_ServerThread, SIGNAL(started()), this, SLOT(procInit()));
	connect(m_ServerThread, SIGNAL(finished()), this, SLOT(deleteLater()));
	m_ServerThread->start();
}

ElConvServer::~ElConvServer()
{
}

void ElConvServer::procInit()
{
	m_pclServer = new QTcpServer;
	connect(m_pclServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
	if(false == m_pclServer->listen(QHostAddress::Any, LISTEN_PORT)){
		qDebug() << "Server could not start";
		return;
	}
	qDebug() << "listen start";
}

void ElConvServer::newConnection()
{
	qDebug() << "newConnection()";
	if(!m_pclSocket)
	{
		m_pclSocket = m_pclServer->nextPendingConnection();
		connect(m_pclSocket, SIGNAL(connected()), this, SLOT(connected()));
		connect(m_pclSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
		connect(m_pclSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
		connect(m_pclSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
		connect(m_pclSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));
		m_bConnect = true;
		emit sigConnect(true);
//		QTimer::singleShot(5000, this, SLOT(ConnectTimeout()));
	}
	else{
		qDebug() << "no more connect...?";
	}
}

void ElConvServer::connected()
{
	qDebug() << "server connected";
	m_bConnect = false;
}

void ElConvServer::ConnectTimeout()
{
	if(m_bConnect){
		qDebug() << "ConnectTimeout";
		m_bConnect = false;
		disconnected();
	}
}

void ElConvServer::readyRead()
{
	while(m_pclSocket->bytesAvailable())
	{
		QByteArray res = m_pclSocket->read(MAX_READBUF);
		emit sigRecv(res);
	}
}

void ElConvServer::disconnected()
{
	qDebug() << "server disconnected";
	disconnect(m_pclSocket, SIGNAL(connected()), this, SLOT(connected()));
	disconnect(m_pclSocket, SIGNAL(readyRead()), this, SLOT(readyRead()));
	disconnect(m_pclSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));
	disconnect(m_pclSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));
	disconnect(m_pclSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));
	m_pclSocket->close();
	m_pclSocket = NULL;
	emit sigConnect(false);
}

void ElConvServer::bytesWritten(qint64 data)
{
	qDebug() << "bytesWritten:" << data;
}

void ElConvServer::slotSend(QByteArray data)
{
	m_pclSocket->write(data);
}

void ElConvServer::error()
{
	qDebug() << "error:" << m_pclSocket->errorString();
	disconnected();
}
