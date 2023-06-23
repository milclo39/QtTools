#ifndef USBDEVICE_H
#define USBDEVICE_H

#include <mach/mach.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/usb/IOUSBLib.h>

#include <QThread>
#include <QWaitCondition>
#include <QTimer>

#if 0 //elmo add
#define USE_LIBUSB
#include "UsbDriver/mac/libusb/libusb.h"
#endif

typedef IOUSBDeviceInterface187 IOUSBDeviceInterfaceXXX;
typedef IOUSBInterfaceInterface182 IOUSBInterfaceInterfaceXXX;

class ClUsbDriver;
class ClDeviceInfo;
class ClIoDevice;

namespace IoDevice
{
	enum IO_RESULT
	{
		IO_SUCCESS,
		IO_FAILURE,
		IO_TIMEOUT_TRUE,
		IO_TIMEOUT_FALSE,
		IO_WAIT_ABANDONED
	};
}

//--------------------------------------------------------------------------------//
/*!
	@brief		接続検知スレッドクラス
*/
//--------------------------------------------------------------------------------//
class ClPollingThread : public QThread
{
	Q_OBJECT

public:
	ClPollingThread();
	void setDevice(ClIoDevice *pclDevice){m_pclDevice = pclDevice;}
	void setProductId(qint32 lProductId){m_lProductId = lProductId;}
	void stop();

protected:
	void run();

private slots:
	void slotPolling();

protected:
	ClIoDevice *m_pclDevice;
	QTimer m_clTimer;
	QWaitCondition m_clWait;
	qint32 m_lProductId;
	bool m_bIsStop;
};
//--------------------------------------------------------------------------------//

//コールバックの引数
typedef struct
{
	ClUsbDriver *pclDriver;
	SInt32 lProductId;
}stCallback;

//--------------------------------------------------------------------------------//
/*!
	@brief		USB通信クラス
*/
//--------------------------------------------------------------------------------//
class ClIoDevice
{
public:
	ClIoDevice(UInt32 ulProductId, bool bEnableOpen = true);
	~ClIoDevice();

	void setDriver(ClUsbDriver *pclDriver){m_pclDriver = pclDriver;}
	void setDevinfo(ClDeviceInfo *pclInfo){m_pclInfo = pclInfo;}
	ClUsbDriver *getDriver(){return m_pclDriver;}
	ClDeviceInfo *getDevinfo(){return m_pclInfo;}

	bool openDevice();
	void closeDevice();
	bool resetPipe();

	IoDevice::IO_RESULT readEP(void *lpBuffer, UInt32 nLength, UInt32 *pnLength, UInt32 nTimeout);
	IoDevice::IO_RESULT writeEP(void *lpBuffer, UInt32 nLength, UInt32 *pnLength, UInt32 nTimeout);
	IoDevice::IO_RESULT readEP2(void *lpBuffer, UInt32 nLength, UInt32 *pnLength, UInt32 nTimeout);
	IoDevice::IO_RESULT writeEP2(void *lpBuffer, UInt32 nLength, UInt32 *pnLength, UInt32 nTimeout);

#if defined(USE_LIBUSB) //elmo add
    bool isOpened();	//オープンしているか取得

    int write(QByteArray clData, qint32 lTimeout = 0, quint8 ucEndPoint = -1);	//書き込み
    QByteArray read(qint32 lTimeout = 0, quint8 ucEndPoint = -1, qint32 *plError = NULL);	//読み込み

protected:
    quint8 getEndPoint(quint8 ucEndpoint);	//エンドポイント取得
    int findEndpoints();	//エンドポイント検索

    libusb_device *m_pstDevice;	//!< デバイス
    libusb_device_handle *m_pstDeviceHandle;	//!< デバイスハンドル
    QList<quint8> m_clEndpointList;	//!< エンドポイントリスト
    QList<int> m_clClaimedInterfaces;	//!< 使用するインターフェース番号リスト
    bool m_bIsOpened;	//!< 開いたフラグ
#endif

protected:
	bool findDevice(io_service_t usbDeviceRef);
	bool findInterfaces(io_service_t usbInterfaceRef);
	bool findPipes(UInt8 ucNumPipe);
	void closeInterfaces();

protected:
	ClUsbDriver *m_pclDriver;
	ClDeviceInfo *m_pclInfo;
	ClPollingThread m_clThread;
	stCallback m_stCallbackParam;

	CFRunLoopRef m_currentLoop;

	mach_port_t m_masterPort;
	io_iterator_t m_ioIterator;
	io_service_t m_usbDeviceRef;

	IOUSBDeviceInterfaceXXX **m_device;
	io_iterator_t m_findDeviceIterator;

	io_service_t m_usbInterfaceRef;
	IOUSBInterfaceInterfaceXXX **m_interface;

	IONotificationPortRef m_notifyPort;
	io_object_t m_notification;

	UInt8 m_ucReadEP;
	UInt8 m_ucWriteEP;
	UInt8 m_ucReadEP2;
	UInt8 m_ucWriteEP2;

	UInt32 m_ulProductId;

	bool m_bEnableOpen;
};
//--------------------------------------------------------------------------------//
#endif
