#ifndef NDADRIVER_WIN32_H
#define NDADRIVER_WIN32_H

#include "ClNDADriver/ClNDADriver_global.h"

#include <windows.h>
#include <windowsx.h>
#include <winioctl.h>
#include <WinNT.h>

#include "Sxuptp_win.h"

//--------------------------------------------------------------------------------
//	Constant definition
//--------------------------------------------------------------------------------
#define MAX_PSERVER			16
#define MAX_DEVICE			64

#define STATUS_AVAILABLE	1
#define STATUS_CONNECTED	2
#define STATUS_INUSE		3
#define STATUS_NOTFOUND		4

//--------------------------------------------------------------------------------
//	Struct definition
//--------------------------------------------------------------------------------
typedef struct _SXUSBDEVINFO {
	char	szPortName[5120];				// ex: dev01,dev02,dev03
	TCHAR	szDeviceName[SXUPTP_DEVICE];
	DWORD	dwIpPc;
	WORD	wStatus;
	WORD	wVid;
	WORD	wPid;
	char	szLocation[3072];				// ex: M112M0,M112M1,M112M2
	WORD	wClass;
} SXUSBDEVINFO, *LPSXUSBDEVINFO;


//--------------------------------------------------------------------------------
//	Silex Driver class
//--------------------------------------------------------------------------------------
class ClNDADriver : public QWidget
{
	Q_OBJECT

public:
	ClNDADriver(QWidget *parent);
	virtual ~ClNDADriver();

	//initialize
	virtual qint32 initialize( QStringList clStrListServerIP, qint32 lRuleNo );				//initialize

	//get flg status
	virtual bool isInitialize();															//return whether is it already initialized

	//set broadcast address
	virtual void setBroadcastAddress( QStringList clStrListServerIP );						//

	//get infomation
	QMap<qint32, QString> getSrvIPAddList(){return m_clListIP;}								//
	QMap<qint32, QString> getSrvMacAddList(){return m_clListMac;}							//
	QMap<qint32, QString> getSrvHostNameList(){return m_clListHostName;}					//

	QMap<qint32, QString> getSrvNameList(){return m_clListName;}							//
	QMap<qint32, QString> getSrvDevList(qint32 lIndexSrv){return m_clListDevice[lIndexSrv];}//
protected:

	//search
	virtual QStringList searchDevice();														//search device

	virtual QMap<qint32, QString> getDeviceList(qint32 lIndex);								//

	//open/close
	virtual qint32 openDevice(LPSXPSERVER lpsxsrv, LPSXUSBDEVINFO lpsxdev);					//open for a device
	virtual qint32 closeDevice(LPSXPSERVER lpsxsrv, LPSXUSBDEVINFO lpsxdev);				//close for a device

	//get status
	virtual QString getStatus(qint32 lIndexSrv, qint32 lIndexDev);							//
	virtual QString getSystemStatus(qint32 lIndexSrv);										//
	virtual QString getServerStatus(qint32 lIndexSrv);										//
	virtual QString getInstanceId(qint32 lIndexSrv, qint32 lIndexDev);						//

	//reboot
	virtual BOOL reboot(qint32 lIndexSrv);													//

public:
	void searchSrv(QStringList &clStrListName){ slotSearchDevice(clStrListName); }
	void getDevList(){ slotGetDeviceList(); }
	void getDevStatus(qint32 lIndexSrv, qint32 lIndexDev, QString &clStrStatus){ slotGetStatus( lIndexSrv, lIndexDev, clStrStatus ); }

signals:
	//error
	void sigErrorFatal(qint32 lError);														//is sent when detect fatal error (wanna exit for this application)
	void sigErrorNormal(qint32 lError);														//is sent when detect normal error

	//open/close
	void sigOpenDevice(qint32 lIndex);														//is sent when device open
	void sigCloseDevice(qint32 lIndex);														//is sent when device close

protected slots:
	//search
	void slotSearchDevice(QStringList &clStrListName);										//

	//request device list
	void slotGetDeviceList();																//

	//open/close
	void slotOpenDevice(qint32 lIndexSrv, qint32 lIndexDev);								//
	void slotOpenDeviceAll(qint32 lIndexSrv);												//
	void slotCloseDevice(qint32 lIndexSrv, qint32 lIndexDev);								//
	void slotCloseDeviceAll(qint32 lIndexSrv);												//

	//get status
	void slotGetStatus(qint32 lIndexSrv, qint32 lIndexDev, QString &clStrStatus);			//
	void slotGetSystemStatus(qint32 lIndexSrv, QString &clStrStatus);						//
	void slotGetServerStatus(qint32 lIndexSrv, QString &clStrStatus);						//
	void slotGetInstanceId(qint32 lIndexSrv, qint32 lIndexDev, QString &clStrStatus);		//

	//reboot
	void slotReboot(qint32 lIndexSrv);														//

protected:
	DWORD					m_dwIp[MAX_PSERVER];											//検索するサーバーIPアドレスリスト
	QStringList				m_clStrListSrv;													//検索時に対象とするIPアドレスリスト

	QMap<qint32, QString>	m_clListIP;														//検知したサーバーのIPアドレスリスト
	QMap<qint32, QString>	m_clListMac;													//検知したサーバーのMacアドレスリスト
	QMap<qint32, QString>	m_clListHostName;												//検知したサーバーのホスト名リスト

	QMap<qint32, QString>	m_clListName;													//検出したサーバーの表示文言リスト
	QMap<qint32, QString>	m_clListDevice[56];												//検出したサーバーに属するデバイスリスト Key:index_srv-index_dev

	bool					m_bInitialized;													//初期化フラグ
};
//--------------------------------------------------------------------------------------

#endif // NDADRIVER_WIN32_H
