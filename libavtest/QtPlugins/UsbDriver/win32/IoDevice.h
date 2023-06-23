/*
 * iodevice.h
 *
 * by Yutaka Hibino
 *
 * Copyright (c) 2004 ELMO CO., LTD.
 */

#ifndef __IODEVICE_H__
#define __IODEVICE_H__

#include "ClUsbDriver_global.h"

/**
 * I/O Class for a USB Device
 */
class IoDevice
{
public:
	IoDevice(const GUID* ) : m_guid(0) {}
	virtual ~IoDevice() {}

	enum IO_RESULT {
		IO_SUCCESS,
		IO_FAILURE,
		IO_TIMEOUT_TRUE,
		IO_TIMEOUT_FALSE,
		IO_WAIT_ABANDONED,
		IO_INVALID_HANDLE,
		IO_NOT_CONNECT
	};

	virtual BOOL IsAlive() = 0;
	virtual BOOL OpenReadEP() = 0;  // PIPE0
	virtual void CloseReadEP() = 0;
	virtual BOOL OpenWriteEP() = 0; // PIPE1
	virtual void CloseWriteEP() = 0;
	virtual BOOL OpenReadEP2() = 0;  // PIPE0
	virtual void CloseReadEP2() = 0;
	virtual BOOL OpenWriteEP2() = 0; // PIPE1
	virtual void CloseWriteEP2() = 0;
	virtual BOOL IsReadEPOpen() = 0;
	virtual BOOL IsWriteEPOpen() = 0;
	virtual BOOL IsReadEPOpen2() = 0;
	virtual BOOL IsWriteEPOpen2() = 0;
	virtual BOOL ResetPipe() = 0;
	virtual BOOL FlushDriverLog() = 0;

	virtual IO_RESULT ReadEP(LPVOID lpBuffer, DWORD nLength, LPDWORD lpnLength, DWORD dwTimeout) = 0;
	virtual IO_RESULT WriteEP(LPCVOID lpBuffer, DWORD nLength, LPDWORD lpnLength, DWORD dwTimeout) = 0;
	virtual IO_RESULT ReadEP2(LPVOID lpBuffer, DWORD nLength, LPDWORD lpnLength, DWORD dwTimeout) = 0;
	virtual IO_RESULT WriteEP2(LPCVOID lpBuffer, DWORD nLength, LPDWORD lpnLength, DWORD dwTimeout) = 0;

protected:
	IoDevice() {}
	GUID* m_guid;
	virtual BOOL GetOneDeviceName(IN HDEVINFO HardwareDeviceInfo,
						  IN PSP_INTERFACE_DEVICE_DATA DeviceInfoData,
						  IN char* devName,
						  size_t devNameLen) = 0;
	virtual BOOL GetUsbDeviceName(char* outNameBuf, size_t outNameBufLen) = 0;
	virtual HANDLE OpenFile(char* filename) = 0;
private:
	virtual BOOL SetGuid(const GUID* guid) = 0;
};

#endif // #ifndef __USBDEVICE_H__
