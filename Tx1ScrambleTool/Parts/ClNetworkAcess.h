//--------------------------------------------------------------------------------//
/*!
	@file	ClNetworkAcess.h
	@brief	ClNetworkAcess
	@author	星島
*/
//--------------------------------------------------------------------------------//
#ifndef CLNETWORKACESS_H
#define CLNETWORKACESS_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

//--------------------------------------------------------------------------------//
/*!
	@brief	ClNetworkAcess
*/
//--------------------------------------------------------------------------------//
class ClNetworkAcess : public QObject
{
	Q_OBJECT
public:
	ClNetworkAcess(QObject *pclParent = NULL);				//コンストラクタ
	virtual ~ClNetworkAcess();								//デストラクタ

	void initialize();										//初期化
	void SendServer(QByteArray qCmd, QString strUrl);
	void SetProxy(QString strAddr, int nPort, int nType);	// プロキシ設定

signals:
	/*!
		@brief		serverからの返信コマンド通知
		@param[in]	qRecvData : 返信データ
	*/
	void sigRecvServer(QByteArray qRecvData);

protected slots:
	void slotPostFin(QNetworkReply* pclReply);				//serverからの戻り
private:
	QNetworkAccessManager *m_pclManager;
	QNetworkReply *m_pclReply;

};

#endif // CLNETWORKACESS_H
