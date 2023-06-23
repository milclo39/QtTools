#ifndef CLPRESENTERCO10_H
#define CLPRESENTERCO10_H
#include "mac/ClDeviceInfo_mac.h"
#include "ClUsbDriver_global.h"
#include <QMutex>

class ClMacObject;

//--------------------------------------------------------------------------------//
/*!
	@brief		コマンド送信スレッド
*/
//--------------------------------------------------------------------------------//
class ClCommandThread : public QThread
{
	Q_OBJECT

public:
	ClCommandThread();
	void stop(){m_bIsStop = true;}
	void setCommand(qint32 lCommand){m_lSendCommand = lCommand;}
	qint32 getCommand(){return m_lSendCommand;}
	void setObject(ClMacObject *pclObject){m_pclObject = pclObject;}
	void setMutex(QMutex *pclMutex){m_pclCommandMutex = pclMutex;}

	void execFocus(qint32 lAdd);

protected:
	void run();

protected:
	bool m_bIsStop;
	qint32 m_lSendCommand;
	ClMacObject *m_pclObject;
	QMutex *m_pclCommandMutex;
	qint32 m_lFocusValue;
};
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		CO-10制御クラス
*/
//--------------------------------------------------------------------------------//
class ClPresenterCo10 : public ClDeviceInfo
{
	Q_OBJECT

public:
	ClPresenterCo10(qint32 lProductId = PRODUCT_CO10, QObject *parent = 0);
	virtual ~ClPresenterCo10();

	virtual qint32 open();
	virtual qint32 CommandPresenter(qint32 CmdID, QVariant &qParameter);
	virtual void setConnected(bool bIsConnect);
	virtual void checkRestart();

protected:
	virtual qint32 getPicture(QVariant &qParam);

	virtual qint32 getModelName(QVariant &qParam);
	virtual qint32 getVersion(QVariant &qParam);

private:
	ClMacObject *m_pclObject;
	ClCommandThread m_clThread;
	bool m_bIsConnect;
	QMutex m_clCommandMutex;
};
//--------------------------------------------------------------------------------//
#endif
