//--------------------------------------------------------------------------------//
/*!
	@file	ClProcessWidgetBase.h
	@brief	ClProcessWidgetBase
	@author	星島
*/
//--------------------------------------------------------------------------------//
#ifndef CLPROCESSWIDGETBASE_H
#define CLPROCESSWIDGETBASE_H

#include "ClProgressIndicator.h"
#include <QWidget>
#include <QStackedLayout>
#include <QJsonValue>
#include <QJsonArray>


//--------------------------------------------------------------------------------//
/*!
	@brief	ClProcessWidgetBase
*/
//--------------------------------------------------------------------------------//
class ClProcessWidgetBase : public QWidget
{
	Q_OBJECT
public:
	ClProcessWidgetBase(QWidget *pclParent = NULL);				//コンストラクタ
	virtual ~ClProcessWidgetBase();								//デストラクタ

	virtual bool initalize();									//初期化
	virtual QString getProcessName(){return m_strProcessName;}	//プロセス名取得
	virtual void setOverlayIndicator(bool bIsUse);				//オーバーレイインジケーター

public slots:

signals:

protected:
	virtual QWidget *createStartWidget();						// 開始Widget
	virtual QWidget *createIndicatorWidget();					// インジケータ用Widget
	virtual QByteArray getObject(QString strCmd);

	QByteArray encrypt(const QByteArray qCommand);
	QByteArray decrypt(QByteArray qArrayData);
	QByteArray RSA_encrypt(const QByteArray input);
	QByteArray RSA_decrypt(const QByteArray rsaCrypto);
	QString getSecret(int secretLength = 16);

	QByteArray m_AESKey;						//AES key

protected slots:

private:
	QStackedLayout *m_pclProcessLayout;			//!< スタックドレイヤ
	QString m_strProcessName;					// プロセス名
	QLabel *m_pclLbIndicator;					//　インジケータ用ラベル
	ClProgressIndicator *m_pclIndicator;
};

#endif // CLPROCESSWIDGETBASE_H
