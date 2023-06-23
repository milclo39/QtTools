#include "mac/ClPresenterL1ex.h"
#include "ClUsbDriver/ClUsbDriver_mac.h"
#include "ClUsbDriver/ClUsbDriver_global.h"
#include "Common/CmnDefine.h"
#include "ClUsbDriver/Command_Presenter.h"
#include "ClUsbCmd.h"
#include "ElmoUsbCmd.h"
#include "UsbDevice.h"
#include <QDateTime>
#include "time.h"

//--------------------------------------------------------------------------------
/*!
	@class		ClPresenterL1ex
	@brief
 */
//--------------------------------------------------------------------------------
/*!
	@brief		コンストラクタ
 */
//--------------------------------------------------------------------------------
ClPresenterL1ex::ClPresenterL1ex(qint32 lProductId, QObject *parent) : ClDeviceInfo(lProductId, parent)
{
	m_lCmdID		= 0;										//Command ID
}
//--------------------------------------------------------------------------------
/*!
	@brief		デストラクタ
 */
//--------------------------------------------------------------------------------
ClPresenterL1ex::~ClPresenterL1ex()
{
	if(m_pUsbDevice != NULL){
		m_pUsbDevice->closeDevice();
		delete m_pUsbDevice;
		m_pUsbDevice = NULL;
	}

	m_pOutCmdEndp	= NULL;
	m_pInCmdEndp	= NULL;
	m_pOutDataEndp	= NULL;
	m_pInDataEndp	= NULL;
}
//--------------------------------------------------------------------------------
/*!
	@brief		L-1ex系の書画カメラをOpenする
	@return		IM_ERROR_SUCCESS	成功
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL1ex::open()
{

	if(isOpen() == true){										//開かれているなら
		return IM_ERROR_SUCCESS;
	}
	qint32 lEpRes = setupEndpoint();							//is overrided setupEndpoint of call
	if(lEpRes != IM_ERROR_SUCCESS){								//
		return lEpRes;											//
	}

	if(m_lProductId == PRODUCT_L1EX)
	{				//L-1EXの場合
		m_pUsbDevice = new ClIoDevice(m_lProductId);				//
	}
	else if(m_lProductId == PRODUCT_P10)
	{
		m_pUsbDevice = new ClIoDevice(m_lProductId);				//P10の場合
	}
	else
	{
		return IM_ERROR_NO_DEVICE;								//失敗
	}
	if(m_pUsbDevice == NULL)
	{
		return IM_ERROR_FATAL;
	}
	//デバイスクラスにクラス設定
	m_pUsbDevice->setDriver(dynamic_cast<ClUsbDriver*>(parent()));
	m_pUsbDevice->setDevinfo(this);

	if(m_pUsbDevice->openDevice() != TRUE){						//デバイスファイルオープン
		return IM_ERROR_NO_DEVICE;
	}
	m_pUsbDevice->resetPipe();									//バスをリセット

	if(QIODevice::open(ReadWrite | Unbuffered) == false)
	{		//QIODeviceのOpen
		return IM_ERROR_NO_DEVICE;
	}

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL1ex::setupEndpoint()
{
	m_pOutCmdEndp	= new eENDPOINT;
	m_pInCmdEndp	= new eENDPOINT;
	m_pOutDataEndp	= new eENDPOINT;
	m_pInDataEndp	= new eENDPOINT;

	*m_pOutCmdEndp	= eEP1;										//コマンド用送信EP
	*m_pInCmdEndp	= eEP2;										//コマンド用受信EP
	*m_pOutDataEndp	= eEP3;										//データ用送信EP
	*m_pInDataEndp	= eEP4;										//データ用受信EP

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL1ex::CommandPresenter(qint32 CmdID, QVariant& qParameter)
{
	qint32 lRes = IM_ERROR_NOT_OPEN;							//未接続
	m_lCmdID = CmdID;											//コマンドIDを格納

	DBGFL("CommandPresenter windows");

	switch(CmdID){
	/* --------------------------------------------------------- +/
							V1 Command
	/+ --------------------------------------------------------- */
	case GET_PICTURE:											//画像取得
		{
			lRes = getPicture(qParameter);
			break;
		}
	case RELEASE_PICTURE:										//取得した画像領域の解放
		{
			lRes = IM_ERROR_SUCCESS;
			break;
		}
	case GET_SIZE_PICTURE:										//直前に取得したサイズの返却
		{
			lRes = IM_ERROR_SUCCESS;							//取得成功
			qParameter = m_iPictureSize;						//
			break;
		}
	case GET_RECT_PICTURE:										//画像の大きさ取得
		{
			lRes = IM_ERROR_SUCCESS;							//取得成功
			qParameter.setValue<eKINDRECTID>(m_iPictureRect);	//
			break;
		}
	case SET_RECT_PICTURE:										//画像の大きさ設定
		{
			lRes = IM_ERROR_INVALID_PARAMETER;					//初期化
			if(qParameter.type() == QVariant::UserType){
				if(chINRANGE(SIZE_PICTURE_HIGH,qParameter.value<eKINDRECTID>(),SIZE_PICTURE_LOW)){
					lRes = IM_ERROR_SUCCESS;					//取得成功
					m_iPictureRect = qParameter.value<eKINDRECTID>();
				}
			}
			break;
		}
	case GET_MODEL_NAME:										//機種名取得
		{
			lRes = getModelName(qParameter);
			break;
		}
	case GET_MODEL_VERSION:										//バージョン取得
		{
			lRes = getVersion(qParameter);
			break;
		}
	case GET_HEIGHT_PICTURE:									//現在の画像の縦幅
		{
			lRes = IM_ERROR_SUCCESS;							//取得成功
			qParameter = m_usHeight;							//
			break;
		}
	case GET_WIDTH_PICTURE:										//現在の画像の横幅
		{
			lRes = IM_ERROR_SUCCESS;							//取得成功
			qParameter = m_usWidth;								//
			break;
		}
	case ZOOM_WIDE:												//拡大
		{
			ClZoomCmd clCmd(EL_WIDE);
			lRes = execCommand(clCmd);
			break;
		}
	case ZOOM_TELE:												//縮小
		{
			ClZoomCmd clCmd(EL_TELE);
			lRes = execCommand(clCmd);
			break;
		}
	case ZOOM_STOP:												//ストップ
		{
			ClZoomCmd clCmd(EL_STOP);
			lRes = execCommand(clCmd);
			break;
		}
	case IRIS_OPEN:												//開く
		{
			ClIrisCmd clCmd(EL_OPEN);
			lRes = execCommand(clCmd);
			break;
		}
	case IRIS_CLOSE:											//閉じる
		{
			ClIrisCmd clCmd(EL_CLOSE);
			lRes = execCommand(clCmd);
			break;
		}
	case IRIS_STOP:												//ストップ
		{
			ClIrisCmd clCmd(EL_ISTOP);
			lRes = execCommand(clCmd);
			break;
		}
	case IRIS_INITIALIZE:										//初期位置
		{
			ClIrisCmd clCmd(EL_NORMAL);
			lRes = execCommand(clCmd);
			break;
		}
	case FOCUS_AUTO:											//自動
		{
			ClAutoFocusCmd clCmd;
			lRes = execCommand(clCmd);
			break;
		}
	case FOCUS_NEAR:											//近く
		{
			ClFocusCmd clCmd(EL_NEAR);
			lRes = execCommand(clCmd);
			break;
		}
	case FOCUS_FAR:												//遠く
		{
			ClFocusCmd clCmd(EL_FAR);
			lRes = execCommand(clCmd);
			break;
		}
	case FOCUS_STOP:											//ストップ
		{
			ClFocusCmd clCmd(EL_STOP);
			lRes = execCommand(clCmd);
			break;
		}
	case ROTATION:												//回転要求
		{
			if(qParameter.isNull() == true){					//NULL判定
				lRes = IM_ERROR_INVALID_PARAMETER;				//ParameterがNULLなら失敗
				break;
			}
			ClRotationCmd clCmd(qParameter.toInt());
			lRes = execCommand(clCmd);
			break;
		}
	case POSI_NEGA:												//ネガ or ポジ
		{
			if(qParameter.isNull() == true){					//NULL判定
				lRes = IM_ERROR_INVALID_PARAMETER;				//ParameterがNULLなら失敗
				break;
			}
			ClPosiNegaCmd clCmd(qParameter.toInt());
			lRes = execCommand(clCmd);
			break;
		}
	case COLOR_BW:												//Color or BW
		{
			if(qParameter.isNull() == true){					//NULL判定
				lRes = IM_ERROR_INVALID_PARAMETER;				//ParameterがNULLなら失敗
				break;
			}
			ClColorBWCmd clCmd(qParameter.toInt());
			lRes = execCommand(clCmd);
			break;
		}
	case SLOW_SHUTTER:											//低速シャッター
		{
			if(qParameter.isNull() == true){					//NULL判定
				lRes = IM_ERROR_INVALID_PARAMETER;				//ParameterがNULLなら失敗
				break;
			}
			ClSlowShutterCmd clCmd(qParameter.toInt());
			lRes = execCommand(clCmd);
			break;
		}
	case IMAGE_MODE:											//イメージモード
		{
			if(qParameter.isNull() == true){					//NULL判定
				lRes = IM_ERROR_INVALID_PARAMETER;				//ParameterがNULLなら失敗
				break;
			}
			ClImageModeCmd clCmd(qParameter.toInt());
			lRes = execCommand(clCmd);
			break;
		}
	case MICROSCOPE_MODE:										//顕微鏡モード
		{
			if(qParameter.isNull() == true){					//NULL判定
				lRes = IM_ERROR_INVALID_PARAMETER;				//ParameterがNULLなら失敗
				break;
			}
			ClMicroscopeModeCmd clCmd(qParameter.toInt());
			lRes = execCommand(clCmd);
			break;
		}
	/* --------------------------------------------------------- +/
						外部入力コマンド
	/+ --------------------------------------------------------- */
	case EXTERNAL_INPUT:										//外部入力コマンド V1
		{
			lRes = externalInputCmd(qParameter);
			break;
		}
	default:
		lRes = IM_ERROR_INVALID_PARAMETER;						//対応したコマンドがない
		break;
	}

	return lRes;
}
//--------------------------------------------------------------------------------
/*!
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL1ex::getPicture(QVariant &qParam)			//画像取得関数
{
	if(qParam.type() == QVariant::UserType)
	{
		if(chINRANGE(SIZE_PICTURE_HIGH, qParam.value<eKINDRECTID>(), SIZE_PICTURE_LOW))
		{
			m_iPictureRect = qParam.value<eKINDRECTID>();
		}
		else
		{
			m_iPictureRect = RECTID_PICTURE_HIGH;
		}
	}
	else if(qParam.type() == QVariant::StringList)
	{
		eKINDRECTID eRect = static_cast<eKINDRECTID>(qParam.toStringList().at(0).toInt());
		if(chINRANGE(SIZE_PICTURE_HIGH, eRect, SIZE_PICTURE_LOW))
		{
			m_iPictureRect= eRect;
		}
		else if(m_iPictureRect == RECTID_UNSUPPORT && m_iPictureRect == RECTID_PICTURE_DEFAULT)
		{
			m_iPictureRect = RECTID_PICTURE_HIGH;
		}
	}
	else
	{
		if(m_iPictureRect == RECTID_UNSUPPORT)
		{
			m_iPictureRect = RECTID_PICTURE_HIGH;
		}
	}

	QDataStream qCmdStream(this);								//create data stream for getPicture

	ClPictCmd clRecive(m_iPictureRect);							//this variable is for reception
	ClPictCmd clPict(m_iPictureRect);							//this variable is for send

MEASU_BEGIN
	qCmdStream << clPict;										//send command to device

	qCmdStream >> clRecive;										//receive command from device
MEASU_END

	//受信が成功したかを確認(ReadPastEndは期待のデータサイズor型が異なるエラーの様だが中身は問題ないのでここでは通す（Qt5.10の問題か）)
	if(!(qCmdStream.status() == QDataStream::Ok || qCmdStream.status() == QDataStream::ReadPastEnd)){
		qParam.clear();
		return IM_ERROR_CAPTURE_FAILURE;						//return IM_ERROR_COMMAND_FAILURE
	}
	//受信サイズが0でないことを確認
	if(clRecive.getDataSize() == 0){							//The data size is zero when may mode sd card
		qParam.clear();
		return IM_ERROR_CANNOT_OUTPUT_IMAGE;					//return IM_ERROR_CANNOT_OUTPUT_IMAGE
	}
	QSize qSize;
	if(analyJpegHeader(*clRecive.getJpegArray(),qSize) == true){
		m_usHeight = static_cast<quint16>(qSize.height());
		m_usWidth = static_cast<quint16>(qSize.width());
	}else{
		return IM_ERROR_OTHER;
	}
	//画像のデータサイズを格納
	m_iPictureSize = clRecive.getJpegArray()->length();
	//戻す変数にコピー
	qParam = *clRecive.getJpegArray();							//transfar jpeg data as output

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL1ex::getModelName(QVariant &qParam)
{
	QDataStream qCmdStream(this);								//create data stream for getPicture

	ClModelNameCmd clRecive;									//this variable is for reception
	ClModelNameCmd clCmd;

	qCmdStream << clCmd;										//send command to device

	qCmdStream >> clRecive;										//receeive command from device

	if(qCmdStream.status() != QDataStream::Ok){					//if confirm whether QDataStream status is OK
		return IM_ERROR_COMMAND_FAILURE;
	}
	qParam = clRecive.getModelName();							//output model name to QVariant

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL1ex::getVersion(QVariant &qParam)
{
	QDataStream qCmdStream(this);								//create data stream for getPicture
	QStringList strVerList;
	for(qint32 i = EL_VER_HEAD; i <= EL_VER_FPGA; i++ ){
		ClModelVerCmd clRecive;									//this variable is for reception
		ClModelVerCmd clCmd(i);

		qCmdStream << clCmd;									//send command to device

		qCmdStream >> clRecive;									//receeive command from device

		if(qCmdStream.status() != QDataStream::Ok){				//if confirm whether QDataStream status is OK
			return IM_ERROR_COMMAND_FAILURE;
		}

		QString strVersion;
		switch(i){
		case EL_VER_HEAD:
			strVersion = "Camera Head ";
			break;
		case EL_VER_MAIN:
			strVersion = "Camera Main ";
			break;
		case EL_VER_FPGA:
			strVersion = "Camera Engine ";
			break;
		default:
			break;
		}
		strVersion += clRecive.getVersion();
		strVerList << strVersion;								//add version string to QStringList
	}
	qParam = strVerList;
	return IM_ERROR_SUCCESS;									//output version to QVariant
}
//--------------------------------------------------------------------------------
/*!
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL1ex::execCaptureInit()						//I clear the capture flag
{
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL1ex::getCaptureStatus(QVariant &/*qParam*/)	//acquire the capture status
{
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief		外部入力コマンド V1
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL1ex::externalInputCmd(QVariant &qParam)
{
	QDataStream qCmdStream(this);								//create data stream for getPicture

	ClUsbCmd clRecive;											//this variable is for reception
	ClUsbCmd clCmd;

	PELMO_PACKET		sReturn;
	PELMO_COMMAND		pRetPacket;
	sReturn = (PELMO_PACKET)qParam.toByteArray().constData();
	pRetPacket = (PELMO_COMMAND)sReturn->Data;
	clCmd.setPacketType(sReturn->Header.PacketType);
	clCmd.setTransId(sReturn->Header.TransID);
	clCmd.setDataSize(sReturn->Header.DataSize);
	clCmd.setCode(pRetPacket->OpCode);
	clCmd.setParam1(pRetPacket->Param1);
	clCmd.setParam2(pRetPacket->Param2);
	clCmd.setParam3(pRetPacket->Param3);
	clCmd.setParam4(pRetPacket->Param4);
	clCmd.setParam5(pRetPacket->Param5);

	qCmdStream << clCmd;										//send command to device

	qCmdStream >> clRecive;										//receeive command from device

	if(qCmdStream.status() != QDataStream::Ok){					//if confirm whether QDataStream status is OK
		return IM_ERROR_COMMAND_FAILURE;
	}

	QByteArray qArrayRes;
	QDataStream qWork(&qArrayRes, QIODevice::WriteOnly);
	qWork.setByteOrder(QDataStream::LittleEndian);
	qWork	<< qint16(clRecive.getPagetType())
			<< qint16(clRecive.getTransId())
			<< qint32(clRecive.getDataSize())
			<< qint16(clRecive.getCode())
			<< qint16(0x00)
			<< qint32(clRecive.getParam1())
			<< qint32(clRecive.getParam2())
			<< qint32(clRecive.getParam3())
			<< qint32(clRecive.getParam4())
			<< qint32(clRecive.getParam5());
	qParam = qArrayRes;

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			readData
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint64	ClPresenterL1ex::readData ( char * data, qint64 maxSize )
{
	qint32 lReqLength = 0;
	qint32 lRet = 0;
    stREADDATACMD *pReadDataCmd = NULL;
    if(isOpen() == false){
		return -1;
	}

    if(maxSize > INT_MAX || maxSize <= 0){
        DBG("got error from writeData");
		return -1;
	}
    if(NULL == data){
        DBG("got error from readData");
        return -1;
    }
    pReadDataCmd = reinterpret_cast<stREADDATACMD *>(data);
    if(pReadDataCmd->llMaxSize > INT_MAX || pReadDataCmd->llMaxSize <= 0){
        DBG("got error from readData");
        return -1;
    }
    lReqLength = pReadDataCmd->llMaxSize;
    ClUsbCmd clUsb(pReadDataCmd->pData,lReqLength);							//

	if(m_lCmdID == GET_PICTURE){								//if code is Jpeg data
		if(pReadDataCmd->pByteArray == NULL){
			DBG("got error from readData");
			return -1;
		}

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
                QByteArray *pqArray = reinterpret_cast<QByteArray*>(pReadDataCmd->pByteArray);
                *pqArray = qVarList.at(1).toByteArray();
			}
		}

        //画像以外を受信したときの処理(SD,録画、失敗)
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
            memcpy(pReadDataCmd->pData, qCmdArray.data(), qCmdArray.length());
		}

	}else{
		QVariant qParam(lReqLength);
		lRet = resCommand(m_pInCmdEndp, qParam);
		if(lRet > 0){
			if(pReadDataCmd->llMaxSize < lRet){
				lRet = pReadDataCmd->llMaxSize;
			}
			if(qParam.type() == QVariant::ByteArray){
				QByteArray qCmdArray = qParam.toByteArray();
				memcpy(pReadDataCmd->pData, qCmdArray.data(), lRet);			//copy
				pReadDataCmd->llMaxSize = lRet;
#if 0  // for USB packet moniter in qDebug
				QByteArray qDbgArray(pReadDataCmd->pData,lRet);
				for(int i = 0; i < qDbgArray.length(); i=i+2){
					DBGFL("qDbgArray : %02x %02x",QChar(qDbgArray.at(i)).unicode(),QChar(qDbgArray.at(i+1)).unicode());
				}
#endif
			}
		}
	}
	if(lRet < 0){												//guess, Disconnected USB?
		bool a_bRet = 0;
		a_bRet = m_pUsbDevice->resetPipe();						//バスをリセット
		if(a_bRet != TRUE){
			DBGFL( "got error from resetPipe : %s",a_bRet == TRUE ? "TRUE" : "FALSE");
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
qint64	ClPresenterL1ex::writeData ( const char * data, qint64 maxSize )
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

	//各コマンドの値ではなく、こちらで割り振った番号によって判定
	if(m_lCmdID == GET_PICTURE){								//if code is Jpeg data
		lRet = reqPicture(m_pOutDataEndp, qParam);
	}else{														//画像以外のパケット送信
		lRet = reqCommand(m_pOutCmdEndp, qParam);
	}
	if(lRet < 0){												//guess, Disconnected USB?
		bool a_bRet = 0;
		a_bRet = m_pUsbDevice->resetPipe();						//バスをリセット
		if(a_bRet != TRUE){
			DBGFL( "got error from resetPipe : %s",a_bRet == TRUE ? "TRUE" : "FALSE");
		}
		lRet = -1;
	}
	return lRet;
}
//--------------------------------------------------------------------------------
/*!
	@fn			reqPicture
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL1ex::reqPicture(eENDPOINT	*pOutEndp, QVariant &param)
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
qint32 ClPresenterL1ex::resPicture(eENDPOINT	*pInEndp, QVariant &qParam)
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
					ioRes = m_pUsbDevice->readEP((LPVOID)pcBuffer,
												lreqLength,
												(UInt32*)&lActual_length,
												m_iTimeout);
				}else if((*pInEndp) == eEP4){
					ioRes = m_pUsbDevice->readEP2((LPVOID)pcBuffer,
												lreqLength,
												(UInt32*)&lActual_length,
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

			lRestLength -= lBlockSize;
			lBlockSize -= (lActual_length - siHeader);
			qJpegData.append(pcBuffer + siHeader, lActual_length - siHeader);
			while(0 < lBlockSize){
				if(lBlockSize < JPEG_PICTURESIZE){
					lreqLength = lBlockSize;
				}
				qint32 lActual_count = 0;
				do{
					IoDevice::IO_RESULT ioRes = IoDevice::IO_SUCCESS;
					if((*pInEndp) == eEP2){
						ioRes = m_pUsbDevice->readEP((LPVOID)pcBuffer,
													lreqLength,
													(UInt32*)&lActual_length,
													m_iTimeout);
					}else if((*pInEndp) == eEP4){
						ioRes = m_pUsbDevice->readEP2((LPVOID)pcBuffer,
													lreqLength,
													(UInt32*)&lActual_length,
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
qint32 ClPresenterL1ex::reqCommand(eENDPOINT	*pOutEndp, QVariant &param)
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
		if((*pOutEndp) == eEP1){
			ioRes = m_pUsbDevice->writeEP((LPVOID)qCmdArray.data(),
										qCmdArray.length(),
										(UInt32*)&lActual_length,
										m_iTimeout);
		}else if((*pOutEndp) == eEP3){
			ioRes = m_pUsbDevice->writeEP2((LPVOID)qCmdArray.data(),
										qCmdArray.length(),
										(UInt32*)&lActual_length,
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
qint32 ClPresenterL1ex::resCommand(eENDPOINT	*pInEndp, QVariant &param)
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
			ioRes = m_pUsbDevice->readEP((LPVOID)pucRecvData,
										lLength,
										(UInt32*)&lActual_length,
										m_iTimeout);
		}else if((*pInEndp) == eEP4){
			ioRes = m_pUsbDevice->readEP2((LPVOID)pucRecvData,
										lLength,
										(UInt32*)&lActual_length,
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
	param.setValue<QByteArray>(qResArray);

	return lActual_length;
}
//--------------------------------------------------------------------------------
/*!
	@fn			analyJpegHeader
	@brief
	@return		bool
 */
//--------------------------------------------------------------------------------
bool ClPresenterL1ex::analyJpegHeader(const QByteArray qJpgData, QSize &qSize)
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
	@fn			execCommand
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL1ex::execCommand(ClUsbCmd &clCmd)
{
	QDataStream qCmdStream(this);						//create data stream for getPicture

	ClUsbCmd clRecive;									//this variable is for reception

	qCmdStream << clCmd;								//send command to device

	qCmdStream >> clRecive;								//receeive command from device

	//受信が成功したかを確認(ReadPastEndは期待のデータサイズor型が異なるエラーの様だが中身は問題ないのでここでは通す（Qt5.10の問題か）)
	if(!(qCmdStream.status() == QDataStream::Ok || qCmdStream.status() == QDataStream::ReadPastEnd)){
		return IM_ERROR_COMMAND_FAILURE;
	}

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
