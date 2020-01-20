#include <QLayout>
#include <QDebug>
#include "widget.h"
#include <QNetworkInterface>

Widget::Widget(QWidget *parent)
	: QWidget(parent)
{
	// 画面設定
	m_pclEdtServer = new QLineEdit("w4a.elmocloud.com");
	m_pclEdtPort = new QLineEdit("443");
	m_pclEdtApplication = new QLineEdit("elmo-cloud-rec1");
	m_pclEdtStreamName = new QLineEdit("xxx");
	m_pclBtnConnect = new QPushButton("SetURL");
	connect(m_pclBtnConnect, SIGNAL(clicked()), this, SLOT(slotConnectClicked()));

	QString strLocalIP = "";
	foreach (const QHostAddress &address, QNetworkInterface::allAddresses()){
		if (address.protocol() == QAbstractSocket::IPv4Protocol
			&& address != QHostAddress(QHostAddress::LocalHost)){
			 strLocalIP = address.toString();
			 break;
		}
	}

	QVBoxLayout *layout = new QVBoxLayout;
	{
		QHBoxLayout *layoutSub = new QHBoxLayout;
		layout->addWidget(new QLabel(strLocalIP));
		layout->addLayout(layoutSub);
	}
	layout->addStretch();
	{
		QHBoxLayout *layoutSub = new QHBoxLayout;
		layoutSub->addWidget(new QLabel("URL:"));
		layoutSub->addWidget(m_pclEdtServer);
		layout->addLayout(layoutSub);
	}
	{
		QHBoxLayout *layoutSub = new QHBoxLayout;
		layoutSub->addWidget(new QLabel("Port:"));
		layoutSub->addWidget(m_pclEdtPort);
		layout->addLayout(layoutSub);
	}
	{
		QHBoxLayout *layoutSub = new QHBoxLayout;
		layoutSub->addWidget(new QLabel("App:"));
		layoutSub->addWidget(m_pclEdtApplication);
		layout->addLayout(layoutSub);
	}
	{
		QHBoxLayout *layoutSub = new QHBoxLayout;
		layoutSub->addWidget(new QLabel("Stream:"));
		layoutSub->addWidget(m_pclEdtStreamName);
		layout->addLayout(layoutSub);
	}
	layout->addStretch();
	layout->addWidget(m_pclBtnConnect);
	layout->addStretch();
	this->setLayout(layout);

	m_pclConvServer = new ElConvServer;
	m_pclConvClient = new ElConvClient;
	connect(m_pclConvServer, SIGNAL(sigRecv(QByteArray)), m_pclConvClient, SLOT(slotSend(QByteArray)));
	connect(m_pclConvClient, SIGNAL(sigRecv(QByteArray)), m_pclConvServer, SLOT(slotSend(QByteArray)));
	connect(m_pclConvServer, SIGNAL(sigConnect(bool)), m_pclConvClient, SLOT(slotConnect(bool)));
	connect(m_pclConvClient, SIGNAL(sigError()), m_pclConvServer, SLOT(disconnected()));

	m_pclClientThread = new QThread;
	m_pclConvClient->moveToThread(m_pclClientThread);
	m_pclClientThread->start();
}

Widget::~Widget()
{
}

void Widget::slotConnectClicked()
{
	QString strUrl = "https://" + m_pclEdtServer->text()
					+ ":" + m_pclEdtPort->text()
					+ "/" + m_pclEdtApplication->text()
					+ "/" + m_pclEdtStreamName->text();
	m_pclConvClient->setUrl(strUrl, "");
}
