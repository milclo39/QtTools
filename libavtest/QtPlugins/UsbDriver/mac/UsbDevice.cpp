#include <CoreFoundation/CFNumber.h>
#include <CoreFoundation/CoreFoundation.h>

#include <IOKit/IOCFPlugIn.h>
#include <IOKit/IOMessage.h>

#include "UsbDevice.h"
#include "ClUsbDriver/ClUsbDriver_global.h"
#include "ClUsbDriver/ClUsbDriver_mac.h"
#include "mac/ClDeviceInfo_mac.h"

#if defined(USE_LIBUSB) //elmo add
static libusb_context *g_ctx = NULL;
#endif

#define POLLING_INTERVAL	1000	//!< ポーリング間隔

//--------------------------------------------------------------------------------//
/*!
	@brief		デバイス切断通知コールバック
*/
//--------------------------------------------------------------------------------//
void deviceNotificationRoutine(void *refCon, io_service_t service, natural_t messageType, void*messageArgument)
{
	if(messageType == kIOMessageServiceIsTerminated)
	{
		stCallback *pstRef = reinterpret_cast<stCallback*>(refCon);
		pstRef->pclDriver->close(pstRef->lProductId);
	}
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
//ClPollingThread
//--------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------//
/*!
	@brief		コンストラクタ
*/
//--------------------------------------------------------------------------------//
ClPollingThread::ClPollingThread()
{
	m_pclDevice = NULL;
	m_lProductId = 0;
	m_bIsStop = false;

	m_clTimer.setSingleShot(false);
	connect(&m_clTimer, SIGNAL(timeout()), this, SLOT(slotPolling()));
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		スレッド停止
*/
//--------------------------------------------------------------------------------//
void ClPollingThread::stop()
{
	m_bIsStop = true;
	m_clTimer.stop();		//タイマ停止
	m_clWait.wakeAll();		//待ち解除
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		メイン処理
*/
//--------------------------------------------------------------------------------//
void ClPollingThread::run()
{
	m_clTimer.start(POLLING_INTERVAL);	//タイマ起動

	QMutex clMutex;
	while(m_bIsStop == false)
	{
		m_clWait.wait(&clMutex);
		if(m_bIsStop == false &&
		   m_pclDevice != NULL)
		{
			if(m_pclDevice->openDevice() == true)
			{
				ClDeviceInfo *pclInfo = m_pclDevice->getDevinfo();
				if(pclInfo != NULL)
				{
					pclInfo->setConnected(true);	//オープン処理
				}
				ClUsbDriver *pclDriver = m_pclDevice->getDriver();
				if(pclDriver != NULL)
				{
					pclDriver->open(m_lProductId);	//オープン通知
				}
				break;
			}
		}
	}
	m_clTimer.stop();	//タイマ停止
	m_bIsStop = false;	//フラグ戻しておく
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		タイマ通知
*/
//--------------------------------------------------------------------------------//
void ClPollingThread::slotPolling()
{
	m_clWait.wakeAll();
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
//ClIoDevice
//--------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------//
/*!
	@brief		コンストラクタ
*/
//--------------------------------------------------------------------------------//
ClIoDevice::ClIoDevice(UInt32 ulProductId, bool bEnableOpen)
{
	m_pclDriver = NULL;
#if 0 //elmo del
	m_clThread.setDevice(this);
	m_clThread.setProductId(ulProductId);
#endif
	m_currentLoop = CFRunLoopGetCurrent();

	m_masterPort = 0;
    m_ioIterator = 0;
	m_usbDeviceRef = 0;
	m_device = 0;
	m_findDeviceIterator = 0;
	m_usbInterfaceRef = 0;
	m_interface = 0;
	m_notifyPort = 0;
	m_notification = 0;

	m_ucReadEP = 0;
	m_ucWriteEP = 0;
	m_ucReadEP2 = 0;
	m_ucWriteEP2 = 0;

	m_ulProductId = ulProductId;

	m_bEnableOpen = bEnableOpen;

#if defined(USE_LIBUSB) //elmo add
    m_pstDevice = NULL;
    m_pstDeviceHandle = NULL;
    m_bIsOpened = false;
    libusb_init(&g_ctx);
#endif
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		デストラクタ
*/
//--------------------------------------------------------------------------------//
ClIoDevice::~ClIoDevice()
{
#if 0 //elmo del
    if(m_clThread.isRunning() == true)
	{
		m_clThread.stop();
		m_clThread.wait();
    }
#endif
#if defined(USE_LIBUSB) //elmo add
    libusb_close(m_pstDeviceHandle);	//クローズ
    m_pstDeviceHandle = NULL;
    libusb_exit(g_ctx);
#endif
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		パイプ初期化
*/
//--------------------------------------------------------------------------------//
bool ClIoDevice::resetPipe()
{
	if(m_ucReadEP != 0)
	{
		(*m_interface)->ClearPipeStall(m_interface, m_ucReadEP);
	}
	if(m_ucReadEP2 != 0)
	{
		(*m_interface)->ClearPipeStall(m_interface, m_ucReadEP2);
	}
	if(m_ucWriteEP != 0)
	{
		(*m_interface)->ClearPipeStall(m_interface, m_ucWriteEP);
	}
	if(m_ucWriteEP2 != 0)
	{
		(*m_interface)->ClearPipeStall(m_interface, m_ucWriteEP2);
	}
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		パイプ１での読み込み
*/
//--------------------------------------------------------------------------------//
IoDevice::IO_RESULT ClIoDevice::readEP(void *lpBuffer, UInt32 nLength, UInt32 *pnLength, UInt32 nTimeout)
{
#if !defined(USE_LIBUSB) //elmo del
    IOReturn error;
    error = (*m_interface)->ReadPipeTO(m_interface, m_ucReadEP, lpBuffer, &nLength, nTimeout, nTimeout);
	if(error == kIOReturnNotResponding ||
	   error == kIOReturnNoDevice)
	{
        m_pclInfo->setConnected(false);
	}
	if(error != kIOReturnSuccess)
    {
		return IoDevice::IO_FAILURE;
	}
#else //elmo add
    qint32 error;
    QByteArray data = this->read(nTimeout, 0x81, &error);
#endif
    *pnLength = nLength;
	return IoDevice::IO_SUCCESS;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		パイプ１での書き込み
*/
//--------------------------------------------------------------------------------//
IoDevice::IO_RESULT ClIoDevice::writeEP(void *lpBuffer, UInt32 nLength, UInt32 *pnLength, UInt32 nTimeout)
{
#if !defined(USE_LIBUSB) //elmo del
    IOReturn error;
    error = (*m_interface)->WritePipeTO(m_interface, m_ucWriteEP, lpBuffer, nLength, nTimeout, nTimeout);
    if(error == kIOReturnNotResponding ||
	   error == kIOReturnNoDevice)
    {
		m_pclInfo->setConnected(false);
	}
	if(error != kIOReturnSuccess)
	{
        return IoDevice::IO_FAILURE;
	}
#else //elmo add
    QByteArray data = QByteArray::fromRawData((char *)lpBuffer, nLength);
    this->write(data, nTimeout, 0x02);
#endif
    *pnLength = nLength;
	return IoDevice::IO_SUCCESS;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		パイプ2での読み込み
*/
//--------------------------------------------------------------------------------//
IoDevice::IO_RESULT ClIoDevice::readEP2(void *lpBuffer, UInt32 nLength, UInt32 *pnLength, UInt32 nTimeout)
{
#if !defined(USE_LIBUSB) //elmo del
    IOReturn error;
    error = (*m_interface)->ReadPipeTO(m_interface, m_ucReadEP2, lpBuffer, &nLength, nTimeout, nTimeout);
    if(error == kIOReturnNotResponding ||
	   error == kIOReturnNoDevice)
	{
        m_pclInfo->setConnected(false);
	}
	if(error != kIOReturnSuccess)
	{
        return IoDevice::IO_FAILURE;
	}
    *pnLength = nLength;
#else //elmo add
    qint32 error;
    QByteArray data = this->read(nTimeout, 0x83, &error);
    if(error == IM_ERROR_SUCCESS)
    {
        memcpy(lpBuffer, data.data(), data.length());
    }
    *pnLength = data.length();
#endif
	return IoDevice::IO_SUCCESS;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		パイプ2での書き込み
*/
//--------------------------------------------------------------------------------//
IoDevice::IO_RESULT ClIoDevice::writeEP2(void *lpBuffer, UInt32 nLength, UInt32 *pnLength, UInt32 nTimeout)
{
#if !defined(USE_LIBUSB) //elmo del
    IOReturn error;
    error = (*m_interface)->WritePipeTO(m_interface, m_ucWriteEP2, lpBuffer, nLength, nTimeout, nTimeout);
	if(error == kIOReturnNotResponding ||
	   error == kIOReturnNoDevice)
	{
        m_pclInfo->setConnected(false);
	}
	if(error != kIOReturnSuccess)
	{
        return IoDevice::IO_FAILURE;
	}
#else //elmo add
    QByteArray data = QByteArray::fromRawData((char *)lpBuffer, nLength);
    int iRet = this->write(data, nTimeout, 0x04);
#endif
    *pnLength = nLength;
	return IoDevice::IO_SUCCESS;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		デバイスオープン
*/
//--------------------------------------------------------------------------------//
bool ClIoDevice::openDevice()
{
#if !defined(USE_LIBUSB) //elmo del
    if(m_masterPort == 0)
	{
		kern_return_t error = IOMasterPort(MACH_PORT_NULL, &m_masterPort);
		if(error != 0)
        {
            return false;
		}
	}

    //USBのマッチング辞書作成
	CFMutableDictionaryRef matchingDictionary = 0;
	matchingDictionary = IOServiceMatching(kIOUSBDeviceClassName);
    if(matchingDictionary == 0)
	{
        return false;
	}

    CFNumberRef numberRef;
    //エルモのベンダIDのCFNumber取得
    UInt32 lVendor = VENDOR_ELMO;
    numberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &lVendor);
    if(numberRef == 0)
    {
        return false;
    }
    CFDictionaryAddValue(matchingDictionary, CFSTR(kUSBVendorID), numberRef);	//マッチング辞書にベンダID追加
    CFRelease(numberRef);

    //カメラのプロダクトIDのCFNumber取得
    UInt32 lProductId = GETPRODUCTID(m_ulProductId);
    numberRef = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &lProductId);
    if(numberRef == 0)
    {
        return false;
    }
    CFDictionaryAddValue(matchingDictionary, CFSTR(kUSBProductID), numberRef);	//マッチング辞書にプロダクトID追加
    CFRelease(numberRef);

    IOServiceGetMatchingServices(m_masterPort, matchingDictionary, &m_ioIterator);

    bool bRes = false;
    m_usbDeviceRef = IOIteratorNext(m_ioIterator);
    if(m_usbDeviceRef != 0)
    {
        if(m_bEnableOpen == true)												//フラグがtrueならOpenをさせる
        {
            bRes = findDevice(m_usbDeviceRef);
        }
        else																	//フラグがfalseならOpenせずに登録させる
        {
            bRes = true;
        }

        if(bRes == true)
        {
            m_notifyPort = IONotificationPortCreate(m_masterPort);
            CFRunLoopAddSource(m_currentLoop,
                               IONotificationPortGetRunLoopSource(m_notifyPort),
                               kCFRunLoopDefaultMode);

            //コールバックの引数生成
            m_stCallbackParam.pclDriver = m_pclDriver;
            m_stCallbackParam.lProductId = m_ulProductId;

            //イベント通知登録
            IOServiceAddInterestNotification(m_notifyPort,
                                             m_usbDeviceRef,
                                             kIOGeneralInterest,
                                             deviceNotificationRoutine,
                                             &m_stCallbackParam,
                                             &m_notification);
            bRes = true;
        }
    }
#if 0
    //ポーリングスレッド起動
    if(bRes == false &&
       m_clThread.isRunning() == false)
    {
        m_clThread.start(QThread::IdlePriority);
    }
#endif
    return bRes;
#else //elmo add
    int iRet = LIBUSB_SUCCESS;
    libusb_device **devices;

    int count = libusb_get_device_list(g_ctx, &devices);
    if(count < 0)
    {
        return false;
    }
    libusb_device_descriptor desc;
    for(int i = 0; i < count; i++)
    {
        libusb_device *device = devices[i];
        libusb_get_device_descriptor(device, &desc);
        //printf("[%d :: %x %x >> %x %x]",i,desc.idVendor,desc.idProduct,VENDOR_ELMO,m_ulProductId);
        if(desc.idVendor == VENDOR_ELMO && desc.idProduct == (m_ulProductId << 16) >> 16)
        {
            m_pstDevice = device;
            break;
        }
    }
    if(NULL == m_pstDevice)
    {
        return false;
    }

    iRet = libusb_open(m_pstDevice, &m_pstDeviceHandle);
    if(iRet != LIBUSB_SUCCESS)
    {
        printf("Open error:%d", iRet);
        libusb_free_device_list(devices, 1);
        return false;
    }

    iRet = findEndpoints();
    if(iRet != LIBUSB_SUCCESS)
    {
        libusb_close(m_pstDeviceHandle);	//クローズ
        m_pstDeviceHandle = NULL;
        libusb_free_device_list(devices, 1);
        return false;
    }

    libusb_free_device_list(devices, 1);

    m_bIsOpened = true;

    return true;
#endif
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		デバイス初期化
*/
//--------------------------------------------------------------------------------//
bool ClIoDevice::findDevice(io_service_t usbDeviceRef)
{
	IOReturn error;
    IOCFPlugInInterface **ioDevice;
	SInt32 lScore = 0;
	UInt8 ucNumConf = 0;
	IOUSBConfigurationDescriptor *confDescriptor;
	IOUSBFindInterfaceRequest interfaceRequest;

    error = IOCreatePlugInInterfaceForService(usbDeviceRef, kIOUSBDeviceUserClientTypeID,
											  kIOCFPlugInInterfaceID, &ioDevice, reinterpret_cast<SInt32*>(&lScore));
	if(error != 0 || ioDevice == 0)
    {
        return false;
	}

    error = (*ioDevice)->QueryInterface(ioDevice, CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID187),
										reinterpret_cast<LPVOID*>(&m_device));
	(*ioDevice)->Release(ioDevice);
	if(error != 0 || m_device == 0)
	{
        return false;
	}

	error = (*m_device)->USBDeviceOpen(m_device);
    if(error != 0)
    {
        return false;
	}

	error = (*m_device)->GetNumberOfConfigurations(m_device, &ucNumConf);
	if(error != 0 || ucNumConf == 0)
	{
        (*m_device)->USBDeviceClose(m_device);
		(*m_device)->Release(m_device);
        return false;
	}

	error = (*m_device)->GetConfigurationDescriptorPtr(m_device, 0, &confDescriptor);
	if(error != 0)
	{
        (*m_device)->USBDeviceClose(m_device);
		(*m_device)->Release(m_device);
        return false;
	}

	error = (*m_device)->SetConfiguration(m_device, confDescriptor->bConfigurationValue);
	if(error != 0)
	{
        (*m_device)->USBDeviceClose(m_device);
		(*m_device)->Release(m_device);
        return false;
	}

    interfaceRequest.bInterfaceClass = kIOUSBFindInterfaceDontCare;
	interfaceRequest.bInterfaceSubClass = kIOUSBFindInterfaceDontCare;
	interfaceRequest.bInterfaceProtocol = kIOUSBFindInterfaceDontCare;
	interfaceRequest.bAlternateSetting = kIOUSBFindInterfaceDontCare;

	error = (*m_device)->CreateInterfaceIterator(m_device, &interfaceRequest, &m_findDeviceIterator);
	if(error != 0)
	{
        (*m_device)->USBDeviceClose(m_device);
		(*m_device)->Release(m_device);
        return false;
	}

	m_usbInterfaceRef = IOIteratorNext(m_findDeviceIterator);
	if(m_usbInterfaceRef != 0)
	{
        return findInterfaces(m_usbInterfaceRef);
	}

    return false;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		インターフェース初期化
*/
//--------------------------------------------------------------------------------//
bool ClIoDevice::findInterfaces(io_service_t usbInterfaceRef)
{
	IOReturn error;
	IOCFPlugInInterface **ioDevice;
	SInt32 lScore = 0;
	UInt8 ucNumPipes = 0;

	error = IOCreatePlugInInterfaceForService(usbInterfaceRef, kIOUSBInterfaceUserClientTypeID,
											  kIOCFPlugInInterfaceID, &ioDevice, reinterpret_cast<SInt32*>(&lScore));
	if(error != 0 || ioDevice == 0)
	{
		return false;
	}

    error = (*ioDevice)->QueryInterface(ioDevice, CFUUIDGetUUIDBytes(kIOUSBInterfaceInterfaceID182),
										reinterpret_cast<LPVOID*>(&m_interface));
	(*ioDevice)->Release(ioDevice);
	if(error != 0 || m_interface == 0)
	{
		return false;
	}

	error = (*m_interface)->USBInterfaceOpen(m_interface);
	if(error != 0)
	{
		return false;
	}

	error = (*m_interface)->GetNumEndpoints(m_interface, &ucNumPipes);
	if(error != 0)
	{
		(*m_interface)->USBInterfaceClose(m_interface);
		(*m_interface)->Release(m_interface);
		return false;
	}

	if(ucNumPipes == 0)
	{
		error = (*m_interface)->SetAlternateInterface(m_interface, 1);
		if(error != 0)
		{
			(*m_interface)->USBInterfaceClose(m_interface);
			(*m_interface)->Release(m_interface);
			return false;
		}
		error = (*m_interface)->GetNumEndpoints(m_interface, &ucNumPipes);
		if(error != 0)
		{
			(*m_interface)->USBInterfaceClose(m_interface);
			(*m_interface)->Release(m_interface);
			return false;
		}
		ucNumPipes = 13;	//SetAlternateInterfaceの後にGetNumEndPointsは動かないらしい
	}

	if(ucNumPipes > 0)
	{
		return findPipes(ucNumPipes);
	}
	else
	{
		return false;
	}
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		パイプ初期化
*/
//--------------------------------------------------------------------------------//
bool ClIoDevice::findPipes(UInt8 ucNumPipes)
{
	IOReturn error;
	UInt8 ucDirection = 0, ucNumber = 0, ucTransferType = 0, ucInterval = 0;
	UInt16 usMaxPacketSize = 0;

	for(int i = 1; i <= ucNumPipes; i++)
	{
		error = (*m_interface)->GetPipeProperties(m_interface, i, &ucDirection, &ucNumber,
													  &ucTransferType, &usMaxPacketSize, &ucInterval);
		if(error != 0)
		{
			return false;
        }
        if(ucTransferType != kUSBBulk)
		{
			continue;
		}
		if(ucDirection == kUSBIn && m_ucReadEP == 0)
        {
			m_ucReadEP = i;
			(*m_interface)->ResetPipe(m_interface, m_ucReadEP);
		}
		else if(ucDirection == kUSBIn && m_ucReadEP2 == 0)
		{
            m_ucReadEP2 = i;
			(*m_interface)->ResetPipe(m_interface, m_ucReadEP2);
		}
		if(ucDirection == kUSBOut && m_ucWriteEP == 0)
		{
            m_ucWriteEP = i;
			(*m_interface)->ResetPipe(m_interface, m_ucWriteEP);
		}
		else if(ucDirection == kUSBOut && m_ucWriteEP2 == 0)
		{
            m_ucWriteEP2 = i;
			(*m_interface)->ResetPipe(m_interface, m_ucWriteEP2);
		}
	}

	return true;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		デバイスクローズ
*/
//--------------------------------------------------------------------------------//
void ClIoDevice::closeDevice()
{
#if !defined(USE_LIBUSB) //elmo del
    if(m_notification != 0)
	{
        IOObjectRelease(m_notification);
		m_notification = 0;
	}
	if(m_notifyPort != NULL)
	{
        IONotificationPortDestroy(m_notifyPort);
		m_notifyPort = NULL;
	}

	//デバイスClose処理
	if(m_usbDeviceRef != 0)
	{
		if((m_ucReadEP != m_ucReadEP2 && m_ucWriteEP != m_ucWriteEP2) &&
			m_device != NULL)
		{
            (*m_device)->USBDeviceReEnumerate(m_device, 0);
		}

		if(m_usbInterfaceRef != 0)
		{
            closeInterfaces();
			IOObjectRelease(m_usbInterfaceRef);
		}
		IOObjectRelease(m_findDeviceIterator);
		m_findDeviceIterator = 0;

		if(m_device != NULL)
		{
            (*m_device)->USBDeviceClose(m_device);
			(*m_device)->Release(m_device);
			m_device = NULL;
		}

		IOObjectRelease(m_usbDeviceRef);
		m_usbDeviceRef = 0;
	}

    if(m_ioIterator != 0)
	{
        IOObjectRelease(m_ioIterator);
		m_ioIterator = 0;
	}

    if(m_masterPort != 0)
	{
        mach_port_deallocate(mach_task_self(), m_masterPort);
		m_masterPort = 0;
	}

	m_ucReadEP = 0;
	m_ucReadEP2 = 0;
	m_ucWriteEP = 0;
	m_ucWriteEP2 = 0;
#if 0
    if(m_clThread.isRunning() == false)
	{
		m_clThread.start(QThread::IdlePriority);	//ポーリングスレッド起動
	}
#endif
#else //elmo add
    if(isOpened() == true)
    {
        while(m_clClaimedInterfaces.isEmpty() == false)
        {
            libusb_release_interface(m_pstDeviceHandle, m_clClaimedInterfaces.takeFirst());	//インターフェース使用登録解除
        }
        libusb_close(m_pstDeviceHandle);
        m_pstDeviceHandle = NULL;
        m_bIsOpened = false;
    }
#endif
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		インターフェースクローズ
*/
//--------------------------------------------------------------------------------//
void ClIoDevice::closeInterfaces()
{
	IOReturn error;
	error = (*m_interface)->USBInterfaceClose(m_interface);
	if(error != 0)
	{
		return;
	}
    (*m_interface)->Release(m_interface);
}
//--------------------------------------------------------------------------------//

#if defined(USE_LIBUSB) //elmo add
//--------------------------------------------------------------------------------//
/*!
    @brief	オープンしているか取得
    @retval	オープンしていたらtrue
*/
//--------------------------------------------------------------------------------//
bool ClIoDevice::isOpened()
{
    return m_bIsOpened;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		書き込み
    @param[in]	clData : 書き込むデータ
    @param[in]	lTimeout : 書き込みタイムアウト時間(msec)
    @param[in]	ucEndPoint : 書き込むエンドポイント番号
    @retval		書き込んだデータサイズ、負数ならばエラー値
*/
//--------------------------------------------------------------------------------//
int ClIoDevice::write(QByteArray clData, qint32 lTimeout, quint8 ucEndPoint)
{
    if(isOpened() == false)
    {
        return LIBUSB_ERROR_NO_DEVICE;	//開いてない
    }

    quint8 ucUseEndPoint = getEndPoint(ucEndPoint);	//エンドポイント取得
    //quint8 ucUseEndPoint = getEndPoint(LIBUSB_ENDPOINT_OUT & ucEndPoint);	//エンドポイント取得
    if(ucUseEndPoint == 0)
    {
        qDebug("Not found endpoint");
        return LIBUSB_ERROR_NO_DEVICE;	//エンドポイントがなかった
    }

    unsigned char *pucData = reinterpret_cast<unsigned char*>(clData.data());
    int lLength = clData.length();
    int iRet = 0;

    int iRes = libusb_bulk_transfer(m_pstDeviceHandle, ucUseEndPoint, pucData,
                                    lLength, &iRet, lTimeout);
    printf("[write >> 0x%02x %d >> %d]",ucUseEndPoint,iRet,iRes);
    if(iRes != LIBUSB_SUCCESS)
    {
        qDebug("write error:%d", iRes);
        if(iRes == LIBUSB_ERROR_NO_DEVICE)
        {
            closeDevice();
        }
        //エラー値設定
        if(iRet == LIBUSB_ERROR_NO_DEVICE)
        {
            iRet = IM_ERROR_NO_DEVICE;	//デバイスがない
        }
        else if(iRet == LIBUSB_ERROR_BUSY)
        {
            iRet = IM_ERROR_DEVICE_BUSY;	//ビジー
        }
        else if(iRet == LIBUSB_ERROR_TIMEOUT)
        {
            iRet = IM_ERROR_FATAL;
        }
        else if(iRet < 0)
        {
            iRet = IM_ERROR_FATAL;
        }
        return iRet;
    }

    return iRet;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		読み込み
    @param[in]	lTimeout : 読み込みタイムアウト時間(msec)
    @param[in]	ucEndPoint : 読み込むエンドポイント番号
    @param[out]	plError : エラー値格納先
    @retval		読み込んだデータ
*/
//--------------------------------------------------------------------------------//
QByteArray ClIoDevice::read(qint32 lTimeout, quint8 ucEndPoint, qint32 *plError)
{
    QByteArray clRet;
    if(isOpened() == false)
    {
        if(plError != NULL)
        {
            *plError = LIBUSB_ERROR_NO_DEVICE;
        }
        return clRet;	//開いてない
    }

    quint8 ucUseEndPoint = getEndPoint(ucEndPoint);	//エンドポイント取得
    //quint8 ucUseEndPoint = getEndPoint(LIBUSB_ENDPOINT_IN & ucEndPoint);	//エンドポイント取得
    if(ucUseEndPoint == 0)
    {
        if(plError != NULL)
        {
            *plError = LIBUSB_ERROR_NO_DEVICE;
        }
        qDebug("Not found endpoint");
        return clRet;	//エンドポイントがなかった
    }

    unsigned char ucReadBuffer[JPEG_PICTURESIZE] = { 0x00 };
    int iLength = 0;	//受信サイズ
    int iRet = LIBUSB_SUCCESS;

    iRet = libusb_bulk_transfer(m_pstDeviceHandle, ucUseEndPoint, ucReadBuffer,
                                    sizeof(ucReadBuffer), &iLength, lTimeout);
    printf("[read >> 0x%02x %d >> %d]",ucUseEndPoint,iLength,iRet);
    if(iRet == LIBUSB_SUCCESS)
    {
        clRet.append(reinterpret_cast<char*>(ucReadBuffer), iLength);	//受信データ追加
    }
    else if(iRet == LIBUSB_ERROR_NO_DEVICE)
    {
        closeDevice();
        if(plError != NULL)
        {
            *plError = IM_ERROR_NO_DEVICE;
        }
        return QByteArray();
    }
    else
    {
        qDebug("read error:%d", iRet);
    }

    if(plError != NULL)
    {
        //エラー値設定
        if(iRet == LIBUSB_ERROR_NO_DEVICE)
        {
            *plError = IM_ERROR_NO_DEVICE;	//デバイスがない
        }
        else if(iRet == LIBUSB_ERROR_BUSY)
        {
            *plError = IM_ERROR_DEVICE_BUSY;	//ビジー
        }
        else if(iRet == LIBUSB_ERROR_TIMEOUT)
        {
            *plError = IM_ERROR_FATAL;
        }
        else if(iRet < 0)
        {
            *plError = IM_ERROR_FATAL;
        }
    }

    return clRet;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		エンドポイント取得
    @param[in]	ucEndpoint : 使おうとするエンドポイント
    @retval		ucEndpointが使用可能ならucEndpoint
    @retval		ucEndPointが使用不可ならリストではじめに見つかった同じ向きのエンドポイント
    @retval		使えるエンドポイントがなければ0
*/
//--------------------------------------------------------------------------------//
quint8 ClIoDevice::getEndPoint(quint8 ucEndpoint)
{
    quint8 ucRet = 0;
    quint8 ucEndpointInout = (ucEndpoint & 0x80);	//エンドポイントの向き
    if(m_clEndpointList.contains(ucEndpoint) == false)		//使えないエンドポイントだった
    {
        for(int i = 0; i < m_clEndpointList.count(); i++)
        {
            quint8 ucTemp = m_clEndpointList.at(i);
            if((ucTemp & 0x80) == ucEndpointInout)	//向きが同じ
            {
                ucRet = ucTemp;	//使えるエンドポイントに変更
                break;
            }
        }
    }
    else
    {
        ucRet = ucEndpoint;	//そのまま使えた
    }
    return ucRet;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief	エンドポイント探索
    @retval	エラー値
*/
//--------------------------------------------------------------------------------//
int ClIoDevice::findEndpoints()
{
    m_clEndpointList.clear();	//エンドポイントリストクリア

    int iRet = LIBUSB_SUCCESS;
    //デバイスデスクリプタ
    libusb_device_descriptor stDeviceDescriptor;
    iRet = libusb_get_device_descriptor(m_pstDevice, &stDeviceDescriptor);
    if(iRet != LIBUSB_SUCCESS)
    {
        qDebug("Get device descriptor error:%d", iRet);
        return iRet;
    }

    int iRetFind = LIBUSB_SUCCESS;
    bool bIsLoopEnd = false;	//ループ終了フラグ
    for(int i = 0; i < stDeviceDescriptor.bNumConfigurations; i++)
    {
        //コンフィグデスクリプタ
        libusb_config_descriptor *pstConfigDescriptor;
        iRetFind = libusb_get_config_descriptor(m_pstDevice, i, &pstConfigDescriptor);
        if(iRetFind != LIBUSB_SUCCESS)
        {
            qDebug("Get config descriptor error:%d", iRet);
            continue;	//取得失敗
        }

        //インターフェース
        for(int j = 0; j < pstConfigDescriptor->bNumInterfaces; j++)
        {
            libusb_interface stInterface = pstConfigDescriptor->interface[j];

            //インターフェースデスクリプタ
            for(int k = 0; k < stInterface.num_altsetting; k++)
            {
                libusb_interface_descriptor stInterfaceDescriptor = stInterface.altsetting[k];

                if(1 == libusb_kernel_driver_active(m_pstDeviceHandle, j))
                {
                    if(0 != libusb_detach_kernel_driver(m_pstDeviceHandle, j))
                    {
                        break;
                    }
                }

                int iRetClaim = libusb_claim_interface(m_pstDeviceHandle, j);	//インターフェース使用登録
                if(iRetClaim != LIBUSB_SUCCESS)
                {
                    break;	//登録できなかったので次のインターフェース
                }
                else
                {
                    m_clClaimedInterfaces.append(j);	//使用インターフェースリストに登録

                    ///libusb_set_configurationした方が良さそうだがすると書きできなくなるのでやらない
                    //エンドポイントデスクリプタ
                    for(int l = 0; l < stInterfaceDescriptor.bNumEndpoints; l++)
                    {
                        libusb_endpoint_descriptor stEndpointDescriptor = stInterfaceDescriptor.endpoint[l];
                        m_clEndpointList.append(stEndpointDescriptor.bEndpointAddress);	//エンドポイント追加
                        //printf("[end point :: 0x%02x %d]",stEndpointDescriptor.bEndpointAddress,stInterfaceDescriptor.bDescriptorType);
                    }
                    bIsLoopEnd = true;	//ループ終了する
                }
            }
            if(bIsLoopEnd == true)
            {
                break;	//探索終了
            }
        }

        libusb_free_config_descriptor(pstConfigDescriptor);	//コンフィグデスクリプタ解放
        if(bIsLoopEnd == true)
        {
            break;	//探索終了
        }
    }

    if(m_clEndpointList.isEmpty() == true)
    {
        iRet = LIBUSB_ERROR_NOT_FOUND;	//見つからなかったエラー
    }
    return iRet;
}
//--------------------------------------------------------------------------------//
#endif
