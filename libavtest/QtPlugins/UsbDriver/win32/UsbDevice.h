// UsbDevice.h: UsbDevice
//
//////////////////////////////////////////////////////////////////////

#ifndef __USBDEVICE_H__
#define __USBDEVICE_H__

#include "ClUsbDriver_global.h"
#include "iodevice.h"

#define MUTEX_NAME_USBDEV		L"EL_USBDEV_MUTEX"
/**
 * I/O Class for a USB Device
 */
class ClIoDevice : public IoDevice
{
public:
	ClIoDevice(const GUID* guid);
	virtual ~ClIoDevice();

	virtual BOOL IsAlive();
	virtual BOOL OpenReadEP();  // PIPE0 EP1
	virtual void CloseReadEP();
	virtual BOOL OpenWriteEP(); // PIPE1 EP2
	virtual void CloseWriteEP();
	virtual BOOL OpenReadEP2();  // PIPE0 EP3
	virtual void CloseReadEP2();
	virtual BOOL OpenWriteEP2(); // PIPE1 EP4
	virtual void CloseWriteEP2();
	virtual BOOL IsReadEPOpen();
	virtual BOOL IsWriteEPOpen();
	virtual BOOL IsReadEPOpen2();
	virtual BOOL IsWriteEPOpen2();
	virtual BOOL ResetPipe();
	virtual BOOL FlushDriverLog();

	virtual IoDevice::IO_RESULT ReadEP(LPVOID lpBuffer, DWORD nLength, LPDWORD lpnLength, DWORD dwTimeout);
	virtual IoDevice::IO_RESULT WriteEP(LPCVOID lpBuffer, DWORD nLength, LPDWORD lpnLength, DWORD dwTimeout);
	virtual IoDevice::IO_RESULT ReadEP2(LPVOID lpBuffer, DWORD nLength, LPDWORD lpnLength, DWORD dwTimeout);
	virtual IoDevice::IO_RESULT WriteEP2(LPCVOID lpBuffer, DWORD nLength, LPDWORD lpnLength, DWORD dwTimeout);
protected:
	enum {PIPE_NAME_LEN = 32};
	enum {FILE_NAME_LEN = 256};

	HANDLE m_inPipeHandle;
	HANDLE m_outPipeHandle;
	HANDLE m_inPipeHandle2;
	HANDLE m_outPipeHandle2;

	OVERLAPPED m_WOverlapped;
	OVERLAPPED m_ROverlapped;
	OVERLAPPED m_WOverlapped2;
	OVERLAPPED m_ROverlapped2;

	char m_inPipeName[PIPE_NAME_LEN];    // pipe name for EP1
	char m_outPipeName[PIPE_NAME_LEN];    // pipe name for EP2
	char m_inPipeName2[PIPE_NAME_LEN];    // pipe name for EP1
	char m_outPipeName2[PIPE_NAME_LEN];    // pipe name for EP2

	char m_device_name[FILE_NAME_LEN];//generated from the GUID registered by the driver itself
	char m_file_name[FILE_NAME_LEN];

	virtual BOOL GetOneDeviceName(IN HDEVINFO HardwareDeviceInfo,
						  IN PSP_INTERFACE_DEVICE_DATA DeviceInfoData,
						  IN char* devName,
						  size_t devNameLen);
	virtual BOOL GetUsbDeviceName(char* outNameBuf, size_t outNameBufLen);

	virtual HANDLE OpenFile(char* filename);
private:
	ClIoDevice() {}
	virtual BOOL SetGuid(const GUID* guid);
private:
	bool m_bReadEPOpen;
	bool m_bWriteEPOpen;
	bool m_bReadEPOpen2;
	bool m_bWriteEPOpen2;

	HANDLE m_hMutex; //
};

#endif // #ifndef __USBDEVICE_H__
