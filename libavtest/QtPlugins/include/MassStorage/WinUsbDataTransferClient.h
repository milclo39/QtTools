//--------------------------------------------------------------------------------//
/*!
	@file	WinUsbDataTransferClient.h
	@brief	MassStorage
	@author	星島
*/
//--------------------------------------------------------------------------------//
#ifndef WINUSBDATATRANSFERCLIENT_H
#define WINUSBDATATRANSFERCLIENT_H

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
#include <QThread>

#include "UsbDataTransfer.h"

namespace spyder{

class WinUsbDataTransferClient : public UsbDataTransfer
{
	Q_OBJECT
public:
	WinUsbDataTransferClient();
	~WinUsbDataTransferClient();

	virtual kTransferResult Init(const char* cmdFile);

	// for HostPC
	virtual kTransferResult SendData(uint8_t *msg, size_t mlen, uint8_t **reply, size_t *rlen);
	virtual kTransferResult SendReset();

	bool deviceChangeCheck();

private:
	void resetServer();
	QString m_strDisc;
};
}
#endif // WINUSBDATATRANSFERCLIENT_H
