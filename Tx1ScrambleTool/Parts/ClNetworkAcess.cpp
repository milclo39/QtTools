//--------------------------------------------------------------------------------//
/*!
	@file	ClNetworkAcess.cpp
	@brief	ClNetworkAcess
	@author	星島
*/
//--------------------------------------------------------------------------------//
#include "ClNetworkAcess.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QNetworkProxy>
#include "Common/CC1Command.h"


//--------------------------------------------------------------------------------//
/*!
	@brief	コンストラクタ
*/
//--------------------------------------------------------------------------------//
ClNetworkAcess::ClNetworkAcess(QObject *pclParent) : QObject(pclParent)
{

}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief	デストラクタ
*/
//--------------------------------------------------------------------------------//
ClNetworkAcess::~ClNetworkAcess()
{

}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief	初期化
*/
//--------------------------------------------------------------------------------//
void ClNetworkAcess::initialize()
{
	m_pclManager = new QNetworkAccessManager(this);
	connect(m_pclManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slotPostFin(QNetworkReply*)));
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief	serverへ通知
	@param[in]	qCmd : コマンド
	@param[in]	strUrl : リクエストURL
*/
//--------------------------------------------------------------------------------//
void ClNetworkAcess::SendServer(QByteArray qCmd, QString strUrl)
{
	QNetworkRequest clReqest = QNetworkRequest();
	clReqest.setUrl(QUrl(strUrl));									//リクエストURL
	QSslConfiguration qsslConf = clReqest.sslConfiguration();
	qsslConf.setProtocol(QSsl::TlsV1_0);
	clReqest.setSslConfiguration(qsslConf);							//sslセット
	clReqest.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));		//contextを指定


	qDebug() << qCmd;
	m_pclReply = m_pclManager->post(clReqest, qCmd);

}
//--------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------//
/*!
	@brief	Proxy設定
	@param[in]	strAddr : アドレス
	@param[in]	nPort : ポート
	@param[in]	nType : タイプ
*/
//--------------------------------------------------------------------------------//
void ClNetworkAcess::SetProxy(QString strAddr, int nPort, int nType)
{
	QNetworkProxy proxy;
	proxy.setHostName(strAddr);
	proxy.setPort(nPort);
	proxy.setType((QNetworkProxy::ProxyType)nType);
	m_pclManager->setProxy(proxy);
}
//--------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------//
/*!
	@brief	返信
	@param[in]	pclReply : 戻り値
*/
//--------------------------------------------------------------------------------//
void ClNetworkAcess::slotPostFin(QNetworkReply *pclReply)
{
	if(pclReply->error() != QNetworkReply::NoError){
		QByteArray res = pclReply->readAll();
		qDebug() << __LINE__ << res;
		emit sigRecvServer(res);
	}
	else
	{
		QByteArray res = pclReply->readAll();
		qDebug() << __LINE__ << res;
		emit sigRecvServer(res);
	}


}
//--------------------------------------------------------------------------------//
