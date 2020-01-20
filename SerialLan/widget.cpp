#include "widget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QFileDialog>
#include <QApplication>
#include <QSettings>
#include <QMessageBox>
#include <QNetworkProxy>

Widget::Widget(QWidget *parent) : QWidget(parent)
{
	QFont font = qApp->font();
	font.setPixelSize(22);
	this->setFont(font);

	QSettings settings(qApp->applicationDirPath() + "\\setting.ini", QSettings::IniFormat);
	m_pclEdtText = new QTextEdit;
	m_pclEdtLanText = new QTextEdit;
	m_pclEdtAddr = new QLineEdit(settings.value("ADDR", "localhost").toString());
	m_pclEdtPort = new QLineEdit(settings.value("PORT", "1024").toString());
	m_pclEdtComPath = new QLineEdit(settings.value("COMPATH", "").toString());
	m_pclEdtLanPath = new QLineEdit(settings.value("LANPATH", "").toString());
	m_pclEdtComSendData = new QLineEdit;
	m_pclEdtLanSendData = new QLineEdit;

	m_pclBtnOpen = new QPushButton("Open");
	connect(m_pclBtnOpen, SIGNAL(clicked()), this, SLOT(slotOpenClicked()));
	m_pclBtnComRefer = new QPushButton("...");
	connect(m_pclBtnComRefer, SIGNAL(clicked()), this, SLOT(slotComReferClicked()));
	m_pclBtnComSend = new QPushButton("Send");
	connect(m_pclBtnComSend, SIGNAL(clicked()), this, SLOT(slotComSendClicked()));

	m_pclBtnConnect = new QPushButton("Connect");
	connect(m_pclBtnConnect, SIGNAL(clicked()), this, SLOT(slotConnectClicked()));
	m_pclBtnLanRefer = new QPushButton("...");
	connect(m_pclBtnLanRefer, SIGNAL(clicked()), this, SLOT(slotLanReferClicked()));
	m_pclBtnLanSend = new QPushButton("Send");
	connect(m_pclBtnLanSend, SIGNAL(clicked()), this, SLOT(slotLanSendClicked()));

	m_pclChkHex = new QCheckBox("HEX");
	QPushButton *pclBtnComClear = new QPushButton("Clear");
	connect(pclBtnComClear, SIGNAL(clicked()), this, SLOT(slotComClearClicked()));
	QPushButton *pclBtnLanClear = new QPushButton("Clear");
	connect(pclBtnLanClear, SIGNAL(clicked()), this, SLOT(slotLanClearClicked()));

	m_pclCmbSerial = new QComboBox();

	m_pclPortInfo = QSerialPortInfo::availablePorts();
	foreach(QSerialPortInfo info, m_pclPortInfo){
		qDebug() << "Name        : " << info.portName();
		qDebug() << "Description : " << info.description();
		qDebug() << "Manufacturer: " << info.manufacturer();
		m_pclCmbSerial->addItem(info.portName());
	}
	// COM
	m_pclSerial = new QSerialPort("COM1");
	connect(m_pclSerial, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));

	// Eth
	m_pclSocket = new QTcpSocket(this);
//	m_pclSocket->setProxy(QNetworkProxy::NoProxy);
	QNetworkProxyFactory::setUseSystemConfiguration(false);
	connect(m_pclSocket, SIGNAL(connected()), this, SLOT(tcpConnect()));
	connect(m_pclSocket, SIGNAL(disconnected()), this, SLOT(tcpDisconnect()));
	connect(m_pclSocket, SIGNAL(readyRead()), this, SLOT(slotReadyReadLan()));
	connect(m_pclSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));

	QHBoxLayout *layout1 = new QHBoxLayout;
	layout1->addWidget(m_pclBtnOpen);
	layout1->addWidget(m_pclCmbSerial);
	layout1->addStretch();
	layout1->addWidget(m_pclChkHex);
	layout1->addWidget(pclBtnComClear);

	QHBoxLayout *layout2 = new QHBoxLayout;
	layout2->addWidget(m_pclEdtComPath);
	layout2->addWidget(m_pclBtnComRefer);

	QHBoxLayout *layout3 = new QHBoxLayout;
	layout3->addWidget(m_pclEdtComSendData);
	layout3->addWidget(m_pclBtnComSend);

	QFrame *line = new QFrame;
	line->setFrameStyle(QFrame::HLine);

	QHBoxLayout *layout1b = new QHBoxLayout;
	layout1b->addWidget(m_pclBtnConnect);
	layout1b->addWidget(m_pclEdtAddr, 3);
	layout1b->addWidget(m_pclEdtPort, 1);
	layout1b->addWidget(pclBtnLanClear);

	QHBoxLayout *layout2b = new QHBoxLayout;
	layout2b->addWidget(m_pclEdtLanPath);
	layout2b->addWidget(m_pclBtnLanRefer);

	QHBoxLayout *layout3b = new QHBoxLayout;
	layout3b->addWidget(m_pclEdtLanSendData);
	layout3b->addWidget(m_pclBtnLanSend);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(new QLabel("COM"));
	layout->addWidget(m_pclEdtText);
	layout->addLayout(layout1);
	layout->addLayout(layout2);
	layout->addLayout(layout3);
	layout->addWidget(new QLabel("Eth"));
	layout->addWidget(m_pclEdtLanText);
	layout->addLayout(layout1b);
	layout->addLayout(layout2b);
	layout->addLayout(layout3b);
	this->setLayout(layout);
}

Widget::~Widget()
{
	QSettings settings(qApp->applicationDirPath() + "\\setting.ini", QSettings::IniFormat);
	settings.setValue("ADDR", m_pclEdtAddr->text());
	settings.setValue("PORT", m_pclEdtPort->text());
	settings.setValue("COMPATH", m_pclEdtComPath->text());
	settings.setValue("LANPATH", m_pclEdtLanPath->text());
}

void Widget::slotReadyRead()
{
	QByteArray data = m_pclSerial->readAll();
	if(m_pclChkHex->isChecked()){
		m_pclEdtText->append(data.toHex());
	}
	else{
		m_pclEdtText->append(data);
	}
}

void Widget::slotOpenClicked()
{
	if(!m_pclSerial->isOpen()){
		m_pclSerial->setPort(m_pclPortInfo[m_pclCmbSerial->currentIndex()]);
		if (m_pclSerial->open(QIODevice::ReadWrite)){
			m_pclEdtText->append("--- open " + m_pclCmbSerial->currentText() + " ---");
			m_pclBtnOpen->setText("close");
		}
		else{	//error
			qDebug() << "open error";
			QMessageBox clMsgBox( QMessageBox::Critical, NULL, tr("open error"), QMessageBox::Ok, this);
			clMsgBox.exec();
		}
	}
	else{
		m_pclSerial->close();
		m_pclEdtText->append("--- close ---");
		m_pclBtnOpen->setText("open");
	}
}

void Widget::slotConnectClicked()
{
	if(!m_pclSocket->isOpen()){
		qDebug() << m_pclEdtAddr->text() << m_pclEdtPort->text();
		m_pclSocket->connectToHost(m_pclEdtAddr->text(), m_pclEdtPort->text().toInt());
	}
	else{
		m_pclSocket->close();
	}
}

void Widget::slotComReferClicked()
{
	QFileDialog::Options options;
	QString filePath = QFileDialog::getOpenFileName(this, tr("open file"), m_pclEdtComPath->text()
										, "BIN (*.bin);;ALL (*.*)", NULL, options);
	if(filePath == ""){
		return;	//cancel
	}
	m_pclEdtComPath->setText(filePath);
	m_pclEdtComSendData->clear();
}

void Widget::slotLanReferClicked()
{
	QFileDialog::Options options;
	QString filePath = QFileDialog::getOpenFileName(this, tr("open file"), m_pclEdtLanPath->text()
										, "BIN (*.bin);;ALL (*.*)", NULL, options);
	if(filePath == ""){
		return;	//cancel
	}
	m_pclEdtLanPath->setText(filePath);
	m_pclEdtLanSendData->clear();
}

void Widget::slotComSendClicked()
{
	if(m_pclEdtComSendData->text() != ""){
		m_pclEdtText->append("S>" + m_pclEdtComSendData->text());
		if(m_pclChkHex->isChecked()){
			QByteArray data;
			for(int i = 0; i < m_pclEdtLanSendData->text().length() / 2; i++){
				data.append(m_pclEdtLanSendData->text().mid(i * 2, 2).toInt(NULL, 16));
			}
			m_pclSocket->write(data);
		}
		else{
			m_pclSocket->write(m_pclEdtComSendData->text().toLocal8Bit());
		}
	}
	else{
		QFile file(m_pclEdtComPath->text());
		if(file.open(QIODevice::ReadOnly)){
			QByteArray data = file.readAll();
			m_pclEdtText->append("S>" + data);
			m_pclSerial->write(data);
		}
	}
}

void Widget::slotLanSendClicked()
{
	if(m_pclEdtLanSendData->text() != ""){
		m_pclEdtLanText->append("S>" + m_pclEdtLanSendData->text());
		if(m_pclChkHex->isChecked()){
			QByteArray data;
			for(int i = 0; i < m_pclEdtLanSendData->text().length() / 2; i++){
				data.append(m_pclEdtLanSendData->text().mid(i * 2, 2).toInt(NULL, 16));
			}
			m_pclSocket->write(data);
		}
		else{
			m_pclSocket->write(m_pclEdtLanSendData->text().toLocal8Bit());
		}
	}
	else{
		QFile file(m_pclEdtLanPath->text());
		if(file.open(QIODevice::ReadOnly)){
			QByteArray data = file.readAll();
			m_pclEdtLanText->append("S>" + data);
			m_pclSocket->write(data);
		}
	}
}

void Widget::slotComClearClicked()
{
	m_pclEdtText->clear();
}

void Widget::slotLanClearClicked()
{
	m_pclEdtLanText->clear();
}

void Widget::slotReadyReadLan()
{
	QByteArray data = m_pclSocket->readAll();
	if(m_pclChkHex->isChecked()){
		m_pclEdtLanText->append("R>" + data.toHex());
	}
	else{
		m_pclEdtLanText->append("R>" + data);
	}
}

void Widget::tcpConnect()
{
	qDebug() << "tcpConnect";
	m_pclEdtLanText->append("--- connect to " + m_pclEdtAddr->text() + " ---");
	m_pclBtnConnect->setText("disconnect");
}

void Widget::tcpDisconnect()
{
	qDebug() << "tcpClose";
	m_pclEdtLanText->append("--- disconnect ---");
	m_pclSocket->close();
	m_pclBtnConnect->setText("connect");
}

void Widget::error()
{
	qDebug() << "connect error";
	m_pclSocket->close();
	QMessageBox clMsgBox( QMessageBox::Critical, NULL, tr("connect error"), QMessageBox::Ok ,this);
	clMsgBox.exec();
}
