﻿//#define __USB_DEBUG
#include "ClUsbDriver/ClUsbDriver_global.h"
#include "ClUsbDriver/ClUsbDriver_win.h"
#include "ClUsbDriver/Command_Presenter.h"
#include "Common/CmnDefine.h"
#include "DriverDefine.h"
#include "ClImDrvCtrl.h"
#include "EL_GUIDs.h"
#include "setupapi.h"			//
#include "qtdxuttimer.h"
#include "win32/Ricoh_lib/ClAdaptRicohDeviceCtl.h"

#include <QMessageBox>

#define	DEFAULT_RECV_PARAM	(1)

//--------------------------------------------------------------------------------
/*!
	@fn			ClThreadDeviceOpen
	@brief		Constructor
 */
//--------------------------------------------------------------------------------
ClThreadDeviceOpen::ClThreadDeviceOpen(QWidget *parent, QMap<qint32, QString> map) :
	QThread( parent ),
	m_mapTarget( map )
{
	m_bStop = false;
	m_bSend = false;
	m_lReceive = DEFAULT_RECV_PARAM;
}
//--------------------------------------------------------------------------------
/*!
	@fn			~ClThreadDeviceOpen
	@brief		Destructor
 */
//--------------------------------------------------------------------------------
ClThreadDeviceOpen::~ClThreadDeviceOpen()
{
	m_mapTarget.clear();
}
//--------------------------------------------------------------------------------
/*!
	@fn			run
	@brief
	@return
 */
//--------------------------------------------------------------------------------
void ClThreadDeviceOpen::run()
{
	qint32 lProductId = PRODUCT_NONE;

	while(m_mapTarget.isEmpty() == false && m_bStop == false)
	{
		if(m_bSend == false && m_lReceive == DEFAULT_RECV_PARAM)
		{
			m_bSend = true;
			m_lReceive  = DEFAULT_RECV_PARAM;
#if 0 //PC内蔵カメラのインカムが最初に選択されたくないとのことだったので2つ目のカメラからオープンするようにしたもの
			lProductId = m_mapTarget.keys().first();
#else
			if(m_mapTarget.keys().count() > 1)										//リストに2つ以上登録がある場合
			{
				lProductId = m_mapTarget.keys().at(1);								//2つ目のカメラのプロダクトID取得
			}
			else																	//リストに1つ以下の登録しかない場合
			{
				lProductId = m_mapTarget.keys().first();							//1つ目のカメラのプロダクトIDを取得
			}
#endif
#ifdef QT_DEBUG
			qDebug() << "open thread -> " << lProductId;
#endif
			emit sigOpenDevice( lProductId );
		}
		else if(m_bSend == true && m_lReceive == IM_ERROR_SUCCESS)
		{
#ifdef QT_DEBUG
			qDebug() << "open thread -> success " << lProductId;
#endif
			return;
		}
		else if(m_bSend == true && m_lReceive < IM_ERROR_SUCCESS)
		{
#ifdef QT_DEBUG
			qDebug() << "open thread -> failure " << lProductId;
#endif
			m_bSend = false;
			m_lReceive  = DEFAULT_RECV_PARAM;
			m_mapTarget.remove( lProductId );
		}
#ifdef Q_OS_WIN
		Sleep(500);
#else
		usleep(500000);
#endif
	}
}

//--------------------------------------------------------------------------------
/*!
	@fn			ClUsbDriver
	@brief		Constructor
 */
//--------------------------------------------------------------------------------
ClUsbDriver::ClUsbDriver(QWidget *parent = 0) : QWidget(parent)
{
	m_lErrLevel		= 3;						//set default for m_lErrLevel
	m_bInitialized	= false;					//set false for flagInitialize
	m_bZeroImage	= false;
	m_bStreamState	= false;					//0:stop stream, 1:start stream

	//register to ELMO production list
	m_clProductList[PRODUCT_P10S]	= QUuid(GUID_P10S);
	m_clProductList[PRODUCT_L1N]	= QUuid(GUID_L1N);
	m_clProductList[PRODUCT_FF1]	= QUuid(GUID_FF1);
	m_clProductList[PRODUCT_P30S]	= QUuid(GUID_P30S);
	m_clProductList[PRODUCT_L1EX]	= QUuid(GUID_L1EX);
	m_clProductList[PRODUCT_P10]	= QUuid(GUID_P10);
	m_clProductList[PRODUCT_MC1EX]	= QUuid(GUID_MC1EX);
	m_clProductList[PRODUCT_CO10]	= QUuid(GUID_CO10);
	m_clProductList[PRODUCT_SESAME]	= QUuid(GUID_SESAME);
	m_clProductList[PRODUCT_MOBILE]	= QUuid(GUID_MOBILE);

	qRegisterMetaType<eKINDSTREAM>("eKINDSTREAM");		//SIGNAL-SLOTで使えるようにQVariantに登録

	m_qStrSendFilePath.clear();
	m_mapUvcPathV.clear();
	m_mapUvcPathA.clear();

	m_mapConnectProduct.clear();
	m_mapOpenProduct.clear();
	m_lProductIdOther = PRODUCT_NONE;
	m_pclThreadDeviceOpen = NULL;

	m_bIsEnableOtherCamera = true;				//他社カメラの有効/無効判定フラグ(初期値=有効)
}
//--------------------------------------------------------------------------------
/*!
	@fn			~ClUsbDriver
	@brief		Destructor
 */
//--------------------------------------------------------------------------------
ClUsbDriver::~ClUsbDriver()
{
	if(isInitialize() == true){
		closeDevice();
	}
	m_mapUvcPathV.clear();
	m_mapUvcPathA.clear();
}
//--------------------------------------------------------------------------------
/*!
	@fn			initialize
	@brief
	@return
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::initialize()
{
	if(m_bInitialized == true){
		return IM_ERROR_SUCCESS;
	}

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
		while(1)
		{
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
						if( m_bIsEnableOtherCamera == false )
						{
							strDevPath = QString::fromWCharArray( pstDetail->DevicePath );
						}
						else
						{
							strDevPath = QString::fromWCharArray( pstDetail->DevicePath ).toLower();
						}
					}
					delete[] pcAlloc;
					pcAlloc = NULL;
					pstDetail = NULL;
				}
			}

			::SetupDiDestroyDeviceInfoList(hDevinfo);
			if(bFoundDev == true)									 //found device
			{
				if( false != checkDevPath( strDevPath, qIte.value() ) )
				{
					if( m_bIsEnableOtherCamera == true )
					{
						if(m_mapUvcPathV.contains( qIte.key() ) == false)
						{
							m_mapConnectProduct.insert( qIte.key(), strDevPath );			//挿されたデバイスとしてリストに登録
						}
						else
						{
							m_mapConnectProduct.insert( qIte.key(), m_mapUvcPathV.value( qIte.key(), QString() ) );
						}
					}
					m_bInitialized = true;
					qint32 lRes = IM_ERROR_SUCCESS;
					lRes = openDevice(qIte.key(), strDevPath);
					if(lRes != IM_ERROR_SUCCESS){
#ifdef QT_DEBUG
						qDebug() << "elmo open err" << qIte.key() << strDevPath;
#endif
						emit sigErrorNormal(lRes);
					}else{
#ifdef QT_DEBUG
						qDebug() << "elmo open suc" << qIte.key() << strDevPath;
#endif
						emit sigOpenDevice(qIte.key());
					}
					m_bInitialized = false;
				}
				else if( false != checkDevPathOther( strDevPath, qIte.value() ) )
				{
					if(m_mapUvcPathV.contains( m_lProductIdOther ) == false)
					{
						m_mapConnectProduct.insert( m_lProductIdOther, strDevPath );	//挿されたデバイスとしてリストに登録
					}
					else
					{
						m_mapConnectProduct.insert( m_lProductIdOther, m_mapUvcPathV.value( m_lProductIdOther, QString() ) );
					}
				}
			}
			else
			{
				break;
			}
			index++;
		}
	}
	m_bInitialized = true;

	cretaeThreadDeviceOpen();															//接続スレッド生成

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			isInitialize
	@brief
	@return
 */
//--------------------------------------------------------------------------------
bool ClUsbDriver::isInitialize()
{
	return m_bInitialized;
}
//--------------------------------------------------------------------------------
/*!
	@fn			openDevice
	@brief
	@return		ProductId of opened device
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::openDevice(qint32 lProductId, QString strDevPath)
{
	ClImDrvCtrl *pImCtrl = NULL;
	if(isInitialize() == false){
#ifdef QT_DEBUG
		qDebug() << "openDevice :: not initialze -> " << lProductId;
#endif
		return IM_ERROR_NOT_INIT;
	}

	if(isOpened(lProductId) == true){
#ifdef QT_DEBUG
		qDebug() << "openDevice :: already open device -> " << lProductId;
#endif
		return IM_ERROR_ALREADY_OPENED;
	}
	if( m_bIsEnableOtherCamera == true )
	{
		if(m_mapOpenProduct.isEmpty() == false)
		{
#ifdef QT_DEBUG
			qDebug() << "openDevice :: reg already open device list -> " << lProductId;
#endif
			return IM_ERROR_ALREADY_OPENED;
		}
		if( (lProductId >> 16) != 0x09a1 && GETPRODUCTID( PRODUCT_MOBILE ) == lProductId )
		{
			if( ( false == m_mapUvcPathV.contains( lProductId ) )  ||
				( false == m_mapUvcPathA.contains( lProductId ) )
			)
			{
#ifdef QT_DEBUG
				qDebug() << "openDevice :: uvc path is not contains -> " << lProductId;
#endif
				return IM_ERROR_NOT_OPEN;
			}
			strDevPath = m_mapUvcPathV.value( lProductId, QString() );
		}
	}
	//	UVC系の場合、映像と音声が両方揃っていたら、
	//	映像デバイスのパスを使ってopenする
	if( PRODUCT_MOBILE == lProductId )
	{
		if( ( false == m_mapUvcPathV.contains( lProductId ) )  ||
			( false == m_mapUvcPathA.contains( lProductId ) )
		)
		{
#ifdef QT_DEBUG
			qDebug() << "openDevice :: uvc path is not contains -> " << lProductId;
#endif
			return IM_ERROR_NOT_OPEN;
		}
		strDevPath = m_mapUvcPathV.value( lProductId, QString() );
	}
#if 0 // Qt6ではここで強制終了するのでイテレータはやめる
	QMap<qint32, QObject*>::const_iterator qIte = m_ImDrvCtrlList.find(lProductId);
	while(qIte++ != m_ImDrvCtrlList.end()){
        if(qIte.key() == lProductId){
            pImCtrl = dynamic_cast<ClImDrvCtrl*>(qIte.value());
        }
	}
#else
    QObject* pValue = m_ImDrvCtrlList.value(lProductId);
    if(pValue != NULL){
        pImCtrl = dynamic_cast<ClImDrvCtrl*>(pValue);
    }
#endif
	if(pImCtrl == NULL){
		pImCtrl = new ClImDrvCtrl(lProductId, parent());
		if(pImCtrl->initialize() != IM_ERROR_SUCCESS){
#ifdef QT_DEBUG
			qDebug() << "openDevice :: initialize failure -> " << lProductId;
#endif
			delete pImCtrl;
			pImCtrl = NULL;
			return IM_ERROR_NOT_INIT;
		}
	}
	if(pImCtrl->OpenDriver(strDevPath) == false){
#ifdef QT_DEBUG
		qDebug() << "openDevice :: open driver failure -> " << lProductId;
#endif
		DBGFL("OpenDriver failure");
		delete pImCtrl;
		pImCtrl = NULL;
		return IM_ERROR_NOT_OPEN;
	}
	m_ImDrvCtrlList[lProductId] = pImCtrl;

	if( m_bIsEnableOtherCamera == true )
	{
		m_mapOpenProduct.insert( lProductId, strDevPath );
		if( m_mapConnectProduct.contains( lProductId ) == false )
		{
			if(m_mapUvcPathV.contains( m_lProductIdOther ) == false)
			{
				m_mapConnectProduct.insert( lProductId, strDevPath );
			}
			else
			{
				m_mapConnectProduct.insert( lProductId, m_mapUvcPathV.value( lProductId, QString() ) );
			}
		}
#ifdef QT_DEBUG
		qDebug() << "openDevice :: open device -> " << lProductId;
#endif
	}

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			closeDevice
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::closeDevice(qint32 lProductId)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux

	//コマンド失敗で閉じられてしまった場合にList消去されないため、ここでListから外す
	if(isOpened(lProductId) == false && m_ImDrvCtrlList.contains(lProductId))
	{
		pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
		m_ImDrvCtrlList.remove(lProductId);
		if(pImCtrl != NULL)
		{
			delete pImCtrl;
			pImCtrl = NULL;
		}
	}

	if( m_bIsEnableOtherCamera == false )
	{
		//	UVC用リストクリア
		if( 0 == lProductId )
		{
			m_mapUvcPathV.clear();
			m_mapUvcPathA.clear();
		}
		else
		{
			m_mapUvcPathV.remove( lProductId );
			m_mapUvcPathA.remove( lProductId );
		}
	}

	if(isInitialize() == false || isOpened(lProductId) == false){		//if is not initialized or is not opened
#ifdef QT_DEBUG
		qDebug() << "closeDevice :: check status is failure -> " << isInitialize() << isOpened(lProductId);
#endif
		return;															//return
	}
	if(lProductId == 0){
		QMapIterator<qint32, QObject*> ite(m_ImDrvCtrlList);		//create iterator in look for ELMO productions
		while(ite.hasNext()){
			ite.next();
			pImCtrl = dynamic_cast<ClImDrvCtrl*>(ite.value());
			if(pImCtrl != NULL){
				stopStream(pImCtrl->getProductId(), JPEG_STREAM);		//stop stream
				stopStream(pImCtrl->getProductId(), RAW_STREAM);		//stop stream

				pImCtrl->CloseDriver();								//Call for CloseDriver
				m_ImDrvCtrlList.remove(ite.key());

				if( m_bIsEnableOtherCamera == true )
				{
					m_mapOpenProduct.remove( lProductId );
#ifdef QT_DEBUG
					qDebug() << "close device -> " << lProductId;
#endif
				}

				delete pImCtrl;
				pImCtrl = NULL;
			}
		}
	}else{
		stopStream(lProductId, JPEG_STREAM);						//stop stream
		stopStream(lProductId, RAW_STREAM);							//stop stream

		pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
		if(pImCtrl == NULL){									//if pImCtrl is NULL
#ifdef QT_DEBUG
			qDebug() << "closeDevice :: ClImDrvCtrl is null -> " << lProductId;
#endif
			return;												//return
		}

		pImCtrl->CloseDriver();									//Call for CloseDriver
		m_ImDrvCtrlList.remove(lProductId);

		if( m_bIsEnableOtherCamera == true )
		{
			m_mapOpenProduct.remove( lProductId );
#ifdef QT_DEBUG
			qDebug() << "close device -> " << lProductId;
#endif
		}

		delete pImCtrl;
		pImCtrl = NULL;
	}

	return;
}
//--------------------------------------------------------------------------------
/*!
	@fn			startStream
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::startStream(qint32 lProductId, eKINDSTREAM iKind, qint32 iFrate, qint32 iQuality)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false){							// is not initialized
		return IM_ERROR_NOT_INIT;							// return IM_ERROR_NOT_INIT
	}

	if(isOpened(lProductId) == false){						// is not opened
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	if(iFrate <= 0 || iFrate > 30){							//fool proof
		iFrate = 30;
	}

	if(iQuality <= 0 || iQuality > 100){					//quality
		iQuality = 80;
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}
	return pImCtrl->startStream(this,iKind,iFrate,iQuality);
}
//--------------------------------------------------------------------------------
/*!
	@fn			stopStream
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::stopStream(qint32 lProductId, eKINDSTREAM iKind)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false){							// is not initialized
		return IM_ERROR_NOT_INIT;							// return IM_ERROR_NOT_INIT
	}
	if(isOpened(lProductId) == false){						// is not opened
		return IM_ERROR_NOT_OPEN;							// return IM_ERROR_NOT_OPEN
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}
	return pImCtrl->stopStream(iKind);
}

//--------------------------------------------------------------------------------
/*!
	@fn			isOpened
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
bool ClUsbDriver::isOpened(qint32 lProductId)
{
	bool bRes = false;

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	if(isInitialize() == false){									// is not initialized
		return false;												//return false
	}
	if(lProductId == 0){
		QMapIterator<qint32, QObject*> ite(m_ImDrvCtrlList);		//create iterator in look for ELMO productions
		while(ite.hasNext()){
			ite.next();
			pImCtrl = dynamic_cast<ClImDrvCtrl*>(ite.value());
			if(pImCtrl != NULL){
				bRes = pImCtrl->IsOpenDriver();						//Call for CloseDriver
			}
		}
	}else{
		pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
		if(pImCtrl == NULL){										//if pImCtrl is NULL

			if( m_bIsEnableOtherCamera == true )
			{
				m_mapOpenProduct.remove( lProductId );				//別の場所で閉じられていることがあるので使用中リストにある場合はリストから削除する
			}

			return false;											//return false
		}
		bRes =  pImCtrl->IsOpenDriver();							//return whether a device opened
	}

	if( m_bIsEnableOtherCamera == true )
	{
		if(bRes == false)
		{
			m_mapOpenProduct.remove( lProductId );					//別の場所で閉じられていることがあるので使用中リストにある場合はリストから削除する
		}
	}

	return bRes;
}
//--------------------------------------------------------------------------------
/*!
	@fn			isStreaming
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
bool ClUsbDriver::isStreaming(qint32 lProductId, eKINDSTREAM iKind)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false){							// is not initialized
		return false;										// return false
	}
	if(isOpened(lProductId) == false){						// is not opened
#if 0
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS)
#endif
		{
			return false;									// return IM_ERROR_NOT_OPEN
		}
	}
	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return false;										//return false
	}
	return pImCtrl->isStreaming(iKind);
}
//--------------------------------------------------------------------------------
/*!
	@fn			getPictureRect
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::getPictureRect(qint32 lProductId, QSize &qSize)
{
	qint32 lRet = IM_ERROR_SUCCESS;

	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	qint32 lHeight = 0;
	qint32 lWidth = 0;

	if(isInitialize() == false)
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_HEIGHT_PICTURE, qParam);
	if(lRet == IM_ERROR_SUCCESS)
	{
		if(qParam.type() == QVariant::Int){
			lHeight = qParam.toInt();
		}else{
			return IM_ERROR_OTHER;
		}
	}
	lRet = pImCtrl->CommandPresenter(GET_WIDTH_PICTURE, qParam);
	if(lRet == IM_ERROR_SUCCESS)
	{
		if(qParam.type() == QVariant::Int){
			lWidth = qParam.toInt();
		}else{
			return IM_ERROR_OTHER;
		}
		qSize.setHeight(lHeight);
		qSize.setWidth(lWidth);
	}

	return lRet;
}

//--------------------------------------------------------------------------------
/*!
	@fn			getRectId
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::getRectId(qint32 lProductId, eKINDRECTID &lRectId)
{
	qint32 lRet = IM_ERROR_SUCCESS;

	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux

	if(isInitialize() == false)
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_RECT_PICTURE, qParam);

	if(lRet == IM_ERROR_SUCCESS)
	{
		if(qParam.type() == QVariant::UserType){
			lRectId = qParam.value<eKINDRECTID>();
		}else{
			lRet = IM_ERROR_OTHER;
		}
	}

	return lRet;
}
//--------------------------------------------------------------------------------
/*!
	@fn			getPictureSize
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::getPictureSize(qint32 lProductId, qint32 &lSizeId)
{
	qint32 lRet = IM_ERROR_SUCCESS;

	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false)
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_SIZE_PICTURE, qParam);
	if(lRet == IM_ERROR_SUCCESS)
	{
		if(qParam.type() == QVariant::Int){
			lSizeId = qParam.toInt();
		}else{
			lRet = IM_ERROR_OTHER;
		}
	}
	return lRet;
}
//--------------------------------------------------------------------------------
/*!
	@fn			setRectId(qint32, eKINDRECTID)
	@brief		set sizeID of picture
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::setRectId(qint32 lProductId, eKINDRECTID lRectId)
{
	qint32 lRet = IM_ERROR_SUCCESS;

	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false)
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}
	QVariant qParam;
//	qParam.setValue<eKINDRECTID>(lRectId);  //qt6
    qParam = lRectId;
	lRet = pImCtrl->CommandPresenter(SET_RECT_PICTURE, qParam);

	return lRet;
}
//--------------------------------------------------------------------------------
/*!
	@fn			execCameraControl(qint32, qint32)
	@brief		exec camera control command
	@return		result of exec
	@param		lProductId	:	productID
				command		:	command
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::execCameraControl(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;

	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false)								// is not initialized
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}
	QVariant param;
	lRet = pImCtrl->CommandPresenter(command, param);

	return lRet;
}
//--------------------------------------------------------------------------------
/*!
	@fn			captureJpeg(qint32, eKINDSTREAM, QByteArray&, QVariant&)
	@brief		capture JPEG image
	@return		result
	@param		lProductId			:	productID
	@param		qOutputJpgData		:	reference of store capture data
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::captureJpeg(qint32 lProductId, QByteArray &qOutputJpgData, QVariant &qParam, eKINDRECTID lRectId = RECTID_PICTURE_DEFAULT)
{

	qint32 lRes = IM_ERROR_SUCCESS;

	ClImDrvCtrl	*pImCtrl = NULL;								//Image Mate driver control for Linux
	QVariant qvParam;
	if(isInitialize() == false)									// is not initialized
	{
		return IM_ERROR_NOT_INIT;
	}

	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;							// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){										//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;								//return IM_ERROR_NOT_OPEN
	}

	if(lRectId == RECTID_PICTURE_DEFAULT){
		qint32 lRectRet = pImCtrl->CommandPresenter(GET_RECT_PICTURE, qvParam);
		if(lRectRet == IM_ERROR_SUCCESS){
			if(qvParam.type() != QVariant::UserType){
				qvParam.setValue<eKINDRECTID>(RECTID_PICTURE_DEFAULT);
			}
		}else{
			qvParam.setValue<eKINDRECTID>(RECTID_PICTURE_DEFAULT);
		}
	}else{
		qvParam = lRectId;
	}

	QVariant qImgData;
	if(m_bStreamState == true)									//ストリーム中の場合は、ストリームで取得した画像を取りに行く
	{
		lRes = pImCtrl->getStreamImg(qImgData);
	}

	if(m_bStreamState == false ||								//ストリーム中でない場合は、画像取得コマンド送信
				qImgData.toByteArray().isNull() )
	{
		QString qStrRectId = QString("%1").arg(lRectId);		//RectIDを送信する型Stringに変換
		if( false			== qvParam.isNull() &&				//NULLでない場合
			qvParam.type()	== QVariant::Int )					//Int型の値が格納されている場合
		{
			qint32 lRectId = qvParam.toInt();					//Intに変換
			qStrRectId = QString("%1").arg(lRectId);			//格納されている値を使用する
		}

		QString qStrQuality = QString("80");					//品質を送信する型Stringに変換
		if(!qParam.isNull()){
			if(qParam.type() == QVariant::Int){					//タイプがIntの場合
				qint32 iQuality = qParam.toInt();				//Int型に変換
				if(iQuality <= 0 || iQuality > 100){			//品質の値が不正な場合は 80 に設定する
					iQuality = 80;
				}
				qStrQuality = QString("%1").arg(iQuality);		//String型に変換して設定
			}else if(qParam.type() == QVariant::String){		//タイプがStringの場合
				qStrQuality = qParam.toString();				//String型に変換して設定
			}else{												//その他のタイプの場合
				return IM_ERROR_INVALID_PARAMETER;				//エラー
			}
		}

		QStringList qStrList;									//RectIdと画像の圧縮率を送るためリストにする
		qStrList.append(qStrRectId);
		qStrList.append(qStrQuality);
		QVariant qvParamList = qStrList;						//StringListを送るが、返ってくるのはByteArrayの画像データがくるので注意
		lRes = pImCtrl->CommandPresenter(GET_PICTURE,qvParamList);
		qvParam = qvParamList;
	}else{
		qvParam = qImgData;
	}

	if(lRes == IM_ERROR_SUCCESS)
	{
		if(qvParam.type() == QVariant::ByteArray){
			qOutputJpgData = qvParam.toByteArray();
		}
	}

	return lRes;
}
//--------------------------------------------------------------------------------
/*!
	@fn			captureRaw(qint32, eKINDSTREAM, QImage&, QVariant&)
	@brief		capture Rawdata image
	@return		result
	@param		lProductId	:	productID
				data		:	reference of store capture data
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::captureRaw(qint32 lProductId, QImage &qOutputRawData, QVariant &qParam, eKINDRECTID lRectId = RECTID_PICTURE_DEFAULT)
{
	qint32 lRes = IM_ERROR_SUCCESS;

	QByteArray qJpgData;
	lRes = captureJpeg(lProductId, qJpgData, qParam, lRectId);
	if(lRes == IM_ERROR_SUCCESS)
	{
		if(qOutputRawData.loadFromData(qJpgData, "jpg") == false)
		{
			lRes = IM_ERROR_DECORD;
		}
	}
	return lRes;
}
//--------------------------------------------------------------------------------
/*!
	@fn			chkAllDeviceInfo
	@brief
	@return		QString
 */
//--------------------------------------------------------------------------------
QString ClUsbDriver::chkAllDeviceInfo()
{
	return "I make it soon";
}

//--------------------------------------------------------------------------------
/*!
	@fn			sendImgFileInfoV2(qint32, QVariant)
	@brief		send image file info
	@return		result
	@param		lProductId			:	productID
	@param		qParam				:	image file info
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::sendImgFileInfoV2(qint32 lProductId, QVariant &qParam)
{
	qint32 lRes = IM_ERROR_SUCCESS;
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false)								// is not initialized
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}
	lRes = pImCtrl->CommandPresenter(SEND_UPIM_INFO_V2,qParam);
	return lRes;
}
//--------------------------------------------------------------------------------
/*!
	@fn			sendFwFileInfoV2(qint32, QVariant)
	@brief		send fw file info
	@return		result
	@param		lProductId			:	productID
	@param		qParam				:	fw file info
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::sendFwFileInfoV2(qint32 lProductId, QVariant &qParam)
{
	qint32 lRes = IM_ERROR_SUCCESS;
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false)								// is not initialized
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}
	lRes = pImCtrl->CommandPresenter(SEND_UPFW_INFO_V2,qParam);

	//送信ファイルのパスを記憶
	if(lRes == IM_ERROR_SUCCESS && qParam.type() == QVariant::StringList){
		m_qStrSendFilePath = qParam.toStringList().at(0);
#ifdef QT_DEBUG
		qDebug() << "copy file path" << m_qStrSendFilePath;
#endif
	}

	return lRes;
}
//--------------------------------------------------------------------------------
/*!
	@fn			uploadFileV2(qint32, QVariant)
	@brief		upload file
	@return		result
	@param		lProductId			:	productID
	@param		qParam				:	file data
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::uploadFileV2(qint32 lProductId, QVariant &qParam)
{
	qint32 lRes = IM_ERROR_SUCCESS;
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false)								// is not initialized
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}

	if(m_qStrSendFilePath.isNull()){						//ファイル情報送信済みか確認
		return IM_ERROR_COMMAND_FAILURE;
	}
	qParam = m_qStrSendFilePath;
	lRes = pImCtrl->startSendFile(this, qParam);
	m_qStrSendFilePath.clear();								//一度送ったら消去
	return lRes;
}

//--------------------------------------------------------------------------------
/*!
	@fn			winEvent
	@brief
	@return		NONE
*/
//--------------------------------------------------------------------------------
bool ClUsbDriver::winEvent(MSG *message, long *result)
{
	bool bRet = false;
	*result = true;
	PDEV_BROADCAST_HDR pstBroadcastHdr;

	if(message->message == WM_DEVICECHANGE)
	{
		if(message->wParam == DBT_DEVICEARRIVAL)				//signal of connect device
		{
			pstBroadcastHdr = reinterpret_cast<PDEV_BROADCAST_HDR>(message->lParam);
			if(pstBroadcastHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
			{
				DEV_BROADCAST_DEVICEINTERFACE *pDevInterface = reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE*>(pstBroadcastHdr);
				if( m_bIsEnableOtherCamera == false )
				{
					QString  strDevPath = QString::fromWCharArray( pDevInterface->dbcc_name );
					//discriminate presenter
					QMapIterator<qint32, QUuid> qIte(m_clProductList);
					while(qIte.hasNext())
					{
						qIte.next();
						if( ( qIte.value() == pDevInterface->dbcc_classguid ) &&				//挿されたデバイスであるか判定
							( false != checkDevPath( strDevPath, qIte.value() ) )				//自社カメラが接続された
						)
						{
#ifdef QT_DEBUG
							qDebug() << "elmo open exec" << qIte.key();
#endif
							qint32 lRes = IM_ERROR_SUCCESS;
							lRes = openDevice(qIte.key(), strDevPath);
							if(lRes != IM_ERROR_SUCCESS){
#ifdef QT_DEBUG
								qDebug() << "elmo open err" << qIte.key() << strDevPath;
#endif
								emit sigErrorNormal(lRes);
							}else{
#ifdef QT_DEBUG
								qDebug() << "elmo open suc" << qIte.key() << strDevPath;
#endif
								emit sigOpenDevice(qIte.key());
							}
						}
					}
				}
				else
				{
					bool bIsOtherCam = false;
					QString  strDevPath = QString::fromWCharArray( pDevInterface->dbcc_name ).toLower();
					//discriminate presenter
					QMapIterator<qint32, QUuid> qIte(m_clProductList);
					while(qIte.hasNext())
					{
						qIte.next();
						if( ( qIte.value() == pDevInterface->dbcc_classguid ) &&				//挿されたデバイスであるか判定
							( false != checkDevPath( strDevPath, qIte.value() ) )				//自社カメラが接続された
						)
						{
							bIsOtherCam = false;
							if(m_mapUvcPathV.contains( qIte.key() ) == false)
							{
								m_mapConnectProduct.insert( qIte.key(), strDevPath );			//挿されたデバイスとしてリストに登録
							}
							else
							{
								m_mapConnectProduct.insert( qIte.key(), m_mapUvcPathV.value( qIte.key(), QString() ) );
							}
							qint32 lRes = IM_ERROR_SUCCESS;
							lRes = openDevice(qIte.key(), strDevPath);
							if(lRes != IM_ERROR_SUCCESS){
#ifdef QT_DEBUG
								qDebug() << "elmo open err" << qIte.key() << strDevPath;
#endif
								emit sigErrorNormal(lRes);
							}else{
#ifdef QT_DEBUG
								qDebug() << "elmo open suc" << qIte.key() << strDevPath;
#endif
								emit sigOpenDevice(qIte.key());
							}
						}
						else if( ( qIte.value() == pDevInterface->dbcc_classguid ) &&			//挿されたデバイスであるか判定
								 ( false != checkDevPathOther( strDevPath, qIte.value() ) )		//他社カメラが接続された
						)
						{
							bIsOtherCam = true;
							if(m_mapUvcPathV.contains( m_lProductIdOther ) == false)
							{
								m_mapConnectProduct.insert( m_lProductIdOther, strDevPath );	//挿されたデバイスとしてリストに登録
							}
							else
							{
								m_mapConnectProduct.insert( m_lProductIdOther, m_mapUvcPathV.value( m_lProductIdOther, QString() ) );
							}
						}
					}
					if( bIsOtherCam == true )													//
					{
						cretaeThreadDeviceOpen();												//接続スレッド生成
					}
				}
			}
		}
		else if(message->wParam == DBT_DEVICEREMOVECOMPLETE)	//signal of device disconnect
		{
			pstBroadcastHdr = reinterpret_cast<PDEV_BROADCAST_HDR>(message->lParam);
			if(pstBroadcastHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE)
			{
				DEV_BROADCAST_DEVICEINTERFACE *pDevInterface = reinterpret_cast<DEV_BROADCAST_DEVICEINTERFACE*>(pstBroadcastHdr);

				if( m_bIsEnableOtherCamera == false )
				{
					QString  strDevPath = QString::fromWCharArray( pDevInterface->dbcc_name );
					//discriminate presenter
					QMapIterator<qint32, QUuid> qIte(m_clProductList);
					while(qIte.hasNext())
					{
						qIte.next();
						if(qIte.value() == pDevInterface->dbcc_classguid)						//抜かれたデバイスであるか判定
						{
							//	MO-1, CO-10の場合はDevicePathも確認する
							if( ( GUID_MOBILE == pDevInterface->dbcc_classguid )  ||
								( GUID_CO10 == pDevInterface->dbcc_classguid )
							)
							{
								QString  strUvcPathV = m_mapUvcPathV.value( qIte.key(), QString() );
								if( ( false == strUvcPathV.isEmpty() )  &&
									( 0 == strUvcPathV.compare( strDevPath, Qt::CaseInsensitive ) )
								)
								{
									closeDevice(qIte.key());
									emit sigCloseDevice(qIte.key());
									break;
								}
							}
							//	それ以外は今まで通り
							else
							{
								closeDevice(qIte.key());
								emit sigCloseDevice(qIte.key());
							}
						}
					}
				}
				else
				{
					QString  strDevPath = QString::fromWCharArray( pDevInterface->dbcc_name ).toLower();
					if(m_mapConnectProduct.values().contains( strDevPath ) == true)		//検出デバイスリストに含まれている
					{
						qint32 lProductId = m_mapConnectProduct.key( strDevPath );		//プロダクトID取得

						//検出デバイスリスト削除
						if(m_mapConnectProduct.contains( lProductId ) == true)
						{
							m_mapConnectProduct.remove( lProductId );					//念のため削除
						}

						//接続デバイスに含まれていないなら切断する
						if(m_mapOpenProduct.values().contains( strDevPath ) == false)	//接続デバイスリストに含まれていない
						{
							//使用中リストに登録があるかを念のため確認
							bool bIsRegListOpen = m_mapOpenProduct.contains( lProductId );

							//MO-1/CO-10/他社製品
							bool bIsClose = false;
							if( lProductId == PRODUCT_MOBILE || lProductId == PRODUCT_CO10 || (lProductId >> 16) != 0x09a1 )
							{
								QString  strUvcPathV = m_mapUvcPathV.value( lProductId, QString() );
								if( ( false == strUvcPathV.isEmpty() )  &&
									( 0 == strUvcPathV.compare( strDevPath, Qt::CaseInsensitive ) )
								)
								{
#ifdef QT_DEBUG
									qDebug() << "connect uvc close" << lProductId << strDevPath;
#endif
									closeDevice( lProductId );
									if(bIsRegListOpen == true)
									{
										emit sigCloseDevice( lProductId );
									}
									bIsClose = true;
								}
							}
							//ELMO
							else
							{
#ifdef QT_DEBUG
								qDebug() << "connect elmo close" << lProductId << strDevPath;
#endif
								closeDevice( lProductId );
								if(bIsRegListOpen == true)
								{
									emit sigCloseDevice( lProductId );
								}
								bIsClose = true;
							}

							//実行判定
							if(bIsClose == true)
							{
								//UVCリスト削除
								if(m_mapUvcPathV.contains( lProductId ) == true)
								{
									m_mapUvcPathV.remove( lProductId );					//closeDevice時に削除しないのでここで削除する
								}
								if(m_mapUvcPathA.contains( lProductId ) == true)
								{
									m_mapUvcPathA.remove( lProductId );					//
								}
							}
						}
					}
					if(m_mapOpenProduct.values().contains( strDevPath ) == true)		//接続デバイスリストに含まれている
					{
						qint32 lProductId = m_mapOpenProduct.key( strDevPath );			//プロダクトID取得

						//MO-1/CO-10/他社製品
						bool bIsClose = false;
						if( lProductId == PRODUCT_MOBILE || lProductId == PRODUCT_CO10 || (lProductId >> 16) != 0x09a1 )
						{
							QString  strUvcPathV = m_mapUvcPathV.value( lProductId, QString() );
							if( ( false == strUvcPathV.isEmpty() )  &&
								( 0 == strUvcPathV.compare( strDevPath, Qt::CaseInsensitive ) )
							)
							{
#ifdef QT_DEBUG
								qDebug() << "use uvc close" << lProductId << strDevPath;
#endif
								closeDevice( lProductId );
								emit sigCloseDevice( lProductId );
								bIsClose = true;
							}
						}
						//ELMO
						else
						{
#ifdef QT_DEBUG
							qDebug() << "use elmo close" << lProductId << strDevPath;
#endif
							closeDevice( lProductId );
							emit sigCloseDevice( lProductId );
							bIsClose = true;
						}

						//実行判定
						if(bIsClose == true)
						{
							//UVCリスト削除
							if(m_mapUvcPathV.contains( lProductId ) == true)
							{
								m_mapUvcPathV.remove( lProductId );						//closeDevice時に削除しないのでここで削除する
							}
							if(m_mapUvcPathA.contains( lProductId ) == true)
							{
								m_mapUvcPathA.remove( lProductId );						//
							}

							//接続デバイスリスト削除
							if(m_mapOpenProduct.contains( lProductId ) == true)
							{
								m_mapOpenProduct.remove( lProductId );					//念のため削除
							}
						}
					}
				}
			}
		}
	}
	return bRet;
}

//--------------------------------------------------------------------------------
/*!
	@fn			slotOpenDevice
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotOpenDevice(qint32 lProductId)
{
	if(isOpened(lProductId) == true){
		return;
	}
	qint32 lRes = openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString()));
	if(lRes != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRes);
	}else{
		emit sigOpenDevice(lProductId);
	}
	return;
}
//--------------------------------------------------------------------------------
/*!
	@fn			slotCloseDevice
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotCloseDevice(qint32 lProductId)
{
	if(isOpened(lProductId) != true){
		return;
	}
	closeDevice(lProductId);
	emit sigCloseDevice(lProductId);
	return;
}

//--------------------------------------------------------------------------------
/*!
	@fn			slotStartZeroImage
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotStartZeroImage(qint32 lProductId, eKINDSTREAM iKind)
{
	if(m_bZeroImage == true){
		return;
	}
	m_bZeroImage = true;
	emit sigStartZeroImage(lProductId,iKind);
}
//--------------------------------------------------------------------------------
/*!
	@fn			slotEndZeroImage
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotEndZeroImage(qint32 lProductId, eKINDSTREAM iKind)
{
	if(m_bZeroImage == false){
		return;
	}
	m_bZeroImage = false;
	emit sigEndZeroImage(lProductId,iKind);
}
//--------------------------------------------------------------------------------
/*!
    @fn			slotGetStreamFail
    @brief
    @return		NONE
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetStreamFail(qint32 lProductId, eKINDSTREAM iKind)
{
    emit sigGetStreamFail(lProductId,iKind);
}
//--------------------------------------------------------------------------------
/*!
	@fn			slotTerminate
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotTerminate(qint32 lProductId, eKINDSTREAM eKind )
{
	m_bZeroImage = false;
	emit sigTerminate( lProductId, eKind );
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotStartStream(qint32 lProductId, eKINDSTREAM iKind, qint32 lFrate, qint32 lQuality)
	@brief	start capture stream
	@param	lProductId	:	productID
			iKind		:	image type
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotStartStream(qint32 lProductId, eKINDSTREAM iKind, qint32 lFrate, qint32 lQuality)
{
	qint32 lRes = startStream(lProductId, iKind, lFrate, lQuality);
	if(lRes != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRes);
	}
    m_bStreamState = true;
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotStopStream(qint32, eKINDSTREAM)
	@brief	stop capture stream
	@param	lProductId	:	productID
			iKind		:	image type
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotStopStream(qint32 lProductId, eKINDSTREAM iKind)
{
    m_bStreamState = false;
	if(isStreaming(lProductId, iKind) == true){
		qint32 lRes = stopStream(lProductId, iKind);
		if(lRes != IM_ERROR_SUCCESS){
			emit sigErrorNormal(lRes);
		}
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetFRate(qint32, qint32, eKINDSTREAM)
	@brief	get frame lRate
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
			iKind		:	image type
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetFRate(qint32 lProductId, eKINDSTREAM iKind, qint32 &lFrate)
{

	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isStreaming(lProductId, iKind) == true)
	{
		pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
		if(pImCtrl == NULL){								//if pImCtrl is NULL
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;											//return IM_ERROR_NOT_OPEN
		}
		lFrate = pImCtrl->getFrameRate(iKind);
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotSetFRate(qint32, eKINDSTREAM, qint32)
	@brief	set frame lRate
	@param	lProductId	:	productID
			iKind		:	image type
			lFrate		:	frame lRate
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotSetFRate(qint32 lProductId, eKINDSTREAM iKind, qint32 lFrate)
{

	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isStreaming(lProductId, iKind) == true)
	{
		pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
		if(pImCtrl == NULL){								//if pImCtrl is NULL
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;											//return IM_ERROR_NOT_OPEN
		}
		pImCtrl->setFrameRate(lFrate, iKind);
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetQuality(qint32, qint32, eKINDSTREAM)
	@brief	get image quality
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
			iKind		:	image type
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetQuality(qint32 lProductId, eKINDSTREAM iKind, qint32 &lQuality)
{

	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isStreaming(lProductId, iKind) == true)
	{
		pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
		if(pImCtrl == NULL){								//if pImCtrl is NULL
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;											//return IM_ERROR_NOT_OPEN
		}
		lQuality = pImCtrl->getQuality(iKind);
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotSetQuality(qint32, eKINDSTREAM, qint32)
	@brief	set image quality
	@param	lProductId	:	productID
			iKind		:	image type
			lQuality	:	image quality
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotSetQuality(qint32 lProductId, eKINDSTREAM iKind, qint32 lQuality)
{

	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isStreaming(lProductId, iKind) == true)
	{
		pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
		if(pImCtrl == NULL){								//if pImCtrl is NULL
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;											//return IM_ERROR_NOT_OPEN
		}
		pImCtrl->setQuality(lQuality, iKind);
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetStreamParam(qint32, qint32, eKINDSTREAM, qint32, qint32, qint32)
	@brief	get stream param
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
			iKind		:	image type
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetStreamParam(qint32 lProductId, eKINDSTREAM iKind, qint32 &lType, qint32 &lCompType, qint32 &lFrate)
{

	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isStreaming(lProductId, iKind) == true)
	{
		pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
		if(pImCtrl == NULL){								//if pImCtrl is NULL
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;											//return IM_ERROR_NOT_OPEN
		}
		pImCtrl->getStreamParam(iKind, lType, lCompType, lFrate);
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotSetStreamParam(qint32, eKINDSTREAM, qint32, qint32, qint32, qint32)
	@brief	set stream param
	@param	lProductId	:	productID
			iKind		:	image type
			lQuality	:	image quality
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotSetStreamParam(qint32 lProductId, eKINDSTREAM iKind, qint32 lType, qint32 lCompType, qint32 lFrate)
{

	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){								//if pImCtrl is NULL
		emit sigErrorNormal(IM_ERROR_NOT_OPEN);
		return;											//return IM_ERROR_NOT_OPEN
	}
	pImCtrl->setStreamParam(iKind, lType, lCompType, lFrate);

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotCapture(qint32, qint32, eKINDSTREAM)
	@brief	image capture
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
			iKind		:	image type
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotCapture(qint32 lProductId, eKINDSTREAM iKind, qint32 &lStatus, QVariant &qParam, eKINDRECTID lRectId = RECTID_PICTURE_DEFAULT)
{
	if(isInitialize() == false)
	{
		return;
	}

	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	qint32 lRes = IM_ERROR_SUCCESS;
	switch(iKind)
	{
	case JPEG_STREAM:
		{
			QByteArray qArray;
			lRes = captureJpeg(lProductId, qArray, qParam, lRectId);
			if(lRes == IM_ERROR_SUCCESS){
				qParam = qArray;
			}else if(lRes == IM_ERROR_CANNOT_OUTPUT_IMAGE){
				emit sigZeroCapture(lProductId);
			}else{
				emit sigErrorNormal(lRes);
			}
			lStatus = lRes;											//set lStatus of lRes
			break;
		}
	case RAW_STREAM:
		{
			QImage qImg;
			lRes = captureRaw(lProductId, qImg, qParam, lRectId);
			if(lRes == IM_ERROR_SUCCESS){
				qParam = qImg;
			}else if(lRes == IM_ERROR_CANNOT_OUTPUT_IMAGE){
				emit sigZeroCapture(lProductId);
			}else{
				emit sigErrorNormal(lRes);
			}
			lStatus = lRes;
			break;
		}
	default:
		lStatus = IM_ERROR_FATAL;
		emit sigErrorFatal(IM_ERROR_FATAL);
		return;
	}
}
/* ---------------------------------------------------------------------------- +/
									V1 Command
/+ ---------------------------------------------------------------------------- */
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetPresenterName(qint32, qint32)
	@brief	get presenter name
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetPresenterName(qint32 lProductId, QString &qName)
{

	ClImDrvCtrl	*pImCtrl = NULL;								//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;												// return IM_ERROR_NOT_OPEN
		}
	}
	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){										//if pImCtrl is NULL
		return;													//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_MODEL_NAME, qParam);	//

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::String){
		qName = qParam.toString();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetPresenterVersion(qint32, qint32)
	@brief	get presenter version
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetPresenterVersion(qint32 lProductId, QStringList &qVerlist)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}
	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_MODEL_VERSION, qParam);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::StringList){
		qVerlist = qParam.toStringList();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetPresenterList(qint32)
	@brief	get connect presenter list
	@param	ID			:	ID that is sent signal
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetPresenterList(QVariantList &qSendList)
{
	//create list because can't remove list when disconnect presenter
	if( m_bIsEnableOtherCamera == false )
	{
		QMapIterator<qint32, QObject*> qIterate(m_ImDrvCtrlList);
		while(qIterate.hasNext()){
			qIterate.next();
			ClImDrvCtrl *pIm = dynamic_cast<ClImDrvCtrl*>(qIterate.value());
			if(pIm != NULL){
				qSendList << pIm->getProductId();
			}
		}
	}
	else
	{
		QList<qint32> clList = m_mapConnectProduct.keys();
		while(clList.isEmpty() == false)
		{
			qSendList << clList.first();
			clList.removeFirst();
		}
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetPictureRect(qint32, eKINDSTREAM)
	@brief	get size of image
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetPictureRect(qint32 lProductId, QSize &qSize)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = getPictureRect(lProductId, qSize);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------6
/*!
	@fn		slotGetRectId(qint32, eKINDRECTID)
	@brief	get sizeID of image
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetRectId(qint32 lProductId, eKINDRECTID &lRectId)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = getRectId(lProductId, lRectId);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetPictureSize(qint32, qint32)
	@brief	get sizeID of image
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetPictureSize(qint32 lProductId, qint32 &lPictSize)
{
    qint32 lRet = IM_ERROR_SUCCESS;
	lRet = getPictureSize(lProductId, lPictSize);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------6
/*!
	@fn		slotSetRectId(qint32, eKINDRECTID)
	@brief	set sizeID of image
	@param	lProductId	:	productID
			sizeId		:	sizeID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotSetRectId(qint32 lProductId, eKINDRECTID lRectId)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = setRectId(lProductId, lRectId);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotZoom(qint32, qint32)
	@brief	zoom
	@param	lProductId	:	productID
			command		:	command
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotZoom(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotIris(qint32, qint32)
	@brief	iris
	@param	lProductId	:	productID
			command		:	command
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotIris(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotFocus(qint32, qint32)
	@brief	focus
	@param	lProductId	:	productID
			command		:	command
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotFocus(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotRotate(qint32, qint32)
	@brief	image rotate
	@param	lProductId	:	productID
			command		:	command
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotRotate(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotPosiNega(qint32, qint32)
	@brief	posi,nega
	@param	lProductId	:	productID
			command		:	command
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotPosiNega(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotColorBw(qint32, qint32)
	@brief	color,blackwhite
	@param	lProductId	:	prodctID
			command		:	command
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotColorBw(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotSlowShutter(qint32, qint32)
	@brief	slowshutter
	@param	lProductId	:	productID
			command		:	command
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotSlowShutter(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotImageMode(qint32, qint32)
	@brief	imagemode
	@param	lProductId	:	productID
			command		:	command
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotImageMode(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotMicroscopeMode(qint32, qint32)
	@brief	microscope mode
	@param	lProductId	:	productID
			command		:	commnad
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotMicroscopeMode(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotChkAllDevice()
	@brief	print all device for debug
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotChkAllDevice()
{
#ifdef QT_DEBUG
	qDebug() << chkAllDeviceInfo();
#endif
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotSetTranslator()
	@brief
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::slotSetTranslator(qint32 lProductId)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false)								// is not initialized
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}

	pImCtrl->setTranslator();

	return IM_ERROR_SUCCESS;

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotStartInputStream()
	@brief
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::slotStartInputStream(qint32 lProductId)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false)								// is not initialized
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}

	pImCtrl->startInputStream();

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotStopInputStream()
	@brief
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::slotStopInputStream(qint32 lProductId)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false)								// is not initialized
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		return IM_ERROR_NOT_OPEN;							// return IM_ERROR_NOT_OPEN
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}

	pImCtrl->stopInputStream();

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotReopenDevice()
	@brief
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotReopenDevice(qint32 lProductId)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false)								// is not initialized
	{
		return;
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return;							//return IM_ERROR_NOT_OPEN
	}

	pImCtrl->reopen();
}

/* ---------------------------------------------------------------------------- +/
									V2 Command
/+ ---------------------------------------------------------------------------- */
//--------------------------------------------------------------------------------
/*!
	@fn		slotZoomV2(qint32, qint32)
	@brief	zoom
	@param	lProductId	:	productID
			command		:	command
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotZoomV2(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotSetVariantZoomV2(qint32, qint32)
	@brief	zoom
	@param	lProductId	:	productID
			command		:	command
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotSetVariantZoomV2(qint32 lProductId, qint32 lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;								//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;												// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){										//if pImCtrl is NULL
		return;													//return
	}
	QVariant qVariant = lParam;
	lRet = pImCtrl->CommandPresenter(SET_VARIANT_ZOOM_V2, qVariant);	//

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetZoomStatusV2(qint32, qint32)
	@brief	get presenter name
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetZoomStatusV2(qint32 lProductId, qint32 &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;								//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;												// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){										//if pImCtrl is NULL
		return;													//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_ZOOM_STATUS_V2, qParam);	//

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::Int){
		lParam = qParam.toInt();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotFocusV2(qint32, qint32)
	@brief	focus
	@param	lProductId	:	productID
			command		:	command
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotFocusV2(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotSetVariantFocusV2(qint32, qint32)
	@brief	zoom
	@param	lProductId	:	productID
			command		:	command
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotSetVariantFocusV2(qint32 lProductId, qint32 lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;								//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;												// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){										//if pImCtrl is NULL
		return;													//return
	}
	QVariant qVariant = lParam;
	lRet = pImCtrl->CommandPresenter(SET_VARIANT_FOCUS_V2, qVariant);	//

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetFocusStatusV2(qint32, qint32)
	@brief	get presenter name
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetFocusStatusV2(qint32 lProductId, qint32 &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;								//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;												// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){										//if pImCtrl is NULL
		return;													//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_FOCUS_STATUS_V2, qParam);	//

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::Int){
		lParam = qParam.toInt();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotSetAFModeV2(qint32, qint32)
	@brief	get presenter name
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotSetAFModeV2(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetAFModeStatusV2(qint32, qint32)
	@brief	get presenter name
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetAFModeStatusV2(qint32 lProductId, qint32 &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;								//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;												// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){										//if pImCtrl is NULL
		return;													//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_AFMODE_STATUS_V2, qParam);	//

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::Int){
		lParam = qParam.toInt();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotIrisV2(qint32, qint32)
	@brief	iris
	@param	lProductId	:	productID
			command		:	command
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotIrisV2(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotSetVariantIrisV2(qint32, qint32)
	@brief	zoom
	@param	lProductId	:	productID
			command		:	command
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotSetVariantIrisV2(qint32 lProductId, qint32 lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;								//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;												// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){										//if pImCtrl is NULL
		return;													//return
	}
	QVariant qVariant = lParam;
	lRet = pImCtrl->CommandPresenter(SET_VARIANT_IRIS_V2, qVariant);	//

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetIrisStatusV2(qint32, qint32)
	@brief	get presenter name
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetIrisStatusV2(qint32 lProductId, qint32 &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;								//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;												// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){										//if pImCtrl is NULL
		return;													//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_IRIS_STATUS_V2, qParam);	//

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::Int){
		lParam = qParam.toInt();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotSetBRTModeV2(qint32, qint32)
	@brief	get presenter name
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotSetBRTModeV2(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetBRTModeStatusV2(qint32, qint32)
	@brief	get presenter name
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetBRTModeStatusV2(qint32 lProductId, qint32 &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;								//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;												// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){										//if pImCtrl is NULL
		return;													//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_BRTMODE_STATUS_V2, qParam);	//

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::Int){
		lParam = qParam.toInt();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotSendImgFileInfoV2(qint32,qint32,QVariant&)
	@brief	send upload image file info
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotSendImgFileInfoV2(qint32 lProductId,qint32 command,QVariant &qParam)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = sendImgFileInfoV2(lProductId, qParam);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotSendUploadFileInfoV2(qint32,qint32,QVariant&)
	@brief	send upload fw file info
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotSendFwFileInfoV2(qint32 lProductId,qint32 command,QVariant &qParam)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = sendFwFileInfoV2(lProductId, qParam);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotUploadFileV2(qint32,qint32,QVariant&)
	@brief	upload file
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotUploadFileV2(qint32 lProductId, qint32 command, QVariant &qParam)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = uploadFileV2(lProductId, qParam);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetPresenterNameV2(qint32, qint32)
	@brief	get presenter name
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetPresenterNameV2(qint32 lProductId, QString &qName)
{

	ClImDrvCtrl	*pImCtrl = NULL;								//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;												// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){										//if pImCtrl is NULL
		return;													//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_MODEL_NAME_V2, qParam);	//

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::String){
		qName = qParam.toString();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetPresenterVersionV2(qint32, qint32)
	@brief	get presenter version
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetPresenterVersionV2(qint32 lProductId, QStringList &qVerlist)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_MODEL_VERSION_V2, qParam);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::StringList){
		qVerlist = qParam.toStringList();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetPresenterSHVersionV2(qint32, qint32)
	@brief	get presenter sh version
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetPresenterSHVersionV2(qint32 lProductId, QString &qVersion)
{

	ClImDrvCtrl	*pImCtrl = NULL;								//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;												// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){										//if pImCtrl is NULL
		return;													//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_SH_VERSION_V2, qParam);	//

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::String){
		qVersion = qParam.toString();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetPresenterLensVersionV2(qint32, qint32)
	@brief	get presenter lens version
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetPresenterLensVersionV2(qint32 lProductId, QString &qVersion)
{

	ClImDrvCtrl	*pImCtrl = NULL;								//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;												// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){										//if pImCtrl is NULL
		return;													//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_LENS_VERSION_V2, qParam);	//

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::String){
		qVersion = qParam.toString();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetPresenterFPGAVersionV2(qint32, qint32)
	@brief	get presenter fpga version
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetPresenterFPGAVersionV2(qint32 lProductId, QString &qVersion)
{

	ClImDrvCtrl	*pImCtrl = NULL;								//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;												// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){										//if pImCtrl is NULL
		return;													//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_FPGA_VERSION_V2, qParam);	//

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::String){
		qVersion = qParam.toString();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetProductIdV2(qint32, qint32)
	@brief	get product id
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetProductIdV2(qint32 lProductId, QByteArray &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_PRODUCTCODE_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::ByteArray){
		lParam = qParam.toByteArray();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetCurrentModeV2(qint32, QStringList)
	@brief	get current mode
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetCurrentModeV2(qint32 lProductId, QStringList &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_CURRENT_MODE_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::StringList){
		lParam = qParam.toStringList();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetResSwV2(qint32, qint32)
	@brief	get resolution switch
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetResSwV2(qint32 lProductId, qint32 &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_RES_SW_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::Int){
#ifdef QT_DEBUG
		qDebug() << "get res sw" << qParam.type() << qParam.toInt();
#endif
		lParam = qParam.toInt();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetImgOutSwV2(qint32, qint32)
	@brief	get Image Output Switch
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetImgOutSwV2(qint32 lProductId, qint32 &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_IMOUT_SW_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::Int){
		lParam = qParam.toInt();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetCapPushFlgV2(qint32, qint32)
	@brief	get capture button push flg
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetCapPushFlgV2(qint32 lProductId, qint32 &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_CAPPUSH_FLG_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::Int){
		lParam = qParam.toInt();
	}

}

//--------------------------------------------------------------------------------
/*!
	@fn		slotSetSystemTimeV2(qint32, QDateTime)
	@brief	set system time
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotSetSystemTimeV2(qint32 lProductId, QDateTime &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam = lParam;
	lRet = pImCtrl->CommandPresenter(SET_SYSTEM_TIME_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotSetModelNameV2(qint32, QByteArray)
	@brief	set model name
	@param	lProductId	:	productID
			command		:	commnad
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotSetModelNameV2(qint32 lProductId, QByteArray lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam = lParam;
	lRet = pImCtrl->CommandPresenter(SET_MODEL_NAME_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotSetProductCodeV2(qint32, QByteArray)
	@brief	save setting
	@param	lProductId	:	productID
			command		:	commnad
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotSetProductCodeV2(qint32 lProductId, QByteArray lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam = lParam;
	lRet = pImCtrl->CommandPresenter(SET_PRODUCTCODE_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotWhiteBalanceV2(qint32, qint32)
	@brief	set white balance status
	@param	lProductId	:	productID
			command		:	commnad
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotWhiteBalanceV2(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetWhiteStatusV2(qint32, qint32)
	@brief	get white balance status
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetWhiteStatusV2(qint32 lProductId, qint32 &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_WHITE_STATUS_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::Int){
		lParam = qParam.toInt();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotFlickerV2(qint32, qint32)
	@brief	set flicker status
	@param	lProductId	:	productID
			command		:	commnad
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotFlickerV2(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetFlickerStatusV2(qint32, qint32)
	@brief	get flicker status
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetFlickerStatusV2(qint32 lProductId, qint32 &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_FLICKER_STATUS_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::Int){
		lParam = qParam.toInt();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotImageModeV2(qint32, qint32)
	@brief	set image mode
	@param	lProductId	:	productID
			command		:	commnad
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotImageModeV2(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetImageModeStatusV2(qint32, qint32)
	@brief	get image mode
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetImageModeStatusV2(qint32 lProductId, qint32 &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_IM_MODE_STATUS_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::Int){
		lParam = qParam.toInt();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotEdgeV2(qint32, qint32)
	@brief	set edge
	@param	lProductId	:	productID
			command		:	commnad
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotEdgeV2(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetEdgeStatusV2(qint32, qint32)
	@brief	get image mode
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetEdgeStatusV2(qint32 lProductId, qint32 &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_EDGE_STATUS_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::Int){
		lParam = qParam.toInt();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGammaV2(qint32, qint32)
	@brief	set gamma
	@param	lProductId	:	productID
			command		:	commnad
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGammaV2(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetGammaStatusV2(qint32, qint32)
	@brief	get gamma status
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetGammaStatusV2(qint32 lProductId, qint32 &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_GAMMA_STATUS_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::Int){
		lParam = qParam.toInt();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotFreezeV2(qint32, qint32)
	@brief	set freeze
	@param	lProductId	:	productID
			command		:	commnad
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotFreezeV2(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetFreezeStatusV2(qint32, qint32)
	@brief	get freeze status
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetFreezeStatusV2(qint32 lProductId, qint32 &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_FREEZE_STATUS_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::Int){
		lParam = qParam.toInt();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotRotationV2(qint32, qint32)
	@brief	set rotation
	@param	lProductId	:	productID
			command		:	commnad
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotRotationV2(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetRotationStatusV2(qint32, qint32)
	@brief	get rotation status
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetRotationStatusV2(qint32 lProductId, qint32 &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_ROTATION_STATUS_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::Int){
		lParam = qParam.toInt();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotNegaPosiV2(qint32, qint32)
	@brief	set nega/posi
	@param	lProductId	:	productID
			command		:	commnad
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotNegaPosiV2(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetNegaPosiStatusV2(qint32, qint32)
	@brief	get nega/posi status
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetNegaPosiStatusV2(qint32 lProductId, qint32 &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_NEGAPOSI_STATUS_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::Int){
		lParam = qParam.toInt();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotColorMonoV2(qint32, qint32)
	@brief	set color/mono
	@param	lProductId	:	productID
			command		:	commnad
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotColorMonoV2(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetColorMonoStatusV2(qint32, qint32)
	@brief	get color/mono status
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetColorMonoStatusV2(qint32 lProductId, qint32 &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_COLORMONO_STATUS_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::Int){
		lParam = qParam.toInt();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotScopeV2(qint32, qint32)
	@brief	set scope
	@param	lProductId	:	productID
			command		:	commnad
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotScopeV2(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetScopeStatusV2(qint32, qint32)
	@brief	get scope status
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetScopeStatusV2(qint32 lProductId, qint32 &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_SCOPE_STATUS_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::Int){
		lParam = qParam.toInt();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotSetSpeakerStatusV2(qint32, qint32)
	@brief	set speaker status
	@param	lProductId	:	productID
			command		:	commnad
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotSetSpeakerStatusV2(qint32 lProductId, qint32 lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam = lParam;
	lRet = pImCtrl->CommandPresenter(SET_SPEAKER_STATUS_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetSpeakerStatusV2(qint32, QStringList)
	@brief	get speaker status
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetSpeakerStatusV2(qint32 lProductId, QStringList &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_SPEAKER_STATUS_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::StringList){
		lParam = qParam.toStringList();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotVideoV2(qint32, qint32)
	@brief	set video
	@param	lProductId	:	productID
			command		:	commnad
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotVideoV2(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetVideoStatusV2(qint32, qint32)
	@brief	get video status
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetVideoStatusV2(qint32 lProductId, qint32 &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_VIDEO_STATUS_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::Int){
		lParam = qParam.toInt();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotLanguageV2(qint32, qint32)
	@brief	set language
	@param	lProductId	:	productID
			command		:	commnad
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotLanguageV2(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetLanguageStatusV2(qint32, qint32)
	@brief	get langage status
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetLanguageStatusV2(qint32 lProductId, qint32 &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_LANG_STATUS_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::Int){
		lParam = qParam.toInt();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGuideV2(qint32, qint32)
	@brief	set guid
	@param	lProductId	:	productID
			command		:	commnad
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGuideV2(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetGuidStatusV2(qint32, qint32)
	@brief	get guid status
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetGuideStatusV2(qint32 lProductId, qint32 &lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_GUIDE_STATUS_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::Int){
		lParam = qParam.toInt();
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotSaveSettingV2(qint32, qint32)
	@brief	save setting
	@param	lProductId	:	productID
			command		:	commnad
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotSaveSettingV2(qint32 lProductId, qint32 lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam = lParam;
	lRet = pImCtrl->CommandPresenter(SAVE_SETTING_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotLoadSettingV2(qint32, qint32)
	@brief	load setting
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotLoadSettingV2(qint32 lProductId, qint32 lParam)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(LOAD_SETTING_V2, qParam);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::Int){
		lParam = qParam.toInt();
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------------------------------- //
//											For UVC・UACカメラ
// --------------------------------------------------------------------------------------------- //


//--------------------------------------------------------------------------------
/*!
	@fn		slotGetRemoconWidget(qint32 lProductId, QWidget** pclDestRemoconWidget)
	@brief	リモコン取得
	@param	lProductId				:	プロダクトID
			pclDestRemoconWidget	:	格納先
 */
//--------------------------------------------------------------------------------
qint32	ClUsbDriver::slotGetRemoconWidget(qint32 lProductId, QWidget** pclDestRemoconWidget)
{

	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false)								// is not initialized
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}

	*pclDestRemoconWidget = pImCtrl->getRemoconWidget();

	return IM_ERROR_SUCCESS;
}

//--------------------------------------------------------------------------------
/*!
	@fn		slotGetSupportedResolutions(QStringList* clDestResolutions)
	@brief	対応解像度取得
	@param	clDestResolutions		:	格納先
 */
//--------------------------------------------------------------------------------
qint32	ClUsbDriver::slotGetSupportedResolutions(qint32 lProductId, QStringList& clDestResolutions)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false)								// is not initialized
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}

	clDestResolutions = pImCtrl->getSupportedResolutions();

	return IM_ERROR_SUCCESS;
}

//--------------------------------------------------------------------------------
/*!
	@fn		slotGetCurrentResolution(qint32 lProductId, QString& clResolution)
	@brief	現在の解像度取得
	@param	lProductId				:	プロダクトID
	@param	clResolution			:	解像度
 */
//--------------------------------------------------------------------------------
qint32	ClUsbDriver::slotGetCurrentResolution(qint32 lProductId, QString& clResolution)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false)								// is not initialized
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}

	clResolution = pImCtrl->getCurrentResolution();

	return IM_ERROR_SUCCESS;
}

//--------------------------------------------------------------------------------
/*!
	@fn		slotSetCurrentResolution(qint32 lProductId, QString	clResolution)
	@brief	現在の解像度設定
	@param	lProductId				:	プロダクトID
	@param	clResolution			:	解像度
 */
//--------------------------------------------------------------------------------
qint32	ClUsbDriver::slotSetCurrentResolution(qint32 lProductId, QString clResolution)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false)								// is not initialized
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}

    clResolution = QString::number(pImCtrl->setCurrentResolution(clResolution));

	return IM_ERROR_SUCCESS;
}

//--------------------------------------------------------------------------------
/*!
	@fn		slotSetCurrentFrameRate(qint32 lProductId, QString	clFrameRate)
	@brief	現在のフレームレート設定
	@param	lProductId				:	プロダクトID
	@param	bIsFrameRateMax			:	フレームレートを最大にするか判定するフラグ
 */
//--------------------------------------------------------------------------------
qint32	ClUsbDriver::slotSetCurrentFrameRate(qint32 lProductId, bool bIsFrameRateMax)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false)								// is not initialized
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}

	pImCtrl->setCurrentFrameRate(bIsFrameRateMax);

	return IM_ERROR_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/* ---------------------------------------------------------------------------- +/
							External Input Commadn
/+ ---------------------------------------------------------------------------- */
//--------------------------------------------------------------------------------
/*!
	@fn		slotExternalInputCmd(qint32, QVariant, QVariant)
	@brief	get current mode
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotExternalInputCmd(qint32 lProductId, QVariant qVariantReq,  QVariant &qVariantRes)
{

	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	lRet = pImCtrl->CommandPresenter(EXTERNAL_INPUT, qVariantReq);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
	qVariantRes = qVariantReq;

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotExternalInputCmdV2(qint32, QVariant, QVariant)
	@brief	get current mode
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotExternalInputCmdV2(qint32 lProductId, QVariant qVariantReq,  QVariant &qVariantRes)
{
	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId,m_mapUvcPathV.value(lProductId,QString())) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	lRet = pImCtrl->CommandPresenter(EXTERNAL_INPUT_V2, qVariantReq);

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
	qVariantRes = qVariantReq;

}
//--------------------------------------------------------------------------------
/*!
	@fn		slotBusReset(qint32, qint32)
	@brief	bus reset
	@param	lProductId			:	Product Id
			command				:	command Id
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotBusReset(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@brief	デバイスパスをチェック
	@param	strDevPath		デバイスパス
	@param	clGuid			機種用ID
	@retval	true			エルモ用製品
	@retval	false			エルモ用製品でない
 */
//--------------------------------------------------------------------------------
bool
ClUsbDriver::checkDevPath( QString  strDevPath, QUuid  clGuid )
{
	bool	bRet = false;
	int		iIndex1 = 0, iIndex2 = 0;
	QString	strTemp;

	//	USBチェック
	if( false == strDevPath.startsWith( "\\\\?\\usb", Qt::CaseInsensitive ) )
	{
		return false;
	}

	//	Vendor IDチェック
	qint32  lVid = 0;

	iIndex1 = strDevPath.indexOf( "vid_", 0, Qt::CaseInsensitive );
	if( -1 == iIndex1 )
	{
		return false;
	}
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
	lVid = strTemp.toLong( &bRet, 16 );
	if( false == bRet )
	{
		return false;
	}
	if( ( VENDOR_ELMO != lVid )  &&
		( VENDOR_SUNPLUS != lVid )
	)
	{
		return false;
	}

	//	Product Idチェック
	qint32	lPid = 0;
	qint32	lProductId = GETPRODUCTID( m_clProductList.key( clGuid ) );

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
	lPid = strTemp.toLong( &bRet, 16 );
	if( false == bRet )
	{
		return false;
	}
	if( lProductId != lPid )
	{
		return false;
	}

	//	MO-1の場合、映像or音声を調べる
	if( GETPRODUCTID( PRODUCT_MOBILE ) == lProductId )
	{
		qint32  lMi = 0;

		iIndex1 = strDevPath.indexOf( "mi_", 0, Qt::CaseInsensitive );
		if( -1 == iIndex1 )
		{
			return false;
		}
		iIndex2 = iIndex1 + 3;
		while( strDevPath.count() >= iIndex2 )
		{
			if( 0 == isxdigit( strDevPath[iIndex2].toLatin1() ) )
			{
				break;
			}
			iIndex2++;
		}
		strTemp = strDevPath.mid( iIndex1 + 3, iIndex2 - iIndex1 - 3 );
		lMi = strTemp.toLong( &bRet, 16 );
		if( false == bRet )
		{
			return false;
		}
		if( m_bIsEnableOtherCamera == false )
		{
			if( 0x02 == lMi )
			{
				m_mapUvcPathA.insert( m_clProductList.key( clGuid ), strDevPath );
			}
			else
			{
				m_mapUvcPathV.insert( m_clProductList.key( clGuid ), strDevPath );
			}
		}
		else
		{
			if( 0x02 == lMi )
			{
				if( m_mapUvcPathA.contains( m_clProductList.key( clGuid ) ) == false )
				{
					m_mapUvcPathA.insert( m_clProductList.key( clGuid ), strDevPath );
				}
				else
				{
					return false;
				}
			}
			else
			{
				if( m_mapUvcPathV.contains( m_clProductList.key( clGuid ) ) == false )
				{
					m_mapUvcPathV.insert( m_clProductList.key( clGuid ), strDevPath );
				}
				else
				{
					return false;
				}
			}
		}
	}
	//	CO-10の場合、映像のみ
	else if( GETPRODUCTID( PRODUCT_CO10 ) == lProductId )
	{
		if( m_bIsEnableOtherCamera == false )
		{
			m_mapUvcPathV.insert( m_clProductList.key( clGuid ), strDevPath );
		}
		else
		{
			if( m_mapUvcPathV.contains( m_clProductList.key( clGuid ) ) == false )
			{
				m_mapUvcPathV.insert( m_clProductList.key( clGuid ), strDevPath );
			}
			else
			{
				return false;														//既に接続済みなので無視する
			}
		}
	}
	else
	{
		if( m_bIsEnableOtherCamera == true )
		{
			if( m_mapConnectProduct.contains( m_clProductList.key( clGuid ) ) == true )
			{
				return false;														//既に接続済みなので無視する
			}
		}
	}

	return bRet;
}
//--------------------------------------------------------------------------------
/*!
	@brief	他社製品デバイスパスをチェック
	@param	strDevPath		デバイスパス
	@param	clGuid			機種用ID
	@retval	true			エルモ用製品でない
	@retval	false			エルモ用製品
 */
//--------------------------------------------------------------------------------
bool
ClUsbDriver::checkDevPathOther( QString  strDevPath, QUuid  clGuid )
{
	bool	bRet = false;
	int		iIndex1 = 0, iIndex2 = 0;
	QString	strTemp;

	//他社カメラ対応の有効/無効フラグ判定
	if(m_bIsEnableOtherCamera == false)
	{
		return false;
	}

	//	USBチェック
	if( false == strDevPath.startsWith( "\\\\?\\usb", Qt::CaseInsensitive ) )
	{
		return false;
	}

	//	Vendor IDチェック
	qint32  lVid = 0;

	iIndex1 = strDevPath.indexOf( "vid_", 0, Qt::CaseInsensitive );
	if( -1 == iIndex1 )
	{
		return false;
	}
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
	lVid = strTemp.toLong( &bRet, 16 );
	if( false == bRet )
	{
		return false;
	}
	if( ( VENDOR_ELMO == lVid )  ||												//ELMO社のIDの場合は抜ける
		( VENDOR_SUNPLUS == lVid )
	)
	{
		return false;
	}

	//	Product Idチェック
	qint32	lPid = 0;
	qint32	lProductId = GETPRODUCTID( m_clProductList.key( clGuid ) );

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
	lPid = strTemp.toLong( &bRet, 16 );
	if( false == bRet )
	{
		return false;
	}
	if( lProductId == lPid )													//ELMO社製品の場合は抜ける
	{
		return false;
	}

	//	固有ID生成
	qint32 lId = (lVid << 16) + lProductId;										//VID(会社固有ID)とPID(製品ID)を合わせたID生成
	m_lProductIdOther = lId;

	//	MO-1の場合、映像or音声を調べる
	if( GETPRODUCTID( PRODUCT_MOBILE ) == lProductId )
	{
		qint32  lMi = 0;

		iIndex1 = strDevPath.indexOf( "mi_", 0, Qt::CaseInsensitive );
		if( -1 == iIndex1 )
		{
			return false;
		}
		iIndex2 = iIndex1 + 3;
		while( strDevPath.count() >= iIndex2 )
		{
			if( 0 == isxdigit( strDevPath[iIndex2].toLatin1() ) )
			{
				break;
			}
			iIndex2++;
		}
		strTemp = strDevPath.mid( iIndex1 + 3, iIndex2 - iIndex1 - 3 );
		lMi = strTemp.toLong( &bRet, 16 );
		if( false == bRet )
		{
			return false;
		}
		if( 0x02 == lMi)
		{
			if(m_mapUvcPathA.contains( lId ) == false)
			{
				m_mapUvcPathA.insert( lId, strDevPath );
			}
			else
			{
				return false;
			}
		}
		else
		{
			if(m_mapUvcPathV.contains( lId ) == false)
			{
				m_mapUvcPathV.insert( lId, strDevPath );
			}
			else
			{
				return false;
			}
		}
	}
	//	CO-10の場合、映像のみ
	else if( GETPRODUCTID( PRODUCT_CO10 ) == lProductId )
	{
		if(m_mapUvcPathV.contains( lId ) == false)
		{
			m_mapUvcPathV.insert( lId, strDevPath );
		}
		else
		{
			return false;														//既に接続済みなので無視する
		}
	}
	else
	{
		if(m_mapConnectProduct.contains( lId ) == true)
		{
			return false;														//既に接続済みなので無視する
		}
	}

	return bRet;
}
//--------------------------------------------------------------------------------
/*!
	@brief	接続スレッド生成
 */
//--------------------------------------------------------------------------------
void
ClUsbDriver::cretaeThreadDeviceOpen()
{
	//他社カメラ対応の有効/無効フラグ判定
	if(m_bIsEnableOtherCamera == false)
	{
		return;
	}

	//スレッド終了
	if(m_pclThreadDeviceOpen != NULL)
	{
		if(m_pclThreadDeviceOpen->isRunning() == true)
		{
			if(m_pclThreadDeviceOpen->isSend() == true)
			{
				return;
			}
			else
			{
				m_pclThreadDeviceOpen->stop();
				m_pclThreadDeviceOpen->wait();
			}
		}
		delete m_pclThreadDeviceOpen;
		m_pclThreadDeviceOpen = NULL;
	}

	//スレッド開始
	if(m_mapOpenProduct.isEmpty() == true && m_mapConnectProduct.isEmpty() == false)	//検出されたが未接続のデバイスがある場合は接続スレッド開始
	{
		m_pclThreadDeviceOpen = new ClThreadDeviceOpen(this, m_mapConnectProduct);
		connect( m_pclThreadDeviceOpen, SIGNAL(sigOpenDevice(qint32)),
				this, SLOT(slotOpenDevice(qint32)) );
		connect( this, SIGNAL(sigOpenDevice(qint32)),
				m_pclThreadDeviceOpen, SLOT(slotReceive(qint32)) );
		connect( this, SIGNAL(sigErrorNormal(qint32)),
				m_pclThreadDeviceOpen, SLOT(slotReceive(qint32)) );
		m_pclThreadDeviceOpen->start();
	}
}
//--------------------------------------------------------------------------------


