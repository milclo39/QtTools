﻿#include "ClUsbDriver/ClUsbDriver_global.h"
#include "Common/CmnDefine.h"
#include "ClUsbDriver/ClUsbDriver_linux.h"
#include "ClUsbDriver/Command_Presenter.h"
#include "linux/ClPresenterL1n.h"
#include "ClUsbCmd.h"
#include "ElmoUsbCmd.h"

#define JPEGHEAD_SIZE (575)
static const char	gpucJPEGHEAD_HED[] = {
	0xff, 0xd8, 0xff, 0xdb, 0x00, 0x84, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
	0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x05, 0x03, 0x03, 0x03, 0x03, 0x03, 0x06, 0x04, 0x04, 0x03,
	0x05, 0x07, 0x06, 0x07, 0x07, 0x07, 0x06, 0x07, 0x06, 0x08, 0x08, 0x0b, 0x09, 0x08, 0x08, 0x0a,
	0x08, 0x06, 0x07, 0x09, 0x0d, 0x09, 0x0a, 0x0b, 0x0b, 0x0c, 0x0c, 0x0c, 0x07, 0x09, 0x0d, 0x0e,
	0x0d, 0x0c, 0x0e, 0x0b, 0x0c, 0x0c, 0x0c, 0x01, 0x02, 0x02, 0x02, 0x03, 0x02, 0x03, 0x06, 0x03,
	0x03, 0x06, 0x0c, 0x08, 0x07, 0x08, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
	0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
	0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c,
	0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0xff, 0xc4, 0x01, 0xa2, 0x00, 0x00, 0x01, 0x05,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,
	0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02,
	0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7d, 0x01, 0x02, 0x03, 0x00, 0x04, 0x11,
	0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22, 0x71, 0x14, 0x32, 0x81, 0x91,
	0xa1, 0x08, 0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09,
	0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x34, 0x35, 0x36, 0x37,
	0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57,
	0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77,
	0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
	0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
	0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
	0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8,
	0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0x01, 0x00, 0x03, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,
	0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04,
	0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04,
	0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22, 0x32, 0x81, 0x08, 0x14,
	0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16,
	0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x35, 0x36,
	0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a, 0x53, 0x54, 0x55, 0x56,
	0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76,
	0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94,
	0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2,
	0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9,
	0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
	0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xff, 0xc0, 0x00, 0x11,
	0x08, 0x03, 0x00, 0x04, 0x00, 0x03, 0x01, 0x21, 0x00, 0x02, 0x11, 0x01, 0x03, 0x11, 0x01
};

//--------------------------------------------------------------------------------
/*!
	@class		ClPresenterL1n
	@brief
 */
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
ClPresenterL1n::ClPresenterL1n(qint32 lProductId, QObject *parent) :
	ClDeviceInfo(lProductId,parent)
{
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
ClPresenterL1n::~ClPresenterL1n()
{
}
//--------------------------------------------------------------------------------
/*!
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL1n::setupEndpoint()
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
						m_pInDataEndp = pEndp;
					}
				}else{
					if(uniEdaddr.edpNumber == 2){
						m_pOutCmdEndp = pEndp;
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
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL1n::CommandPresenter(qint32 CmdID, QVariant& qParameter)
{
	qint32 lRes = IM_ERROR_NOT_OPEN;						//未接続


	DBGFL("CommandPresenter linux");
	QtDXUTTimer qTimer;

	switch(CmdID){
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
	default:
		lRes = IM_ERROR_INVALID_PARAMETER;					//対応したコマンドがない
		break;
	}

	return lRes;

}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL1n::getPicture(QVariant &qParam)		//画像取得関数
{
	m_iPictureRect = RECTID_UNSUPPORT;

	QDataStream qCmdStream(this);						//create data stream for getPicture

	ClPictCmd clRecive(RECTID_UNSUPPORT);					//this variable is for reception
	ClPictCmd clPict(RECTID_UNSUPPORT);					//this variable is for send

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
	qSize.setWidth(clRecive.getParam2());
	qSize.setHeight(clRecive.getParam3());
	QByteArray qJpegData;
	QBuffer qBufStream(&qJpegData);
	qBufStream.open(QBuffer::WriteOnly|QBuffer::Unbuffered);
	qBufStream.setTextModeEnabled(false);
	qBufStream.write(gpucJPEGHEAD_HED, JPEGHEAD_SIZE);
	qBufStream.write(*clRecive.getJpegArray());

	if(qSize.isNull() == true){
		if(analyJpegHeader(qJpegData,qSize) == false){
			return IM_ERROR_OTHER;
		}
	}else{
		if(changeJpegHeader(qJpegData,qSize) == false){
			return IM_ERROR_OTHER;
		}
	}
	m_usHeight = static_cast<quint16>(qSize.height());
	m_usWidth = static_cast<quint16>(qSize.width());

	m_iPictureSize = qJpegData.length();				//set Jpeg length to m_iPictureSize
	qParam = qJpegData;									//transfar jpeg data as output
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL1n::getModelName(QVariant &qParam)
{
	qParam = QString("L-1n / TT-02s");					//output model name to QVariant

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL1n::getVersion(QVariant &qParam)
{
	QStringList strVerList;
	qParam = strVerList;
	return IM_ERROR_SUCCESS;								//output version to QVariant
}
//--------------------------------------------------------------------------------
/*!
	@class		ClPresenterP10S
	@brief
 */
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterP10s::getModelName(QVariant &qParam)
{
	qParam = QString("P10S");					//output model name to QVariant

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@class		ClPresenterP30s
	@brief
 */
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterP30s::getModelName(QVariant &qParam)
{
	qParam = QString("P30S");					//output model name to QVariant

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@class		ClPresenterP100
	@brief
 */
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterP100::getModelName(QVariant &qParam)
{
	qParam = QString("P100 / P100N");					//output model name to QVariant

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@class		ClPresenterFF1
	@brief
 */
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterFF1::getModelName(QVariant &qParam)
{
	qParam = QString("FF-1");					//output model name to QVariant

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
