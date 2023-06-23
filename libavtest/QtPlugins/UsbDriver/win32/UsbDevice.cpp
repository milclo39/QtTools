// UsbDevice.cpp: ClIoDevice クラスのインプリメンテーション
//
//////////////////////////////////////////////////////////////////////


#include <malloc.h>


#include "ioctl.h"

#if 0
extern "C" {
#pragma warning(disable: 4200)
#include <usbdi.h>
#pragma warning(default: 4200)
}
#endif

#include "UsbDevice.h"

ClIoDevice::ClIoDevice(const GUID* guid) : IoDevice(NULL)
{
	if (!SetGuid(guid)) {
		return;
	}

	m_inPipeHandle = INVALID_HANDLE_VALUE;
	m_outPipeHandle = INVALID_HANDLE_VALUE;
	m_inPipeHandle2 = INVALID_HANDLE_VALUE;
	m_outPipeHandle2 = INVALID_HANDLE_VALUE;

#if 1
	strcpy_s(m_inPipeName, sizeof(m_inPipeName), "PIPE01");
	strcpy_s(m_outPipeName, sizeof(m_outPipeName), "PIPE02");
#else
	strcpy_s(m_inPipeName, sizeof(m_inPipeName), "PIPE03");
	strcpy_s(m_outPipeName, sizeof(m_outPipeName), "PIPE04");
#endif
#if 0
	strcpy_s(m_inPipeName2, sizeof(m_inPipeName2), "PIPE01");
	strcpy_s(m_outPipeName2, sizeof(m_outPipeName2), "PIPE02");
#else
	strcpy_s(m_inPipeName2, sizeof(m_inPipeName2), "PIPE03");
	strcpy_s(m_outPipeName2, sizeof(m_outPipeName2), "PIPE04");
#endif
	strcpy_s(m_device_name, sizeof(m_device_name), "");

	m_WOverlapped.hEvent = NULL;
	m_ROverlapped.hEvent = NULL;
	m_WOverlapped2.hEvent = NULL;
	m_ROverlapped2.hEvent = NULL;

	m_bReadEPOpen = false;
	m_bWriteEPOpen = false;
	m_bReadEPOpen2 = false;
	m_bWriteEPOpen2 = false;

	//InitializeCriticalSectionAndSpinCount(&m_hMutex, 0x80000400); // win 2000対応 ヘルプ参照
	m_hMutex = CreateMutex(NULL, FALSE, MUTEX_NAME_USBDEV);	//ミューテックス生成
}

ClIoDevice::~ClIoDevice()
{
	if (m_guid != NULL) {
		delete m_guid;
	}

	if (m_inPipeHandle != INVALID_HANDLE_VALUE) {
		CloseReadEP();
	}

	if (m_outPipeHandle != INVALID_HANDLE_VALUE) {
		CloseWriteEP();
	}

	if (m_inPipeHandle2 != INVALID_HANDLE_VALUE) {
		CloseReadEP2();
	}

	if (m_outPipeHandle2 != INVALID_HANDLE_VALUE) {
		CloseWriteEP2();
	}


	CloseHandle(m_hMutex);
	//DeleteCriticalSection(&m_hMutex);
}

BOOL ClIoDevice::SetGuid(const GUID* guid)
{
	if (m_guid == NULL) {
		m_guid = new GUID;
	}

	if (m_guid == NULL) {
		return FALSE;
	}

	*(this->m_guid) = *guid;

	return TRUE;
}

BOOL ClIoDevice::GetOneDeviceName(
							 HDEVINFO HardwareDeviceInfo,
							 PSP_INTERFACE_DEVICE_DATA   DeviceInfoData,
							 char *devName,
							 size_t devNameLen
							 )
{
	PSP_INTERFACE_DEVICE_DETAIL_DATA_A functionClassDeviceData = NULL;
	ULONG requiredLength = 0;
	BOOL result = FALSE;

	//
	// allocate a function class device data structure to receive the
	// goods about this particular device.
	//
	SetupDiGetInterfaceDeviceDetail(
			HardwareDeviceInfo,
			DeviceInfoData,
			NULL, // probing so no output buffer yet
			0, // probing so output buffer length of zero
			&requiredLength,
			NULL); // not interested in the specific dev-node

	ULONG predictedLength = requiredLength;

	functionClassDeviceData = (PSP_INTERFACE_DEVICE_DETAIL_DATA_A)malloc(predictedLength);
	if (functionClassDeviceData == NULL) {
		return result;
	}

	functionClassDeviceData->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA_A);

	//
	// Retrieve the information from Plug and Play.
	//
	if (SetupDiGetInterfaceDeviceDetailA(
										HardwareDeviceInfo,
										DeviceInfoData,
										functionClassDeviceData,
										predictedLength,
										&requiredLength,
										NULL
										)) {
		strcpy_s(devName, devNameLen, functionClassDeviceData->DevicePath);
		result = TRUE;
	}

	if (functionClassDeviceData != NULL) {
		free(functionClassDeviceData);
	}

	return result;
}


BOOL ClIoDevice::GetUsbDeviceName(char* outNameBuf, size_t outNameBufLen)
{
	BOOL	found = FALSE;
	DWORD	i = 0;
	HDEVINFO	hardwareDeviceInfo;

	hardwareDeviceInfo = SetupDiGetClassDevs(
						   m_guid,
						   NULL, // Define no enumerator (global)
						   NULL, // Define no
						   (DIGCF_PRESENT | // Only Devices present
						   DIGCF_INTERFACEDEVICE)
						   ); // Function class devices.

	if (hardwareDeviceInfo == INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	SP_INTERFACE_DEVICE_DATA deviceInfoData;
	deviceInfoData.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);

	for (i = 0; !found;) {
		if (SetupDiEnumDeviceInterfaces(
										hardwareDeviceInfo,
										0, // We don't care about specific PDOs
										m_guid,
										i,
										&deviceInfoData)) {
			found = GetOneDeviceName(hardwareDeviceInfo, &deviceInfoData, outNameBuf, outNameBufLen);
		} else {
			if (ERROR_NO_MORE_ITEMS == ::GetLastError()) {
				break;
			}
			i++;
		}
	}

	SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);

	return found;
}


HANDLE ClIoDevice::OpenFile(char *PipeName)
{
	HANDLE h;
	bool second = false;

	for(;;) {
		if (!strcmp(m_device_name, "")) {
			if (!GetUsbDeviceName(m_device_name, sizeof(m_device_name))) {
				return INVALID_HANDLE_VALUE;
			}
		}

		strcpy_s(m_file_name, sizeof(m_device_name), m_device_name);

		if (PipeName) {
			strcat_s(m_file_name, sizeof(m_file_name), "\\");
			strcat_s(m_file_name, sizeof(m_file_name), PipeName);
		}

		h = CreateFileA(m_file_name,
					   GENERIC_WRITE | GENERIC_READ,
					   FILE_SHARE_WRITE | FILE_SHARE_READ,
					   NULL,
					   OPEN_EXISTING,
					   FILE_FLAG_OVERLAPPED,
					   NULL);
		if (h == INVALID_HANDLE_VALUE) {
			strcpy_s(m_device_name, sizeof(m_device_name), "");
			if (second) {
				break;
			}
			second = true;
			continue;
		} else {
			break;
		}
	}

	return h;
}

BOOL ClIoDevice::OpenReadEP()
{
	BOOL result = TRUE;

	if (m_inPipeHandle == INVALID_HANDLE_VALUE) { // if not, handle is already opened.
		m_inPipeHandle = OpenFile(m_inPipeName);
		if (m_inPipeHandle == INVALID_HANDLE_VALUE) {
			result = FALSE;
		} else {
			m_ROverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			if (m_ROverlapped.hEvent == NULL) {
				CloseReadEP();
				result = FALSE;
			} else {
				m_ROverlapped.Internal = 0;
				m_ROverlapped.InternalHigh = 0;
				m_ROverlapped.Offset = 0;
				m_ROverlapped.OffsetHigh = 0;

				m_bReadEPOpen = true;
			}
		}
	}

	return result;
}


void ClIoDevice::CloseReadEP()
{
	WaitForSingleObject(m_hMutex, INFINITE);

	if (m_inPipeHandle != INVALID_HANDLE_VALUE) {
		CloseHandle(m_inPipeHandle);
		m_inPipeHandle = INVALID_HANDLE_VALUE;
	}

	if (m_ROverlapped.hEvent) {
		CloseHandle(m_ROverlapped.hEvent);
		m_ROverlapped.hEvent = NULL;
	}

	m_bReadEPOpen = false;

	ReleaseMutex(m_hMutex);
}


BOOL ClIoDevice::OpenWriteEP()
{
	BOOL result = TRUE;

	if (m_outPipeHandle == INVALID_HANDLE_VALUE) { // if not, handle is already opened.
		m_outPipeHandle = OpenFile(m_outPipeName);
		if (m_outPipeHandle == INVALID_HANDLE_VALUE) {
			result = FALSE;
		} else {
			m_WOverlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			if (m_WOverlapped.hEvent == NULL) {
				CloseWriteEP();
				result = FALSE;
			} else {
				m_WOverlapped.Internal = 0;
				m_WOverlapped.InternalHigh = 0;
				m_WOverlapped.Offset = 0;
				m_WOverlapped.OffsetHigh = 0;

				m_bWriteEPOpen = true;
			}
		}
	}

	return result;
}


void ClIoDevice::CloseWriteEP()
{
	WaitForSingleObject(m_hMutex, INFINITE);

	if (m_outPipeHandle != INVALID_HANDLE_VALUE) {
		CloseHandle(m_outPipeHandle);
		m_outPipeHandle = INVALID_HANDLE_VALUE;
	}

	if (m_WOverlapped.hEvent) {
		CloseHandle(m_WOverlapped.hEvent);
		m_WOverlapped.hEvent = NULL;
	}

	m_bWriteEPOpen = false;

	ReleaseMutex(m_hMutex);
}


BOOL ClIoDevice::OpenReadEP2()
{
	BOOL result = TRUE;

	if (m_inPipeHandle2 == INVALID_HANDLE_VALUE) { // if not, handle is already opened.
		m_inPipeHandle2 = OpenFile(m_inPipeName2);
		if (m_inPipeHandle2 == INVALID_HANDLE_VALUE) {
			result = FALSE;
		} else {
			m_ROverlapped2.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			if (m_ROverlapped2.hEvent == NULL) {
				CloseReadEP2();
				result = FALSE;
			} else {
				m_ROverlapped2.Internal = 0;
				m_ROverlapped2.InternalHigh = 0;
				m_ROverlapped2.Offset = 0;
				m_ROverlapped2.OffsetHigh = 0;

				m_bReadEPOpen2 = true;
			}
		}
	}

	return result;
}


void ClIoDevice::CloseReadEP2()
{
	WaitForSingleObject(m_hMutex, INFINITE);

	if (m_inPipeHandle2 != INVALID_HANDLE_VALUE) {
		CloseHandle(m_inPipeHandle2);
		m_inPipeHandle2 = INVALID_HANDLE_VALUE;
	}

	if (m_ROverlapped2.hEvent) {
		CloseHandle(m_ROverlapped2.hEvent);
		m_ROverlapped2.hEvent = NULL;
	}

	m_bReadEPOpen2 = false;

	ReleaseMutex(m_hMutex);
}

BOOL ClIoDevice::OpenWriteEP2()
{
	BOOL result = TRUE;

	if (m_outPipeHandle2 == INVALID_HANDLE_VALUE) { // if not, handle is already opened.
		m_outPipeHandle2 = OpenFile(m_outPipeName2);
		if (m_outPipeHandle2 == INVALID_HANDLE_VALUE) {
			result = FALSE;
		} else {
			m_WOverlapped2.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
			if (m_WOverlapped2.hEvent == NULL) {
				CloseWriteEP2();
				result = FALSE;
			} else {
				m_WOverlapped2.Internal = 0;
				m_WOverlapped2.InternalHigh = 0;
				m_WOverlapped2.Offset = 0;
				m_WOverlapped2.OffsetHigh = 0;

				m_bWriteEPOpen2 = true;
			}
		}
	}

	return result;
}


void ClIoDevice::CloseWriteEP2()
{
	WaitForSingleObject(m_hMutex, INFINITE);

	if (m_outPipeHandle2 != INVALID_HANDLE_VALUE) {
		CloseHandle(m_outPipeHandle2);
		m_outPipeHandle2 = INVALID_HANDLE_VALUE;
	}

	if (m_WOverlapped2.hEvent) {
		CloseHandle(m_WOverlapped2.hEvent);
		m_WOverlapped2.hEvent = NULL;
	}

	m_bWriteEPOpen2 = false;

	ReleaseMutex(m_hMutex);
}

IoDevice::IO_RESULT ClIoDevice::ReadEP(LPVOID lpBuffer, DWORD nLength, LPDWORD lpnLength, DWORD dwTimeout)
{
	IoDevice::IO_RESULT result = IO_SUCCESS;

	if (m_inPipeHandle == INVALID_HANDLE_VALUE) {
		result = IoDevice::IO_INVALID_HANDLE;
	} else {
		ResetEvent(m_ROverlapped.hEvent);
		BOOL ioresult = ReadFile(m_inPipeHandle, lpBuffer, nLength, lpnLength, &m_ROverlapped);
		if (ioresult == 0) {
			DWORD lasterror = GetLastError();
			if (lasterror == ERROR_IO_PENDING) {
				DWORD wait_reason = WaitForSingleObject(m_ROverlapped.hEvent, dwTimeout);
				if (wait_reason == WAIT_FAILED) {
					result = IoDevice::IO_FAILURE;
				} else if (wait_reason == WAIT_TIMEOUT) {
					if (CancelIo(m_inPipeHandle)) {
						result = IoDevice::IO_TIMEOUT_TRUE;
					} else {
						result = IoDevice::IO_TIMEOUT_FALSE;
					}
				} else if (wait_reason == WAIT_OBJECT_0) {
					BOOL overlapped_result = GetOverlappedResult(m_inPipeHandle, &m_ROverlapped, lpnLength, FALSE);
					if (overlapped_result == FALSE) {
						DWORD lasterror_overlapped = GetLastError();
						if (lasterror_overlapped == ERROR_DEVICE_NOT_CONNECTED) {
							CloseReadEP();
							CloseWriteEP();
							CloseReadEP2();
							CloseWriteEP2();
							result = IoDevice::IO_FAILURE;
						}
					}
				} else if (wait_reason == WAIT_ABANDONED) {
					result = IoDevice::IO_WAIT_ABANDONED;
				}
			} else if (lasterror == ERROR_DEVICE_NOT_CONNECTED) {
				CloseReadEP();
				CloseWriteEP();
				CloseReadEP2();
				CloseWriteEP2();
				result = IoDevice::IO_NOT_CONNECT;
			} else {
				Q_ASSERT(0);
			}
		}
	}

	return result;
}


IoDevice::IO_RESULT ClIoDevice::WriteEP(LPCVOID lpBuffer, DWORD nLength, LPDWORD lpnLength, DWORD dwTimeout)
{
	IoDevice::IO_RESULT result = IO_SUCCESS;

	if (m_outPipeHandle == INVALID_HANDLE_VALUE) {
		result = IoDevice::IO_INVALID_HANDLE;
	} else {
		ResetEvent(m_WOverlapped.hEvent);
		BOOL ioresult = WriteFile(m_outPipeHandle, lpBuffer, nLength, lpnLength, &m_WOverlapped);
		if (ioresult == 0) {
			DWORD lasterror = GetLastError();
			if (lasterror == ERROR_IO_PENDING) {
				DWORD wait_reason = WaitForSingleObject(m_WOverlapped.hEvent, dwTimeout);
				if (wait_reason == WAIT_FAILED) {
					result = IoDevice::IO_FAILURE;
				} else if (wait_reason == WAIT_TIMEOUT) {
					if (CancelIo(m_outPipeHandle)) {
						result = IoDevice::IO_TIMEOUT_TRUE;
					} else {
						result = IoDevice::IO_TIMEOUT_FALSE;
					}
				} else if (wait_reason == WAIT_OBJECT_0) {
					BOOL overlapped_result = GetOverlappedResult(m_outPipeHandle, &m_WOverlapped, lpnLength, FALSE);
					if (overlapped_result == FALSE) {
						DWORD lasterror_overlapped = GetLastError();
						if (lasterror_overlapped == ERROR_DEVICE_NOT_CONNECTED) {
							CloseReadEP();
							CloseWriteEP();
							CloseReadEP2();
							CloseWriteEP2();
							result = IoDevice::IO_FAILURE;
						}
					}
				} else if (wait_reason == WAIT_ABANDONED) {
					result = IoDevice::IO_WAIT_ABANDONED;
				}
			} else if (lasterror == ERROR_DEVICE_NOT_CONNECTED) {
				CloseReadEP();
				CloseWriteEP();
				CloseReadEP2();
				CloseWriteEP2();
				result = IoDevice::IO_NOT_CONNECT;
			} else {
				Q_ASSERT(0);
			}
		}
	}

	return result;
}

IoDevice::IO_RESULT ClIoDevice::ReadEP2(LPVOID lpBuffer, DWORD nLength, LPDWORD lpnLength, DWORD dwTimeout)
{
	IoDevice::IO_RESULT result = IO_SUCCESS;

	if (m_inPipeHandle2 == INVALID_HANDLE_VALUE) {
		result = IoDevice::IO_INVALID_HANDLE;
	} else {
		ResetEvent(m_ROverlapped2.hEvent);
		BOOL ioresult = ReadFile(m_inPipeHandle2, lpBuffer, nLength, lpnLength, &m_ROverlapped2);
		if (ioresult == 0) {
			DWORD lasterror = GetLastError();
			if (lasterror == ERROR_IO_PENDING) {
				DWORD wait_reason = WaitForSingleObject(m_ROverlapped2.hEvent, dwTimeout);
				if (wait_reason == WAIT_FAILED) {
					result = IoDevice::IO_FAILURE;
				} else if (wait_reason == WAIT_TIMEOUT) {
					if (CancelIo(m_inPipeHandle2)) {
						result = IoDevice::IO_TIMEOUT_TRUE;
					} else {
						result = IoDevice::IO_TIMEOUT_FALSE;
					}
				} else if (wait_reason == WAIT_OBJECT_0) {
					BOOL overlapped_result = GetOverlappedResult(m_inPipeHandle2, &m_ROverlapped2, lpnLength, FALSE);
					if (overlapped_result == FALSE) {
						DWORD lasterror_overlapped = GetLastError();
						if (lasterror_overlapped == ERROR_DEVICE_NOT_CONNECTED) {
							CloseReadEP();
							CloseWriteEP();
							CloseReadEP2();
							CloseWriteEP2();
							result = IoDevice::IO_FAILURE;
						}
					}
				} else if (wait_reason == WAIT_ABANDONED) {
					result = IoDevice::IO_WAIT_ABANDONED;
				}
			} else if (lasterror == ERROR_DEVICE_NOT_CONNECTED) {
				CloseReadEP();
				CloseWriteEP();
				CloseReadEP2();
				CloseWriteEP2();
				result = IoDevice::IO_NOT_CONNECT;
			} else {
				Q_ASSERT(0);
			}
		}
	}

	return result;
}

IoDevice::IO_RESULT ClIoDevice::WriteEP2(LPCVOID lpBuffer, DWORD nLength, LPDWORD lpnLength, DWORD dwTimeout)
{
	IoDevice::IO_RESULT result = IO_SUCCESS;

	if (m_outPipeHandle2 == INVALID_HANDLE_VALUE) {
		result = IoDevice::IO_INVALID_HANDLE;
	} else {
		ResetEvent(m_WOverlapped2.hEvent);
		BOOL ioresult = WriteFile(m_outPipeHandle2, lpBuffer, nLength, lpnLength, &m_WOverlapped2);
		if (ioresult == 0) {
			DWORD lasterror = GetLastError();
			if (lasterror == ERROR_IO_PENDING) {
				DWORD wait_reason = WaitForSingleObject(m_WOverlapped2.hEvent, dwTimeout);
				if (wait_reason == WAIT_FAILED) {
					result = IoDevice::IO_FAILURE;
				} else if (wait_reason == WAIT_TIMEOUT) {
					if (CancelIo(m_outPipeHandle2)) {
						result = IoDevice::IO_TIMEOUT_TRUE;
					} else {
						result = IoDevice::IO_TIMEOUT_FALSE;
					}
				} else if (wait_reason == WAIT_OBJECT_0) {
					BOOL overlapped_result = GetOverlappedResult(m_outPipeHandle2, &m_WOverlapped2, lpnLength, FALSE);
					if (overlapped_result == FALSE) {
						DWORD lasterror_overlapped = GetLastError();
						if (lasterror_overlapped == ERROR_DEVICE_NOT_CONNECTED) {
							CloseReadEP();
							CloseWriteEP();
							CloseReadEP2();
							CloseWriteEP2();
							result = IoDevice::IO_FAILURE;
						}
					}
				} else if (wait_reason == WAIT_ABANDONED) {
					result = IoDevice::IO_WAIT_ABANDONED;
				}
			} else if (lasterror == ERROR_DEVICE_NOT_CONNECTED) {
				CloseReadEP();
				CloseWriteEP();
				CloseReadEP2();
				CloseWriteEP2();
				result = IoDevice::IO_NOT_CONNECT;
			} else {
				Q_ASSERT(0);
			}
		}
	}

	return result;
}

BOOL ClIoDevice::IsAlive()
{
#if 0
	char dummyName[FILE_NAME_LEN];
	return GetClIoDeviceName(dummyName, sizeof(dummyName));
#else

	LPSTR	lpOutBuffer;							//出力用バッファ
	DWORD	dwOutBufferSize;						//出力用バッファサイズ
	BOOL	Alive = FALSE;

	HANDLE hDevice = OpenFile(NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
		return 0;

	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hEvent == NULL) {
		CloseHandle(hDevice);
		return 0;
	}

	OVERLAPPED ol;
	ol.Internal = 0;
	ol.InternalHigh = 0;
	ol.Offset = 0;
	ol.OffsetHigh = 0;
	ol.hEvent = hEvent;

	lpOutBuffer = (LPSTR)&Alive;
	dwOutBufferSize = sizeof(Alive);

	DWORD length;
	if (DeviceIoControl(hDevice, IOCTL_USB_GET_STATUS, NULL, 0, lpOutBuffer, dwOutBufferSize, &length, &ol) == 0) {
		if (GetLastError() == ERROR_IO_PENDING) {
			DWORD reason = WaitForSingleObject(hEvent, 5000);
			if (reason == WAIT_TIMEOUT) {
				CancelIo(hDevice);
			}
		}
	}

	CloseHandle(hEvent);
	CloseHandle(hDevice);
	return Alive;
#endif


}

BOOL ClIoDevice::IsReadEPOpen()
{
	if (m_bReadEPOpen) {
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL ClIoDevice::IsWriteEPOpen()
{
	if (m_bWriteEPOpen) {
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL ClIoDevice::IsReadEPOpen2()
{
	if (m_bReadEPOpen2) {
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL ClIoDevice::IsWriteEPOpen2()
{
	if (m_bWriteEPOpen2) {
		return TRUE;
	} else {
		return FALSE;
	}
}

BOOL ClIoDevice::ResetPipe()
{
#if 1
	BOOL ret = TRUE;
	if (!IsAlive())
		return FALSE;

	HANDLE hDevice = OpenFile(NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
		return FALSE;

	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hEvent == NULL) {
		CloseHandle(hDevice);
		return FALSE;
	}

	OVERLAPPED ol;
	ol.Internal = 0;
	ol.InternalHigh = 0;
	ol.Offset = 0;
	ol.OffsetHigh = 0;
	ol.hEvent = hEvent;

	DWORD length;
	if (DeviceIoControl(hDevice, IOCTL_USB_RESET_PIPE, NULL, 0, NULL, 0, &length, &ol) == 0) {
		if (GetLastError() == ERROR_IO_PENDING) {
			DWORD reason = WaitForSingleObject(hEvent, 5000);
			if (reason == WAIT_TIMEOUT) {
				CancelIo(hDevice);
				ret = FALSE;
			}
		}
	}

	CloseHandle(hEvent);
	CloseHandle(hDevice);
	return ret;
#endif
}

BOOL ClIoDevice::FlushDriverLog()
{
	BOOL ret = TRUE;
	if (!IsAlive())
		return FALSE;

	HANDLE hDevice = OpenFile(NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
		return FALSE;

	HANDLE hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (hEvent == NULL) {
		CloseHandle(hDevice);
		return FALSE;
	}

	OVERLAPPED ol;
	ol.Internal = 0;
	ol.InternalHigh = 0;
	ol.Offset = 0;
	ol.OffsetHigh = 0;
	ol.hEvent = hEvent;

	BYTE logdata[4096];
	DWORD loglen = sizeof(logdata);
	DWORD readlen;

	if (DeviceIoControl(hDevice, IOCTL_USB_GETLOG, NULL, 0, logdata, loglen, &readlen, &ol) == 0) {
		if (GetLastError() == ERROR_IO_PENDING) {
			DWORD reason = WaitForSingleObject(hEvent, 5000);
			if (reason == WAIT_TIMEOUT) {
				CancelIo(hDevice);
				ret = FALSE;
			}
		}
	}

	CloseHandle(hEvent);
	CloseHandle(hDevice);
	return ret;
}
