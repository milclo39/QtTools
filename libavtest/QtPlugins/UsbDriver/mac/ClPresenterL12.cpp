﻿#include "mac/ClPresenterL12.h"
#include "ClUsbDriver/ClUsbDriver_mac.h"
#include "ClUsbDriver/ClUsbDriver_global.h"
#include "Common/CmnDefine.h"
#include "ClUsbDriver/Command_Presenter.h"
#include "ClUsbCmd.h"
#include "ElmoUsbCmd.h"
#include "ClUsbCmdV2.h"
#include "ElmoUsbCmdV2.h"
#include "UsbDevice.h"

#include <QDateTime>
#include "time.h"

#define ELUSB_WT_USBPACKET_LENGTH	(512)	//512バイト境界
#define ELUSB_WT_SENDLENGTH_ADJUST	(4)		//512バイト境界調整

//--------------------------------------------------------------------------------
/*!
	@brief		閏年の年の、各月の日数
 */
//--------------------------------------------------------------------------------
const int gpiLeapDays[] =
{
	-1, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};
//--------------------------------------------------------------------------------
/*!
	@brief		閏年のでない年の、各月の日数
 */
//--------------------------------------------------------------------------------
const int gpiNormalDays[] =
{
	-1, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};


//--------------------------------------------------------------------------------
/*!
	@class		ClPresenterL12
	@brief
 */
//--------------------------------------------------------------------------------
/*!
	@brief		コンストラクタ
 */
//--------------------------------------------------------------------------------
ClPresenterL12::ClPresenterL12(qint32 lProductId, QObject *parent) : ClDeviceInfo(lProductId, parent)
{
	m_lCmdID		= 0;										//Command ID
	m_lJpgSize		= 0;										//Jpg Size
	m_lJudgeCmdVer	= 0;										//Command Version

	m_lImgDataSize	= 0;										//1画像分のJpgサイズ
	m_lFileSize		= 0;										//作成する画像ファイルのサイズ
	m_lSessionID	= 0;										//SessionID格納用
	m_lFileType		= 0;										//ファイルの種別
	m_lTimeStamp	= 0;										//タイムスタンプ

	m_clStrFilePath.clear();									//アップロード ファイルパス
	m_lUpFileSize	= 0;										//アップロード ファイルサイズ
	m_lUploadCmd	= NULL;										//アップロード コマンド

	m_qArrayBackStream.clear();									//未送信ストリーム

	m_lWroteSize	= 0;
	m_lProgressPer	= 0;
	m_lStatus		= 1;
}
//--------------------------------------------------------------------------------
/*!
	@brief		デストラクタ
 */
//--------------------------------------------------------------------------------
ClPresenterL12::~ClPresenterL12()
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
qint32 ClPresenterL12::open()
{

	if(isOpen() == true){										//開かれているなら
		return IM_ERROR_SUCCESS;
	}
	qint32 lEpRes = setupEndpoint();							//is overrided setupEndpoint of call
	if(lEpRes != IM_ERROR_SUCCESS){								//
		return lEpRes;											//
	}

	if(m_lProductId == PRODUCT_SESAME)
	{
		m_pUsbDevice = new ClIoDevice(m_lProductId);			//L-12の場合
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
qint32 ClPresenterL12::setupEndpoint()
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
qint32 ClPresenterL12::CommandPresenter(qint32 CmdID, QVariant& qParameter)
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
							V2 Command
	/+ --------------------------------------------------------- */
	case GET_PICTURE_V2:										//画像取得
		{
			lRes = getPictureV2(qParameter);
			break;
		}
	case START_STREAM_V2:										//ストリーム取得 開始要求
		{
			lRes = startStreamCmdV2();
			break;
		}
	case GET_STREAM_V2:											//ストリーム取得 受信
		{
			lRes = getStreamV2(qParameter);
			break;
		}
	case STOP_STREAM_V2:										//ストリーム取得 終了要求＋受信
		{
			lRes = stopStreamCmdV2();
			break;
		}
	case SEND_UPIM_INFO_V2:										//画像アップロード 前処理 (情報取得)
		{
			lRes = sendImgFileInfoCmdV2(qParameter);
			break;
		}
	case SEND_UPFW_INFO_V2:										//FWアップロード 前処理 (情報取得)
		{
			lRes = sendFwFileInfoCmdV2(qParameter);
			break;
		}
	case UPLOAD_FILE_V2:										//ファイルアップロード 実行
		{
			lRes = uploadFileCmdV2(qParameter);
			break;
		}
	case ZOOM_WIDE_V2:
		{
			ClZoomWideCmdV2 clCmd;
			lRes = execCommandV2(clCmd);
			break;
		}
	case ZOOM_TELE_V2:
		{
			ClZoomTeleCmdV2 clCmd;
			lRes = execCommandV2(clCmd);
			break;
		}
	case ZOOM_STOP_V2:
		{
			ClZoomStopCmdV2 clCmd;
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_VARIANT_ZOOM_V2:
		{
			ClZoomVariantSetCmdV2 clCmd(qParameter.toInt());
			lRes = execCommandV2(clCmd);
			break;
		}
	case GET_ZOOM_STATUS_V2:
		{
			ClZoomStatusCmdV2 clCmd;
			lRes = execGetStatusCommand(clCmd, qParameter);
			break;
		}
	case FOCUS_NEAR_V2:
		{
			ClFocusNearCmdV2 clCmd;
			lRes = execCommandV2(clCmd);
			break;
		}
	case FOCUS_FAR_V2:
		{
			ClFocusFarCmdV2 clCmd;
			lRes = execCommandV2(clCmd);
			break;
		}
	case FOCUS_AUTO_V2:
		{
			ClAutoFocusCmdV2 clCmd;
			lRes = execCommandV2(clCmd);
			break;
		}
	case FOCUS_STOP_V2:
		{
			ClFocusStopCmdV2 clCmd;
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_VARIANT_FOCUS_V2:
		{
			ClFocusVariantSetCmdV2 clCmd(qParameter.toInt());
			lRes = execCommandV2(clCmd);
			break;
		}
	case GET_FOCUS_STATUS_V2:
		{
			ClFocusStatusCmdV2 clCmd;
			lRes = execGetStatusCommand(clCmd, qParameter);
			break;
		}
	case SET_AFMODE_ON_V2:
		{
			ClAFModeSetCmdV2 clCmd(EL_PRM_ON);
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_AFMODE_OFF_V2:
		{
			ClAFModeSetCmdV2 clCmd(EL_PRM_OFF);
			lRes = execCommandV2(clCmd);
			break;
		}
	case GET_AFMODE_STATUS_V2:
		{
			ClAFModeStatusCmdV2 clCmd;
			lRes = execGetStatusCommand(clCmd, qParameter);
			break;
		}
	case IRIS_OPEN_V2:
		{
			ClIrisOpenCmdV2 clCmd;
			lRes = execCommandV2(clCmd);
			break;
		}
	case IRIS_CLOSE_V2:
		{
			ClIrisCloseCmdV2 clCmd;
			lRes = execCommandV2(clCmd);
			break;
		}
	case IRIS_INITIALIZE_V2:
		{
			ClIrisInitCmdV2 clCmd;
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_VARIANT_IRIS_V2:
		{
			ClIrisVariantSetCmdV2 clCmd(qParameter.toInt());
			lRes = execCommandV2(clCmd);
			break;
		}
	case GET_IRIS_STATUS_V2:
		{
			ClIrisStatusCmdV2 clCmd;
			lRes = execGetStatusCommand(clCmd, qParameter);
			break;
		}
	case SET_BRTMODE_ON_V2:
		{
			ClBRTModeSetCmdV2 clCmd(EL_PRM_ON);
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_BRTMODE_OFF_V2:
		{
			ClBRTModeSetCmdV2 clCmd(EL_PRM_OFF);
			lRes = execCommandV2(clCmd);
			break;
		}
	case GET_BRTMODE_STATUS_V2:
		{
			ClBRTModeStatusCmdV2 clCmd;
			lRes = execGetStatusCommand(clCmd, qParameter);
			break;
		}
	case GET_MODEL_NAME_V2:										//機種名取得
		{
			lRes = getModelNameV2(qParameter);
			break;
		}
	case GET_MODEL_VERSION_V2:									//バージョン取得
		{
			lRes = getVersionV2(qParameter);
			break;
		}
	case GET_SH_VERSION_V2:										//SH バージョン取得
		{
			lRes = getSHVersionV2(qParameter);
			break;
		}
	case GET_LENS_VERSION_V2:									//レンズ バージョン取得
		{
			lRes = getLensVersionV2(qParameter);
			break;
		}
	case GET_FPGA_VERSION_V2:									//FPGA バージョン取得
		{
			lRes = getFPGAVersionV2(qParameter);
			break;
		}
	case GET_PRODUCTCODE_V2:									//機種コード取得
		{
			lRes = getProductCodeV2(qParameter);
			break;
		}
	case GET_CURRENT_MODE_V2:									//モード取得
		{
			lRes = getCurrentModeV2(qParameter);
			break;
		}
	case GET_RES_SW_V2:
		{
			ClResSwCmdV2 clCmd;
			lRes = execGetStatusCommand(clCmd, qParameter);
			break;
		}
	case GET_IMOUT_SW_V2:
		{
			ClImgOutSwCmdV2 clCmd;
			lRes = execGetStatusCommand(clCmd, qParameter);
			break;
		}
	case GET_CAPPUSH_FLG_V2:
		{
			ClCapPushFlgSwCmdV2 clCmd;
			lRes = execGetStatusCommand(clCmd, qParameter);
			break;
		}
	case SET_SYSTEM_TIME_V2:									//時刻設定
		{
			lRes = setSystemTimeV2(qParameter);
			break;
		}
	case SET_MODEL_NAME_V2:
		{
			lRes = setModelNameV2(qParameter);
			break;
		}
	case SET_PRODUCTCODE_V2:
		{
			lRes = setProductCodeV2(qParameter);
			break;
		}
	case SET_WHITE_AUTO_V2:
		{
			ClWhiteSetCmdV2 clCmd(EL_PRM_AUTO);
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_WHITE_ONEPUSH_V2:
		{
			ClWhiteSetCmdV2 clCmd(EL_PRM_ONEPUSH);
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_WHITE_MANUAL_V2:
		{
			ClWhiteSetCmdV2 clCmd(EL_PRM_MANUAL);
			lRes = execCommandV2(clCmd);
			break;
		}
	case GET_WHITE_STATUS_V2:
		{
			ClWhiteStatusCmdV2 clCmd;
			lRes = execGetStatusCommand(clCmd, qParameter);
			break;
		}
	case SET_FLICKER_50HZ_V2:
		{
			ClFlickerSetCmdV2 clCmd(EL_PRM_FLICKER50);
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_FLICKER_60HZ_V2:
		{
			ClFlickerSetCmdV2 clCmd(EL_PRM_FLICKER60);
			lRes = execCommandV2(clCmd);
			break;
		}
	case GET_FLICKER_STATUS_V2:
		{
			ClFlickerStatusCmdV2 clCmd;
			lRes = execGetStatusCommand(clCmd, qParameter);
			break;
		}
	case SET_IM_MODE_TEXT1_V2:
		{
			ClImageModeSetCmdV2 clCmd(EL_PRM_TEXT01);
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_IM_MODE_TEXT2_V2:
		{
			ClImageModeSetCmdV2 clCmd(EL_PRM_TEXT02);
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_IM_MODE_TEXT3_V2:
		{
			ClImageModeSetCmdV2 clCmd(EL_PRM_TEXT03);
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_IM_MODE_COLOR1_V2:
		{
			ClImageModeSetCmdV2 clCmd(EL_PRM_COLOR01);
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_IM_MODE_COLOR2_V2:
		{
			ClImageModeSetCmdV2 clCmd(EL_PRM_COLOR02);
			lRes = execCommandV2(clCmd);
			break;
		}
	case GET_IM_MODE_STATUS_V2:
		{
			ClImageModeStatusCmdV2 clCmd;
			lRes = execGetStatusCommand(clCmd, qParameter);
			break;
		}
	case SET_EDGE_LOW_V2:
		{
			ClEdgeSetCmdV2 clCmd(EL_PRM_LOW);
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_EDGE_MIDDLE_V2:
		{
			ClEdgeSetCmdV2 clCmd(EL_PRM_MIDDLE);
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_EDGE_HIGH_V2:
		{
			ClEdgeSetCmdV2 clCmd(EL_PRM_HIGH);
			lRes = execCommandV2(clCmd);
			break;
		}
	case GET_EDGE_STATUS_V2:
		{
			ClEdgeStatusCmdV2 clCmd;
			lRes = execGetStatusCommand(clCmd, qParameter);
			break;
		}
	case SET_GAMMA_LOW_V2:
		{
			ClGammaSetCmdV2 clCmd(EL_PRM_LOW);
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_GAMMA_MIDDLE_V2:
		{
			ClGammaSetCmdV2 clCmd(EL_PRM_MIDDLE);
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_GAMMA_HIGH_V2:
		{
			ClGammaSetCmdV2 clCmd(EL_PRM_HIGH);
			lRes = execCommandV2(clCmd);
			break;
		}
	case GET_GAMMA_STATUS_V2:
		{
			ClGammaStatusCmdV2 clCmd;
			lRes = execGetStatusCommand(clCmd, qParameter);
			break;
		}
	case SET_FREEZE_ON_V2:
		{
			ClFreezeSetCmdV2 clCmd(EL_PRM_ON);
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_FREEZE_OFF_V2:
		{
			ClFreezeSetCmdV2 clCmd(EL_PRM_OFF);
			lRes = execCommandV2(clCmd);
			break;
		}
	case GET_FREEZE_STATUS_V2:
		{
			ClFreezeStatusCmdV2 clCmd;
			lRes = execGetStatusCommand(clCmd, qParameter);
			break;
		}
	case SET_ROTATION_ON_V2:
		{
			ClRotationSetCmdV2 clCmd(EL_PRM_ON);
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_ROTATION_OFF_V2:
		{
			ClRotationSetCmdV2 clCmd(EL_PRM_OFF);
			lRes = execCommandV2(clCmd);
			break;
		}
	case GET_ROTATION_STATUS_V2:
		{
			ClRotationStatusCmdV2 clCmd;
			lRes = execGetStatusCommand(clCmd, qParameter);
			break;
		}
	case SET_NEGAPOSI_POSI_V2:
		{
			ClNegaPosiSetCmdV2 clCmd(EL_PRM_POSI);
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_NEGAPOSI_NEGA_V2:
		{
			ClNegaPosiSetCmdV2 clCmd(EL_PRM_NEGA);
			lRes = execCommandV2(clCmd);
			break;
		}
	case GET_NEGAPOSI_STATUS_V2:
		{
			ClNegaPosiStatusCmdV2 clCmd;
			lRes = execGetStatusCommand(clCmd, qParameter);
			break;
		}
	case SET_COLORMONO_COLOR_V2:
		{
			ClColorMonoSetCmdV2 clCmd(EL_PRM_COLOR);
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_COLORMONO_MONO_V2:
		{
			ClColorMonoSetCmdV2 clCmd(EL_PRM_BLACKANDWHITE);
			lRes = execCommandV2(clCmd);
			break;
		}
	case GET_COLORMONO_STATUS_V2:
		{
			ClColorMonoStatusCmdV2 clCmd;
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_SCOPE_ON_V2:
		{
			ClScopeSetCmdV2 clCmd(EL_PRM_ON);
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_SCOPE_OFF_V2:
		{
			ClScopeSetCmdV2 clCmd(EL_PRM_OFF);
			lRes = execCommandV2(clCmd);
			break;
		}
	case GET_SCOPE_STATUS_V2:
		{
			ClScopeStatusCmdV2 clCmd;
			lRes = execGetStatusCommand(clCmd, qParameter);
			break;
		}
	case SET_SPEAKER_STATUS_V2:
		{
			if(qParameter.type() != QVariant::Int)
			{
				return IM_ERROR_COMMAND_FAILURE;
			}

			ClSpeakerSetCmdV2 clCmd(qParameter.toInt());
			lRes = execCommandV2(clCmd);
			break;
		}
	case GET_SPEAKER_STATUS_V2:
		{
			ClSpeakerStatusCmdV2 clCmd;
			lRes = execGetStatusCommand(clCmd, qParameter);
			break;
		}
	case SET_VIDEO_NTSC_V2:
		{
			ClVideoSetCmdV2 clCmd(EL_PRM_NTSC);
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_VIDEO_PAL_V2:
		{
			ClVideoSetCmdV2 clCmd(EL_PRM_PAL);
			lRes = execCommandV2(clCmd);
			break;
		}
	case GET_VIDEO_STATUS_V2:
		{
			ClVideoStatusCmdV2 clCmd;
			lRes = execGetStatusCommand(clCmd, qParameter);
			break;
		}
	case SET_LANG_JPN_V2:
		{
			ClLanguageSetCmdV2 clCmd(EL_PRM_JAPANESE);
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_LANG_ENG_V2:
		{
			ClLanguageSetCmdV2 clCmd(EL_PRM_ENGLISH);
			lRes = execCommandV2(clCmd);
			break;
		}
	case GET_LANG_STATUS_V2:
		{
			ClLanguageStatusCmdV2 clCmd;
			lRes = execGetStatusCommand(clCmd, qParameter);
			break;
		}
	case SET_GUIDE_ON_V2:
		{
			ClGuideSetCmdV2 clCmd(EL_PRM_ON);
			lRes = execCommandV2(clCmd);
			break;
		}
	case SET_GUIDE_OFF_V2:
		{
			ClGuideSetCmdV2 clCmd(EL_PRM_OFF);
			lRes = execCommandV2(clCmd);
			break;
		}
	case GET_GUIDE_STATUS_V2:
		{
			ClGuideStatusCmdV2 clCmd;
			lRes = execGetStatusCommand(clCmd, qParameter);
			break;
		}
	case SAVE_SETTING_V2:
		{
			break;
		}
	case LOAD_SETTING_V2:
		{
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
	case EXTERNAL_INPUT_V2:										//外部入力コマンド V2
		{
			lRes = externalInputCmdV2(qParameter);
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
qint32 ClPresenterL12::getPicture(QVariant &qParam)			//画像取得関数
{
	qint32 lQuality = JPEG_QUARITY_DEF;
	if(qParam.type() == QVariant::StringList){
		QStringList clParamList = qParam.toStringList();
		if(clParamList.count() == 2)
		{
			QString strKindRect = clParamList.at(0);
			m_iPictureRect = static_cast<eKINDRECTID>(strKindRect.toInt());
			if(chINRANGE(SIZE_PICTURE_HIGH,m_iPictureRect,SIZE_PICTURE_LOW) == false){
				m_iPictureRect = RECTID_PICTURE_HIGH;
			}

			QString strQuality = clParamList.at(1);
			lQuality = strQuality.toInt();
		}
	}else{
		if(m_iPictureRect == RECTID_UNSUPPORT){
			m_iPictureRect = RECTID_PICTURE_HIGH;
		}
	}

	QDataStream qCmdStream(this);								//create data stream for getPicture

	ClPictCmd clRecive(m_iPictureRect);							//this variable is for reception
	ClPictCmd clPict(m_iPictureRect);							//this variable is for send
	clPict.setParam1(lQuality);

MEASU_BEGIN
	qCmdStream << clPict;										//send command to device

	qCmdStream >> clRecive;										//receive command from device
MEASU_END

	//受信サイズが0でないことを確認
	if(clRecive.getDataSize() == 0){							//The data size is zero when may mode sd card
		qParam.clear();
		if(clRecive.getCode() == IM_ERROR_SUCCESS)
		{
			switch(clRecive.getParam2())
			{
			case CAMERA_MODE_SD:			//SD			no break
			case CAMERA_MODE_REC:			//Rec			no break
			case CAMERA_MODE_VIDEOREPLAY:	//Movie
				{
					return IM_ERROR_CANNOT_OUTPUT_IMAGE;
				}
			case CAMERA_MODE_BUSY:	//ぶつかった
				{
					return IM_ERROR_DEVICE_BUSY;
				}
			}
		}
		else
		{
			if(clRecive.getParam2() == CAMERA_MODE_FAIL)	//ガチエラー
			{
				return IM_ERROR_CAPTURE_FAILURE;
			}
		}
		return IM_ERROR_CANNOT_OUTPUT_IMAGE;
	}
	//受信が成功したかを確認(ReadPastEndは期待のデータサイズor型が異なるエラーの様だが中身は問題ないのでここでは通す（Qt5.10の問題か）)
	if(!(qCmdStream.status() == QDataStream::Ok || qCmdStream.status() == QDataStream::ReadPastEnd)){
		qParam.clear();
		return IM_ERROR_CAPTURE_FAILURE;						//return IM_ERROR_COMMAND_FAILURE
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
qint32 ClPresenterL12::getModelName(QVariant &qParam)
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
qint32 ClPresenterL12::getVersion(QVariant &qParam)
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
qint32 ClPresenterL12::execCaptureInit()						//I clear the capture flag
{
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::getCaptureStatus(QVariant &/*qParam*/)	//acquire the capture status
{
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief		Jpg取得 V2コマンド
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::getPictureV2(QVariant &qParam)			//画像取得関数	V2コマンド
{
	qint32 lQuality = JPEG_QUARITY_DEF;
	if(qParam.type() == QVariant::StringList){
		QStringList clParamList = qParam.toStringList();
		if(clParamList.count() == 2)
		{
			QString strKindRect = clParamList.at(0);
			m_iPictureRect = static_cast<eKINDRECTID>(strKindRect.toInt());
			if(chINRANGE(SIZE_PICTURE_HIGH,m_iPictureRect,SIZE_PICTURE_LOW) == false){
				m_iPictureRect = RECTID_PICTURE_HIGH;
			}

			QString strQuality = clParamList.at(1);
			lQuality = strQuality.toInt();
		}
	}else{
		if(m_iPictureRect == RECTID_UNSUPPORT){
			m_iPictureRect = RECTID_PICTURE_HIGH;
		}
	}

	QDataStream qCmdStream(this);								//create data stream for getPicture

	ClPictCmdV2 clRecive(m_iPictureRect);						//this variable is for reception
	ClPictCmdV2 clPict(m_iPictureRect);							//this variable is for send
	clPict.setParam1(lQuality);

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
	@brief		ストリーム開始要求
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::startStreamCmdV2()						//ストリーム情報取得関数	V2コマンド
{
	QDataStream qCmdStream(this);								//create data stream for getPicture

	ClStartStreamCmdV2 clSend(m_iPictureRect);					//this variable is for send

MEASU_BEGIN
	qCmdStream << clSend;										//send command to device
MEASU_END
	if(qCmdStream.status() != QDataStream::Ok){					//if confirm whether QDataStream status is OK
		return IM_ERROR_CAPTURE_FAILURE;						//return IM_ERROR_COMMAND_FAILURE
	}
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief		ストリーム取得
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::getStreamV2(QVariant &qParam)			//ストリーム取得
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

	QDataStream qCmdStream(this);								//create data stream for getPicture

	ClStartStreamCmdV2 clRecive(m_iPictureRect);				//this variable is for reception

MEASU_BEGIN
	qCmdStream >> clRecive;										//receive command from device		受信のみ
MEASU_END
	//受信が成功したかを確認
	if(qCmdStream.status() != QDataStream::Ok){					//if confirm whether QDataStream status is OK
		qParam.clear();
		return IM_ERROR_CAPTURE_FAILURE;						//return IM_ERROR_COMMAND_FAILURE
	}

	//受信サイズが0でないことを確認
	if(clRecive.getDataSize() == 0){							//The data size is zero when may mode sd card
		qParam.clear();
		return IM_ERROR_CANNOT_OUTPUT_IMAGE;					//return IM_ERROR_CANNOT_OUTPUT_IMAGE
	}
	//画像データならJpegHeaderの解析を行う
	if(clRecive.getJpegArray()->at(0) == CHECK_CHAR_FF &&		//1st 1byte data is 0xFF
	   clRecive.getJpegArray()->at(1) == CHECK_CHAR_D8 )		//2nd 1byte data is 0xD8
	{
		QSize qSize;
		if(analyJpegHeader(*clRecive.getJpegArray(),qSize) == true){
			m_usHeight = static_cast<quint16>(qSize.height());
			m_usWidth = static_cast<quint16>(qSize.width());
		}else{
			return IM_ERROR_OTHER;
		}
	}
	//画像のデータサイズを格納
	m_iPictureSize = clRecive.getJpegArray()->length();
	//戻す変数にコピー
	qParam = *clRecive.getJpegArray();							//transfar jpeg data as output
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief		ストリーム終了
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::stopStreamCmdV2()						//ストリーム終了
{
	QDataStream qCmdStream(this);								//create data stream for getPicture

	ClStopStreamCmdV2 clSend;									//this variable is for reception
	ClStopStreamCmdV2 clRecive;									//this variable is for reception
	clSend.setParam0(m_lSessionID);								//set session ID
	clSend.setSessionBit();

	qCmdStream << clSend;										//send command to device

	do{
		qCmdStream >> clRecive;									//receeive command from device
	}while(clRecive.getDataSize() != 32);


	if(qCmdStream.status() != QDataStream::Ok){					//if confirm whether QDataStream status is OK
		return IM_ERROR_COMMAND_FAILURE;
	}

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief		受け取ったファイルパスからファイル情報を取得・送信、SessinID取得
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::sendImgFileInfoCmdV2(QVariant &qParam)	//アップロード画像情報
{
	m_lUploadCmd = NULL;										//コマンド確認用変数を初期化
	//指定したファイルのパスを取得
	QString qWork;
	qWork = qParam.toStringList().at(0);						//参照したファイルパスを取得
	QFile qFile(qWork);
	if(!qFile.open(QFile::ReadOnly))
	{
		return IM_ERROR_COMMAND_FAILURE;
	}
	qFile.close();
	//ファイル情報取得
	FILE_INFO fileInfo;
	QFileInfo qFileInfo(qWork);
	if(qFileInfo.isReadable())		//読込可能ファイル
	if(qFileInfo.isWritable())		//書込可能ファイル

	fileInfo.ulFileAttributes = 0x001;	//ファイル属性をひとまず読み取り専用に設定
	QDateTime clTimeCreate(qFileInfo.created());									//作成日
	fileInfo.stCreationTime.ulHighDateTime = atoi(clTimeCreate.toString("yyyy").toLocal8Bit().constData());
	fileInfo.stCreationTime.ulLowDateTime = atoi(clTimeCreate.toString("MMddhhmmss").toLocal8Bit().constData());
	QDateTime clTimeRead(qFileInfo.lastRead());									//アクセス
	fileInfo.stLastAccessTime.ulHighDateTime = atoi(clTimeRead.toString("yyyy").toLocal8Bit().constData());
	fileInfo.stLastAccessTime.ulLowDateTime = atoi(clTimeRead.toString("MMddhhmmss").toLocal8Bit().constData());
	QDateTime clTimeWrite(qFileInfo.lastModified());								//最終更新日
	fileInfo.stLastWriteTime.ulHighDateTime = atoi(clTimeWrite.toString("yyyy").toLocal8Bit().constData());
	fileInfo.stLastWriteTime.ulLowDateTime = atoi(clTimeWrite.toString("MMddhhmmss").toLocal8Bit().constData());
	qint64 lFileSize = qFileInfo.size();
	fileInfo.ulFileSizeHigh = lFileSize >> 32;										//上位32bit
	fileInfo.ulFileSizeLow = (lFileSize << 32)>>32;									//下位32bit
	QByteArray qImgInfoArray;
	QDataStream qImgInfoWork(&qImgInfoArray,QIODevice::ReadWrite);
	qImgInfoWork << (qint32)0x0000 << (qint32)0x0000						//Param0,Param1の領域分空ける			4byte, 4byte
				 << (qint32)fileInfo.ulFileAttributes						//file attribute		ファイル属性	4byte
				 << (qint32)fileInfo.stCreationTime.ulHighDateTime			//create time high		作成日			8byte
				 << (qint32)fileInfo.stCreationTime.ulLowDateTime			//create time low
				 << (qint32)fileInfo.stLastAccessTime.ulHighDateTime		//last access time high	最終アクセス	8byte
				 << (qint32)fileInfo.stLastAccessTime.ulLowDateTime			//last access time low
				 << (qint32)fileInfo.stLastWriteTime.ulHighDateTime			//last write time high	最終更新日		8byte
				 << (qint32)fileInfo.stLastWriteTime.ulLowDateTime			//last write time low
				 << (qint32)fileInfo.ulFileSizeHigh							//file size high		ファイルサイズ	8byte
				 << (qint32)fileInfo.ulFileSizeLow;							//file size low

	//ファイル名関連情報
	QString clStrFileName = QString("%1").arg(qFileInfo.fileName());
	qint32 lFileNameLength = clStrFileName.length();
	//ショートファイル名 16byte(ファイル名8byte+"."+拡張子3byteよりも大きいサイズのため) ASCII Code
	QString clStrFileNameShort	= qFileInfo.fileName().toUpper();
	QString clStrFileNameExt	= qFileInfo.fileName().toUpper();
	clStrFileNameShort.remove(lFileNameLength-4,4);
	if(clStrFileNameShort.length() >= 8){
		clStrFileNameShort.replace(7, clStrFileNameShort.length() - 7, '~');
	}
	clStrFileNameExt.remove(0, lFileNameLength-3);
	clStrFileNameShort.append(".");
	clStrFileNameShort.append(clStrFileNameExt);
	qImgInfoWork << clStrFileNameShort.toLatin1().constData()
				 << (qint32)0x00;

	//ファイル名のサイズ 4byte
	qImgInfoWork << (qint32)lFileNameLength;

	//ファイル名(256byte) 文字列なので一文字ずつ変換していれる UNI Code
	for(qint32 i = 0; i < 256; i++)
	{
		if(i < lFileNameLength)
		{
			qImgInfoWork << (qint8)clStrFileName.toUtf8().at(i);			//file name
		}
		else
		{
			qImgInfoWork << (qint8)0x00;									//0
		}
	}

	//デバイスタイプを数字に変換
	QString clStrDevType;
	clStrDevType = qParam.toStringList().at(1);
	qint32 lDevType = atoi(clStrDevType.toLocal8Bit().constData());

	//画像情報送信＋SessionID取得
	QDataStream qCmdStream(this);								//create data stream for getPicture
	ClUpImgFileCmdV2 clSendInfo;								//this variable is for reception
	ClUpImgFileCmdV2 clReciveInfo;								//this variable is for reception
	clSendInfo.setParam(qImgInfoArray);							//画像情報を一度記入（Param0,Param1の領域は空）
	clSendInfo.setParam0(lDevType);								//渡されたデバイスタイプをセット
	clSendInfo.setParam1((qint32)lFileSize);					//ファイルサイズをセット
	qCmdStream << clSendInfo;									//send command to device
	qCmdStream >> clReciveInfo;									//receeive command from device
	if(qCmdStream.status() != QDataStream::Ok){					//if confirm whether QDataStream status is OK
		return IM_ERROR_COMMAND_FAILURE;
	}

	m_lSessionID = clReciveInfo.getParam0();					//画像アップロードに必要なSessionIDを設定
	m_clStrFilePath	= qParam.toStringList().at(0);				//ファイルパス格納
	m_lUpFileSize	= lFileSize;								//ファイルサイズ格納
	m_lUploadCmd	= m_lCmdID;									//コマンド格納

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief		受け取ったファイルパスからサイズを取得・送信、SessinID取得
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::sendFwFileInfoCmdV2(QVariant &qParam)	//ファームウェア情報
{
	//データが渡されているか確認
	if(qParam.toStringList().isEmpty() || qParam.toStringList().count() != 2)
	{
		return IM_ERROR_COMMAND_FAILURE;
	}
	m_lUploadCmd = NULL;										//コマンド確認用変数を初期化
	//ファイルサイズ取得
	qint64 lFileSize = 0;
	QFile qFile(qParam.toStringList().at(0));
	if(!qFile.open(QFile::ReadOnly)){
		return IM_ERROR_COMMAND_FAILURE;
	}
	lFileSize = qFile.size();
	qFile.close();

	//強制書き込みフラグ
	QString clStrWriteFlg(qParam.toStringList().at(1));
	qint32 lWriteFlg = clStrWriteFlg.toInt();

	//ファームウェア情報送信＋SessionID取得
	QDataStream qCmdStream(this);								//create data stream for getPicture
	ClUpFwFileCmdV2 clSendInfo;									//this variable is for reception
	ClUpFwFileCmdV2 clReciveInfo;								//this variable is for reception
	clSendInfo.setParam0((qint32)lFileSize);					//ファイルサイズをセット
	clSendInfo.setParam1((qint32)lWriteFlg);					//書き込みフラグをセット
	qCmdStream << clSendInfo;									//send command to device
	qCmdStream >> clReciveInfo;									//receeive command from device
	if(qCmdStream.status() != QDataStream::Ok){					//if confirm whether QDataStream status is OK
		return IM_ERROR_COMMAND_FAILURE;
	}
	m_lSessionID	= clReciveInfo.getParam0();					//画像アップロードに必要なSessionIDを設定
	m_clStrFilePath	= qParam.toStringList().at(0);				//ファイルパス格納
	m_lUpFileSize	= lFileSize;								//ファイルサイズ格納
	m_lUploadCmd	= m_lCmdID;									//コマンド格納

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief		ファイルデータアップロード
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::uploadFileCmdV2(QVariant &qParam)		//ファイルアップロード
{
	if(qParam.type() != QVariant::ByteArray)
	{
		return false;
	}
	QByteArray qArrayTmp = qParam.toByteArray();
	if(qArrayTmp.length() != 0)
	{
		QDataStream qCmdStream(this);								//create data stream for getPicture for req
		char cID1	= m_lSessionID>>24;							//SessionIDをParamの中にはめ込む準備
		char cID2	= m_lSessionID>>16;
		char cID3	= m_lSessionID>> 8;
		char cID4	= m_lSessionID;
		char cID[]	= {cID4, cID3, cID2, cID1};
		QByteArray qArrayID(cID, 4);							//Session ID
		QByteArray qArrayDummy(20, NULL);						//Dummy

		QByteArray qArrayData;
		qArrayData.clear();										//一応クリアする
		qArrayData.append(qArrayID);							//Session IDをQByteArrayにセット
		qArrayData.append(qArrayDummy);							//
		qArrayData.append(qArrayTmp);							//画像データをQByteArrayにセット

		ClUpFwFileCmdV2 clSend;								//this variable is for reception
		ClUpFwFileCmdV2 clRecive;							//this variable is for reception
		clSend.setDataSize(qArrayData.length());
		clSend.setParam(qArrayData);
		clSend.setSessionBit();								//HeaderVer : 0b0000****, SessionBit : 0b000*0000, SessionBit : 0b00*00000

		if((m_lUpFileSize - m_lWroteSize) - qArrayTmp.length() <= 0)
		{
			clSend.setFinishBit();
		}else{
			clSend.clrFinishBit();
		}
		qCmdStream << clSend;								//送信へ...
		qCmdStream >> clRecive;								//受信へ...
		if(qCmdStream.status() != QDataStream::Ok){			//if confirm whether QDataStream status is OK
			emit sigFwUpdateProgress(0,-1);					//シグナル送信
			return IM_ERROR_COMMAND_FAILURE;
		}

		//進捗のパーセンテージとステータスを格納
		m_lWroteSize = m_lWroteSize + qArrayTmp.length();
		m_lProgressPer = (qint32)((m_lWroteSize*100)/m_lUpFileSize);
		m_lStatus = 1;
		if((m_lUpFileSize - m_lWroteSize) - qArrayTmp.length() <= 0)
		{
			m_lStatus = 0;
		}

		emit sigFwUpdateProgress(m_lProgressPer,m_lStatus);		//シグナル送信
	}
	else
	{
		QDataStream qCmdStreamWait(this);						//
		ClUpFwFileCmdV2 clRecive;							//this variable is for reception
		qCmdStreamWait >> clRecive;							//受信
		if(qCmdStreamWait.status() != QDataStream::Ok){		//if confirm whether QDataStream status is OK
			emit sigFwUpdateProgress(0,-1);					//シグナル送信
			return IM_ERROR_COMMAND_FAILURE;
		}

		//進捗のパーセンテージとステータスを格納
		unsigned short lTotalSize = clRecive.getTotalSize();
		unsigned short lWroteSize = clRecive.getWroteSize();
		m_lProgressPer = 0;
		if(lWroteSize != 0 && lTotalSize != 0){
			m_lProgressPer = (qint32)((lWroteSize*100)/lTotalSize);
		}
		m_lStatus = clRecive.getStatus();

		emit sigFwUpdateProgress(m_lProgressPer,m_lStatus);		//シグナル送信

		if(m_lStatus < IM_ERROR_SUCCESS){						//失敗
			return IM_ERROR_COMMAND_FAILURE;
		}else if(m_lStatus == IM_ERROR_SUCCESS){				//完了
			m_lWroteSize	= 0;
			m_lProgressPer	= 0;
			m_lStatus		= 1;
			return 1;
		}
	}

	m_lUploadCmd = NULL;											//コマンド確認用変数を初期化

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief		機種名取得
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::getModelNameV2(QVariant &qParam)
{
	QDataStream qCmdStream(this);								//create data stream for getPicture

	ClModelNameCmdV2 clRecive;									//this variable is for reception
	ClModelNameCmdV2 clCmd;

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
	@brief		バージョン取得
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::getVersionV2(QVariant &qParam)
{
	QStringList strVerList;
	qint32 lRes = IM_ERROR_SUCCESS;

	//SH
	lRes = getSHVersionV2(qParam);
	if(lRes != IM_ERROR_SUCCESS){
		return lRes;
	}else{
		strVerList << qParam.toString();
	}
	//Lens
	lRes = getLensVersionV2(qParam);
	if(lRes != IM_ERROR_SUCCESS){
		return lRes;
	}else{
		strVerList << qParam.toString();
	}
	//FPGA
	lRes = getFPGAVersionV2(qParam);
	if(lRes != IM_ERROR_SUCCESS){
		return lRes;
	}else{
		strVerList << qParam.toString();
	}
	qParam.clear();
	qParam = strVerList;

	return IM_ERROR_SUCCESS;									//output version to QVariant
}
//--------------------------------------------------------------------------------
/*!
	@brief		SHバージョン取得
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::getSHVersionV2(QVariant &qParam)
{
	QDataStream qCmdStream(this);								//create data stream for getPicture

	ClSHVerCmdV2 clRecive;										//this variable is for reception
	ClSHVerCmdV2 clCmd;

	qCmdStream << clCmd;										//send command to device

	qCmdStream >> clRecive;										//receeive command from device

	if(qCmdStream.status() != QDataStream::Ok){					//if confirm whether QDataStream status is OK
		return IM_ERROR_COMMAND_FAILURE;
	}
	QString strVersion;
	strVersion = "Camera Head ";
	strVersion += clRecive.getVersion();						//output model name to QVariant
	qParam = strVersion;

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief		レンズバージョン取得
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::getLensVersionV2(QVariant &qParam)
{
	QDataStream qCmdStream(this);								//create data stream for getPicture

	ClLensVerCmdV2 clRecive;										//this variable is for reception
	ClLensVerCmdV2 clCmd;

	qCmdStream << clCmd;										//send command to device

	qCmdStream >> clRecive;										//receeive command from device

	if(qCmdStream.status() != QDataStream::Ok){					//if confirm whether QDataStream status is OK
		return IM_ERROR_COMMAND_FAILURE;
	}
	QString strVersion;
	strVersion = "Camera Main ";
	strVersion += clRecive.getVersion();						//output model name to QVariant
	qParam = strVersion;

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief		FPGAバージョン取得
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::getFPGAVersionV2(QVariant &qParam)
{
	QDataStream qCmdStream(this);								//create data stream for getPicture

	ClFPGAVerCmdV2 clRecive;										//this variable is for reception
	ClFPGAVerCmdV2 clCmd;

	qCmdStream << clCmd;										//send command to device

	qCmdStream >> clRecive;										//receeive command from device

	if(qCmdStream.status() != QDataStream::Ok){					//if confirm whether QDataStream status is OK
		return IM_ERROR_COMMAND_FAILURE;
	}
	QString strVersion;
	strVersion = "Camera Engine ";
	strVersion += clRecive.getVersion();						//output model name to QVariant
	qParam = strVersion;

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief		機種番号取得
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::getProductCodeV2(QVariant &qParam)
{
	QDataStream qCmdStream(this);								//create data stream for getPicture

	ClProductCodeCmdV2 clRecive;									//this variable is for reception
	ClProductCodeCmdV2 clCmd;

	qCmdStream << clCmd;										//send command to device

	qCmdStream >> clRecive;										//receeive command from device

	if(qCmdStream.status() != QDataStream::Ok){					//if confirm whether QDataStream status is OK
		return IM_ERROR_COMMAND_FAILURE;
	}
	qParam = clRecive.getProductCode();							//output product id to QVariant

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief		モード取得
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::getCurrentModeV2(QVariant &qParam)
{
	QDataStream qCmdStream(this);								//create data stream for getPicture

	ClCurrentModeCmdV2 clRecive;								//this variable is for reception
	ClCurrentModeCmdV2 clCmd;

	qCmdStream << clCmd;										//send command to device

	qCmdStream >> clRecive;										//receeive command from device

	if(qCmdStream.status() != QDataStream::Ok){					//if confirm whether QDataStream status is OK
		return IM_ERROR_COMMAND_FAILURE;
	}
	qParam = clRecive.getCurrentMode();							//output current mode to QVariant

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief		時刻設定
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::setSystemTimeV2(QVariant &qParam)
{
	//計算に使用する値の準備
	QTime qTime;
	qTime = qParam.toDateTime().time();
	QDate qDate;
	qDate = qParam.toDateTime().date();

	//2000/1/1 00:00:00 からの秒数を計算
	qint32 lSysTime = 0;
	lSysTime += qTime.second();									//秒を追加
	lSysTime += qTime.minute()	* 60;							//分を秒に直して追加
	lSysTime += qTime.hour()	* 60 * 60;						//時を秒に直して追加
	lSysTime += (qDate.day()-1)	* 60 * 60 * 24;					//日を秒に直して追加
	if(isLeapYear((unsigned long)qDate.year()) == 0){			//指定された年から閏年か判定
		qint32 lCurrentMonth = qDate.month();					//指定された月より前の月の日数分の秒数を追加
		for(qint32 lMonthCnt = 1; lMonthCnt < lCurrentMonth; lMonthCnt++){
			lSysTime += gpiLeapDays[lMonthCnt] * 60 * 60 * 24;
		}
	}else{														//閏年でない場合
		qint32 lCurrentMonth = qDate.month();					//指定された月より前の月の日数分の秒数を追加
		for(qint32 lMonthCnt = 1; lMonthCnt < lCurrentMonth; lMonthCnt++){
			lSysTime += gpiNormalDays[lMonthCnt] * 60 * 60 * 24;
		}
	}
	for(qint32 lYearCnt = 2000; lYearCnt < qDate.year(); lYearCnt++){
		if(isLeapYear((unsigned long)lYearCnt) == 0){
			for(qint32 lMonthCnt = 1; lMonthCnt <= 12; lMonthCnt++){
				lSysTime += gpiLeapDays[lMonthCnt] * 60 * 60 * 24;
			}
		}else{
			for(qint32 lMonthCnt = 1; lMonthCnt <= 12; lMonthCnt++){
				lSysTime += gpiNormalDays[lMonthCnt] * 60 * 60 * 24;
			}
		}
	}

	//設定コマンド送信
	QDataStream qCmdStream(this);								//create data stream for getPicture

	ClSysTimeCmdV2 clRecive;									//this variable is for reception
	ClSysTimeCmdV2 clCmd;
	clCmd.setParam0(lSysTime);

	qCmdStream << clCmd;										//send command to device

	qCmdStream >> clRecive;										//receeive command from device

	if(qCmdStream.status() != QDataStream::Ok){					//if confirm whether QDataStream status is OK
		return IM_ERROR_COMMAND_FAILURE;
	}
	qParam = clRecive.getCommand();								//output current mode to QVariant

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief		閏年か否かを返す
	@return		NONE
 */
//--------------------------------------------------------------------------------
long ClPresenterL12::isLeapYear(unsigned long ulYear)
{
	long	lReturn = -1;

	if( ( ulYear % 4 ) == 0 )
	{
		if( ( ulYear % 100 ) == 0 )
		{
			if( ( ulYear % 400 ) == 0 )
			{
				lReturn = 0;
			}
		}
		else
		{
			lReturn = 0;
		}
	}

	return lReturn;
}
//--------------------------------------------------------------------------------
/*!
	@brief		機種名設定
*/
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::setModelNameV2(QVariant &qParam)
{
	if(qParam.type() != QVariant::ByteArray)
	{
		return IM_ERROR_COMMAND_FAILURE;
	}

	QDataStream clCmdStream(this);

	ClModelNameSetCmdV2 clCmd;
	ClModelNameSetCmdV2 clReceive;
	QByteArray clData = qParam.toByteArray();
	clCmd.setParam(clData);

	clCmdStream << clCmd;		//書き込み
	clCmdStream >> clReceive;	//読み込み

	if(clCmdStream.status() != QDataStream::Ok)
	{
		return IM_ERROR_COMMAND_FAILURE;
	}
	qParam = clReceive.getParam0();
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief		機種コード設定
*/
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::setProductCodeV2(QVariant &qParam)
{
	if(qParam.type() != QVariant::ByteArray)
	{
		return IM_ERROR_COMMAND_FAILURE;
	}

	QDataStream clCmdStream(this);

	ClProductCodeSetCmdV2 clCmd;
	ClProductCodeSetCmdV2 clReceive;
	QByteArray clData = qParam.toByteArray();
	clCmd.setParam(clData);

	clCmdStream << clCmd;		//書き込み
	clCmdStream >> clReceive;	//読み込み

	if(clCmdStream.status() != QDataStream::Ok)
	{
		return IM_ERROR_COMMAND_FAILURE;
	}
	qParam = clReceive.getParam0();
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief		外部入力コマンド V1
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::externalInputCmd(QVariant &qParam)
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
	@brief		外部入力コマンド V2
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::externalInputCmdV2(QVariant &qParam)
{
	QDataStream qCmdStream(this);								//create data stream for getPicture

	ClUsbCmdV2 clRecive;										//this variable is for reception
	ClUsbCmdV2 clCmd;

	PELMO_PACKET_V2		sReturn;
	PELMO_COMMAND_V2	pRetPacket;
	sReturn = (PELMO_PACKET_V2)qParam.toByteArray().constData();
	pRetPacket = (PELMO_COMMAND_V2)sReturn->Data;
	clCmd.setPacketType(sReturn->Header.HeaderVer);
	clCmd.setTransId(sReturn->Header.TransID);
	clCmd.setCommand(sReturn->Header.OpCode);
	clCmd.setDataSize(sReturn->Header.DataSize);
	clCmd.setParam0(pRetPacket->Param0);
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
	qWork	<< qint8(clRecive.getHeaderVer())
			<< qint8(clRecive.getTransId())
			<< qint16(clRecive.getCommand())
			<< qint32(clRecive.getDataSize())
			<< qint32(clRecive.getParam0())
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
qint64	ClPresenterL12::readData ( char * data, qint64 maxSize )
{
	qint32 lReqLength = 0;
	qint32 lRet = 0;
	stREADDATACMD *pReadDataCmd = NULL;

	if(isOpen() == false){
		return -1;
	}
	if(maxSize > INT_MAX || maxSize <= 0){
		DBG("got error from readData");
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
		if(lRet <= 0 && qParam.type() == QVariant::ByteArray)
		{
			clUsb = ClUsbCmd(qParam.toByteArray().data(), qParam.toByteArray().length());
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
			memcpy(pReadDataCmd->pData, qCmdArray.data(), qCmdArray.length());
		}

	}else if(m_lCmdID == GET_PICTURE_V2){						//if code is Jpeg data V2
		if(pReadDataCmd->pByteArray == NULL){
			DBG("got error from readData");
			return -1;
		}

		QVariant qParam(lReqLength);
		lRet = resPictureV2(m_pInDataEndp, qParam);

		if(lRet > 0 && qParam.type() == QVariant::List){
			QVariantList qVarList = qParam.toList();
			//List 0 画像サイズを格納
			if(qVarList.at(0).type() == QVariant::Size){
				QSize qSize = qVarList.at(0).toSize();
				clUsb.setParam2(qSize.width());
				clUsb.setParam3(qSize.height());
			}
			//List 1 画像データをByteArrayに変換して格納
			if(qVarList.at(1).type() == QVariant::ByteArray){
				QByteArray *pqArray = reinterpret_cast<QByteArray*>(pReadDataCmd->pByteArray);
				*pqArray = qVarList.at(1).toByteArray();
			}
		}
		if(lRet >= 0){											//
			QByteArray qCmdArray;
			QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
			qWork.setByteOrder(QDataStream::LittleEndian);		//
			qWork << (qint8)clUsb.getPagetType()				//set PacketType
				<< (qint8)clUsb.getTransId()					//set Trans ID
				<< (qint16)clUsb.getCode()						//set Command
				<< (qint32)clUsb.getDataSize()					//set DataSize
				<< clUsb.getParam1()
				<< clUsb.getParam2()
				<< clUsb.getParam3()
				<< clUsb.getParam4()
				<< clUsb.getParam5();
			memcpy(pReadDataCmd->pData, qCmdArray.data(), qCmdArray.length());
		}

	}else if(m_lCmdID == GET_STREAM_V2){						//if code is Jpeg data stream
		if(pReadDataCmd->pByteArray == NULL){
			DBG("got error from readData");
			return -1;
		}

		QVariant qParam(lReqLength);
		lRet = resStreamV2(m_pInDataEndp, qParam);										//ストリーム受信
		if(lRet >= IM_ERROR_SUCCESS && qParam.type() == QVariant::List)					//取得成功、ほしい形式かを確認
		{
			QVariantList qVarList = qParam.toList();
			//List 0 データサイズを格納
			if(qVarList.at(0).type() == QVariant::Size){
				QSize qSize = qVarList.at(0).toSize();
				clUsb.setParam2(qSize.width());
				clUsb.setParam3(qSize.height());
			}
			//List 1 画像・音声データをByteArrayに変換して格納
			if(qVarList.at(1).toByteArray().length() == 32){							//他スレッドのストリーム開始時に実行されるGET_RECT_PICTUREの返信用
				qDebug("not stream data : ather thread rect id");
			}else{
				//データの格納
				if(qVarList.at(1).type() == QVariant::ByteArray){
					QByteArray *pqArray = reinterpret_cast<QByteArray*>(pReadDataCmd->pByteArray);
					*pqArray = qVarList.at(1).toByteArray();
				}
			}
		}
		if(lRet >= 0){											//
			QByteArray qCmdArray;
			QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
			qWork.setByteOrder(QDataStream::LittleEndian);		//
			qWork << (qint8)clUsb.getPagetType()				//set PacketType
				<< (qint8)clUsb.getTransId()					//set Trans ID
				<< (qint16)clUsb.getCode()						//set Command
				<< (qint32)clUsb.getDataSize()					//set DataSize
				<< clUsb.getParam1()
				<< clUsb.getParam2()
				<< clUsb.getParam3()
				<< clUsb.getParam4()
				<< clUsb.getParam5();
			memcpy(pReadDataCmd->pData, qCmdArray.data(), qCmdArray.length());
		}

	}else if(m_lCmdID == STOP_STREAM_V2){						//if code is stop Jpeg data stream

		/*
			残りのストリームの受信＋終了コマンドの受信
		*/
		QVariant qParam(lReqLength);
		lRet = resStreamV2(m_pInDataEndp, qParam);

		if(lRet == 32)										//終了コマンドの返信の場合
		{
			qint64 llWork = lRet;
			if(pReadDataCmd->llMaxSize < lRet){
				llWork = pReadDataCmd->llMaxSize;
			}
			if(qParam.type() == QVariant::ByteArray){
				QByteArray qCmdArray = qParam.toByteArray();
				memcpy(pReadDataCmd->pData, qCmdArray.data(), llWork);		//copy
				pReadDataCmd->llMaxSize = lRet;
			}
		}
		else												//残りストリーム受信の場合
		{
			if(lRet > 0 && qParam.type() == QVariant::List){
				QVariantList qVarList = qParam.toList();
				//List 0 画像サイズを格納
				if(qVarList.at(0).type() == QVariant::Size){
					QSize qSize = qVarList.at(0).toSize();
					clUsb.setParam2(qSize.width());
					clUsb.setParam3(qSize.height());
				}
				//List 1 画像データをByteArrayに変換して格納
				// 受け取ったParam5に入っている画像格納用変数m_qJpegArrayのアドレスを入れて、m_qJpegArrayへ画像データを格納する
				if(qVarList.at(1).type() == QVariant::ByteArray){
					QByteArray *pqArray = reinterpret_cast<QByteArray*>(pReadDataCmd->pByteArray);
					*pqArray = qVarList.at(1).toByteArray();
				}
			}
			if(lRet >= 0){											//
				QByteArray qCmdArray;
				QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
				qWork.setByteOrder(QDataStream::LittleEndian);		//
				qWork << (qint8)clUsb.getPagetType()				//set PacketType
					<< (qint8)clUsb.getTransId()					//set Trans ID
					<< (qint16)clUsb.getCode()						//set Command
					<< (qint32)clUsb.getDataSize()					//set DataSize
					<< clUsb.getParam1()
					<< clUsb.getParam2()
					<< clUsb.getParam3()
					<< clUsb.getParam4()
					<< clUsb.getParam5();
				memcpy(pReadDataCmd->pData, qCmdArray.data(), qCmdArray.length());
			}
		}
	}else if(m_lCmdID == SEND_UPIM_INFO_V2 || (m_lCmdID == UPLOAD_FILE_V2 && m_lUploadCmd == SEND_UPIM_INFO_V2)){
		QVariant qParam(lReqLength);
		lRet = resCommand(m_pInDataEndp, qParam);
		if(lRet > 0){
			qint64 llWork = lRet;
			if(pReadDataCmd->llMaxSize < lRet){
				llWork = pReadDataCmd->llMaxSize;
			}
			if(qParam.type() == QVariant::ByteArray){
				QByteArray qCmdArray = qParam.toByteArray();
				memcpy(pReadDataCmd->pData, qCmdArray.data(), llWork);			//copy
				pReadDataCmd->llMaxSize = lRet;
			}
		}
	}else if(m_lCmdID == SEND_UPFW_INFO_V2 || (m_lCmdID == UPLOAD_FILE_V2 && m_lUploadCmd == SEND_UPFW_INFO_V2)){
		QVariant qParam(lReqLength);
		lRet = resCommand(m_pInCmdEndp, qParam);
		if(lRet > 0){
			qint64 llWork = lRet;
			if(pReadDataCmd->llMaxSize < lRet){
				llWork = pReadDataCmd->llMaxSize;
			}
			if(qParam.type() == QVariant::ByteArray){
				QByteArray qCmdArray = qParam.toByteArray();
				memcpy(pReadDataCmd->pData, qCmdArray.data(), llWork);			//copy
				pReadDataCmd->llMaxSize = lRet;
			}
		}
	}else{
		QVariant qParam(lReqLength);
		lRet = resCommand(m_pInCmdEndp, qParam);
		if(lRet > 0){
			qint64 llWork = lRet;
			if(pReadDataCmd->llMaxSize < lRet){
				llWork = pReadDataCmd->llMaxSize;
			}
			if(qParam.type() == QVariant::ByteArray){
				QByteArray qCmdArray = qParam.toByteArray();
				memcpy(pReadDataCmd->pData, qCmdArray.data(), llWork);			//copy
				pReadDataCmd->llMaxSize = lRet;
#if 0  // for USB packet moniter in qDebug
				QByteArray qDbgArray(pReadDataCmd->pData,llWork);
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
qint64	ClPresenterL12::writeData ( const char * data, qint64 maxSize )
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
	}else if(m_lCmdID == GET_PICTURE_V2){						//if code is Jpeg data V2
		lRet = reqPictureV2(m_pOutDataEndp, qParam);
	}else if(m_lCmdID == START_STREAM_V2){						//if code is start Jpeg data stream
		lRet = reqStreamV2(m_pOutDataEndp, qParam);
	}else if(m_lCmdID == STOP_STREAM_V2){						//if code is stop Jpeg data stream
		lRet = reqCommand(m_pOutDataEndp, qParam);
	}else if(m_lCmdID == SEND_UPIM_INFO_V2 ||
			(m_lCmdID == UPLOAD_FILE_V2 &&
			 m_lUploadCmd == SEND_UPIM_INFO_V2)){				//if code is upload image
		lRet = reqCommand(m_pOutDataEndp, qParam);
	}else if(m_lCmdID == SEND_UPFW_INFO_V2 ||
			(m_lCmdID == UPLOAD_FILE_V2 &&
			 m_lUploadCmd == SEND_UPFW_INFO_V2)){				//if code is upload fw
		lRet = reqCommand(m_pOutCmdEndp, qParam);
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
qint32 ClPresenterL12::reqPicture(eENDPOINT	*pOutEndp, QVariant &param)
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
qint32 ClPresenterL12::resPicture(eENDPOINT	*pInEndp, QVariant &qParam)
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
qint32 ClPresenterL12::reqCommand(eENDPOINT	*pOutEndp, QVariant &param)
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
qint32 ClPresenterL12::resCommand(eENDPOINT	*pInEndp, QVariant &param)
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
bool ClPresenterL12::analyJpegHeader(const QByteArray qJpgData, QSize &qSize)
{
	typedef union{
		struct{
			quint8	ucH;
			quint8	ucL;
		};
		quint16	usPoint;
	}uniPOINT;
	// サイズ0のワーニングがでるのでブロック
	if(0 == qJpgData.size() || qJpgData.at(0) != CHECK_CHAR_FF || qJpgData.at(1) != CHECK_CHAR_D8){
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
qint32 ClPresenterL12::execCommand(ClUsbCmd &clCmd)
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
/* **************************************************************************** +/

									V2 Command

/+ **************************************************************************** */
//--------------------------------------------------------------------------------
/*!
	@fn			reqPictureV2
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::reqPictureV2(eENDPOINT	*pOutEndp, QVariant &param)
{
	qint32 lRet = 0;

	Q_ASSERT(pOutEndp);
	lRet = reqCommand(pOutEndp, param);
	return lRet;
}
//--------------------------------------------------------------------------------
/*!
	@fn			resPictureV2
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::resPictureV2(eENDPOINT	*pInEndp, QVariant &qParam)
{
	qint32 lActual_length = 0;
	qint32 lRet = 0;
	qint32 lJpegLength = 0;
	char pcBuffer[ELMO_MAXPACKETSIZE_V2];
	QByteArray qJpegData;
	QSize qSize(0,0);													//コマンド仕様上サイズは入らないため(0,0)を格納
	Q_ASSERT(pInEndp);

	lRet = resCommand(pInEndp, qParam);									//画像情報を受信
	if(lRet < 0){
		return -1;
	}
	//変数の宣言
	qint32 lActual_count	= 0;										//読込回数
	qint32 lreqLength		= 0;										//読みいくサイズ
	qint32 lBlockSize		= 0;										//パケットのサイズ
	qint32 lRestLength		= 0;										//画像の残り読込サイズ
	qint32 siHeader			= sizeof(ELMO_PACKET_HEADER_V2);			//Header部分のサイズ
	PELMO_PACKET_HEADER_V2	pstHeader;									//ヘッダー抽出用
	PELMO_PACKET_V2			sReturn;
	PELMO_COMMAND_V2		pRetPacket;

	//SessionID等をセット
	if(qParam.type() == QVariant::ByteArray)
	{
		//取得した情報を送信時の形に直す
		sReturn		= (PELMO_PACKET_V2)qParam.toByteArray().constData();
		pRetPacket	= (PELMO_COMMAND_V2)sReturn->Data;
		m_lSessionID	= pRetPacket->Param0;							//SessionID 格納
		lJpegLength		= pRetPacket->Param1;							//Jpgの画像サイズ
		lRestLength		= lJpegLength;									//Jpgの画像サイズ

		//1枚分になるまで要求＋取得を繰り返す
		while(0 < lRestLength){

			//要求 -----------------------------------------------------
			ClPictCmdV2 clPict(m_iPictureRect);							//this variable is for send
			clPict.setSessionBit();										//SessinBit set
			clPict.clrFinishBit();										//Finish Bit clear
			clPict.setParam0(m_lSessionID);								//set sessin id
			clPict.setParam1(0);										//set param 1
			clPict.setParam2(0);										//set param 2
			clPict.setParam3(0);										//set param 3
			clPict.setParam4(0);										//set param 4
			clPict.setParam5(0);										//set param 5
			QByteArray qArrayReq;
			QDataStream qWork(&qArrayReq,QIODevice::ReadWrite);
			qWork.setByteOrder(QDataStream::LittleEndian);	//
			qWork
				<< clPict.getHeaderVer()								//set PacketType
				<< clPict.getTransId()									//set Trans ID
				<< clPict.getCommand()									//set code
				<< clPict.getDataSize()									//set DataSize
				<< clPict.getParam0()									//set Param0
				<< clPict.getParam1()									//set Param1
				<< clPict.getParam2()									//set Param2
				<< clPict.getParam3()									//set Param3
				<< clPict.getParam4()									//set Param4
				<< clPict.getParam5();									//set Param5
			QVariant qParamReq(qArrayReq);
			lRet = reqCommand(m_pOutDataEndp, qParamReq);				//画像要求
			if(lRet < 0){
				return -1;
			}

			//取得 -----------------------------------------------------
			DBGFL("RestLength : %d",lRestLength);
			//サイズを計算
			if(lRestLength > ELMO_MAXPACKETSIZE_V2 - siHeader - ELMO_CMD_SESSION_SIZE)			//画像サイズが読込MAXより大きい場合
			{
				lreqLength	= ELMO_MAXPACKETSIZE_V2;
			}
			else
			{
				lreqLength	= lRestLength + siHeader + ELMO_CMD_SESSION_SIZE;
			}
			//Headerを含む1回目のデータを読み込み
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
			//パケットのData部分のサイズを取得
			pstHeader = reinterpret_cast<PELMO_PACKET_HEADER_V2>(pcBuffer);
			lBlockSize = pstHeader->DataSize;
			//データサイズが0なら抜ける
			if(lBlockSize <= 0){
				DBGFL("got unjust block size : %d",lBlockSize);
				return IM_ERROR_OTHER;
			}
			DBGFL("lBlockSize : %d",lBlockSize);
			//受信したOpCodeが0でないければ失敗なので抜ける
			if(pstHeader->OpCode != IM_ERROR_SUCCESS){
				DBGFL("got OpCode : %d",pstHeader->OpCode);
				return IM_ERROR_OTHER;
			}

			//画像サイズから1回目に受信したデータ内の画像サイズを引き、残りの画像サイズを計算
			lRestLength -= (lBlockSize - ELMO_CMD_SESSION_SIZE);
			//パケット内の残りの画像サイズを取得
			lBlockSize -= (lActual_length - siHeader);
			//返信用変数へコピー
			qJpegData.append(pcBuffer + siHeader + ELMO_CMD_SESSION_SIZE, lActual_length - siHeader - ELMO_CMD_SESSION_SIZE);
			//残りのパケットをサイズになるまで読み、返信用変数へ格納
			while(0 < lBlockSize){
				if(lBlockSize > ELMO_MAXPACKETSIZE_V2)									//画像サイズが読込MAXより大きい場合
				{
					lreqLength	= ELMO_MAXPACKETSIZE_V2;
				}
				else
				{
					lreqLength	= lBlockSize;
				}
				lActual_count = 0;
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
			//Finish Bitがたった場合にループを抜ける
			clPict.setPacketType(pstHeader->HeaderVer);									//Finish Bitでの判定に使用するためここで格納
			if(clPict.getFinishBit() == 1)
			{
				break;
			}
			//----------------------------------------------------------
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
	@fn			reqStreamV2
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::reqStreamV2(eENDPOINT	*pOutEndp, QVariant &param)
{
	qint32 lRet = 0;

	Q_ASSERT(pOutEndp);
	lRet = reqCommand(pOutEndp, param);
	return lRet;
}
//--------------------------------------------------------------------------------
/*!
	@fn			resStreamV2
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::resStreamV2(eENDPOINT	*pInEndp, QVariant &qParam)
{
	//ストリームは64 byteづつ来るためELMO_MAXPACKETSIZE_V2ではなく、ELMO_MAXPACKETSIZEを使用
	qint32 lActual_length = 0;
	qint32 lJpegLength = 0;
	char pcBuffer[ELMO_MAXPACKETSIZE];
	QByteArray qJpegData;
	QSize qSize(0,0);													//コマンド仕様上サイズは入らないため(0,0)を格納
	Q_ASSERT(pInEndp);

	//使用変数の宣言
	qint32 iReadMax			= ELMO_MAXPACKETSIZE;
	qint32 lreqLength		= iReadMax;
	qint32 lBlockSize		= 0;
	qint32 lActual_count	= 0;
	PELMO_PACKET_HEADER_V2 pstHeader;

	//Header込みのデータ受信 -------------------------------------------
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
	pstHeader = reinterpret_cast<PELMO_PACKET_HEADER_V2>(pcBuffer);
	lBlockSize = pstHeader->DataSize;
	if(lBlockSize <= 0){
		DBGFL("got unjust block size : %d",lBlockSize);
		return IM_ERROR_OTHER;
	}
	DBGFL("lBlockSize : %d",lBlockSize);
	if(pstHeader->OpCode != IM_ERROR_SUCCESS){
		DBGFL("got OpCode : %d",pstHeader->OpCode);
		return IM_ERROR_OTHER;
	}

	//ストリーム情報格納 -------------------------------------------------------
	//コマンドV2の形に変換
	PELMO_PACKET_V2		sReturn  = reinterpret_cast<PELMO_PACKET_V2>(pcBuffer);
	PELMO_COMMAND_V2	sCommand = reinterpret_cast<PELMO_COMMAND_V2>(sReturn->Data);
	m_lSessionID	= sCommand->Param0;											//SessionID 格納
	m_lFileType		= sCommand->Param1;											//ファイルタイプ（0:jpg,1:H.264,2:sound)
	m_lTimeStamp	= sCommand->Param2;											//タイムスタンプ
	qint32 siHeader = sizeof(ELMO_PACKET_HEADER_V2);							//Header部分のサイズ
	qint32 lOtherStreamSize = ELMO_STREAM_OTHER_IMG + siHeader;					//Header込みデータの中の画像データ以外のサイズ
	lJpegLength		= lBlockSize - ELMO_STREAM_OTHER_IMG;						//1回目受信データサイズからストリーム情報（SessionIDなど）を抜いたものが画像サイズ
	qSize.setWidth(m_lFileType);												//ファイルタイプ格納

	//ストリーム終了コマンドの返信かを確認 -------------------------------------
	ClStartStreamCmdV2 UsbStream(pcBuffer,lActual_length);
#if 0
	if(UsbStream.getFinishBit() == 1)											//FinishBitが立っている場合
#else
	if(lActual_length == 32)
#endif
	{
		QDataStream qWorkStream(&qJpegData, QIODevice::WriteOnly);
		qWorkStream << UsbStream.getHeaderVer()
					<< UsbStream.getTransId()
					<< UsbStream.getCommand()
					<< UsbStream.getDataSize()
					<< UsbStream.getParam0()
					<< UsbStream.getParam1()
					<< UsbStream.getParam2()
					<< UsbStream.getParam3()
					<< UsbStream.getParam4()
					<< UsbStream.getParam5();
		lJpegLength = lBlockSize + siHeader;
		lBlockSize = 0;
	}
	else																		//通常のストリーム受信の場合
	{
		lBlockSize -= (lActual_length - siHeader);
		qJpegData.append(pcBuffer + lOtherStreamSize,
						 lActual_length - lOtherStreamSize );
	}

	//パケットサイズになるまで取得を繰り返す -----------------------------------
	//  ストリームは画像1枚分を1パケットで送信する
	while(0 < lBlockSize){
		//読込に行くサイズを設定
		if(lBlockSize > ELMO_MAXPACKETSIZE)										//画像サイズが読込MAXより大きい場合
		{
			lreqLength	= ELMO_MAXPACKETSIZE;
		}
		else
		{
			lreqLength	= lBlockSize;
		}
		lActual_count = 0;
		//受信
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
		//返信用変数へ追記
		qJpegData.append(pcBuffer, lActual_length);
		//残りパケットサイズから今読んだサイズを引く
		lBlockSize -= lActual_length;
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
	@brief	V2コマンド実行
*/
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::execCommandV2(ClUsbCmdV2 &clCmd)
{
	QDataStream clCmdStream(this);
	ClUsbCmdV2 clReceive;

	clCmdStream << clCmd;		//書き込み
	clCmdStream >> clReceive;	//読み込み
	//受信が成功したかを確認(ReadPastEndは期待のデータサイズor型が異なるエラーの様だが中身は問題ないのでここでは通す（Qt5.10の問題か）)
	if(!(clCmdStream.status() == QDataStream::Ok || clCmdStream.status() == QDataStream::ReadPastEnd))
	{
		return IM_ERROR_COMMAND_FAILURE;	//コマンド失敗
	}
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief	V2コマンドでのパラメータ取得
*/
//--------------------------------------------------------------------------------
qint32 ClPresenterL12::execGetStatusCommand(ClUsbCmdV2 &clCmd, QVariant &clParam)
{
	QDataStream clCmdStream(this);
	ClUsbCmdV2 clReceive;

	clCmdStream << clCmd;		//書き込み
	clCmdStream >> clReceive;	//読み込み
	//受信が成功したかを確認(ReadPastEndは期待のデータサイズor型が異なるエラーの様だが中身は問題ないのでここでは通す（Qt5.10の問題か）)
	if(!(clCmdStream.status() == QDataStream::Ok || clCmdStream.status() == QDataStream::ReadPastEnd))
	{
		return IM_ERROR_COMMAND_FAILURE;	//コマンド失敗
	}
	clParam = clReceive.getParam0();		//param0のデータ格納
	return IM_ERROR_SUCCESS;
}
