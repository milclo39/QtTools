#include "widget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QDateTime>

#define POLLING_TIME	200

#define METHOD_LIGHT	"LIGHT"
#define METHOD_VERSION	"VERSION"
#define METHOD_TELE		"TELEKEY"
#define METHOD_WIDE		"WIDEKEY"
#define METHOD_FREEZE	"FREEZEKEY"
#define METHOD_KEYSTATE	"KEYSTATE"

//--------------------------------------------------------------------------------//
/*!
	@brief	コンストラクタ
*/
//--------------------------------------------------------------------------------//
Widget::Widget(QWidget *parent) : QWidget(parent)
{
//	m_pclLiveWidget = NULL;
	initialize();
	m_bStart = false;
	m_iJsonId = 0;
}

Widget::~Widget()
{
	slotLiveClosed();
}

//--------------------------------------------------------------------------------//
/*!
	@brief	初期化
*/
//--------------------------------------------------------------------------------//
void Widget::initialize()
{
	m_pclEdtText = new QTextEdit("");
	m_pclEdtSend = new QLineEdit("rtsp://192.168.1.25/stream");
	m_pclEdtPolling = new QLineEdit(QString::number(POLLING_TIME));

	m_pclBtnSend = new QPushButton("Start");
	connect(m_pclBtnSend, SIGNAL(clicked()), this, SLOT(slotSendClicked()));

	m_pclBtnMode = new QPushButton("to Station");
	connect(m_pclBtnMode, SIGNAL(clicked()), this, SLOT(slotModeClicked()));

	m_pclBtnClear = new QPushButton("Clear");
	connect(m_pclBtnClear, SIGNAL(clicked()), this, SLOT(slotClearClicked()));

	m_pclBtnLive = new QPushButton("Live");
	connect(m_pclBtnLive, SIGNAL(clicked()), this, SLOT(slotLiveClicked()));

	m_pclLblZoomPos = new QLabel;
	m_pclZoomLevel = new QSlider(Qt::Horizontal);
	m_pclZoomLevel->setTickPosition(QSlider::TicksBelow);
	m_pclZoomLevel->setTickInterval(100);
	m_pclZoomLevel->setMaximum(1000);
	connect(m_pclZoomLevel, SIGNAL(sliderMoved(qint32)), this, SLOT(slotSliderMoved(qint32)));
	slotSliderMoved(0);

	// スライダー
	QHBoxLayout *layoutSld = new QHBoxLayout;
	layoutSld->addWidget(m_pclLblZoomPos);
	layoutSld->addWidget(m_pclZoomLevel);

	// ボタン群
	QHBoxLayout *layoutBtn = new QHBoxLayout;
	layoutBtn->addWidget(m_pclBtnSend);
//	layoutBtn->addWidget(m_pclBtnLive);
	{
		QLabel *pclLabel = new QLabel("Polling(ms)");
		layoutBtn->addWidget(pclLabel);
	}
	layoutBtn->addWidget(m_pclEdtPolling);

	QVBoxLayout *layout = new QVBoxLayout;
//	layout->addWidget(m_pclEdtSend);
	layout->addWidget(m_pclEdtText);
	layout->addLayout(layoutSld);
	layout->addLayout(layoutBtn);
	this->setLayout(layout);

	connect(this, SIGNAL(sigReceiveData(QByteArray)), this, SLOT(slotReceiveData(QByteArray)));
	connect(&m_pclUvc, SIGNAL(sigChangedDevice(bool)), this, SLOT(slotChangedDevice(bool)));

	if(m_pclUvc.Init(NULL) != kSuccess){
		qWarning("Could not open uvc drive.");
		this->setEnabled(false);
//		return;
	}else{
		this->setEnabled(true);
	}

	m_pclActTimer = new QTimer;
	connect(m_pclActTimer, SIGNAL(timeout()), this, SLOT(slotTimerProc()));
}

QJsonObject Widget::getObject(QString strMethod)
{
	QJsonObject	l_clObjParamRoot;
	QJsonObject jsonObjDetail;
	QJsonArray array;

	if(strMethod == "ZOOM"){
		jsonObjDetail["GET"] = "CURRENT";
		array.append(jsonObjDetail);
	}
	l_clObjParamRoot["id"] = ++m_iJsonId;
	l_clObjParamRoot["jsonrpc"] = "2.0";
	l_clObjParamRoot["method"] = strMethod;
	l_clObjParamRoot["params"]= array;

	m_strMethod = strMethod;
	return l_clObjParamRoot;
}

void Widget::slotReceiveData(QByteArray data)
{
	QJsonParseError clErr;
	QJsonDocument jDoc = QJsonDocument::fromJson(data,&clErr);
	QString strDate = QDateTime::currentDateTime().toString("hh:mm:ss.zzz ");

	if(!jDoc.isObject() || (true == jDoc.object().isEmpty())){
		m_pclEdtText->append(strDate + "No Response");
//		slotSendClicked();
		return;
	}
	else if(true == jDoc.object().keys().contains("error")){
		m_pclEdtText->append(strDate + "Response Error");
//		slotSendClicked();
		return;
	}
	if(m_strMethod == "ZOOM"){
		int param = jDoc.object().find("result").value().toArray().at(0).toObject().find("CURRENT").value().toInt();
		int value = m_pclZoomLevel->value();
		if(m_iPreZoom < param){
			if(value < m_pclZoomLevel->maximum()){
				m_pclZoomLevel->setValue(value + 1);
			}
		}
		else if(m_iPreZoom > param){
			if(value > m_pclZoomLevel->minimum()){
				m_pclZoomLevel->setValue(value - 1);
			}
		}
		m_iPreZoom = param;
	}
	else if(m_strMethod == METHOD_TELE){
		QJsonObject objResult = jDoc.object().find("result").value().toArray().at(0).toObject();
		int iTele = objResult.find("STATE").value().toInt();
		int value = m_pclZoomLevel->value();
		if(iTele == 1){
			m_pclEdtText->append(strDate + " Tele Key Press");
			if(value < m_pclZoomLevel->maximum()){
				m_pclZoomLevel->setValue(value + 1);
				slotSliderMoved(value + 1);
			}
		}
	}
	else if(m_strMethod == METHOD_KEYSTATE){
		QJsonObject objResult = jDoc.object().find("result").value().toArray().at(0).toObject();
		int iState = objResult.find("STATE").value().toInt();

		if(iState == 6){
			m_pclEdtText->append(strDate + " Tele+Wide Key Press");
			return;	// 同時押しは無効
		}
		int value = m_pclZoomLevel->value();
		if(iState == 1){
			m_pclEdtText->append(strDate + " Tele Key Press");
			if(value < m_pclZoomLevel->maximum()){
				m_pclZoomLevel->setValue(value + 1);
				slotSliderMoved(value + 1);
			}
		}
		else if(iState == 2){
			m_pclEdtText->append(strDate + " Wide Key Press");
			if(value > m_pclZoomLevel->minimum()){
				m_pclZoomLevel->setValue(value - 1);
				slotSliderMoved(value - 1);
			}
		}
		else if(iState == 4){
			m_pclEdtText->append(strDate + " Freeze Key Press");
		}
		else{

		}
	}
}

void Widget::slotTimerProc()
{
//	QJsonObject obj = getObject("ZOOM");
	QJsonObject obj = getObject(METHOD_KEYSTATE);
	QJsonDocument doc(obj);
	QByteArray data(doc.toJson(QJsonDocument::Compact));
	qDebug() << "sendData" << data;
	QByteArray recvData;
	recvData = m_pclUvc.sendData(data);
	qDebug() << "recvData" << recvData;

	emit sigReceiveData(recvData);
}

void Widget::slotChangedDevice(bool bIsConnect)
{
	if(true == bIsConnect){
		this->setEnabled(true);
	}else{
		this->setEnabled(false);
	}
}

void Widget::slotSendClicked()
{
	QString strDate = QDateTime::currentDateTime().toString("hh:mm:ss.zzz ");
	if(m_bStart == false){
		m_pclEdtText->append(strDate + " Polling Start");
		qDebug() << m_pclEdtPolling->text().toInt();
		m_pclActTimer->setInterval(m_pclEdtPolling->text().toInt());
		m_pclActTimer->start();
		m_pclBtnSend->setText("Stop");
		m_bStart = true;
	}
	else{
		m_pclEdtText->append(strDate + " Polling Stop");
		m_pclActTimer->stop();
		m_pclBtnSend->setText("Start");
		m_bStart = false;
	}
}

void Widget::slotClearClicked()
{
	m_pclEdtText->clear();
}

void Widget::slotModeClicked()
{
}

//--------------------------------------------------------------------------------//
/*!
	@brief	ライブ押下
*/
//--------------------------------------------------------------------------------//
void Widget::slotLiveClicked()
{
#if 0
	QString strUrl = QString(m_pclEdtSend->text());
	if(m_pclLiveWidget == NULL){
		m_pclLiveWidget = new ClLiveWidget();
		m_pclLiveWidget->slotStreamStart(strUrl);
		m_pclLiveWidget->show();
		connect(m_pclLiveWidget, SIGNAL(sigClosed()), this, SLOT(slotLiveClosed()));
	}
#endif
}
//--------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------//
/*!
	@brief	ライブ終了
*/
//--------------------------------------------------------------------------------//
void Widget::slotLiveClosed()
{
#if 0
	if(m_pclLiveWidget){
		m_pclLiveWidget->slotStreamStop();
		m_pclLiveWidget = NULL;
		delete m_pclLiveWidget;
	}
#endif
}

void Widget::slotSliderMoved(qint32 value)
{
	m_pclLblZoomPos->setText(QString("%1").arg(value, 4, 10, QChar('0')));
}
