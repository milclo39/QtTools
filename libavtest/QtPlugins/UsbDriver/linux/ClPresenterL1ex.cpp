#include "ClUsbDriver/ClUsbDriver_global.h"
#include "Common/CmnDefine.h"
#include "ClUsbDriver/ClUsbDriver_linux.h"
#include "ClUsbDriver/Command_Presenter.h"
#include "linux/ClPresenterL1ex.h"
#include "ClUsbCmd.h"
#include "ElmoUsbCmd.h"


//--------------------------------------------------------------------------------
/*!
	@class		ClPresenterL1ex
	@brief
 */
//--------------------------------------------------------------------------------
/*!
	@fn			ClPresenterL1ex
	@brief
 */
//--------------------------------------------------------------------------------
ClPresenterL1ex::ClPresenterL1ex(qint32 lProductId, QObject *parent) : ClDeviceInfo(lProductId, parent)
{
}
//--------------------------------------------------------------------------------
/*!
	@fn			~ClPresenterL1ex
	@brief
 */
//--------------------------------------------------------------------------------
ClPresenterL1ex::~ClPresenterL1ex()
{

}
//--------------------------------------------------------------------------------
/*!
	@fn			setupEndpoint
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL1ex::setupEndpoint()
{
	qint32 res = LIBUSB_SUCCESS;

	m_pDescriptor = new libusb_device_descriptor();
	res = libusb_get_device_descriptor(m_pDevice,m_pDescriptor);
	if(res != LIBUSB_SUCCESS){
		DBGFL("libusb_get_device_descriptor failure");
		return res;
	}

	res = libusb_get_active_config_descriptor(m_pDevice,&m_pConfig);
	if(res != LIBUSB_SUCCESS){
		DBGFL("libusb_get_active_config_descriptor failure");
		return res;
	}

	const libusb_interface *interface = NULL;						//interface
	const libusb_interface_descriptor *interdesc = NULL;			//interface descriptor

	res = LIBUSB_ERROR_PIPE;
	for(qint32 i = 0; i < m_pConfig->bNumInterfaces; i++ ){
		interface = &m_pConfig->interface[i];
		for(qint32 j = 0; j < interface->num_altsetting; j++ ){
			interdesc = &interface->altsetting[j];
			for(qint32 k = 0; k < interdesc->bNumEndpoints; k++ ){
				const libusb_endpoint_descriptor *pEndp = NULL;
				unENDPADDRESS uniEdaddr = {0};

				pEndp = &interdesc->endpoint[k];
				uniEdaddr.cData = pEndp->bEndpointAddress;
				res = libusb_clear_halt(m_pHandle,pEndp->bEndpointAddress);
				if(res != LIBUSB_SUCCESS){
					DBGFL("got error from libusb_clear_halt : %d",res);
					return res;
				}

				if(uniEdaddr.direction == ENDPOINT_IN){
					if(uniEdaddr.edpNumber == 1){
						m_pInCmdEndp = pEndp;
					}else if(uniEdaddr.edpNumber == 3){
						m_pInDataEndp = pEndp;
					}
				}else{
					if(uniEdaddr.edpNumber == 2){
						m_pOutCmdEndp = pEndp;
					}else if(uniEdaddr.edpNumber == 4){
						m_pOutDataEndp = pEndp;
					}
				}
				res = LIBUSB_SUCCESS;
			}
		}
	}

	return res;
}
//--------------------------------------------------------------------------------
/*!
	@fn			CommandPresenter
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL1ex::CommandPresenter(qint32 clCmdID, QVariant& qParameter)
{
	qint32 lRes = IM_ERROR_NOT_OPEN;						//未接続


	DBGFL("CommandPresenter linux");

	switch(clCmdID){
	case GET_PICTURE:											//画像取得
		{
			lRes = getPicture(qParameter);
			break;
		}
	case RELEASE_PICTURE:								//取得した画像領域の解放
		{
			lRes = IM_ERROR_SUCCESS;
			break;
		}
	case GET_SIZE_PICTURE:								//直前に取得したサイズの返却
		{
			lRes = IM_ERROR_SUCCESS;					//取得成功
			qParameter = m_iPictureSize;				//
			break;
		}
	case GET_RECT_PICTURE:								//画像の大きさ取得
		{
			lRes = IM_ERROR_SUCCESS;					//取得成功
			qParameter.setValue<eKINDRECTID>(m_iPictureRect);	//
			break;
		}
	case SET_RECT_PICTURE:								//画像の大きさ設定
		{
			lRes = IM_ERROR_INVALID_PARAMETER;			//初期化
			if(qParameter.type() == QVariant::UserType){
				if(chINRANGE(SIZE_PICTURE_HIGH,qParameter.value<eKINDRECTID>(),SIZE_PICTURE_LOW)){
					lRes = IM_ERROR_SUCCESS;					//取得成功
					m_iPictureRect = qParameter.value<eKINDRECTID>();
				}
			}
			break;
		}
	case GET_MODEL_NAME:								//機種名取得
		{
			lRes = getModelName(qParameter);
			break;
		}
	case GET_MODEL_VERSION:									//バージョン取得
		{
			lRes = getVersion(qParameter);
			break;
		}
	case GET_HEIGHT_PICTURE:								//現在の画像の縦幅
		{
			lRes = IM_ERROR_SUCCESS;					//取得成功
			qParameter = m_usHeight;					//
			break;
		}
	case GET_WIDTH_PICTURE:								//現在の画像の横幅
		{
			lRes = IM_ERROR_SUCCESS;					//取得成功
			qParameter = m_usWidth;						//
			break;
		}
	case ZOOM_WIDE:											//拡大
		{
			ClZoomCmd clCmd(EL_WIDE);
			lRes = execCommand(clCmd);
			break;
		}
	case ZOOM_TELE:											//縮小
		{
			ClZoomCmd clCmd(EL_TELE);
			lRes = execCommand(clCmd);
			break;
		}
	case ZOOM_STOP:											//ストップ
		{
			ClZoomCmd clCmd(EL_STOP);
			lRes = execCommand(clCmd);
			break;
		}
	case IRIS_OPEN:											//開く
		{
			ClIrisCmd clCmd(EL_OPEN);
			lRes = execCommand(clCmd);
			break;
		}
	case IRIS_CLOSE:										//閉じる
		{
			ClIrisCmd clCmd(EL_CLOSE);
			lRes = execCommand(clCmd);
			break;
		}
	case IRIS_STOP:											//ストップ
		{
			ClIrisCmd clCmd(EL_ISTOP);
			lRes = execCommand(clCmd);
			break;
		}
	case IRIS_INITIALIZE:									//初期位置
		{
			ClIrisCmd clCmd(EL_NORMAL);
			lRes = execCommand(clCmd);
			break;
		}
	case FOCUS_AUTO:										//自動
		{
			ClAutoFocusCmd clCmd;
			lRes = execCommand(clCmd);
			break;
		}
	case FOCUS_NEAR:										//近く
		{
			ClFocusCmd clCmd(EL_NEAR);
			lRes = execCommand(clCmd);
			break;
		}
	case FOCUS_FAR:											//遠く
		{
			ClFocusCmd clCmd(EL_FAR);
			lRes = execCommand(clCmd);
			break;
		}
	case FOCUS_STOP:										//ストップ
		{
			ClFocusCmd clCmd(EL_STOP);
			lRes = execCommand(clCmd);
			break;
		}
	case ROTATION:											//回転要求
		{
			if(qParameter.isNull() == true){							//NULL判定
				lRes = IM_ERROR_INVALID_PARAMETER;			//ParameterがNULLなら失敗
				break;
			}
			ClRotationCmd clCmd(qParameter.toInt());
			lRes = execCommand(clCmd);
			break;
		}
	case POSI_NEGA:											//ネガ or ポジ
		{
			if(qParameter.isNull() == true){				//NULL判定
				lRes = IM_ERROR_INVALID_PARAMETER;			//ParameterがNULLなら失敗
				break;
			}
			ClPosiNegaCmd clCmd(qParameter.toInt());
			lRes = execCommand(clCmd);
			break;
		}
	case COLOR_BW:											//Color or BW
		{
			if(qParameter.isNull() == true){							//NULL判定
				lRes = IM_ERROR_INVALID_PARAMETER;			//ParameterがNULLなら失敗
				break;
			}
			ClColorBWCmd clCmd(qParameter.toInt());
			lRes = execCommand(clCmd);
			break;
		}
	case SLOW_SHUTTER:										//低速シャッター
		{
			if(qParameter.isNull() == true){							//NULL判定
				lRes = IM_ERROR_INVALID_PARAMETER;			//ParameterがNULLなら失敗
				break;
			}
			ClSlowShutterCmd clCmd(qParameter.toInt());
			lRes = execCommand(clCmd);
			break;
		}
	case IMAGE_MODE:										//イメージモード
		{
			if(qParameter.isNull() == true){							//NULL判定
				lRes = IM_ERROR_INVALID_PARAMETER;			//ParameterがNULLなら失敗
				break;
			}
			ClImageModeCmd clCmd(qParameter.toInt());
			lRes = execCommand(clCmd);
			break;
		}
	case MICROSCOPE_MODE:									//顕微鏡モード
		{
			if(qParameter.isNull() == true){							//NULL判定
				lRes = IM_ERROR_INVALID_PARAMETER;			//ParameterがNULLなら失敗
				break;
			}
			ClMicroscopeModeCmd clCmd(qParameter.toInt());
			lRes = execCommand(clCmd);
			break;
		}
	default:
		lRes = IM_ERROR_INVALID_PARAMETER;					//対応したコマンドがない
		break;
	}

	return lRes;

}
//--------------------------------------------------------------------------------
/*!
	@fn			getPicture
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL1ex::getPicture(QVariant &qParam)		//画像取得関数
{
	if(qParam.type() == QVariant::UserType){
		if(chINRANGE(SIZE_PICTURE_HIGH,qParam.value<eKINDRECTID>(),SIZE_PICTURE_LOW)){
			m_iPictureRect = qParam.value<eKINDRECTID>();
		}else{
			m_iPictureRect = RECTID_PICTURE_HIGH;
		}
	}else{
		if(m_iPictureRect == RECTID_UNSUPPORT){
			m_iPictureRect = RECTID_PICTURE_HIGH;
		}
	}

	QDataStream qCmdStream(this);				//create data stream for getPicture

	ClPictCmd clRecive(m_iPictureRect);					//this variable is for reception
	ClPictCmd clPict(m_iPictureRect);					//this variable is for send

MEASU_BEGIN
	qCmdStream << clPict;								//send command to device

	qCmdStream >> clRecive;								//receive command from device
MEASU_END

	if(qCmdStream.status() != QDataStream::Ok){			//if confirm whether QDataStream status is OK
		qParam.clear();
		return IM_ERROR_CAPTURE_FAILURE;				//return IM_ERROR_COMMAND_FAILURE
	}
	if(clRecive.getDataSize() == 0){					//The data size is zero when may mode sd card
		qParam.clear();
		return IM_ERROR_CANNOT_OUTPUT_IMAGE;			//return IM_ERROR_CANNOT_OUTPUT_IMAGE
	}
	QSize qSize;
	if(analyJpegHeader(*clRecive.getJpegArray(),qSize) == true){
		m_usHeight = static_cast<quint16>(qSize.height());
		m_usWidth = static_cast<quint16>(qSize.width());
	}else{
		return IM_ERROR_OTHER;
	}
	m_iPictureSize = clRecive.getJpegArray()->length();
	qParam = *clRecive.getJpegArray();					//transfar jpeg data as output
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			getModelName
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL1ex::getModelName(QVariant &qParam)
{
	QDataStream qCmdStream(this);				//create data stream for getPicture

	ClModelNameCmd clRecive;							//this variable is for reception
	ClModelNameCmd clCmd;

	qCmdStream << clCmd;								//send command to device

	qCmdStream >> clRecive;								//receeive command from device

	if(qCmdStream.status() != QDataStream::Ok){			//if confirm whether QDataStream status is OK
		return IM_ERROR_COMMAND_FAILURE;
	}
	qParam = clRecive.getModelName();					//output model name to QVariant

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			getVersion
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL1ex::getVersion(QVariant &qParam)
{
	QDataStream qCmdStream(this);					//create data stream for getPicture
	QStringList strVerList;
	for(qint32 i = EL_VER_HEAD; i <= EL_VER_FPGA; i++ ){
		ClModelVerCmd clRecive;								//this variable is for reception
		ClModelVerCmd clCmd(i);

		qCmdStream << clCmd;								//send command to device

		qCmdStream >> clRecive;								//receeive command from device

		if(qCmdStream.status() != QDataStream::Ok){			//if confirm whether QDataStream status is OK
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
		strVerList << strVersion;							//add version string to QStringList
	}
	qParam = strVerList;
	return IM_ERROR_SUCCESS;								//output version to QVariant
}
//--------------------------------------------------------------------------------
/*!
	@fn			execCaptureInit
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
	@fn			getCaptureStatus
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL1ex::getCaptureStatus(QVariant &/*qParam*/)		//acquire the capture status
{
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
