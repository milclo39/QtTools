#include "widget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>

Widget::Widget(QWidget *parent) : QWidget(parent)
{
	m_pclEdtText = new QLineEdit;
	m_pclEdtPos = new QLineEdit;
	m_pclEdtPrs = new QLineEdit;

	m_pclBtnOpen = new QPushButton("Open");
	connect(m_pclBtnOpen, SIGNAL(clicked()), this, SLOT(slotOpenClicked()));


	m_pclBtnSend = new QPushButton("Send");
	connect(m_pclBtnSend, SIGNAL(clicked()), this, SLOT(slotSendClicked()));
//	m_pclCmbSerial = new QComboBox();

	QHBoxLayout *layout1 = new QHBoxLayout;
	layout1->addWidget(m_pclBtnOpen);
	layout1->addWidget(m_pclBtnSend);

	QHBoxLayout *layout2 = new QHBoxLayout;
	layout2->addWidget(m_pclEdtPos);
	layout2->addWidget(m_pclEdtPrs);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(m_pclEdtText);
	layout->addLayout(layout1);
	layout->addLayout(layout2);
	this->setLayout(layout);
}

Widget::~Widget()
{
}

void Widget::slotOpenClicked()
{
	if(m_pclBtnOpen->text() == "Open"){
		m_pclHidThread = new ClHidThread;
		connect(m_pclHidThread, SIGNAL(sigRecv(QByteArray)), this, SLOT(slotRecv(QByteArray)));
		m_pclBtnOpen->setText("Close");
	}
	else{
		disconnect(m_pclHidThread, SIGNAL(sigRecv(QByteArray)), this, SLOT(slotRecv(QByteArray)));
		delete m_pclHidThread;
		m_pclBtnOpen->setText("Open");
	}
}

void Widget::slotSendClicked()
{
	qDebug() << m_pclHidThread->isRunning();
}

void Widget::slotRecv(QByteArray clData)
{
	QString str;
	for(int i = 0; i < clData.size(); i++){
		str.append(QString::number((quint8)clData[i], 16).rightJustified(2, '0') + " ");
	}
	m_pclEdtText->setText(str);
	if(m_pclHidThread->m_iVid == 0x09a1){	// elmo
		m_pclEdtPos->setText(QString::number((clData[3] << 8) + (quint8)clData[2]) + "," +QString::number((clData[5] << 8) + (quint8)clData[4]));
		m_pclEdtPrs->setText(QString::number((clData[7] << 8) + (quint8)clData[6]));
	}
	else{	// wacom
		m_pclEdtPos->setText(QString::number((clData[3] << 8) + (quint8)clData[4]) + "," +QString::number((clData[5] << 8) + (quint8)clData[6]));
		m_pclEdtPrs->setText(QString::number((quint8)clData[7]));
	}
}
