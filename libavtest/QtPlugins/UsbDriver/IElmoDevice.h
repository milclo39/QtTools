/*------------------------------------------------------------------------------*/
/*!
	@file		IElmoDevice.h

	@brief		Interface to operate a device

	@author		株式会社エルモ社  映像技術第二部

	Confidential

	Copyright(c) Elmo Industrial Co.,Ltd.		http://www.elmo.co.jp/
*/
/*------------------------------------------------------------------------------*/
#pragma once

/*------------------------------------------------------------------------------*/
/*!
 * @class	IElmoDevice
 */
/*------------------------------------------------------------------------------*/
#include "IDeviceInfo.h"

class IElmoDevice
{
public:
	virtual ~IElmoDevice(){}										//destructor
	virtual IDeviceInfo *getIDeviceInfo() = 0;						//Get IDeviceInfo from a device
	virtual WORD wGetDeviceID() = 0;								//Get DeviceID from a device

	virtual	BOOL bOpen() = 0;										//open a device
	virtual	BOOL bClose() = 0;										//close a device
	virtual	size_t uiRead(char *buf, size_t size) = 0;				//Read data from a device
	virtual	size_t uiWrite(char *buf, size_t size) = 0;				//Write data to a device
	virtual	int iIoctl(ULONG lCmd, ULONG lArg) = 0;					//Send command to a device
};

