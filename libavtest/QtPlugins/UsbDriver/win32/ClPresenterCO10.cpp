#define __USB_DEBUG

#include "ClUsbDriver/ClUsbDriver_global.h"
#include "Common/CmnDefine.h"
#include "ClUsbDriver/ClUsbDriver_win.h"
#include "win32/ClDeviceInfo_win.h"
#include "ClUsbDriver/Command_Presenter.h"
#include "win32/ClPresenterCO10.h"

#define ID_AF                   0x32

#define Index_Zoom_Start   0x00					//ZOOM START
#define Index_Zoom_Stop    0x01					//ZOOM STOP
#define Index_Focus_Start  0x02					//FUCUS START
#define Index_Focus_Stop   0x03					//FOCUS START

#define Value_Zoom_Wide    0x01					//ZOOM WIDE
#define Value_Zoom_Tele    0x00					//ZOOM TELE
#define Value_Zoom_Stop    0x01					//ZOOM STOP
#define Value_Focus_Near   0x00					//FOCUS NEAR
#define Value_Focus_Far    0x01					//FOCUS FAR
#define Value_Focus_Stop   0x00					//FOCUS STOP

typedef int (WINAPI *PresenterV2_SDK_DetectCaptureDeviceEx_FNC)(UCHAR *);
static PresenterV2_SDK_DetectCaptureDeviceEx_FNC PresenterV2_SDK_DetectCaptureDeviceEx;

typedef int (WINAPI *PresenterV2_OneTrigger_FNC)(DWORD Id);
static PresenterV2_OneTrigger_FNC PresenterV2_OneTrigger;

typedef int (WINAPI *PresenterV2_SDK_Set_FNC)(BYTE Index, DWORD Value);
static PresenterV2_SDK_Set_FNC PresenterV2_SDK_Set;

typedef int (WINAPI *PresenterV2_SDK_SetBrightness_FNC)(DWORD Value);
static PresenterV2_SDK_SetBrightness_FNC PresenterV2_SDK_SetBrightness;

typedef int (WINAPI *PresenterV2_SDK_GetBrightness_FNC)(DWORD *pValue);
static PresenterV2_SDK_GetBrightness_FNC PresenterV2_SDK_GetBrightness;

typedef int (WINAPI *PresenterV2_QueryFirmwareVersion_FNC)(BYTE *pFwVersion);
static PresenterV2_QueryFirmwareVersion_FNC PresenterV2_QueryFirmwareVersion;

//--------------------------------------------------------------------------------
/*!
	@fn			ClPresenterCO10
	@brief
 */
//--------------------------------------------------------------------------------
ClPresenterCO10::ClPresenterCO10(qint32 lProductId, QObject *parent) :
	ClDeviceInfo(lProductId, parent)
{
	m_pclGraphManager	= NULL;
	bufferCheckedOut	= FALSE;
	frame_timeout_ms	= 2000L;

	m_ucBuffer			= NULL;														//ワークバッファ
	m_ucJpgBuffer		= NULL;														//取得データ格納
}
//--------------------------------------------------------------------------------
/*!
	@fn			~ClPresenterCO10
	@brief
 */
//--------------------------------------------------------------------------------
ClPresenterCO10::~ClPresenterCO10()
{
}
//--------------------------------------------------------------------------------
/*!
	@fn			open
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterCO10::open()
{
	if(isOpen() == true){
		return IM_ERROR_SUCCESS;
	}

	if(m_pclGraphManager != NULL){													//すでにオープン済み
		return IM_ERROR_ALREADY_OPENED;
	}

	//==========================================================================
	// ＤＬＬロード
	//==========================================================================
	m_qLib.setFileName(NAME_LUMENS_DLL);
	m_qLib.load();
	//==========================================================================
	// PresenterV2_SDK_DetectCaptureDeviceEx
	//==========================================================================
	PresenterV2_SDK_DetectCaptureDeviceEx = (PresenterV2_SDK_DetectCaptureDeviceEx_FNC)m_qLib.resolve("PresenterV2_SDK_DetectCaptureDeviceEx");
	if (NULL == PresenterV2_SDK_DetectCaptureDeviceEx)
	{
		m_qLib.unload();
		return IM_ERROR_READ_FAILURE;
	}
	//==========================================================================
	// PresenterV2_OneTrigger
	//==========================================================================
	PresenterV2_OneTrigger = (PresenterV2_OneTrigger_FNC)m_qLib.resolve("PresenterV2_OneTrigger");
	if (NULL == PresenterV2_OneTrigger)
	{
		m_qLib.unload();
		return IM_ERROR_READ_FAILURE;
	}
	//==========================================================================
	// PresenterV2_SDK_Set
	//==========================================================================
	PresenterV2_SDK_Set = (PresenterV2_SDK_Set_FNC)m_qLib.resolve("PresenterV2_SDK_Set");
	if (NULL == PresenterV2_SDK_Set)
	{
		m_qLib.unload();
		return IM_ERROR_READ_FAILURE;
	}
	//==========================================================================
	// PresenterV2_SDK_SetBrightness
	//==========================================================================
	PresenterV2_SDK_SetBrightness = (PresenterV2_SDK_SetBrightness_FNC)m_qLib.resolve("PresenterV2_SDK_SetBrightness");
	if (NULL == PresenterV2_SDK_SetBrightness)
	{
		m_qLib.unload();
		return IM_ERROR_READ_FAILURE;
	}
	//==========================================================================
	// PresenterV2_SDK_GetBrightness
	//==========================================================================
	PresenterV2_SDK_GetBrightness = (PresenterV2_SDK_GetBrightness_FNC)m_qLib.resolve("PresenterV2_SDK_GetBrightness");
	if (NULL == PresenterV2_SDK_GetBrightness)
	{
		m_qLib.unload();
		return IM_ERROR_READ_FAILURE;
	}
	//==========================================================================
	// PresenterV2_QueryFirmwareVersion
	//==========================================================================
	PresenterV2_QueryFirmwareVersion = (PresenterV2_QueryFirmwareVersion_FNC)m_qLib.resolve("PresenterV2_QueryFirmwareVersion");
	if (NULL == PresenterV2_QueryFirmwareVersion)
	{
		m_qLib.unload();
		return IM_ERROR_READ_FAILURE;
	}

	unsigned char IsDeviceExist = 0;
	int ret =  PresenterV2_SDK_DetectCaptureDeviceEx(&IsDeviceExist);
	if(!(ret == 0 && IsDeviceExist == 1)){											//CO10接続の場合
		qDebug("Conect State : FALSE\n");
		m_qLib.unload();
		return IM_ERROR_READ_FAILURE;
	}																				//非接続

	/////////////////////////////////////////////////////////////

	char config_default[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><dsvl_input><camera show_format_dialog=\"false\" friendly_name=\"Visual Presenter V2\"><pixel_format><RGB24 flip_h=\"true\" flip_v=\"true\"/></pixel_format></camera></dsvl_input>";
	CoInitialize(NULL);

	m_pclGraphManager = new ELVDLib_VideoSource();

	if(FAILED(m_pclGraphManager->buildString(config_default)))						//ConfigによるCameraの初期化
	{
		m_pclGraphManager->release();
		delete m_pclGraphManager;
		m_pclGraphManager = NULL;
		CoUninitialize();
		return FALSE;
	}
	if(FAILED(m_pclGraphManager->bufferEnable()))									//メモリ使用許可
	{
		m_pclGraphManager->release();
		delete m_pclGraphManager;
		m_pclGraphManager = NULL;
		CoUninitialize();
		return FALSE;
	}
	if(FAILED(m_pclGraphManager->run()))											//マネージャ開始
	{
		m_pclGraphManager->bufferDisable();
		m_pclGraphManager->release();
		delete m_pclGraphManager;
		m_pclGraphManager = NULL;
		CoUninitialize();
		return FALSE;
	}
	PIXELFORMAT pixelFmt;
	if(FAILED(m_pclGraphManager->getFormat(&frame_width, &frame_height, NULL, &pixelFmt)))	//MediaFormat取得
	{
		m_pclGraphManager->stop(true);
		m_pclGraphManager->bufferDisable();
		m_pclGraphManager->release();
		delete m_pclGraphManager;
		m_pclGraphManager = NULL;
		CoUninitialize();
		return FALSE;																//MediaFormat取得に失敗
	}
	if(pixelFmt != PIXELFORMAT_RGB24)												//RGB24で受ける事が前提
	{
		m_pclGraphManager->stop(true);
		m_pclGraphManager->bufferDisable();
		m_pclGraphManager->release();
		delete m_pclGraphManager;
		m_pclGraphManager = NULL;
		CoUninitialize();
		return FALSE;
	}

	//連続呼び出し時のSleep用タイマーセット
	m_qTimeSleep.start();															//タイマー開始
	m_lWaitTime = MAIN_FPS_SLEEP;													//待機する目標時間
	m_lNextTime = m_qTimeSleep.elapsed() + m_lWaitTime;								//

	if(QIODevice::open(ReadWrite | Unbuffered) == false)							//QIODeviceのOpen
	{
		return IM_ERROR_NO_DEVICE;
	}

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			close
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClPresenterCO10::close()
{
	if(isOpen() == true){															//if available device
	}

	if(m_qLib.isLoaded())
	{
		m_qLib.unload();
	}

	if(m_pclGraphManager != NULL)
	{
		if (bufferCheckedOut) {
			m_pclGraphManager->bufferCheckin(m_BufHandle, true);
			bufferCheckedOut = FALSE;
		}
		m_pclGraphManager->stop(true);
		m_pclGraphManager->bufferDisable();
		m_pclGraphManager->release();
		delete m_pclGraphManager;
		m_pclGraphManager = NULL;

		// COM should be closed down in the same context
		CoUninitialize();
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn			CommandPresenter
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterCO10::CommandPresenter(qint32 CmdID, QVariant& qParameter)
{
	qint32 lRes = IM_ERROR_NOT_OPEN;						//未接続


	DBGFL("CommandPresenter windows");

	switch(CmdID){
	case GET_PICTURE:										//画像取得
		{
			lRes = getPicture(qParameter);
			break;
		}
	case RELEASE_PICTURE:									//取得した画像領域の解放
		{
			lRes = IM_ERROR_SUCCESS;
			break;
		}
	case GET_SIZE_PICTURE:									//直前に取得したサイズの返却
		{
			lRes = IM_ERROR_SUCCESS;						//取得成功
			qParameter = m_iPictureSize;					//
			break;
		}
	case GET_RECT_PICTURE:									//画像の大きさ取得
		{
			lRes = IM_ERROR_SUCCESS;						//取得成功
			qParameter.setValue<eKINDRECTID>(m_iPictureRect);	//
			break;
		}
	case SET_RECT_PICTURE:									//画像の大きさ設定
		{
			if(qParameter.type() == QVariant::UserType){
				if(chINRANGE(SIZE_PICTURE_HIGH,qParameter.value<eKINDRECTID>(),RECTID_PICTURE_DEFAULT)){
					lRes = IM_ERROR_SUCCESS;					//取得成功
					m_iPictureRect = qParameter.value<eKINDRECTID>();
				}
			}
			break;
		}
	case GET_MODEL_NAME:									//機種名取得
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
			lRes = IM_ERROR_SUCCESS;						//取得成功
			qParameter = m_usHeight;						//
			break;
		}
	case GET_WIDTH_PICTURE:									//現在の画像の横幅
		{
			lRes = IM_ERROR_SUCCESS;						//取得成功
			qParameter = m_usWidth;							//
			break;
		}
	case ZOOM_WIDE:											//拡大
		{
			lRes = execZoom(EL_WIDE);
			break;
		}
	case ZOOM_TELE:											//縮小
		{
			lRes = execZoom(EL_TELE);
			break;
		}
	case ZOOM_STOP:											//ストップ
		{
			lRes = execZoom(EL_STOP);
			break;
		}
	case IRIS_OPEN:											//開く
		{
			lRes = execIris(EL_OPEN);
			break;
		}
	case IRIS_CLOSE:										//閉じる
		{
			lRes = execIris(EL_CLOSE);
			break;
		}
	case IRIS_STOP:											//ストップ
		{
			lRes = execIris(EL_STOP);
			break;
		}
	case IRIS_INITIALIZE:									//初期位置
		{
			lRes = execIris(EL_NORMAL);
			break;
		}
	case FOCUS_AUTO:										//自動
		{
			lRes = execAF();
			break;
		}
	case FOCUS_NEAR:										//近く
		{
			lRes = execFocus(EL_NEAR);
			break;
		}
	case FOCUS_FAR:											//遠く
		{
			lRes = execFocus(EL_FAR);
			break;
		}
	case FOCUS_STOP:										//ストップ
		{
			lRes = execFocus(EL_STOP);
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
qint32 ClPresenterCO10::getPicture(QVariant &qParam)					//画像取得関数
{
	qint32 wait_result;

	// ばかよけ
	if (m_pclGraphManager == NULL)
	{
		return IM_ERROR_NOT_OPEN;
	}

	// 状態確認
	if (bufferCheckedOut)
	{
		if (FAILED(m_pclGraphManager->bufferCheckin(m_BufHandle, true)))
		{
			return IM_ERROR_COMMAND_FAILURE;
		}
		bufferCheckedOut = FALSE;
	}

	// 画像取得
	wait_result = m_pclGraphManager->waitSample(frame_timeout_ms);
	if (wait_result == WAIT_OBJECT_0)
	{
		if (FAILED(m_pclGraphManager->bufferCheckout(&(m_BufHandle),
													 (BYTE **)&m_ucJpgBuffer,
													 (unsigned int *)&frame_width,
													 (unsigned int *)&frame_height,
													 NULL, &(m_Timestamp))))
		{
			return IM_ERROR_CAPTURE_FAILURE;
		}
		bufferCheckedOut = TRUE;
	}
	else																//成功以外は認めない
	{
		return IM_ERROR_COMMAND_FAILURE;
	}

	// データをBGRからRGBに置き換え
	m_ucBuffer = new unsigned char[frame_width * frame_height * 3];
	memset(m_ucBuffer, NULL,frame_width *  frame_height * 3);
	convertBGRtoRGB((const unsigned char *)m_ucJpgBuffer,
					(unsigned char *)m_ucBuffer,
					frame_width, frame_height);							//配置替え

	// 画像を保存
	QImage img(m_ucBuffer, frame_width, frame_height, QImage::Format_RGB888);
	QByteArray qJpgArray;
	QBuffer qBuffer(&qJpgArray);
	qBuffer.open(QIODevice::WriteOnly);
	img.save(&qBuffer, "jpg");
	qParam = qJpgArray;
#if 0
	// 一定時間Sleep
	if(m_qTimeSleep.elapsed() < m_lNextTime)							//すでに超えていたらループしない
	{
		register qint32 work;											//変数へのアクセス速度をあげるためにregisterで宣言
		while((work = m_qTimeSleep.elapsed()) < m_lNextTime)			//目標時間を過ぎるまで繰り返し
		{
			if((m_lNextTime - work) > 10 ){								//目標まで10msあるなら
				_sleep(10);												//10ms寝る
			}else{														//それ以外は1mづつ寝る
				_sleep(1);												//1ms寝る
			}
		}
	}
	m_lNextTime = m_qTimeSleep.elapsed() + m_lWaitTime;					//次にループする時間の更新
#endif
	// サイズを格納
	m_usHeight	= frame_height;
	m_usWidth	= frame_width;
	m_iPictureSize = qJpgArray.length();
	m_iPictureRect = RECTID_UNSUPPORT;

	// 開放
	if(m_ucBuffer != NULL){												//開放
		delete [] m_ucBuffer;
	}

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			execZoom
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterCO10::execZoom(qint32 lParam)
{
	int res = IM_ERROR_SUCCESS;

	if(lParam == EL_WIDE){
		res = PresenterV2_SDK_Set(Index_Zoom_Start, Value_Zoom_Wide);	//Wideズーム
	}else if(lParam == EL_TELE){
		res = PresenterV2_SDK_Set(Index_Zoom_Start, Value_Zoom_Tele);	//Teleズーム
	}else if(lParam == EL_STOP){
		res = PresenterV2_SDK_Set(Index_Zoom_Stop, Value_Zoom_Stop);	//ズーム停止
	}
	if(res != IM_ERROR_SUCCESS){										//エラーだった場合
		return IM_ERROR_COMMAND_FAILURE;								//エラー返却
	}

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			execIris
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterCO10::execIris(qint32 lParam)
{
	int res = IM_ERROR_SUCCESS;
	unsigned long Value = 0;

	if(lParam == EL_NORMAL){											//IRIS NORMAL 設定
		res = PresenterV2_SDK_SetBrightness(50);
	}else if(lParam == EL_OPEN){
		res = PresenterV2_SDK_GetBrightness(&Value);					//現在の明るさ取得 -> Value
		if(res != IM_ERROR_SUCCESS){
			return IM_ERROR_COMMAND_FAILURE;							//エラー返却
		}
		if(Value < 100){
			Value++;													//Value値変更
			res = PresenterV2_SDK_SetBrightness(Value);					//設定した数値の明るさにする
			if(res != IM_ERROR_SUCCESS){
				return IM_ERROR_COMMAND_FAILURE;						//エラー返却
			}
		}
	}else if(lParam == EL_CLOSE){
		res = PresenterV2_SDK_GetBrightness(&Value);					//現在の明るさ取得 -> Value
		if(res != IM_ERROR_SUCCESS){
			return IM_ERROR_COMMAND_FAILURE;							//エラー返却
		}
		if(Value > 0){
			Value--;													//Value値変更
			res = PresenterV2_SDK_SetBrightness(Value);					//設定した数値の明るさにする
			if(res != IM_ERROR_SUCCESS){
				return IM_ERROR_COMMAND_FAILURE;						//エラー返却
			}
		}
	}

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			execAF
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterCO10::execAF()
{
	int res = IM_ERROR_SUCCESS;

	res = PresenterV2_OneTrigger(ID_AF);							//AutoFocus実行
	if(res != IM_ERROR_SUCCESS){
		return IM_ERROR_COMMAND_FAILURE;							//エラー返却
	}

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			execFocus
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterCO10::execFocus(qint32 lParam)
{
	int res = IM_ERROR_SUCCESS;

	if(lParam == EL_FAR){											//FAR 設定
		res = PresenterV2_SDK_Set(Index_Focus_Start, Value_Focus_Far);
	}else if(lParam == EL_NEAR){									//NEAR 設定
		res = PresenterV2_SDK_Set(Index_Focus_Start, Value_Focus_Near);
	}else if(lParam == EL_STOP){									//STOP
		res = PresenterV2_SDK_Set(Index_Focus_Stop, Value_Focus_Stop);
	}
	if(res != IM_ERROR_SUCCESS){
		return IM_ERROR_COMMAND_FAILURE;							//エラー返却
	}

	return IM_ERROR_SUCCESS;
}

//--------------------------------------------------------------------------------
/*!
	@fn			execCommand
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterCO10::execCommand(ClUsbCmd &clCmd)
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
/*!
	@fn			getModelName
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterCO10::getModelName(QVariant &qParam)
{
	qParam = QString("i-Pochette");						//output model name to QVariant

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			getVersion
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterCO10::getVersion(QVariant &qParam)
{
	int res = 0;
	char ucVersin[256];
	memset(ucVersin,NULL,sizeof(ucVersin));
	res = PresenterV2_QueryFirmwareVersion((BYTE *)ucVersin);
	if( res != IM_ERROR_SUCCESS){
		return IM_ERROR_COMMAND_FAILURE;					//エラー返却
	}

	QString clStrVer;
	clStrVer = ucVersin;
	QStringList clStrListVer;
	clStrListVer.append(clStrVer);
	qParam = clStrListVer;

	return IM_ERROR_SUCCESS;								//output version to QVariant
}
//--------------------------------------------------------------------------------
/*!
	@fn			readData
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint64	ClPresenterCO10::readData ( char * data, qint64 maxSize )
{
#ifdef QT_DEBUG
	qDebug() << sizeof(data) << maxSize;
#else
	Q_UNUSED(data)
	Q_UNUSED(maxSize)
#endif
	return -1;
}
//--------------------------------------------------------------------------------
/*!
	@fn			writeData
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint64	ClPresenterCO10::writeData ( const char * data, qint64 maxSize )
{
#ifdef QT_DEBUG
	qDebug() << sizeof(data) << maxSize;
#else
	Q_UNUSED(data)
	Q_UNUSED(maxSize)
#endif
	return -1;
}

/*------------------------------------------------------------------------------*/
/*!
 * @fn		convertBGRtoRGB
 * @brief
 * @param
 * @return	void
 */
/*------------------------------------------------------------------------------*/
void ClPresenterCO10::convertBGRtoRGB( const unsigned char* bgr,
											unsigned char* rgb, qint32 width, qint32 height )
{
	int i;
	int v;
	int rgbOffset = 0;
	int bgrOffset = 0;
#pragma omp parallel
	{
#pragma omp for private(i,rgbOffset,bgrOffset)
		for(v = 0 ; v < height; v++ )
		{
			bgrOffset = v * width * 3;
			rgbOffset = ((height - 1) - v) * width * 3;

			for( i = 0; i < width; i++ )
			{
				rgb[rgbOffset + (i * 3) + 2] = bgr[bgrOffset + (i * 3) + 0];
				rgb[rgbOffset + (i * 3) + 1] = bgr[bgrOffset + (i * 3) + 1];
				rgb[rgbOffset + (i * 3) + 0] = bgr[bgrOffset + (i * 3) + 2];
			}
		}
	}
}

