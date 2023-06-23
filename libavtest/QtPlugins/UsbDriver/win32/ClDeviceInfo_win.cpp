﻿#include "ClUsbDriver/ClUsbDriver_global.h"
#include "Common/CmnDefine.h"
#include "ClUsbDriver/ClUsbDriver_win.h"
#include "ClUsbDriver/Command_Presenter.h"
#include "win32/ClDeviceInfo_win.h"
#include "vndrlist.h"
#include "qtdxuttimer.h"
#include "ElmoStruct.h"
#include "ElmoStructV2.h"
#include "ClUsbCmd.h"
#include "ClUsbCmdV2.h"

//--------------------------------------------------------------------------------
/*!
	@fn			ClDeviceInfo
	@brief
 */
//--------------------------------------------------------------------------------
ClDeviceInfo::ClDeviceInfo(qint32 a_ProductId, QObject *object) : QIODevice(object)
{
	m_lVendorId = GETVENDORID(a_ProductId);						//set default VENDOR_ID
	m_lProductId = GETPRODUCTID(a_ProductId);
    m_iTimeout = 5000;											//time to time-out

	m_pInCmdEndp = NULL;										//set NULL to inEndp
	m_pOutCmdEndp = NULL;										//set NULL to outEndp
	m_pInDataEndp = NULL;										//set NULL to inEndp
	m_pOutDataEndp = NULL;										//set NULL to outEndp

	m_pUsbDevice = NULL;

	m_iPictureSize = 0;
	m_iPictureRect = RECTID_PICTURE_DEFAULT;
	m_usHeight = 0;
	m_usWidth = 0;

	m_pclTrans = NULL;
	setTranslator();
}
//--------------------------------------------------------------------------------
/*!
	@fn			~ClDeviceInfo
	@brief
 */
//--------------------------------------------------------------------------------
ClDeviceInfo::~ClDeviceInfo()
{
	close();
	m_pInCmdEndp = NULL;										//set NULL to inEndp
	m_pOutCmdEndp = NULL;										//set NULL to outEndp
	m_pInDataEndp = NULL;										//set NULL to inEndp
	m_pOutDataEndp = NULL;										//set NULL to outEndp

}
//--------------------------------------------------------------------------------
/*!
	@fn			open
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::open()
{
	if(isOpen() == true){
		return IM_ERROR_SUCCESS;
	}

	return IM_ERROR_NOT_OPEN;
}
//--------------------------------------------------------------------------------
/*!
	@fn			close
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClDeviceInfo::close()
{
	if(isOpen() == true){								//if available device
		if(m_pUsbDevice != NULL){
			m_pUsbDevice->CloseReadEP();
			m_pUsbDevice->CloseReadEP2();
			m_pUsbDevice->CloseWriteEP();
			m_pUsbDevice->CloseWriteEP2();
			delete m_pUsbDevice;
			m_pUsbDevice = NULL;
		}
		QIODevice::close();								//call for rarent close function
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn			readData
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint64	ClDeviceInfo::readData ( char * data, qint64 maxSize )
{
	qint32 lReqLength = 0;
	qint32 lRet = 0;
	if(isOpen() == false){
		return -1;
	}
	if(maxSize > INT_MAX){
		DBG("got error from writeData");
		return -1;
	}
	lReqLength = static_cast<qint32>(maxSize);

	ClUsbCmd clUsb(data,lReqLength);							//

	if(clUsb.getCode() == EL_GET_JPEG_DATA){					//if code is Jpeg data

		QVariant qParam(lReqLength);
		lRet = resPicture(m_pInDataEndp, qParam);
		if(lRet > 0 && qParam.type() == QVariant::List){
			QVariantList qVarList = qParam.toList();
			if(qVarList.at(0).type() == QVariant::Size){
				QSize qSize = qVarList.at(0).toSize();
				clUsb.setParam2(qSize.width());
				clUsb.setParam3(qSize.height());
			}
			if(qVarList.at(1).type() == QVariant::ByteArray){
				QByteArray *pqArray = reinterpret_cast<QByteArray*>(clUsb.getParam1());
				*pqArray = qVarList.at(1).toByteArray();
			}
		}
		if(lRet >= 0){										//
			QByteArray qCmdArray;
			QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
			qWork.setByteOrder(QDataStream::LittleEndian);	//
			qWork << clUsb.getPagetType()						//set PacketType
				<< clUsb.getTransId()							//set Trans ID
				<< clUsb.getDataSize()							//set DataSize
				<< clUsb.getCode()								//set code
				<< qint16(0x00)									//set dummy (Reserved)
				<< clUsb.getParam1()							//set Param1
				<< clUsb.getParam2()							//set Param2
				<< clUsb.getParam3()							//set Param3
				<< clUsb.getParam4()							//set Param4
				<< clUsb.getParam5();							//set Param5
			memcpy(data, qCmdArray.data(), qCmdArray.length());
		}
	}else{
		QVariant qParam(lReqLength);
		lRet = resCommand(m_pInCmdEndp, qParam);
		if(lRet > 0){
			qint64 llWork = lRet;
			if(maxSize < lRet){
				llWork = maxSize;
			}
			if(qParam.type() == QVariant::ByteArray){
				QByteArray qCmdArray = qParam.toByteArray();
				memcpy(data, qCmdArray.data(), llWork);			//copy
#if 0  // for USB packet moniter in qDebug
				QByteArray qDbgArray(data,llWork);
				for(int i = 0; i < qDbgArray.length(); i=i+2){
					DBGFL("qDbgArray : %02x %02x",QChar(qDbgArray.at(i)).unicode(),QChar(qDbgArray.at(i+1)).unicode());
				}
#endif
			}
		}
	}
	if(lRet < 0){											//guess, Disconnected USB?
		BOOL a_bRet = 0;
		a_bRet = m_pUsbDevice->ResetPipe();									//バスをリセット
		if(a_bRet != TRUE){
			DBGFL( "got error from ResetPipe : %s",a_bRet == TRUE ? "TRUE" : "FALSE");
		}
		lRet = -1;
	}
	return lRet;
}
//--------------------------------------------------------------------------------
/*!
	@fn			writeData
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint64	ClDeviceInfo::writeData ( const char * data, qint64 maxSize )
{
	qint32 lReqLength = 0;
	qint32 lRet = 0;

	if(isOpen() == false){
		return -1;
	}
	if(maxSize > INT_MAX){
		DBGFL("got error from writeData");
		return -1;
	}
	lReqLength = static_cast<qint32>(maxSize);
	QByteArray qCmdArray(data,lReqLength);
	QVariant qParam(qCmdArray);

	ClUsbCmd clUsb(data,lReqLength);							//
	if(clUsb.getCode() == EL_GET_JPEG_DATA){					//if code is Jpeg data
		lRet = reqPicture(m_pOutDataEndp, qParam);
	}else{
		lRet = reqCommand(m_pOutCmdEndp, qParam);
	}
	if(lRet < 0){											//guess, Disconnected USB?
		BOOL a_bRet = 0;
		a_bRet = m_pUsbDevice->ResetPipe();									//バスをリセット
		if(a_bRet != TRUE){
			DBGFL( "got error from ResetPipe : %s",a_bRet == TRUE ? "TRUE" : "FALSE");
		}
		lRet = -1;
	}
	return lRet;
}
//--------------------------------------------------------------------------------
/*!
	@fn			setupEndpoint
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::setupEndpoint()
{
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			reqPicture
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::reqPicture(eENDPOINT	*pOutEndp, QVariant &param)
{
	qint32 lRet = 0;

	Q_ASSERT(pOutEndp);
	lRet = reqCommand(pOutEndp, param);
	return lRet;
}
//--------------------------------------------------------------------------------
/*!
	@fn			resPicture
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::resPicture(eENDPOINT	*pInEndp, QVariant &qParam)
{
	qint32 lActual_length = 0;
	qint32 lRet = 0;
	qint32 lJpegLength = 0;
	char pcBuffer[JPEG_PICTURESIZE];									//Jpeg temporary buffer
	QByteArray qJpegData;
	QSize qSize;
	Q_ASSERT(pInEndp);

	lRet = resCommand(pInEndp, qParam);									//画像情報を受信
	if(lRet < 0){
		return -1;
	}
	if(qParam.type() == QVariant::ByteArray){
		QByteArray qCmdArray = qParam.toByteArray();
		ClUsbCmd clUsb(qCmdArray.data(),qCmdArray.length());			//
		if(clUsb.getParam1() == 0){										//if the Param1 is zero, it may be an SD mode
			return 0;													//return for zero
		}
		qSize.setWidth(clUsb.getParam2());
		qSize.setHeight(clUsb.getParam3());

		qint32 siHeader = sizeof(ELMO_PACKET_HEADER);
		lJpegLength = clUsb.getParam1();
		qint32 lRestLength = lJpegLength;
		PELMO_PACKET_HEADER pstHeader;
		while(0 < lRestLength){
			DBGFL("RestLength : %d",lRestLength);
			qint32 lreqLength = JPEG_PICTURESIZE;
			qint32 lBlockSize = 0;
			qint32 lActual_count = 0;
			do{
				IoDevice::IO_RESULT ioRes = IoDevice::IO_SUCCESS;
				if((*pInEndp) == eEP2){
					ioRes = m_pUsbDevice->ReadEP((LPVOID)pcBuffer,
												lreqLength,
												(LPDWORD)&lActual_length,
												m_iTimeout);
				}else if((*pInEndp) == eEP4){
					ioRes = m_pUsbDevice->ReadEP2((LPVOID)pcBuffer,
												lreqLength,
												(LPDWORD)&lActual_length,
												m_iTimeout);
				}
				if(ioRes != IoDevice::IO_SUCCESS){
					DBGFL("got error from libusb_bulk_transfer : %d",ioRes);
					return IM_ERROR_READ_FAILURE;
				}
				if(lActual_count++ > 5){
					DBGFL("lActual_length size is zero : %d",lActual_length);
					return IM_ERROR_DEVICE_BUSY;
				}
			}while(lActual_length == 0);
			pstHeader = reinterpret_cast<PELMO_PACKET_HEADER>(pcBuffer);
			lBlockSize = pstHeader->DataSize;
			if(pstHeader->PacketType != ELMO_PKT_DAT){
				DBGFL("got unjust PacketType : %d",pstHeader->PacketType);
				return IM_ERROR_OTHER;
			}
			if(lBlockSize <= 0){
				DBGFL("got unjust block size : %d",lBlockSize);
				return IM_ERROR_OTHER;
			}
			DBGFL("lBlockSize : %d",lBlockSize);

			lRestLength -= lBlockSize;													//この2つ上下逆の気が...
			lBlockSize -= (lActual_length - siHeader);									//
			qJpegData.append(pcBuffer + siHeader, lActual_length - siHeader);
			while(0 < lBlockSize){
				if(lBlockSize < JPEG_PICTURESIZE){
					lreqLength = lBlockSize;
				}
				qint32 lActual_count = 0;
				do{
					IoDevice::IO_RESULT ioRes = IoDevice::IO_SUCCESS;
					if((*pInEndp) == eEP2){
						ioRes = m_pUsbDevice->ReadEP((LPVOID)pcBuffer,
													lreqLength,
													(LPDWORD)&lActual_length,
													m_iTimeout);
					}else if((*pInEndp) == eEP4){
						ioRes = m_pUsbDevice->ReadEP2((LPVOID)pcBuffer,
													lreqLength,
													(LPDWORD)&lActual_length,
													m_iTimeout);
					}
					if(ioRes != IoDevice::IO_SUCCESS){
						DBGFL("got error from libusb_bulk_transfer : %d",ioRes);
						return IM_ERROR_READ_FAILURE;
					}
					if(lActual_count++ > 5){
						DBGFL("lActual_length size is zero : %d",lActual_length);
						return IM_ERROR_DEVICE_BUSY;
					}
				}while(lActual_length == 0);
				qJpegData.append(pcBuffer, lActual_length);
				lBlockSize -= lActual_length;
			}
		}
	}
	QVariantList qVarList;
	qVarList << qSize;
	qVarList << qJpegData;
	qParam.clear();
	qParam = qVarList;

	return lJpegLength;
}
//--------------------------------------------------------------------------------
/*!
	@fn			reqCommand
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::reqCommand(eENDPOINT	*pOutEndp, QVariant &param)
{
	qint32 lActual_length = 0;								//

	Q_ASSERT(pOutEndp);
	if(param.type() != QVariant::ByteArray){				//
		return -1;
	}

	QByteArray qCmdArray = param.toByteArray();				//
#if 0  // for USB packet moniter in qDebug
	QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
	for(int i = 0; i < qCmdArray.length(); i=i+2){
		DBGFL("data2 : %02x %02x",QChar(qCmdArray.at(i)).unicode(),QChar(qCmdArray.at(i+1)).unicode());
	}
//	DBGFL(QChar(pOutEndp->bEndpointAddress).unicode());
#endif
	qint32 lActual_count = 0;
	do{
		IoDevice::IO_RESULT ioRes = IoDevice::IO_SUCCESS;
		if((*pOutEndp) == eEP2){
			ioRes = m_pUsbDevice->WriteEP((LPVOID)qCmdArray.data(),
										qCmdArray.length(),
										(LPDWORD)&lActual_length,
										m_iTimeout);
		}else if((*pOutEndp) == eEP4){
			ioRes = m_pUsbDevice->WriteEP2((LPVOID)qCmdArray.data(),
										qCmdArray.length(),
										(LPDWORD)&lActual_length,
										m_iTimeout);
		}
		if(ioRes != IoDevice::IO_SUCCESS){
			DBGFL("got error from libusb_bulk_transfer : %d",ioRes);
			return IM_ERROR_READ_FAILURE;
		}
		if(lActual_count++ > 5){
			DBGFL("lActual_length size is zero : %d",lActual_length);
			return IM_ERROR_DEVICE_BUSY;
		}

	}while(lActual_length == 0);

	return lActual_length;
}

//--------------------------------------------------------------------------------
/*!
	@fn			resCommand
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::resCommand(eENDPOINT	*pInEndp, QVariant &param)
{
	qint32 lActual_length = 0;
	char pucRecvData[128];

	Q_ASSERT(pInEndp);
	if(param.type() != QVariant::Int){				//
		return -1;
	}
	qint32 lLength = param.toInt();					//
	qint32 lActual_count = 0;
	do{
		IoDevice::IO_RESULT ioRes = IoDevice::IO_SUCCESS;
		if((*pInEndp) == eEP2){
			ioRes = m_pUsbDevice->ReadEP((LPVOID)pucRecvData,
										lLength,
										(LPDWORD)&lActual_length,
										m_iTimeout);
		}else if((*pInEndp) == eEP4){
			ioRes = m_pUsbDevice->ReadEP2((LPVOID)pucRecvData,
										lLength,
										(LPDWORD)&lActual_length,
										m_iTimeout);
		}
		if(ioRes != IoDevice::IO_SUCCESS){
			DBGFL("got error from libusb_bulk_transfer : %d",ioRes);
			return IM_ERROR_READ_FAILURE;
		}
		if(lActual_count++ > 5){
			DBGFL("lActual_length size is zero : %d",lActual_length);
			return IM_ERROR_DEVICE_BUSY;
		}
	}while(lActual_length == 0);
	QByteArray qResArray(pucRecvData, lActual_length);
	param.clear();
//	param.setValue<QByteArray>(qResArray);  //qt6
    param = qResArray;
    return lActual_length;
}
//--------------------------------------------------------------------------------
/*!
	@fn			analyJpegHeader
	@brief
	@return		bool
 */
//--------------------------------------------------------------------------------
bool ClDeviceInfo::analyJpegHeader(const QByteArray qJpgData, QSize &qSize)
{
	typedef union{
		struct{
			quint8	ucH;
			quint8	ucL;
		};
		quint16	usPoint;
	}uniPOINT;

	if(qJpgData.at(0) != CHECK_CHAR_FF || qJpgData.at(1) != CHECK_CHAR_D8){
		return false;
	}

	for(qint32 i = 0; i < qJpgData.length(); i++){
		if(qJpgData.at(i) == CHECK_CHAR_FF && qJpgData.at(i + 1) == CHECK_CHAR_C0){
			uniPOINT uniHeight;
			uniPOINT uniWidth;
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
			uniHeight.ucH = qJpgData.at(i + JPEG_OFFSET_Y + 1);
			uniHeight.ucL = qJpgData.at(i + JPEG_OFFSET_Y);
			uniWidth.ucH = qJpgData.at(i + JPEG_OFFSET_X + 1);
			uniWidth.ucL = qJpgData.at(i + JPEG_OFFSET_X);
#else
			uniHeight.ucH = qJpgData.at(i + JPEG_OFFSET_Y);
			uniHeight.ucL = qJpgData.at(i + JPEG_OFFSET_Y + 1);
			uniWidth.ucH = qJpgData.at(i + JPEG_OFFSET_X);
			uniWidth.ucL = qJpgData.at(i + JPEG_OFFSET_X + 1);
#endif
			qSize.setHeight(uniHeight.usPoint);
			qSize.setWidth(uniWidth.usPoint);
			return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------------------
/*!
	@fn			changeJpegHeader
	@brief
	@return		bool
 */
//--------------------------------------------------------------------------------
bool ClDeviceInfo::changeJpegHeader(QByteArray &qJpgData, QSize qSize)
{
	typedef union{
		struct{
			quint8	ucH;
			quint8	ucL;
		};
		quint16	usPoint;
	}uniPOINT;

	if(qJpgData.at(0) != CHECK_CHAR_FF || qJpgData.at(1) != CHECK_CHAR_D8){
		return false;
	}

	for(qint32 i = 0; i < qJpgData.length(); i++){
		if(qJpgData.at(i) == CHECK_CHAR_FF && qJpgData.at(i + 1) == CHECK_CHAR_C0){
			uniPOINT uniHeight;
			uniHeight.usPoint = quint16(qSize.height());
			uniPOINT uniWidth;
			uniWidth.usPoint = quint16(qSize.width());
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
			*(qJpgData.data() + i + JPEG_OFFSET_Y + 1) = uniHeight.ucH;
			*(qJpgData.data() + i + JPEG_OFFSET_Y) = uniHeight.ucL;
			*(qJpgData.data() + i + JPEG_OFFSET_X + 1) = uniWidth.ucH;
			*(qJpgData.data() + i + JPEG_OFFSET_X) = uniWidth.ucL;

#else
			qJpgData.at(i + JPEG_OFFSET_Y) = uniHeight.ucH;
			qJpgData.at(i + JPEG_OFFSET_Y + 1) = uniHeight.ucL;
			qJpgData.at(i + JPEG_OFFSET_X) = uniWidth.ucH;
			qJpgData.at(i + JPEG_OFFSET_X + 1) = uniWidth.ucL;
#endif
			return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------------------
/*!
	@fn			CommandPresenter
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::CommandPresenter(qint32 /*clCmd*/, QVariant& /*qParameter*/)
{
	return IM_ERROR_SUCCESS;
}

/*! ------------------------------------------------------------------------------

	@fn			getFuncWidgets
	@brief		操作ウィジェット生成

 --------------------------------------------------------------------------------*/
QWidget* ClDeviceInfo::getRemoconWidget()
{
	return NULL;
}

/*! ------------------------------------------------------------------------------

	@fn			setTranslator
	@brief		言語変換

 --------------------------------------------------------------------------------*/
void	ClDeviceInfo::setTranslator()
{
	if(m_pclTrans != NULL)								//既に言語設定クラスが生成済みな場合
	{
		qApp->removeTranslator(m_pclTrans);				//ロードされていたであろう前の言語ファイルをアンロードする
	}
	else												//言語設定クラスをまだ生成してなかった場合
	{
		m_pclTrans = new QTranslator(this);				//新たに言語設定クラスを生成
	}

	if(m_pclTrans != NULL)								//既に言語設定クラスが生成済みな場合
	{
		qApp->removeTranslator(m_pclTrans);				//ロードされていたであろう前の言語ファイルをアンロードする
	}
	else												//言語設定クラスをまだ生成してなかった場合
	{
		m_pclTrans = new QTranslator(this);				//新たに言語設定クラスを生成
	}
	//	新しいファイルをロード
	if( NULL != m_pclTrans )
	{
		if( m_pclTrans->load( ":/UsbDriver.qm" ) != false )
		{
			qApp->installTranslator( m_pclTrans );
		}
	}

}

/*! ------------------------------------------------------------------------------

	@fn			getSupportedResolutions
	@brief		対応解像度取得

 --------------------------------------------------------------------------------*/
QStringList	ClDeviceInfo::getSupportedResolutions()
{
	return QStringList();
}
//--------------------------------------------------------------------------------
/*!
		@brief	現在の解像度を取得
 */
//--------------------------------------------------------------------------------
QString ClDeviceInfo::getCurrentResolution()
{
	return QString();
}

//--------------------------------------------------------------------------------
/*!
		@brief	現在の解像度を設定
 */
//--------------------------------------------------------------------------------
qint32	ClDeviceInfo::setCurrentResolution(QString strResolution)
{
	return E_FAIL;
}

//--------------------------------------------------------------------------------
/*!
		@brief	現在のフレームレートを設定
 */
//--------------------------------------------------------------------------------
qint32	ClDeviceInfo::setCurrentFrameRate(bool bIsFrameRateMax)
{
	Q_UNUSED(bIsFrameRateMax)
	return E_FAIL;
}

//--------------------------------------------------------------------------------
/*!
	@fn			getPicture
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::getPicture(QVariant &/*qParam*/)		//画像取得関数
{
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			execCommand
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::execCommand(ClUsbCmd &clCmd)
{
	QDataStream qCmdStream(this);						//create data stream for getPicture

	ClUsbCmd clRecive;									//this variable is for reception

	qCmdStream << clCmd;								//send command to device

	qCmdStream >> clRecive;								//receeive command from device

	if(qCmdStream.status() != QDataStream::Ok){			//if confirm whether QDataStream status is OK
		return IM_ERROR_COMMAND_FAILURE;
	}

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			getModelName
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::getModelName(QVariant &/*qParam*/)
{
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			getVersion
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::getVersion(QVariant &/*qParam*/)
{
	return IM_ERROR_SUCCESS;								//output version to QVariant
}
//--------------------------------------------------------------------------------
/*!
	@fn			execCaptureInit
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::execCaptureInit()						//I clear the capture flag
{
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			getCaptureStatus
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::getCaptureStatus(QVariant &/*qParam*/)		//acquire the capture status
{
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------

