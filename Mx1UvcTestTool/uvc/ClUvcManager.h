#ifndef CLUVCMANAGER_H
#define CLUVCMANAGER_H

#include <QObject>
#include <QList>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <io.h>
#include <WinSock2.h>
#include <windows.h>
#include <Windows.h>
#include "tpcshrd.h"
#include <Dbt.h>

//#define USE_XUCTRLLIB
#define TARGET_MODEL	1	//0:Tx-1 1:Mx-1
#ifndef USE_XUCTRLLIB
#include "UvcDriverTxLx1/ClUvcDriverTxLx1.h"
#include "UvcDriverMx1/ClUvcDriverMx1.h"
#endif

typedef enum {
  kSuccess = 0,
  kReset,
  kErrorInvalidParam,
  kErrorInvalidStatus,
  kErrorTimeout,
  kErrorCritical,
} kTransferResult;


class ClUvcManager : public QObject
{
	Q_OBJECT
public:
	ClUvcManager();
	~ClUvcManager();

	bool deviceChangeCheck();
	virtual kTransferResult Init(const char* cmdFile);

	QByteArray sendData(QByteArray qArray);

signals:
	void sigChangedDevice(bool bIsConnect);

private:
	QList<HDEVNOTIFY> m_clDeviceNotifyList;	//!< デバイス変化検出ハンドルリスト
#ifndef USE_XUCTRLLIB
	ClUvcDriver *m_pUvcDriver;
#endif
	bool m_bIsDeviceEnabled;
};

#endif // CLUVCMANAGER_H
