#include "ClL12Presenter.h"

//--------------------------------------------------------------------------------
ClUsbDriverForIM4::ClUsbDriverForIM4(QWidget *parent)
    :	ClUsbDriver(parent)
{
#if defined(Q_OS_WIN)
    m_bIsEnableOtherCamera = false;		//他社カメラ有効/無効フラグ 無効
    m_bInitialized = true;
#elif defined(Q_OS_MAC)
    m_bIsInitialized = false;
    ClUsbDriver::initialize();
#endif
 }
ClUsbDriverForIM4::~ClUsbDriverForIM4()
{

}
//--------------------------------------------------------------------------------

qint32 ClUsbDriverForIM4::initialize()
{
#if defined(Q_OS_WIN)
    //IM4用にInitialize時はスキャンを行わない
    m_bInitialized = true;
#elif defined(Q_OS_MAC)
    m_bIsInitialized = true;
#endif
    return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------

qint32 ClUsbDriverForIM4::scan(ClCamDevice **ppCamDev)
{
    if(isInitialize() == false){
        return IM_ERROR_NOT_INIT;
    }
    if(NULL != *ppCamDev){
        return IM_ERROR_INVALID_PARAMETER;
    }
#if defined(Q_OS_WIN)
    //check device
    QMapIterator<qint32, QUuid> qIte(m_clProductList);
    while(qIte.hasNext())
    {
        qIte.next();
        //register detection of device notification
        DEV_BROADCAST_DEVICEINTERFACE devInfo;
        devInfo.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
        devInfo.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        devInfo.dbcc_classguid = qIte.value();

        HDEVNOTIFY hDevNotify = ::RegisterDeviceNotification((HANDLE)winId(),
                                                             &devInfo, DEVICE_NOTIFY_WINDOW_HANDLE);
        if(hDevNotify <= 0)
        {
            return IM_ERROR_NOT_INIT;
        }

        //check connect device
        int index = 0;		    //device number
        HDEVINFO hDevinfo = NULL;
        const GUID stGuid = qIte.value();
        hDevinfo = ::SetupDiGetClassDevs(&stGuid, NULL, NULL,
                                         DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);	//get device info set
        if(hDevinfo == INVALID_HANDLE_VALUE)
        {
            return IM_ERROR_NOT_INIT;
        }

        SP_INTERFACE_DEVICE_DATA stInterfaceDevData;
        stInterfaceDevData.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);
        bool bFoundDev = true;									//device found flag
        while(::SetupDiEnumDeviceInterfaces(hDevinfo, NULL, &stGuid, index,
                                            &stInterfaceDevData) == FALSE)			//get device info
        {
            qint32 lError = ::GetLastError();
            if(lError != 0 && lError != ERROR_NO_MORE_ITEMS)	//unexpected error
            {
                ::SetupDiDestroyDeviceInfoList(hDevinfo);
                return IM_ERROR_NOT_INIT;
            }
            else if(lError == ERROR_NO_MORE_ITEMS)				//search for device to last but not found
            {
                bFoundDev = false;
                break;
            }
            index++;
        }

        //	デバイスパス取得
        DWORD		ulSize = 0;
        QString		strDevPath;
        char		*pcAlloc = NULL;

        if( false != bFoundDev )
        {
            SetupDiGetInterfaceDeviceDetail( hDevinfo, &stInterfaceDevData, NULL, 0, &ulSize, NULL );
            pcAlloc = new char[ulSize];
            if( NULL != pcAlloc )
            {
                PSP_INTERFACE_DEVICE_DETAIL_DATA  pstDetail = (PSP_INTERFACE_DEVICE_DETAIL_DATA)pcAlloc;
                pstDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
                if( FALSE != SetupDiGetInterfaceDeviceDetail( hDevinfo,
                                                              &stInterfaceDevData, pstDetail, ulSize, NULL, NULL ) )
                {
                    strDevPath = QString::fromWCharArray( pstDetail->DevicePath ).toLower();

                    if(true == strDevPath.contains("vid_09a1")){
                        qint32	lProductId = GETPRODUCTID( qIte.key() );
                        qint32	lPid = 0;
                        int		iIndex1 = 0, iIndex2 = 0;
                        QString	strTemp;

                        iIndex1 = strDevPath.indexOf( "pid_", 0, Qt::CaseInsensitive );
                        iIndex2 = iIndex1 + 4;
                        while( strDevPath.count() >= iIndex2 )
                        {
                            if( 0 == isxdigit( strDevPath[iIndex2].toLatin1() ) )
                            {
                                break;
                            }
                            iIndex2++;
                        }
                        strTemp = strDevPath.mid( iIndex1 + 4, iIndex2 - iIndex1 - 4 );
                        bool bRet = false;
                        lPid = strTemp.toLong( &bRet, 16 );
                        if( false == bRet ){
                            lPid = 0;
                        }
                        //何らかの書画カメラを発見
                        if(lProductId == lPid){
                            ClCamDevice* pCamDev = new ClCamDevice;
                            stCameraInfo info;
                            info.id = strDevPath;
                            info.tag = NULL;
                            info.conn_type = stCameraInfo::ELUSB;
                            info.inUse = true;
                            info.scannedTime = QDateTime::currentDateTime();
                            info.type = stCameraInfo::DOC_CAM;
                            info.name = "Document Camera";
                            pCamDev->setInfo(info);

                            *ppCamDev = pCamDev;
                        }
                    }
                }
                delete[] pcAlloc;
                pcAlloc = NULL;
                pstDetail = NULL;
            }
        }

        ::SetupDiDestroyDeviceInfoList(hDevinfo);
        //カメラデバイスを得た
        if(NULL != *ppCamDev){
            return IM_ERROR_SUCCESS;
        }
    }
#elif defined(Q_OS_MAC)
    int id = 0;
    if((id = ClUsbDriver::checkDeviceAll()) > 0)
    {
        QString clStrPID = QString();
        clStrPID.sprintf("%08x",id);
        ClCamDevice* pCamDev = new ClCamDevice;
        stCameraInfo info;
        info.id = clStrPID;
        info.tag = NULL;
        info.conn_type = stCameraInfo::ELUSB;
        info.inUse = true;
        info.scannedTime = QDateTime::currentDateTime();
        info.type = stCameraInfo::DOC_CAM;
        info.name = "Document Camera";
        pCamDev->setInfo(info);
        *ppCamDev = pCamDev;
    }
    if(NULL != *ppCamDev)
    {
        return IM_ERROR_SUCCESS;
    }
#endif
    return IM_ERROR_NO_DEVICE;
}
//--------------------------------------------------------------------------------
qint32 ClUsbDriverForIM4::openDevice(qint32 lProductId, QString strDevPath)
{
#if defined(Q_OS_WIN)
    //slotOpenDeviceからオープンするには条件が厳しいので裏口を作る
    return ClUsbDriver::openDevice(lProductId, strDevPath);
#elif defined(Q_OS_MAC)
    return ClUsbDriver::openDevice(lProductId);
#endif
}
//--------------------------------------------------------------------------------
qint32 ClUsbDriverForIM4::startStream(qint32 lProductId, eKINDSTREAM kind, qint32 lFrate, qint32 lQuality)
{
    return ClUsbDriver::startStream(lProductId, kind, lFrate, lQuality);

}
//--------------------------------------------------------------------------------
qint32 ClUsbDriverForIM4::stopStream(qint32 lProductId, eKINDSTREAM iKind)
{
    return ClUsbDriver::stopStream(lProductId, iKind);

}
//--------------------------------------------------------------------------------
void ClUsbDriverForIM4::slotZoom(qint32 lProductId, qint32 command)
{
    ClUsbDriver::slotZoom(lProductId, command);
}

//--------------------------------------------------------------------------------
void ClUsbDriverForIM4::slotFocus(qint32 lProductId, qint32 command)
{
    ClUsbDriver::slotFocus(lProductId, command);
}

//--------------------------------------------------------------------------------
void ClUsbDriverForIM4::slotIris(qint32 lProductId, qint32 command)
{
    ClUsbDriver::slotIris(lProductId, command);
}

//--------------------------------------------------------------------------------
void ClUsbDriverForIM4::slotRotate(qint32 lProductId, qint32 command)
{
    ClUsbDriver::slotRotate(lProductId, command);
}

//--------------------------------------------------------------------------------
