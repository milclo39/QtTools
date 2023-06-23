#define __USB_DEBUG

#include "ClUsbDriver/ClUsbDriver_global.h"
#include "Common/CmnDefine.h"
#include "ClUsbDriver/ClUsbDriver_win.h"
#include "win32/ClDeviceInfo_win.h"
#include "ClUsbDriver/Command_Presenter.h"
#include "win32/ClPresenterUVC.h"
#include "RollOverToolBtn/ClRollOverToolBtn.h"
#include "RectWidget/ClRectWidget.h"
#include "UsbDevice.h"


#include <strmif.h>
#include <QDebug>
#include <QMessageBox>

#include <QGroupBox>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QSlider>
#include <QLabel>


// パラメータ
#define SLIDER_DIRECTION					( Qt::Horizontal )	//!< スライダーの向き

#define PARAM_CMD_RES_INVALID				( -1 )				//!< デフォルト（UVCで設定されている設定可能間隔にする場合のコンストラクタでセットする初期値

#define TIME_SEND_GOSTOP_DEF		( 100 )				//!< Go/Stopコマンドを送る時間間隔(msec)
#define TIME_SEND_GOSTOP_IRIS		( 500 )				//!< アイリスGo/Stopコマンドを送る時間間隔(msec)

// 文言
#define WORD_UVC_REMOCON_AUTO		( ClPresenterUVC::tr("Auto") )								//!< リモコン 文言　WB等の自動
#define WORD_UVC_REMOCON_EXPOSE		( ClPresenterUVC::tr("Expose") )							//!< リモコン 文言　露出
#define WORD_UVC_REMOCON_CONTRAST	( ClPresenterUVC::tr("Contrast") )							//!< リモコン 文言　コントラスト
#define WORD_UVC_REMOCON_HUE		( ClPresenterUVC::tr("Hue") )								//!< リモコン 文言　色相
#define WORD_UVC_REMOCON_SATUATION	( ClPresenterUVC::tr("Satuation") )							//!< リモコン 文言　彩度
#define WORD_UVC_REMOCON_SHARPNESS	( ClPresenterUVC::tr("Edge Effect") )						//!< リモコン 文言　鋭度
#define WORD_UVC_REMOCON_GAMMA		( ClPresenterUVC::tr("Gamma") )								//!< リモコン 文言　ガンマ
#define WORD_UVC_REMOCON_WB			( ClPresenterUVC::tr("White Balance") )						//!< リモコン 文言　ホワイトバランス
#define WORD_UVC_REMOCON_BACKLIGHT	( ClPresenterUVC::tr("Backlight") )							//!< リモコン 文言　逆光補正
#define WORD_UVC_REMOCON_GAIN		( ClPresenterUVC::tr("Gain") )								//!< リモコン 文言　ゲイン
#define WORD_UVC_REMOCON_MUTE		( ClPresenterUVC::tr("Mute") )								//!< リモコン 文言　ミュート
#define WORD_UVC_REMOCON_VOLUME		( ClPresenterUVC::tr("Volume (0% - 100%)") )				//!< リモコン 文言　ボリューム

#define WORD_UVC_REMOCON_ZOOM_TELE			( ClPresenterUVC::tr("<b>Zoom IN</b><br>Adjust Zoom (TELE)") )
#define WORD_UVC_REMOCON_ZOOM_WIDE			( ClPresenterUVC::tr("<b>Zoom OUT</b><br>Adjust Zoom (WIDE)") )
#define WORD_UVC_REMOCON_FOCUS_NEAR			( ClPresenterUVC::tr("<b>Focus NEAR</b><br>Adjust Focus (NEAR)") )
#define WORD_UVC_REMOCON_FOCUS_FAR			( ClPresenterUVC::tr("<b>Focus FAR</b><br>Adjust Focus (FAR)") )
#define WORD_UVC_REMOCON_FOCUS_AUTO			( ClPresenterUVC::tr("<b>Auto Focus</b><br>Adjust Focus (AUTO)") )
#define WORD_UVC_REMOCON_BRIGHTNESS_OPEN	( ClPresenterUVC::tr("<b>Brightness(Brighten)</b><br>Brighten camera image") )
#define WORD_UVC_REMOCON_BRIGHTNESS_CLOSE	( ClPresenterUVC::tr("<b>Brightness(Darken)</b><br>Darken camera image") )
#define WORD_UVC_REMOCON_BRIGHTNESS_DEF		( ClPresenterUVC::tr("<b>Reset Brightness</b><br>Return camera image brightness to default") )

// 式
#define	EXP_BOOL_TO_INT(bVal)				( (( bVal ) == false )?( 0 ):( 1 ) )				//!< bool から BOOL
#define	EXP_INT_TO_BOOL(bVal)				( (( bVal ) == 0 )?( false ):( true ) )				//!< BOOL から bool
#define EXP_CREATE_RESOLUTION_STRING(x,y)	( QString("%1 x %2").arg(x).arg(y) )				//!< 解像度を表す文字列の作成

// コマンド送信値検出
#ifndef __DEBUG__
//#define __DEBUG__
#endif

#ifdef	__DEBUG__	// デバッグ用
void outPutRangeDebug( long lMin, long lMax, long lStep, long lDef, long lFlgs )
{
	qDebug() << "param ranges" << "[min : "  << lMin << ", max : " << lMax << ",step : " << lStep << ", def : " << lDef << ", flg : " << lFlgs << "]";
}
void outPutParamDebug( long lCur, bool bIsAuto  )
{
	qDebug() << "current param " << "[cur : "  << lCur << ", IsAuto : " << bIsAuto << "]";
}
void outPutDebug( long lMin, long lMax, long lStep, long lDef, long lFlgs , long lCur, bool bIsAuto  )
{
	outPutRangeDebug(lMin, lMax, lStep, lDef, lFlgs);
	outPutParamDebug(lCur, bIsAuto);
}
#endif

//--------------------------------------------------------------------------------
/*!
	@fn			ClPresenterUVC
	@brief
 */
//--------------------------------------------------------------------------------
ClPresenterUVC::ClPresenterUVC(qint32 lProductId, QString strDevPath, QObject *parent) :
	ClDeviceInfo(lProductId, parent),
	m_lCmdGoStop( -1 ),
	m_clTimerGoStop( this ),
	m_strDevPath( strDevPath )
{
	// 初期化
	m_pclGraphManager	= NULL;		// グラフマネージャ
	bufferCheckedOut	= FALSE;	//
	frame_timeout_ms	= 2000L;	// タイムアウト

	m_ucBuffer			= NULL;		//ワークバッファ
	m_ucJpgBuffer		= NULL;		//取得データ格納

	m_pclRemoconWidget	= NULL;		// リモコン
	m_pclWBAuto			= NULL;		// WBAutoチェックボックス
	m_pclMute			= NULL;		// ミュートチェックボックス

	m_ProductName.clear();			// 怪しげなXmlに設定するプロダクト名

	m_iZoomInterval			= PARAM_CMD_RES_INVALID;	// ズーム設定値間隔
	m_iFocusInterval		= PARAM_CMD_RES_INVALID;	// フォーカス設定値間隔
	m_iBrightnessInterval	= PARAM_CMD_RES_INVALID;	// 明度設定値間隔

#ifdef  _AUDIO_MUTE_SET_BY_VOLUME_
	m_iLastVolume = 0.0f;			//!< ボリューム保持用
#endif

	//	タイマー設定
	m_clTimerGoStop.setSingleShot( false );
	connect( &m_clTimerGoStop, SIGNAL( timeout() ),
			this, SLOT( slotTimeoutGoStop() ) );

	return;
}
//--------------------------------------------------------------------------------
/*!
	@fn			~ClPresenterUVC
	@brief
 */
//--------------------------------------------------------------------------------
ClPresenterUVC::~ClPresenterUVC()
{
}
//--------------------------------------------------------------------------------
/*!
	@fn			open
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterUVC::open()
{
	// すでにオープン済み
	if(isOpen() == true){
		return IM_ERROR_SUCCESS;
	}

	if(m_pclGraphManager != NULL){
		return IM_ERROR_ALREADY_OPENED;
	}

	CoInitialize(NULL);

	m_pclGraphManager = new ELVDLib_VideoSource();

	// ConfigによるCameraの初期化
	if(FAILED( buildGraphMgr() ))
	{
		m_pclGraphManager->release();
		delete m_pclGraphManager;
		m_pclGraphManager = NULL;
		CoUninitialize();
		return IM_ERROR_NOT_OPEN;
	}

	// メモリ使用許可
	if(FAILED(m_pclGraphManager->bufferEnable()))
	{
		m_pclGraphManager->release();
		delete m_pclGraphManager;
		m_pclGraphManager = NULL;
		CoUninitialize();
		return IM_ERROR_NOT_OPEN;
	}

	// マネージャ開始
	if(FAILED(m_pclGraphManager->run()))
	{
		m_pclGraphManager->bufferDisable();
		m_pclGraphManager->release();
		delete m_pclGraphManager;
		m_pclGraphManager = NULL;
		CoUninitialize();
		return FALSE;
	}

	PIXELFORMAT pixelFmt;

	// MediaFormat取得
	if(FAILED(m_pclGraphManager->getFormat(&frame_width, &frame_height, NULL, &pixelFmt)))
	{
		m_pclGraphManager->stop(true);
		m_pclGraphManager->bufferDisable();
		m_pclGraphManager->release();
		delete m_pclGraphManager;
		m_pclGraphManager = NULL;
		CoUninitialize();
		return IM_ERROR_NOT_OPEN;
	}

	// RGB24で受ける事が前提
	if(pixelFmt != PIXELFORMAT_RGB24)
	{
		m_pclGraphManager->stop(true);
		m_pclGraphManager->bufferDisable();
		m_pclGraphManager->release();
		delete m_pclGraphManager;
		m_pclGraphManager = NULL;
		CoUninitialize();
		return IM_ERROR_NOT_OPEN;
	}

	// 連続呼び出し時のSleep用タイマーセット
	m_qTimeSleep.start();									// タイマー開始
	m_lWaitTime = MAIN_FPS_SLEEP;							// 待機する目標時間
	m_lNextTime = m_qTimeSleep.elapsed() + m_lWaitTime;		//

	// QIODeviceのOpen
	if(QIODevice::open(ReadWrite | Unbuffered) == false)
	{
		return IM_ERROR_NO_DEVICE;
	}
#if 0 //リモコン生成していないのでコメントアウト
	// ------------------------------------------ //
	//
	//		Create widgets
	//
	// ------------------------------------------ //
	createRemoconWidget();
#endif
	// ------------------------------------------ //
	//
	//		解像度の取得
	//
	// ------------------------------------------ //
	m_pclGraphManager->getSupportedResolutions(m_stResolution);

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			close
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClPresenterUVC::close()
{
	if(isOpen() == true){															//if available device
		ClDeviceInfo::close();
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
	@fn			reopen
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClPresenterUVC::reopen()
{
	//設定保存
	double dFrameRate = getCurrentFrameRate();
	QString clStrResolution = getCurrentResolution();

	//ばかよけ
	if( 0 >= dFrameRate )
	{
		return;
	}
	if( true == clStrResolution.isEmpty() )
	{
		return;
	}

	//再接続
	close();
	open();

	//設定
	m_pclGraphManager->setCurrentFrameRate( dFrameRate );	//フレームレート設定
	if( getCurrentResolution() != clStrResolution )
	{
		Sleep(8000);										//オープン直後は画像が崩れるので待つ
		setCurrentResolution( clStrResolution );			//解像度設定
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn			GraphManager構築
 */
//--------------------------------------------------------------------------------
HRESULT ClPresenterUVC::buildGraphMgr( void )
{
	if( NULL == m_pclGraphManager )
	{
		return IM_ERROR_NO_MEM;
	}

	// 怪しげなxml
#if 0 //他社UVCカメラ対応前
	QString strXml("<?xml version=\"1.0\" encoding=\"UTF-8\"?><dsvl_input><camera show_format_dialog=\"false\" friendly_name=\"" + m_ProductName + "\"><pixel_format><RGB24 flip_h=\"true\" flip_v=\"true\"/></pixel_format></camera></dsvl_input>");
#endif
#if 1 //他社UVCカメラ対応後
	QString strXml("<?xml version=\"1.0\" encoding=\"UTF-8\"?><dsvl_input><camera show_format_dialog=\"false\" device_name=\"" + m_strDevPath + "\"><pixel_format><RGB24 flip_h=\"true\" flip_v=\"true\"/></pixel_format></camera></dsvl_input>");
#endif
	qint32	lLength = strXml.length();

	// 配列生成
	char* config_default = new char[lLength];

	// 移植
	for(int i = 0; i < lLength; i++){
		config_default[i] = strXml.data()[i].toLatin1();
	}

	// ConfigによるCameraの初期化
	HRESULT  hr = m_pclGraphManager->buildString(config_default);
	delete[] config_default;

	return hr;
}
//---
//--------------------------------------------------------------------------------
/*!
	@fn			CommandPresenter
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterUVC::CommandPresenter(qint32 CmdID, QVariant& qParameter)
{
	qint32 lRes = IM_ERROR_NOT_OPEN;						//未接続
	DBGFL("CommandPresenter windows");

	switch(CmdID){

		// ---------------------------- //
		//		画像
		// ---------------------------- //
	case GET_PICTURE:										// 画像取得
		{
			lRes = getPicture(qParameter);
			break;
		}
	case RELEASE_PICTURE:									// 取得した画像領域の解放
		{
			lRes = IM_ERROR_SUCCESS;
			break;
		}
	case GET_SIZE_PICTURE:									// 直前に取得したサイズの返却
		{
			lRes = IM_ERROR_SUCCESS;
			qParameter = m_iPictureSize;
			break;
		}
	case GET_RECT_PICTURE:									// 画像の大きさ取得
		{
			lRes = IM_ERROR_SUCCESS;
			qParameter.setValue<eKINDRECTID>(m_iPictureRect);
			break;
		}
	case SET_RECT_PICTURE:									// 画像の大きさ設定
		{
			if(qParameter.type() == QVariant::UserType){
				if(chINRANGE(SIZE_PICTURE_HIGH,qParameter.value<eKINDRECTID>(),RECTID_PICTURE_DEFAULT)){
					lRes = IM_ERROR_SUCCESS;
					m_iPictureRect = qParameter.value<eKINDRECTID>();
				}
			}
			break;
		}

		// ---------------------------- //
		//		カメラパラメータ
		// ---------------------------- //
	case GET_MODEL_NAME:									// 機種名取得
		{
			lRes = getModelName(qParameter);
			break;
		}
	case GET_MODEL_VERSION:									// バージョン取得
		{
			lRes = getVersion(qParameter);
			break;
		}

		// ---------------------------- //
		//		画像パラメータ
		// ---------------------------- //
	case GET_HEIGHT_PICTURE:								// 現在の画像の縦幅
		{
			lRes = IM_ERROR_SUCCESS;
			qParameter = m_usHeight;
			break;
		}
	case GET_WIDTH_PICTURE:									// 現在の画像の横幅
		{
			lRes = IM_ERROR_SUCCESS;
			qParameter = m_usWidth;
			break;
		}

		// ---------------------------- //
		//		V1 Command
		// ---------------------------- //
	case ZOOM_WIDE:												//拡大
		{
			lRes = slotProcZoomWide();
			break;
		}
	case ZOOM_TELE:												//縮小
		{
			lRes = slotProcZoomTele();
			break;
		}
	case ZOOM_STOP:												//ストップ
		{
			lRes = slotProcZoomStop();
			break;
		}
	case IRIS_OPEN:												//開く
		{
			lRes = slotProcBrightnessOpen();
			break;
		}
	case IRIS_CLOSE:											//閉じる
		{
			lRes = slotProcBrightnessClose();
			break;
		}
	case IRIS_STOP:												//ストップ
		{
			lRes = slotProcBrightnessStop();
			break;
		}
	case IRIS_INITIALIZE:										//初期位置
		{
			lRes = slotProcBrightnessNomal();
			break;
		}
	case FOCUS_AUTO:											//自動
		{
			lRes = slotProcFocusAuto();
			break;
		}
	case FOCUS_NEAR:											//近く
		{
			lRes = slotProcFocusNear();
			break;
		}
	case FOCUS_FAR:												//遠く
		{
			lRes = slotProcFocusFar();
			break;
		}
	case FOCUS_STOP:											//ストップ
		{
			lRes = slotProcFocusStop();
			break;
		}

	default:												// 対応したコマンドがない
		lRes = IM_ERROR_INVALID_PARAMETER;
		break;
	}

	return lRes;

}

//--------------------------------------------------------------------------------
/*!
		@brief	カメラ操作リモコンを返す
 */
//--------------------------------------------------------------------------------
QWidget* ClPresenterUVC::getRemoconWidget()
{
	return m_pclRemoconWidget;
}

//--------------------------------------------------------------------------------
/*!
		@brief	対応解像度を返す
 */
//--------------------------------------------------------------------------------
QStringList	ClPresenterUVC::getSupportedResolutions()
{
	QStringList	clRetVal;

	clRetVal.clear();

	for(int i = 0; i < m_stResolution.iCount; i++)
	{
		if(false == clRetVal.contains(EXP_CREATE_RESOLUTION_STRING(m_stResolution.stResolutions[i].iWidth, m_stResolution.stResolutions[i].iHeight))){
			clRetVal.append( EXP_CREATE_RESOLUTION_STRING(m_stResolution.stResolutions[i].iWidth, m_stResolution.stResolutions[i].iHeight) );
		}
	}

	return clRetVal;
}

//--------------------------------------------------------------------------------
/*!
		@brief	現在の解像度を取得
 */
//--------------------------------------------------------------------------------
QString ClPresenterUVC::getCurrentResolution()
{
	stRESOLUTION	stCurrRes;

	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return QString("");
	}

	// 現在の解像度を取得
	m_pclGraphManager->getCurrentResolution(stCurrRes);

	return EXP_CREATE_RESOLUTION_STRING(stCurrRes.iWidth, stCurrRes.iHeight);
}

//--------------------------------------------------------------------------------
/*!
		@brief	現在の解像度を設定
 */
//--------------------------------------------------------------------------------
qint32	ClPresenterUVC::setCurrentResolution(QString strResolution)
{
	QStringList	l_clResolutions;	// 引数分解用
	qint32		l_iWidth	= 0;	// 幅格納用
	qint32		l_iHeight	= 0;	// 高さ格納用

	// ばかよけ
	if(true == strResolution.isNull())
	{
		return E_FAIL;
	}
	if(m_pclGraphManager == NULL)
	{
		return E_FAIL;
	}

	// 状態確認
	if (bufferCheckedOut) {
		if (FAILED(m_pclGraphManager->bufferCheckin(m_BufHandle, true)))
		{
			return E_FAIL;
		}
		bufferCheckedOut = FALSE;
	}

	//	現在の解像度確認
	if(getCurrentResolution() == strResolution)
	{
		return S_OK;								// 既に設定した解像度なので成功で抜ける
	}

	// 分解
	l_clResolutions	=	strResolution.split('x');	// 受け取った解像度は文字列なので分解

	// 要素数チェック
	if(l_clResolutions.count() != 2)
	{
		return E_FAIL;
	}

	// 値セット
	l_iWidth	=	l_clResolutions.at(0).toInt();	// 幅セット
	l_iHeight	=	l_clResolutions.at(1).toInt();	// 高さセット

	// 解像度を設定
	HRESULT ret = E_FAIL;
	if(l_iWidth != 0 && l_iHeight != 0)
	{
#ifdef __DEBUG__
		qDebug() << "set current resolution :: resolution -> " << strResolution;
#endif
		stRESOLUTION stResolution = stRESOLUTION(l_iWidth, l_iHeight);
		ret = m_pclGraphManager->setCurrentResolution(stResolution);
	}
	else
	{
#ifdef __DEBUG__
		qDebug() << "set current resolution :: failure";
#endif
		return E_FAIL;
	}

	// 解像度切替の終了待ち
	qint32 lCnt = 0;
	do{
		//終了判定
		if(lCnt > 1000){
			break;
		}else{
			Sleep(10);
			lCnt++;
		}
	}while(getCurrentResolution() != strResolution);

	// 画像取得
	qint32 wait_result;
	wait_result = m_pclGraphManager->waitSample(frame_timeout_ms);
	if (wait_result == WAIT_OBJECT_0)
	{
		if (FAILED(m_pclGraphManager->bufferCheckout(&(m_BufHandle),
													 (BYTE **)&m_ucJpgBuffer,
													 (unsigned int *)&frame_width,
													 (unsigned int *)&frame_height,
													 NULL, &(m_Timestamp))))
		{
			return E_FAIL;
		}
		bufferCheckedOut = TRUE;
	}

	//	初期パラメータ設定
	setDefaultParams();

	return ret;
}

//--------------------------------------------------------------------------------
/*!
		@brief	対応フレームレートを返す
 */
//--------------------------------------------------------------------------------
QString ClPresenterUVC::getSupportedFrameRates(stFRAMERATES& stFrameRates)
{
	QStringList	l_clResolutions;	// 引数分解用
	qint32		l_iWidth	= 0;	// 幅格納用
	qint32		l_iHeight	= 0;	// 高さ格納用

	//　解像度取得
	QString strResolution = getCurrentResolution();

	// ばかよけ
	if(true == strResolution.isNull())
	{
		return NULL;
	}
	if(m_pclGraphManager == NULL)
	{
		return NULL;
	}

	// 分解
	l_clResolutions	=	strResolution.split('x');	// 受け取った解像度は文字列なので分解

	// 要素数チェック
	if(l_clResolutions.count() != 2)
	{
		return NULL;
	}

	// 値セット
	l_iWidth	=	l_clResolutions.at(0).toInt();	// 幅セット
	l_iHeight	=	l_clResolutions.at(1).toInt();	// 高さセット
	stRESOLUTION stResolution = stRESOLUTION(l_iWidth, l_iHeight);

	//　フレームレートの取得
	m_pclGraphManager->getSupportedFrameRates(stResolution, stFrameRates);

	return strResolution;
}

//--------------------------------------------------------------------------------
/*!
		@brief	現在のフレームレートを取得
 */
//--------------------------------------------------------------------------------
double ClPresenterUVC::getCurrentFrameRate()
{
	stFRAMERATE	stCurrFrameRate;

	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return NULL;
	}

	// 現在の解像度を取得
	m_pclGraphManager->getCurrentFrameRate(stCurrFrameRate);

	return stCurrFrameRate.dFrameRate;
}

//--------------------------------------------------------------------------------
/*!
		@brief	現在のフレームレートを設定
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterUVC::setCurrentFrameRate(bool bIsFrameRateMax)
{
	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return E_FAIL;
	}

	// 状態確認
	if (bufferCheckedOut) {
		if (FAILED(m_pclGraphManager->bufferCheckin(m_BufHandle, true)))
		{
			return E_FAIL;
		}
		bufferCheckedOut = FALSE;
	}

	//　フレームレートの取得
	stFRAMERATES stFrameRates;
	QString strResolution = getSupportedFrameRates(stFrameRates);

	// フレームレートの設定
	HRESULT ret = E_FAIL;
	if(bIsFrameRateMax == true)						//最大フレームレートの場合(待ち時間最小)
	{
		double dFrameRate = stFrameRates.stFrameRates[0].dFrameRate;
		if(dFrameRate <= 0)
		{
			return E_FAIL;							//フレームレート0以下はありえないので失敗で抜ける
		}
		if(getCurrentFrameRate() == dFrameRate)
		{
			return S_OK;							//既に設定したいフレームレートなので成功で抜ける
		}
#ifdef __DEBUG__
		qDebug() << "set current frame rate :: resolution -> " << strResolution << "frame rate -> " << dFrameRate;
#endif
		ret = m_pclGraphManager->setCurrentFrameRate( stFrameRates.stFrameRates[0] );
	}
	else											//最小フレームレートの場合(待ち時間最大)
	{
		double dFrameRate = stFrameRates.stFrameRates[1].dFrameRate;
		if(dFrameRate <= 0)
		{
			return E_FAIL;							//フレームレート0以下はありえないので失敗で抜ける
		}
		if(getCurrentFrameRate() == dFrameRate)
		{
			return S_OK;							//既に設定したいフレームレートなので成功で抜ける
		}
#ifdef __DEBUG__
		qDebug() << "set current frame rate :: resolution -> " << strResolution << "frame rate -> " << dFrameRate;
#endif
		ret = m_pclGraphManager->setCurrentFrameRate( stFrameRates.stFrameRates[1] );
	}

	// 画像取得
	qint32 wait_result;
	wait_result = m_pclGraphManager->waitSample(frame_timeout_ms);
	if (wait_result == WAIT_OBJECT_0)
	{
		if (FAILED(m_pclGraphManager->bufferCheckout(&(m_BufHandle),
													 (BYTE **)&m_ucJpgBuffer,
													 (unsigned int *)&frame_width,
													 (unsigned int *)&frame_height,
													 NULL, &(m_Timestamp))))
		{
			return E_FAIL;
		}
		bufferCheckedOut = TRUE;
	}

	//	初期パラメータ設定
	setDefaultParams();

	return ret;
}


//--------------------------------------------------------------------------------
/*!
	@fn			setDefaultParams
	@brief		初期パラメータ設定
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClPresenterUVC::setDefaultParams()
{
	int res = true;
	long lMin = 0, lMax = 0, lStep = 0, lDef = 0, lFlg = 0;

	//	ズーム
	m_pclGraphManager->getParamRange(CC_CameraControl, CameraControl_Zoom, &lMin, &lMax, &lStep, &lDef, &lFlg );
	res = m_pclGraphManager->setParam(CC_CameraControl, CameraControl_Zoom, lDef, false);						//デフォルト値設定

	//	フォーカス
	m_pclGraphManager->getParamRange(CC_CameraControl, CameraControl_Focus, &lMin, &lMax, &lStep, &lDef, &lFlg );
	res = m_pclGraphManager->setParam(CC_CameraControl, CameraControl_Focus, lDef, false);						//デフォルト値設定

	//	アイリス
	m_pclGraphManager->getParamRange(CC_VideoProcAmp, VideoProcAmp_Brightness, &lMin, &lMax, &lStep, &lDef, &lFlg );
	res = m_pclGraphManager->setParam(CC_VideoProcAmp, VideoProcAmp_Brightness, lDef, false);					//デフォルト値設定
}


//--------------------------------------------------------------------------------
/*!
	@fn			getPicture
	@brief		画像取得
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterUVC::getPicture(QVariant &qParam)
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
			return IM_ERROR_CANNOT_OUTPUT_IMAGE;
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
		return IM_ERROR_CANNOT_OUTPUT_IMAGE;
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
		m_ucBuffer = NULL;
	}

	return IM_ERROR_SUCCESS;
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	IM用リモコンウィジェット生成
//
//  ------------------------------------------------------------------------------------- */
void ClPresenterUVC::createRemoconWidget()
{
	// 結果用
	HRESULT	hr = S_OK;

	// 値の変更が可能かどうかの取得用(値の変更可能範囲)
	long lMin;									// 最小値
	long lMax;									// 最大値
	long lSteppingDelta;						// ？
	long lDefault;								// でふぉ値
	long lCapsFlags;							// ？

	long lCurVal;								//　現在の値取得用
	bool bCurAuto;								// 現在の自動設定取得用

	// リモコン初期化
	if( m_pclRemoconWidget != NULL )
	{
		delete m_pclRemoconWidget;
	}

	ClRectWidget* pclRemoconRectWidget = new ClRectWidget;
	pclRemoconRectWidget->setParent(&m_clParent);

	pclRemoconRectWidget->setDragMove(true);
	pclRemoconRectWidget->setImgTopLeft(QPixmap(QString::fromUtf8(":/img/rsc/Parts_remocon_frame_topleft.png")));
	pclRemoconRectWidget->setImgTop(QPixmap(QString::fromUtf8(":/img/rsc/Parts_remocon_frame_top.png")));
	pclRemoconRectWidget->setImgTopRight(QPixmap(QString::fromUtf8(":/img/rsc/Parts_remocon_frame_topright.png")));
	pclRemoconRectWidget->setImgLeft(QPixmap(QString::fromUtf8(":/img/rsc/Parts_remocon_frame_left.png")));
	pclRemoconRectWidget->setImgRight(QPixmap(QString::fromUtf8(":/img/rsc/Parts_remocon_frame_right.png")));
	pclRemoconRectWidget->setImgBottomLeft(QPixmap(QString::fromUtf8(":/img/rsc/Parts_remocon_frame_bottomleft.png")));
	pclRemoconRectWidget->setImgBottom(QPixmap(QString::fromUtf8(":/img/rsc/Parts_remocon_frame_bottom.png")));
	pclRemoconRectWidget->setImgBottomRight(QPixmap(QString::fromUtf8(":/img/rsc/Parts_remocon_frame_bottomright.png")));

	//	タスクバーから消す
	pclRemoconRectWidget->setWindowFlags(pclRemoconRectWidget->windowFlags() | Qt::Tool | Qt::ToolTip);

	m_pclRemoconWidget = pclRemoconRectWidget;

	//背景色設定
	QPalette p = m_pclRemoconWidget->palette();
	p.setColor(QPalette::Window, QColor(255, 255, 255, 255));	//白
	m_pclRemoconWidget->setPalette(p);

	m_pclMainLayout = new QVBoxLayout( m_pclRemoconWidget );

	// ---------------------------------------------------------------- //
	//
	//		UVC
	//
	// ---------------------------------------------------------------- //

	// -------------------------------- //
	//
	//		Zoom
	//
	// -------------------------------- //

	hr = m_pclGraphManager->getParamRange(CC_CameraControl ,CameraControl_Zoom, &lMin, &lMax, &lSteppingDelta, &lDefault, &lCapsFlags);
	if( lMin != lMax && !FAILED(hr) )
	{
		// コマンドの送信時間間隔が継承先によって設定されなかったら設定可能な最小の間隔を設定
		if( m_iZoomInterval == PARAM_CMD_RES_INVALID )
		{
			m_iZoomInterval = lSteppingDelta;
		}

		// レイアウト
		QHBoxLayout*	pclZoomLayout = new QHBoxLayout;

		// Tele
		ClRollOverToolBtn* pclTele = new ClRollOverToolBtn(m_pclRemoconWidget);

		pclTele->setObjectName(QString::fromUtf8("btnZoomTele"));
		pclTele->setMinimumSize(QSize(34, 34));
		pclTele->setMaximumSize(QSize(34, 34));
		pclTele->setFocusPolicy(Qt::NoFocus);
		QIcon iconTele;
		iconTele.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_zoom_tele_off.png"), QSize(), QIcon::Normal, QIcon::On);
		iconTele.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_zoom_tele_down.png"), QSize(), QIcon::Disabled, QIcon::On);
		iconTele.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_zoom_tele_on.png"), QSize(), QIcon::Active, QIcon::On);
		iconTele.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_zoom_tele_down.png"), QSize(), QIcon::Selected, QIcon::On);
		pclTele->setProperty("imgShow", QVariant(iconTele));
		connect( pclTele, SIGNAL(pressed()), this, SLOT(slotProcZoomTele()));
		connect( pclTele, SIGNAL(released()), this, SLOT(slotProcZoomStop()));
		pclZoomLayout->addWidget( pclTele );

		m_pclZoomTeleBtn = pclTele;

		// Wide
		ClRollOverToolBtn* pclWide = new ClRollOverToolBtn(m_pclRemoconWidget);

		pclWide->setObjectName(QString::fromUtf8("btnZoomWide"));
		pclWide->setMinimumSize(QSize(34, 34));
		pclWide->setMaximumSize(QSize(34, 34));
		pclWide->setFocusPolicy(Qt::NoFocus);
		QIcon iconWide;
		iconWide.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_zoom_wide_off.png"), QSize(), QIcon::Normal, QIcon::On);
		iconWide.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_zoom_wide_down.png"), QSize(), QIcon::Disabled, QIcon::On);
		iconWide.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_zoom_wide_on.png"), QSize(), QIcon::Active, QIcon::On);
		iconWide.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_zoom_wide_down.png"), QSize(), QIcon::Selected, QIcon::On);
		pclWide->setProperty("imgShow", QVariant(iconWide));
		connect( pclWide, SIGNAL(pressed()), this, SLOT(slotProcZoomWide()));
		connect( pclWide, SIGNAL(released()), this, SLOT(slotProcZoomStop()));
		pclZoomLayout->addWidget( pclWide );

		m_pclZoomWideBtn = pclWide;

		m_pclMainLayout->addLayout( pclZoomLayout );
	}

	// -------------------------------- //
	//
	//		Focus
	//
	// -------------------------------- //

	hr = m_pclGraphManager->getParamRange(CC_CameraControl ,CameraControl_Focus, &lMin, &lMax, &lSteppingDelta, &lDefault, &lCapsFlags);
	if( lMin != lMax  && !FAILED(hr) )
	{
		// コマンドの送信時間間隔が継承先によって設定されなかったら設定可能な最小の間隔を設定
		if( m_iFocusInterval == PARAM_CMD_RES_INVALID )
		{
			m_iFocusInterval = lSteppingDelta;
		}

		// レイアウト
		QHBoxLayout*	pclFocusLayout = new QHBoxLayout;

		// Near
		ClRollOverToolBtn* pclNear = new ClRollOverToolBtn(m_pclRemoconWidget);

		pclNear->setObjectName(QString::fromUtf8("btnFocusNear"));
		pclNear->setMinimumSize(QSize(24, 24));
		pclNear->setMaximumSize(QSize(24, 24));
		pclNear->setFocusPolicy(Qt::NoFocus);
		QIcon iconNear;
		iconNear.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_near_off.png"), QSize(), QIcon::Normal, QIcon::On);
		iconNear.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_near_down.png"), QSize(), QIcon::Disabled, QIcon::On);
		iconNear.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_near_on.png"), QSize(), QIcon::Active, QIcon::On);
		iconNear.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_near_down.png"), QSize(), QIcon::Selected, QIcon::On);
		pclNear->setProperty("imgShow", QVariant(iconNear));

		connect( pclNear, SIGNAL(pressed()), this, SLOT(slotProcFocusNear()));
		connect( pclNear, SIGNAL(released()), this, SLOT(slotProcFocusStop()));
		pclFocusLayout->addWidget( pclNear );

		m_pclFocusNearBtn = pclNear;

		// AF
		ClRollOverToolBtn* pclAF = new ClRollOverToolBtn(m_pclRemoconWidget);
		pclAF->setObjectName(QString::fromUtf8("btnFocusAuto"));
		QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		sizePolicy.setHeightForWidth(pclAF->sizePolicy().hasHeightForWidth());
		pclAF->setSizePolicy(sizePolicy);
		pclAF->setMinimumSize(QSize(40, 40));
		pclAF->setMaximumSize(QSize(40, 40));
		pclAF->setFocusPolicy(Qt::NoFocus);
		QIcon iconAF;
		iconAF.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_AF_off.png"), QSize(), QIcon::Normal, QIcon::On);
		iconAF.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_AF_down.png"), QSize(), QIcon::Disabled, QIcon::On);
		iconAF.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_AF_on.png"), QSize(), QIcon::Active, QIcon::On);
		iconAF.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_AF_down.png"), QSize(), QIcon::Selected, QIcon::On);
		pclAF->setProperty("imgShow", QVariant(iconAF));
		connect( pclAF, SIGNAL(clicked()), this, SLOT(slotProcFocusAuto()));
		pclFocusLayout->addWidget( pclAF );

		m_pclFocusAutoBtn = pclAF;

		// Far
		ClRollOverToolBtn* pclFar = new ClRollOverToolBtn(m_pclRemoconWidget);

		pclFar->setObjectName(QString::fromUtf8("btnFocusFar"));
		pclFar->setMinimumSize(QSize(24, 24));
		pclFar->setMaximumSize(QSize(24, 24));
		pclFar->setFocusPolicy(Qt::NoFocus);
		QIcon iconFar;
		iconFar.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_far_off.png"), QSize(), QIcon::Normal, QIcon::On);
		iconFar.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_near_down.png"), QSize(), QIcon::Disabled, QIcon::On);
		iconFar.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_far_on.png"), QSize(), QIcon::Active, QIcon::On);
		iconFar.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_far_down.png"), QSize(), QIcon::Selected, QIcon::On);
		pclFar->setProperty("imgShow", QVariant(iconFar));
		connect( pclFar, SIGNAL(pressed()), this, SLOT(slotProcFocusFar()));
		connect( pclFar, SIGNAL(released()), this, SLOT(slotProcFocusStop()));
		pclFocusLayout->addWidget( pclFar );

		m_pclFocusFarBtn = pclFar;

		m_pclMainLayout->addLayout( pclFocusLayout );
	}


	// -------------------------------- //
	//
	//		Brightness
	//
	// -------------------------------- //

	hr = m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_Brightness, &lMin, &lMax, &lSteppingDelta, &lDefault, &lCapsFlags);
	if( lMin != lMax  && !FAILED(hr) )
	{
		// コマンドの送信時間間隔が継承先によって設定されなかったら設定可能な最小の間隔を設定
		if( m_iBrightnessInterval == PARAM_CMD_RES_INVALID )
		{
			m_iBrightnessInterval = lSteppingDelta;
		}

		// レイアウト
		QHBoxLayout*	pclBrightnessLayout = new QHBoxLayout;

		// Open
		ClRollOverToolBtn* pclOpen = new ClRollOverToolBtn(m_pclRemoconWidget);

		pclOpen->setObjectName(QString::fromUtf8("btnIrisOpen"));
		pclOpen->setMinimumSize(QSize(34, 34));
		pclOpen->setMaximumSize(QSize(34, 34));
		pclOpen->setFocusPolicy(Qt::NoFocus);
		QIcon iconOpen;
		iconOpen.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_iris_open_off.png"), QSize(), QIcon::Normal, QIcon::On);
		iconOpen.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_iris_open_down.png"), QSize(), QIcon::Disabled, QIcon::On);
		iconOpen.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_iris_open_on.png"), QSize(), QIcon::Active, QIcon::On);
		iconOpen.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_iris_open_down.png"), QSize(), QIcon::Selected, QIcon::On);
		pclOpen->setProperty("imgShow", QVariant(iconOpen));
		connect( pclOpen, SIGNAL(pressed()), this, SLOT(slotProcBrightnessOpen()));
		connect( pclOpen, SIGNAL(released()), this, SLOT(slotProcBrightnessStop()));
		pclBrightnessLayout->addWidget( pclOpen );

		m_pclBrightOpenBtn = pclOpen;

		// Close
		ClRollOverToolBtn* pclClose =  new ClRollOverToolBtn(m_pclRemoconWidget);

		pclClose->setObjectName(QString::fromUtf8("btnIrisClose"));
		pclClose->setMinimumSize(QSize(34, 34));
		pclClose->setMaximumSize(QSize(34, 34));
		pclClose->setFocusPolicy(Qt::NoFocus);
		QIcon iconClose;
		iconClose.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_iris_close_off.png"), QSize(), QIcon::Normal, QIcon::On);
		iconClose.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_iris_close_down.png"), QSize(), QIcon::Disabled, QIcon::On);
		iconClose.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_iris_close_on.png"), QSize(), QIcon::Active, QIcon::On);
		iconClose.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_iris_close_down.png"), QSize(), QIcon::Selected, QIcon::On);
		pclClose->setProperty("imgShow", QVariant(iconClose));
		connect( pclClose, SIGNAL(pressed()), this, SLOT(slotProcBrightnessClose()));
		connect( pclClose, SIGNAL(released()), this, SLOT(slotProcBrightnessStop()));
		pclBrightnessLayout->addWidget( pclClose );

		m_pclBrightCloseBtn = pclClose;

		m_pclMainLayout->addLayout( pclBrightnessLayout );


		// default
		{
			QHBoxLayout* layoutBrightnessNormal = new QHBoxLayout();
			layoutBrightnessNormal->setSpacing(2);
			layoutBrightnessNormal->setObjectName(QString::fromUtf8("layoutIrisNormal"));

			QSpacerItem* horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
			layoutBrightnessNormal->addItem(horizontalSpacer);

			ClRollOverToolBtn* pclBrightnessNormal = new ClRollOverToolBtn(m_pclRemoconWidget);
			pclBrightnessNormal->setObjectName(QString::fromUtf8("btnIrisNormal"));

			QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
			sizePolicy.setHeightForWidth(pclBrightnessNormal->sizePolicy().hasHeightForWidth());
			pclBrightnessNormal->setSizePolicy(sizePolicy);
			pclBrightnessNormal->setMinimumSize(QSize(64, 30));
			pclBrightnessNormal->setMaximumSize(QSize(64, 30));
			pclBrightnessNormal->setFocusPolicy(Qt::NoFocus);

			QIcon iconNomal;
			iconNomal.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_iris_normal_off.png"), QSize(), QIcon::Normal, QIcon::On);
			iconNomal.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_iris_normal_down.png"), QSize(), QIcon::Disabled, QIcon::On);
			iconNomal.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_iris_normal_on.png"), QSize(), QIcon::Active, QIcon::On);
			iconNomal.addFile(QString::fromUtf8(":/imgs/rsc/Btn_tool_remocon_iris_normal_down.png"), QSize(), QIcon::Selected, QIcon::On);
			pclBrightnessNormal->setProperty("imgShow", QVariant(iconNomal));

			layoutBrightnessNormal->addWidget(pclBrightnessNormal);

			QSpacerItem* horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

			layoutBrightnessNormal->addItem(horizontalSpacer_2);

			connect( pclBrightnessNormal, SIGNAL(clicked()), this, SLOT(slotProcBrightnessNomal()));

			m_pclMainLayout->addLayout(layoutBrightnessNormal);

			m_pclBrightDefBtn = pclBrightnessNormal;
		}
	}

	// -------------------------------- //
	//
	//		Expose
	//
	// -------------------------------- //

	hr = m_pclGraphManager->getParamRange(CC_CameraControl ,CameraControl_Exposure, &lMin, &lMax, &lSteppingDelta, &lDefault, &lCapsFlags);
	if( lMin != lMax  && !FAILED(hr) )
	{
		// レイアウト
		QHBoxLayout*	pclExposeSetterLayout	= new QHBoxLayout;

		// -- 現在の値取得

		hr = m_pclGraphManager->getParam( CC_CameraControl ,CameraControl_Exposure, &lCurVal, &bCurAuto );

		// --

		QSlider* pclSetter = new QSlider;

		pclSetter->setOrientation( SLIDER_DIRECTION );

		pclSetter->setMinimum( lMin );
		pclSetter->setMaximum( lMax );
		pclSetter->setSingleStep( lSteppingDelta );
		if( !FAILED(hr) )
		{
			pclSetter->setValue( lCurVal );
		}


		connect( pclSetter, SIGNAL(valueChanged(int)), this, SLOT(slotSetExpose(int)));
		pclExposeSetterLayout->addWidget( pclSetter );

		// --


		QPushButton* pclSetAuto = new QPushButton;

		pclSetAuto->setText(WORD_UVC_REMOCON_AUTO);
		pclSetAuto->setChecked( bCurAuto );

		connect( pclSetAuto, SIGNAL(clicked()), this, SLOT(slotSetExposeAuto()));

		pclExposeSetterLayout->addWidget( pclSetAuto );

		// --

		// 調節内容を表すラベル
		QLabel*	pclName = new QLabel(WORD_UVC_REMOCON_EXPOSE, m_pclRemoconWidget);
		connect( this, SIGNAL(sigSetTextRemoconExpose(QString)),
				 pclName, SLOT( setText(QString)) );

		m_pclMainLayout->addWidget( pclName );
		m_pclMainLayout->addLayout( pclExposeSetterLayout );
	}

	// -------------------------------- //
	//
	//		Contrast
	//
	// -------------------------------- //

	hr = m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_Contrast, &lMin, &lMax, &lSteppingDelta, &lDefault, &lCapsFlags);
	if( lMin != lMax && !FAILED(hr) )
	{
		// 現在の値取得

		hr = m_pclGraphManager->getParam( CC_VideoProcAmp ,VideoProcAmp_Contrast, &lCurVal, &bCurAuto );

		// レイアウト
		QVBoxLayout*	pclContrastLayout = new QVBoxLayout;

		QSlider* pclSetter = new QSlider;

		pclSetter->setOrientation( SLIDER_DIRECTION );

		pclSetter->setMinimum( lMin );
		pclSetter->setMaximum( lMax );
		if( !FAILED(hr) )
		{
			pclSetter->setValue( lCurVal );
		}

		connect( pclSetter, SIGNAL(valueChanged(int)), this, SLOT(slotSetContrast(int)));

		pclContrastLayout->addWidget( pclSetter );

		// 調節内容を表すラベル
		QLabel*	pclName = new QLabel(WORD_UVC_REMOCON_CONTRAST, m_pclRemoconWidget);
		connect( this, SIGNAL(sigSetTextRemoconContrast(QString)),
				 pclName, SLOT( setText(QString)) );

		m_pclMainLayout->addWidget( pclName );
		m_pclMainLayout->addLayout( pclContrastLayout );
	}

	// -------------------------------- //
	//
	//		Hue
	//
	// -------------------------------- //

	hr = m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_Hue, &lMin, &lMax, &lSteppingDelta, &lDefault, &lCapsFlags);
	if( lMin != lMax  && !FAILED(hr) )
	{
		// 現在の値取得

		hr = m_pclGraphManager->getParam( CC_VideoProcAmp ,VideoProcAmp_Hue, &lCurVal, &bCurAuto );

		// レイアウト
		QVBoxLayout*	pclHueLayout = new QVBoxLayout;

		QSlider* pclSetter = new QSlider;

		pclSetter->setOrientation( SLIDER_DIRECTION );

		pclSetter->setMinimum( lMin );
		pclSetter->setMaximum( lMax );
		if( !FAILED(hr) )
		{
			pclSetter->setValue( lCurVal );
		}


		connect( pclSetter, SIGNAL(valueChanged(int)), this, SLOT(slotSetHue(int)));
		pclHueLayout->addWidget( pclSetter );

		// 調節内容を表すラベル
		QLabel*	pclName = new QLabel(WORD_UVC_REMOCON_HUE, m_pclRemoconWidget);
		connect( this, SIGNAL(sigSetTextRemoconHue(QString)),
				 pclName, SLOT( setText(QString)) );

		m_pclMainLayout->addWidget( pclName );
		m_pclMainLayout->addLayout( pclHueLayout );
	}

	// -------------------------------- //
	//
	//		Satuation
	//
	// -------------------------------- //

	hr = m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_Saturation, &lMin, &lMax, &lSteppingDelta, &lDefault, &lCapsFlags);
	if( lMin != lMax && !FAILED(hr)  )
	{
		// 現在の値取得
		hr = m_pclGraphManager->getParam( CC_VideoProcAmp ,VideoProcAmp_Saturation, &lCurVal, &bCurAuto );

		// レイアウト
		QVBoxLayout*	pclSatuationLayout = new QVBoxLayout;

		QSlider* pclSetter = new QSlider;

		pclSetter->setOrientation( SLIDER_DIRECTION );

		pclSetter->setMinimum( lMin );
		pclSetter->setMaximum( lMax );
		if( !FAILED(hr) )
		{
			pclSetter->setValue( lCurVal );
		}


		connect( pclSetter, SIGNAL(valueChanged(int)), this, SLOT(slotSetSatuation(int)));
		pclSatuationLayout->addWidget( pclSetter );

		// 調節内容を表すラベル
		QLabel*	pclName = new QLabel(WORD_UVC_REMOCON_SATUATION, m_pclRemoconWidget);
		connect( this, SIGNAL(sigSetTextRemoconSatuation(QString)),
				 pclName, SLOT( setText(QString)) );

		m_pclMainLayout->addWidget( pclName );
		m_pclMainLayout->addLayout( pclSatuationLayout );
	}

	// -------------------------------- //
	//
	//		Sharpness
	//
	// -------------------------------- //

	hr = m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_Sharpness, &lMin, &lMax, &lSteppingDelta, &lDefault, &lCapsFlags);
	if( lMin != lMax && !FAILED(hr)  )
	{
		// 現在の値取得
		hr = m_pclGraphManager->getParam( CC_VideoProcAmp ,VideoProcAmp_Sharpness, &lCurVal, &bCurAuto );

		// レイアウト
		QVBoxLayout*	pclSharpnessLayout = new QVBoxLayout;

		QSlider* pclSetter = new QSlider;

		pclSetter->setOrientation( SLIDER_DIRECTION );

		pclSetter->setMinimum( lMin );
		pclSetter->setMaximum( lMax );
		if( !FAILED(hr) )
		{
			pclSetter->setValue( lCurVal );
		}


		connect( pclSetter, SIGNAL(valueChanged(int)), this, SLOT(slotSetSharoness(int)));
		pclSharpnessLayout->addWidget( pclSetter );

		// 調節内容を表すラベル
		QLabel*	pclName = new QLabel(WORD_UVC_REMOCON_SHARPNESS, m_pclRemoconWidget);
		connect( this, SIGNAL(sigSetTextRemoconSharpness(QString)),
				 pclName, SLOT( setText(QString)) );

		m_pclMainLayout->addWidget( pclName );
		m_pclMainLayout->addLayout( pclSharpnessLayout );
	}

	// -------------------------------- //
	//
	//		Gamma
	//
	// -------------------------------- //

	hr = m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_Gamma, &lMin, &lMax, &lSteppingDelta, &lDefault, &lCapsFlags);
	if( lMin != lMax && !FAILED(hr)  )
	{
		// 現在の値取得
		hr = m_pclGraphManager->getParam( CC_VideoProcAmp ,VideoProcAmp_Gamma, &lCurVal, &bCurAuto );

		// レイアウト
		QVBoxLayout*	pclGammaLayout = new QVBoxLayout;

		QSlider* pclSetter = new QSlider;

		pclSetter->setOrientation( SLIDER_DIRECTION );

		pclSetter->setMinimum( lMin );
		pclSetter->setMaximum( lMax );
		if( !FAILED(hr) )
		{
			pclSetter->setValue( lCurVal );
		}


		connect( pclSetter, SIGNAL(valueChanged(int)), this, SLOT(slotSetGamma(int)));
		pclGammaLayout->addWidget( pclSetter );


		// 調節内容を表すラベル
		QLabel*	pclName = new QLabel(WORD_UVC_REMOCON_GAMMA, m_pclRemoconWidget);
		connect( this, SIGNAL(sigSetTextRemoconGamma(QString)),
				 pclName, SLOT( setText(QString)) );

		m_pclMainLayout->addWidget( pclName );
		m_pclMainLayout->addLayout( pclGammaLayout );
	}

	// -------------------------------- //
	//
	//		WB
	//
	// -------------------------------- //

	hr = m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_WhiteBalance, &lMin, &lMax, &lSteppingDelta, &lDefault, &lCapsFlags);
	if( lMin != lMax && !FAILED(hr)  )
	{
		// 現在の値取得
		hr = m_pclGraphManager->getParam( CC_VideoProcAmp ,VideoProcAmp_WhiteBalance, &lCurVal, &bCurAuto );

		// レイアウト
		QHBoxLayout*	pclWBSetterLayout	= new QHBoxLayout;

		// --

		QSlider* pclSetter = new QSlider;

		pclSetter->setOrientation( SLIDER_DIRECTION );

		pclSetter->setMinimum( lMin );
		pclSetter->setMaximum( lMax );
		if( !FAILED(hr) )
		{
			pclSetter->setValue( lCurVal );
		}

		connect( pclSetter, SIGNAL(valueChanged(int)), this, SLOT(slotSetWB(int)));
		pclWBSetterLayout->addWidget( pclSetter );

		// --

		if(m_pclWBAuto == NULL)
		{
			m_pclWBAuto = new QCheckBox(m_pclRemoconWidget);	// なければ生成
		}


		m_pclWBAuto->setText(WORD_UVC_REMOCON_AUTO);

		connect( m_pclWBAuto, SIGNAL(toggled(bool)), this, SLOT(slotSetWBAuto(bool)));

		pclWBSetterLayout->addWidget( m_pclWBAuto );
		if( !FAILED(hr) )
		{
			m_pclWBAuto->setChecked(bCurAuto);
		}

		connect( this, SIGNAL(sigSetCheckedAutoWB(bool)), m_pclWBAuto, SLOT(setChecked(bool)) );

		// --

		// 調節内容を表すラベル
		QLabel*	pclName = new QLabel(WORD_UVC_REMOCON_WB, m_pclRemoconWidget);
		connect( this, SIGNAL(sigSetTextRemoconWB(QString)),
				 pclName, SLOT( setText(QString)) );

		m_pclMainLayout->addWidget( pclName );
		m_pclMainLayout->addLayout( pclWBSetterLayout );
	}

	// -------------------------------- //
	//
	//		Backlight
	//
	// -------------------------------- //

	hr = m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_BacklightCompensation, &lMin, &lMax, &lSteppingDelta, &lDefault, &lCapsFlags);
	if( lMin != lMax && !FAILED(hr)  )
	{
		// 現在の値取得
		hr = m_pclGraphManager->getParam( CC_VideoProcAmp ,VideoProcAmp_BacklightCompensation, &lCurVal, &bCurAuto );

		QVBoxLayout*	pclBacklightLayout = new QVBoxLayout;

		QSlider* pclSetter = new QSlider;

		pclSetter->setOrientation( SLIDER_DIRECTION );

		pclSetter->setMinimum( lMin );
		pclSetter->setMaximum( lMax );
		if( !FAILED(hr) )
		{
			pclSetter->setValue( lCurVal );
		}

		connect( pclSetter, SIGNAL(valueChanged(int)), this, SLOT(slotSetBacklight(int)));
		pclBacklightLayout->addWidget( pclSetter );

		// 調節内容を表すラベル
		QLabel*	pclName = new QLabel(WORD_UVC_REMOCON_BACKLIGHT, m_pclRemoconWidget);
		connect( this, SIGNAL(sigSetTextRemoconBacklight(QString)),
				 pclName, SLOT( setText(QString)) );

		m_pclMainLayout->addWidget( pclName );
		m_pclMainLayout->addLayout( pclBacklightLayout );
	}

	// -------------------------------- //
	//
	//		Gain
	//
	// -------------------------------- //

	hr = m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_Gain, &lMin, &lMax, &lSteppingDelta, &lDefault, &lCapsFlags);
	if( lMin != lMax && !FAILED(hr)  )
	{
		// 現在の値取得
		hr = m_pclGraphManager->getParam( CC_VideoProcAmp ,VideoProcAmp_Gain, &lCurVal, &bCurAuto );

		QVBoxLayout*	pclGainLayout = new QVBoxLayout;

		QSlider* pclSetter = new QSlider;

		pclSetter->setOrientation( SLIDER_DIRECTION );

		pclSetter->setMinimum( lMin );
		pclSetter->setMaximum( lMax );

		if( !FAILED(hr) )
		{
			pclSetter->setValue( lCurVal );
		}

		connect( pclSetter, SIGNAL(valueChanged(int)), this, SLOT(slotSetGain(int)));
		pclGainLayout->addWidget( pclSetter );


		// 調節内容を表すラベル
		QLabel*	pclName = new QLabel(WORD_UVC_REMOCON_GAIN, m_pclRemoconWidget);
		connect( this, SIGNAL(sigSetTextRemoconGain(QString)),
				 pclName, SLOT( setText(QString)) );

		m_pclMainLayout->addWidget( pclName );
		m_pclMainLayout->addLayout( pclGainLayout );
	}

	// ---------------------------------------------------------------- //
	//
	//		UAC
	//
	// ---------------------------------------------------------------- //

	// -------------------------------- //
	//
	//		Volume
	//
	// -------------------------------- //

	{
		bool bIsExitEnable = false;

		// レイアウト
		QHBoxLayout*	pclVolumeSetterLayout	= new QHBoxLayout;

		// -- volume
		DOUBLE	dCurVolume = 0.0f;
		hr = m_pclGraphManager->getCurrentVolume(dCurVolume);
#ifdef  _AUDIO_MUTE_SET_BY_VOLUME_
		m_iLastVolume = dCurVolume;
#endif

		if( !FAILED(hr) )
		{
			QSlider* pclSetter = new QSlider;

			pclSetter->setOrientation( SLIDER_DIRECTION );
			pclSetter->setValue(static_cast<qint32>(dCurVolume * 100));

			pclSetter->setMinimum( 0 );
			pclSetter->setMaximum( 100 );

			connect( pclSetter, SIGNAL(valueChanged(int)), this, SLOT(slotSetVolume(int)));
			pclVolumeSetterLayout->addWidget( pclSetter );

			bIsExitEnable = true;
		}

		// -- mute
		//	(2012.02.22 ミュート機能は無効にする)
#if 0
		BOOL bCurMute;
		hr = m_pclGraphManager->getAudioEnable( bCurMute );

		//何故かFailedになるけどMute操作は反映されるっぽいので応急的にコメントアウト
//		if( !FAILED(hr) )
		{
			if( m_pclMute == NULL )
			{
				m_pclMute = new QCheckBox;				// なければ生成
			}

			if( EXP_INT_TO_BOOL( bCurMute ) == true )	// 音声出力が有効だったらミュートにチェック
			{
				m_pclMute->setChecked( false );
			}
			else										// 音声出力が無効だったらミュートのチェックを外す
			{
				m_pclMute->setChecked( true );
			}

			m_pclMute->setText(WORD_UVC_REMOCON_MUTE);

			connect( m_pclMute, SIGNAL(toggled(bool)), this, SLOT(slotSetMute(bool)));

			connect( this,SIGNAL(sigSetMuteCheck(bool)),m_pclMute,SLOT(setChecked(bool)));

			pclVolumeSetterLayout->addWidget( m_pclMute );

			bIsExitEnable = true;
		}
#endif

		// -- 片方でも有効なものがあった
		if( bIsExitEnable == true )
		{
			// 調節内容を表すラベル
			QLabel*	pclName = new QLabel(WORD_UVC_REMOCON_VOLUME, m_pclRemoconWidget);
			connect( this, SIGNAL(sigSetTextRemoconVolume(QString)),
					 pclName, SLOT( setText(QString)) );

			m_pclMainLayout->addWidget( pclName );						// ラベル文字列の追加
			m_pclMainLayout->addLayout( pclVolumeSetterLayout );		// レイアウトに追加
		}
		else
		{
			delete pclVolumeSetterLayout;								// 音声調節なんてなかった
		}
	}

	// メインレイアウトをセット
	m_pclRemoconWidget->setLayout( m_pclMainLayout );

	m_pclZoomTeleBtn->setToolTip(WORD_UVC_REMOCON_ZOOM_TELE);
	m_pclZoomWideBtn->setToolTip(WORD_UVC_REMOCON_ZOOM_WIDE);
	m_pclFocusNearBtn->setToolTip(WORD_UVC_REMOCON_FOCUS_NEAR);
	m_pclFocusFarBtn->setToolTip(WORD_UVC_REMOCON_FOCUS_FAR);
	m_pclFocusAutoBtn->setToolTip(WORD_UVC_REMOCON_FOCUS_AUTO);
	m_pclBrightOpenBtn->setToolTip(WORD_UVC_REMOCON_BRIGHTNESS_OPEN);
	m_pclBrightCloseBtn->setToolTip(WORD_UVC_REMOCON_BRIGHTNESS_CLOSE);
	m_pclBrightDefBtn->setToolTip(WORD_UVC_REMOCON_BRIGHTNESS_DEF);//	qDebug() << "Create Remocon";
}




//--------------------------------------------------------------------------------
/*!
	@fn			getPicture
	@brief		ストリームの開始
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClPresenterUVC::startStream()
{
	if(m_pclGraphManager != NULL)
	{
		m_pclGraphManager->run();
	}
}

//--------------------------------------------------------------------------------
/*!
	@fn			getPicture
	@brief		ストリームの停止
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClPresenterUVC::stopStream()
{
	if(m_pclGraphManager != NULL)
	{
		m_pclGraphManager->stop(true);
	}
}

//--------------------------------------------------------------------------------
/*!
	@brief		Go/Stopコマンド開始
	@param[in]	lCmd		実行するコマンド
*/
//--------------------------------------------------------------------------------
void
ClPresenterUVC::startGoStop( qint32  lCmd )
{
	//	既に同じコマンドを実行中なら何もしない
	if( m_lCmdGoStop == lCmd )
	{
		return;
	}

	//	現在のコマンドを停止
	stopGoStop();

	//	タイマー設定後、開始
	if( ( IRIS_OPEN == lCmd )  ||  ( IRIS_CLOSE == lCmd ) )
	{
		m_clTimerGoStop.setInterval( TIME_SEND_GOSTOP_IRIS );
	}
	else
	{
		m_clTimerGoStop.setInterval( TIME_SEND_GOSTOP_DEF );
	}
	m_lCmdGoStop = lCmd;
	m_clTimerGoStop.start();

	return;
}

//--------------------------------------------------------------------------------
/*!
	@brief		Go/Stopコマンド停止
*/
//--------------------------------------------------------------------------------
void
ClPresenterUVC::stopGoStop( void )
{
	//	タイマー停止
	m_clTimerGoStop.stop();
	m_lCmdGoStop = -1;

	return;
}

//--------------------------------------------------------------------------------
/*!
	@brief		Go/Stopコマンド実行用タイマーのタイムアウト
*/
//--------------------------------------------------------------------------------
void
ClPresenterUVC::slotTimeoutGoStop( void )
{
	//	コマンド実行
	switch( m_lCmdGoStop )
	{
        case ZOOM_TELE	:	{	slotProcZoomTele();	break;	}
        case ZOOM_WIDE	:	{	slotProcZoomWide(); break;	}
		case IRIS_OPEN	:	{	slotProcBrightnessOpen();	break;	}
		case IRIS_CLOSE	:	{	slotProcBrightnessClose();	break;	}
		case FOCUS_NEAR	:	{	slotProcFocusNear();	break;	}
		case FOCUS_FAR	:	{	slotProcFocusFar();	break;	}
		default	:	{	break;	}
	}
	return;
}



/*! ------------------------------------------------------------------------------------- //
//
//		@brief	フォーカス　ニア
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotProcFocusNear()
{
	int res = true;

	long lMin = 0, lMax = 0, lStep = 0, lDef = 0, lFlg = 0, lCur = 0;
	bool bIsAuto = false;

	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return IM_ERROR_NOT_OPEN;
	}

	// 設定可能範囲取得
	m_pclGraphManager->getParamRange(CC_CameraControl, CameraControl_Focus, &lMin, &lMax, &lStep, &lDef, &lFlg );

#ifdef __DEBUG__ // ----------------- debug
	qDebug() << "Before set focus near. -- ";
	outPutRangeDebug(lMin, lMax, lStep, lDef, lFlg);
#endif

	// 現在値取得
	m_pclGraphManager->getParam(CC_CameraControl, CameraControl_Focus, &lCur, &bIsAuto );

#ifdef __DEBUG__ // ----------------- debug
	outPutParamDebug(lCur, bIsAuto);
#endif

	// 設定可能値域チェック
    if( lMin <= lCur && lCur < lMax )   //2013.01.29 Mac版に01.24行った修正内容に合わせる。　ここから
    {
        lCur += m_iFocusInterval;
        if(lCur > lMax) lCur = lMax;
    }
    else
    {
        stopGoStop();
        return  IM_ERROR_SUCCESS;
    }                                    //2013.01.29 Mac版に01.24行った修正内容に合わせる。　ここまで

	// セット
	res = m_pclGraphManager->setParam(CC_CameraControl, CameraControl_Focus, lCur, false);

#ifdef __DEBUG__ // ----------------- debug
	qDebug() << "After set focus near. -- ";
	m_pclGraphManager->getParam(CC_CameraControl, CameraControl_Focus, &lCur, &bIsAuto );
	outPutParamDebug(lCur, bIsAuto);
#endif

	if(res != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}

	//	Go/Stop開始
	startGoStop( FOCUS_NEAR );

	return IM_ERROR_SUCCESS;
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	フォーカス　ファー
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotProcFocusFar()
{
	int res = true;

	long lMin = 0, lMax = 0, lStep = 0, lDef = 0, lFlg = 0, lCur = 0;
	bool bIsAuto = false;

	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return IM_ERROR_NOT_OPEN;
	}

	// 設定可能範囲取得
	m_pclGraphManager->getParamRange(CC_CameraControl, CameraControl_Focus, &lMin, &lMax, &lStep, &lDef, &lFlg );

#ifdef __DEBUG__ // ----------------- debug
	qDebug() << "Before set focus far. -- ";
	outPutRangeDebug(lMin, lMax, lStep, lDef, lFlg);
#endif

	// 現在値取得
	m_pclGraphManager->getParam(CC_CameraControl, CameraControl_Focus, &lCur, &bIsAuto );

#ifdef __DEBUG__ // ----------------- debug
	outPutParamDebug(lCur, bIsAuto);
#endif

	// 設定可能値域チェック
    if( lMin < lCur && lCur <= lMax )   //2013.01.29 Mac版に01.24行った修正内容に合わせる。　ここから
    {
        lCur -= m_iFocusInterval;
        if(lCur < lMin)lCur = lMin;
    }
    else
    {
        stopGoStop();
        return  IM_ERROR_SUCCESS;
    }                                   //2013.01.29 Mac版に01.24行った修正内容に合わせる。　ここまで

	// セット
	res = m_pclGraphManager->setParam(CC_CameraControl, CameraControl_Focus, lCur, false);

#ifdef __DEBUG__ // ----------------- debug
	qDebug() << "After set focus far. -- ";
	m_pclGraphManager->getParam(CC_CameraControl, CameraControl_Focus, &lCur, &bIsAuto );
	outPutParamDebug(lCur, bIsAuto);
#endif

	if(res != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}

	//	Go/Stop開始
	startGoStop( FOCUS_FAR );

	return IM_ERROR_SUCCESS;
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	フォーカス　ストップ
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotProcFocusStop()
{
	//	Go/Stop停止
	stopGoStop();

	return IM_ERROR_SUCCESS;
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	フォーカス　オート
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotProcFocusAuto()
{
	int res = true;

	long lCurrent;
	bool bIsAuto;

	//	Go/Stop停止
	stopGoStop();

	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return IM_ERROR_NOT_OPEN;
	}

	m_pclGraphManager->getParam(CC_CameraControl, CameraControl_Focus, &lCurrent, &bIsAuto );

	if(bIsAuto == true)
	{
		res = m_pclGraphManager->setParam(CC_CameraControl, CameraControl_Focus, lCurrent, false);

		qint32 i = 0;
		do
		{
			if( i++ == 5 )
			{
				return IM_ERROR_COMMAND_FAILURE;
			}
			m_pclGraphManager->getParam(CC_CameraControl, CameraControl_Focus, &lCurrent, &bIsAuto );
			Sleep(150);
		}while( bIsAuto != false );
	}

	res = m_pclGraphManager->setParam(CC_CameraControl, CameraControl_Focus, lCurrent, true);
	m_pclGraphManager->getParam(CC_CameraControl, CameraControl_Focus, &lCurrent, &bIsAuto );

	if(res != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}

	return IM_ERROR_SUCCESS;
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	ズーム　テレ
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotProcZoomTele()
{
	int res = true;

	long lMin = 0, lMax = 0, lStep = 0, lDef = 0, lFlg = 0, lCur = 0;
	bool bIsAuto = false;

	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return IM_ERROR_NOT_OPEN;
	}

	// 設定可能範囲取得
	m_pclGraphManager->getParamRange(CC_CameraControl, CameraControl_Zoom, &lMin, &lMax, &lStep, &lDef, &lFlg );

#ifdef __DEBUG__ // ----------------- debug
	qDebug() << "Before set zoom tele. -- ";
	outPutRangeDebug(lMin, lMax, lStep, lDef, lFlg);
#endif

	// 現在値取得
	m_pclGraphManager->getParam(CC_CameraControl, CameraControl_Zoom, &lCur, &bIsAuto );

#ifdef __DEBUG__ // ----------------- debug
	outPutParamDebug(lCur, bIsAuto);
#endif

	// 設定可能値域チェック
    if( lMin <= lCur && lCur < lMax )   //2013.01.29 Mac版に01.24行った修正内容に合わせる。　ここから
    {
        lCur += m_iZoomInterval;
        if(lCur > lMax) lCur = lMax;
    }
    else
    {
        stopGoStop();
        return  IM_ERROR_SUCCESS;
    }                                    //2013.01.29 Mac版に01.24行った修正内容に合わせる。　ここまで


	// セット
	res = m_pclGraphManager->setParam(CC_CameraControl, CameraControl_Zoom, lCur, false);

#ifdef __DEBUG__ // ----------------- debug
	qDebug() << "After set zoom tele. -- ";
	m_pclGraphManager->getParam(CC_CameraControl, CameraControl_Zoom, &lCur, &bIsAuto );
	outPutParamDebug(lCur, bIsAuto);
#endif

	if(res != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}

	//	Go/Stop開始
	startGoStop( ZOOM_TELE );

	return IM_ERROR_SUCCESS;
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	ズーム　ワイド
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotProcZoomWide()
{
	int res = true;

	long lMin = 0, lMax = 0, lStep = 0, lDef = 0, lFlg = 0, lCur = 0;
	bool bIsAuto = false;

	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return IM_ERROR_NOT_OPEN;
	}

	// 設定可能範囲取得
	m_pclGraphManager->getParamRange(CC_CameraControl, CameraControl_Zoom, &lMin, &lMax, &lStep, &lDef, &lFlg );

#ifdef __DEBUG__ // ----------------- debug
	qDebug() << "Before set zoom wide. -- ";
	outPutRangeDebug(lMin, lMax, lStep, lDef, lFlg);
#endif

	// 現在値取得
	m_pclGraphManager->getParam(CC_CameraControl, CameraControl_Zoom, &lCur, &bIsAuto );

#ifdef __DEBUG__ // ----------------- debug
	outPutParamDebug(lCur, bIsAuto);
#endif

	// 設定可能値域チェック
    if( lMin < lCur && lCur <= lMax )   //2013.01.29 Mac版に01.24行った修正内容に合わせる。　ここから
    {
        lCur -= m_iZoomInterval;
        if(lCur < lMin)lCur = lMin;
    }
    else
    {
        stopGoStop();
        return  IM_ERROR_SUCCESS;
    }                                   //2013.01.29 Mac版に01.24行った修正内容に合わせる。　ここまで


	// セット
	res = m_pclGraphManager->setParam(CC_CameraControl, CameraControl_Zoom, lCur, false);

#ifdef __DEBUG__ // ----------------- debug
	qDebug() << "After set zoom wide. -- ";
	m_pclGraphManager->getParam(CC_CameraControl, CameraControl_Zoom, &lCur, &bIsAuto );
	outPutParamDebug(lCur, bIsAuto);
#endif

	if(res != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}

	//	Go/Stop開始
	startGoStop( ZOOM_WIDE );

	return IM_ERROR_SUCCESS;
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	ズーム　ストップ
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotProcZoomStop()
{
	//	Go/Stop停止
	stopGoStop();

	return IM_ERROR_SUCCESS;
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	明度　オープン
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotProcBrightnessOpen()
{
	int res = true;

	long lMin = 0, lMax = 0, lStep = 0, lDef = 0, lFlg = 0, lCur = 0;
	bool bIsAuto = false;

	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return IM_ERROR_NOT_OPEN;
	}

	// 設定可能範囲取得
	m_pclGraphManager->getParamRange(CC_VideoProcAmp, VideoProcAmp_Brightness, &lMin, &lMax, &lStep, &lDef, &lFlg );

#ifdef __DEBUG__ // ----------------- debug
	qDebug() << "Before set brightness open. -- ";
	outPutRangeDebug(lMin, lMax, lStep, lDef, lFlg);
#endif

	// 現在値取得
	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_Brightness, &lCur, &bIsAuto );

#ifdef __DEBUG__ // ----------------- debug
	outPutParamDebug(lCur, bIsAuto);
#endif

	// 設定可能値域チェック
    if( lMin <= lCur && lCur < lMax )   //2013.01.29 Mac版に01.24行った修正内容に合わせる。　ここから
    {
        lCur += m_iBrightnessInterval;
        if(lCur > lMax) lCur = lMax;
    }
    else
    {
        stopGoStop();
        return  IM_ERROR_SUCCESS;
    }                                    //2013.01.29 Mac版に01.24行った修正内容に合わせる。　ここまで

	// セット
	res = m_pclGraphManager->setParam(CC_VideoProcAmp, VideoProcAmp_Brightness, lCur, false);

#ifdef __DEBUG__ // ----------------- debug
	qDebug() << "After set brightness open. -- ";
	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_Brightness, &lCur, &bIsAuto );
	outPutParamDebug(lCur, bIsAuto);
#endif

	if(res != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}

	//	Go/Stop開始
	startGoStop( IRIS_OPEN );

	return IM_ERROR_SUCCESS;
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	明度　クローズ
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotProcBrightnessClose()
{
	int res = true;

	long lMin = 0, lMax = 0, lStep = 0, lDef = 0, lFlg = 0, lCur = 0;
	bool bIsAuto = false;

	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return IM_ERROR_NOT_OPEN;
	}

	// 設定可能範囲取得
	m_pclGraphManager->getParamRange(CC_VideoProcAmp, VideoProcAmp_Brightness, &lMin, &lMax, &lStep, &lDef, &lFlg );

#ifdef __DEBUG__ // ----------------- debug
	qDebug() << "Before set brightness close. -- ";
	outPutRangeDebug(lMin, lMax, lStep, lDef, lFlg);
#endif

	// 現在値取得
	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_Brightness, &lCur, &bIsAuto );

#ifdef __DEBUG__ // ----------------- debug
	outPutParamDebug(lCur, bIsAuto);
#endif

	// 設定可能値域チェック
    if( lMin < lCur && lCur <= lMax )   //2013.01.29 Mac版に01.24行った修正内容に合わせる。　ここから
    {
        lCur -= m_iBrightnessInterval;
        if(lCur < lMin)lCur = lMin;
    }
    else
    {
        stopGoStop();
        return  IM_ERROR_SUCCESS;
    }                                   //2013.01.29 Mac版に01.24行った修正内容に合わせる。　ここまで

	// セット
	res = m_pclGraphManager->setParam(CC_VideoProcAmp, VideoProcAmp_Brightness, lCur, false);

#ifdef __DEBUG__ // ----------------- debug
	qDebug() << "After set brightness clsoe. -- ";
	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_Brightness, &lCur, &bIsAuto );
	outPutParamDebug(lCur, bIsAuto);
#endif

	if(res != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}

	//	Go/Stop開始
	startGoStop( IRIS_CLOSE );

	return IM_ERROR_SUCCESS;
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	明度　ストップ
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotProcBrightnessStop()
{
	//	Go/Stop停止
	stopGoStop();

	return IM_ERROR_SUCCESS;
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	明度　ノーマル（でふぉ値にセット
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotProcBrightnessNomal()
{
	int res = true;

	long lMin = 0, lMax = 0, lStep = 0, lDef = 0, lFlg = 0;

	//	Go/Stop停止
	stopGoStop();

	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return IM_ERROR_NOT_OPEN;
	}

	// 設定可能範囲取得
	m_pclGraphManager->getParamRange(CC_VideoProcAmp, VideoProcAmp_Brightness, &lMin, &lMax, &lStep, &lDef, &lFlg );

#ifdef __DEBUG__ // ----------------- debug
	long lCur = 0;
	bool bIsAuto = false;
	qDebug() << "Before set brightness set def. -- ";
	outPutRangeDebug(lMin, lMax, lStep, lDef, lFlg);
#endif

#ifdef __DEBUG__ // ----------------- debug
	// 現在値取得
	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_Brightness, &lCur, &bIsAuto );
	outPutParamDebug(lCur, bIsAuto);
#endif

	// セット
	res = m_pclGraphManager->setParam(CC_VideoProcAmp, VideoProcAmp_Brightness, lDef, false);

#ifdef __DEBUG__ // ----------------- debug
	qDebug() << "After set brightness set def. -- ";
	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_Brightness, &lCur, &bIsAuto );
	outPutParamDebug(lCur, bIsAuto);
#endif

	if(res != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}

	return IM_ERROR_SUCCESS;
}


/*! ------------------------------------------------------------------------------------- //
//
//		@brief	露光 セット
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotSetExpose(int lParam)
{
	bool res;

	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return IM_ERROR_NOT_OPEN;
	}

#ifdef __DEBUG__ // -------- debug

	long lMin = 0, lMax = 0, lStep = 0, lDef = 0, lFlg = 0, lCur = 0;
	bool bIsAuto;

	// セット前
	m_pclGraphManager->getParamRange(CC_CameraControl ,CameraControl_Exposure, &lMin, &lMax, &lStep, &lDef, &lFlg);
	m_pclGraphManager->getParam(CC_CameraControl, CameraControl_Exposure, &lCur, &bIsAuto );
	qDebug() << "Before Params";
	outPutDebug( lMin, lMax, lStep, lDef, lFlg, lCur, bIsAuto );

	qDebug() << "Expose set : " << lParam;

#endif

	// セット
	res = m_pclGraphManager->setParam(CC_CameraControl, CameraControl_Exposure, lParam, false);

#ifdef __DEBUG__

	// セット後
	m_pclGraphManager->getParamRange(CC_CameraControl ,CameraControl_Exposure, &lMin, &lMax, &lStep, &lDef, &lFlg);
	m_pclGraphManager->getParam(CC_CameraControl, CameraControl_Exposure, &lCur, &bIsAuto );
	qDebug() << "After Params";
	outPutDebug( lMin, lMax, lStep, lDef, lFlg, lCur, bIsAuto );

#endif

	if(res != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}

	return IM_ERROR_SUCCESS;
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	露光 オートセット
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotSetExposeAuto()
{
	bool res;

	long lCurrent;
	bool bIsAuto;

	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return IM_ERROR_NOT_OPEN;
	}

	m_pclGraphManager->getParam(CC_CameraControl, CameraControl_Exposure, &lCurrent, &bIsAuto );

	res = m_pclGraphManager->setParam(CC_CameraControl, CameraControl_Exposure, lCurrent, true);

	if(res != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}

	return IM_ERROR_SUCCESS;
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	コントラスト
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotSetContrast(int lParam)
{
	bool res;

	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return IM_ERROR_NOT_OPEN;
	}

#ifdef __DEBUG__ // -------- debug

	long lMin = 0, lMax = 0, lStep = 0, lDef = 0, lFlg = 0, lCur = 0;
	bool bIsAuto;

	// セット前
	m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_Contrast, &lMin, &lMax, &lStep, &lDef, &lFlg);
	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_Contrast, &lCur, &bIsAuto );
	qDebug() << "Before Params";
	outPutDebug( lMin, lMax, lStep, lDef, lFlg, lCur, bIsAuto );

	qDebug() << "Contrast set : " << lParam;

#endif

	// セット
	res = m_pclGraphManager->setParam(CC_VideoProcAmp, VideoProcAmp_Contrast, lParam, false);

#ifdef __DEBUG__


	// セット後
	m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_Contrast, &lMin, &lMax, &lStep, &lDef, &lFlg);
	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_Contrast, &lCur, &bIsAuto );
	qDebug() << "After Params";
	outPutDebug( lMin, lMax, lStep, lDef, lFlg, lCur, bIsAuto );

#endif

	if(res != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}

	return IM_ERROR_SUCCESS;
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	色相
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotSetHue(qint32 lParam)
{
	bool res;

	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return IM_ERROR_NOT_OPEN;
	}

#ifdef __DEBUG__ // -------- debug

	long lMin = 0, lMax = 0, lStep = 0, lDef = 0, lFlg = 0, lCur = 0;
	bool bIsAuto;

	// セット前
	m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_Hue, &lMin, &lMax, &lStep, &lDef, &lFlg);
	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_Hue, &lCur, &bIsAuto );
	qDebug() << "Before Params";
	outPutDebug( lMin, lMax, lStep, lDef, lFlg, lCur, bIsAuto );

	qDebug() << "Hue set : " << lParam;

#endif

	// セット
	res = m_pclGraphManager->setParam(CC_VideoProcAmp, VideoProcAmp_Hue, lParam, false);

#ifdef __DEBUG__

	// セット後
	m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_Hue, &lMin, &lMax, &lStep, &lDef, &lFlg);
	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_Hue, &lCur, &bIsAuto );
	qDebug() << "After Params";
	outPutDebug( lMin, lMax, lStep, lDef, lFlg, lCur, bIsAuto );

#endif

	if(res != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}

	return IM_ERROR_SUCCESS;
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	彩度
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotSetSatuation(qint32 lParam)
{
	bool res;

	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return IM_ERROR_NOT_OPEN;
	}

#ifdef __DEBUG__ // -------- debug

	long lMin = 0, lMax = 0, lStep = 0, lDef = 0, lFlg = 0, lCur = 0;
	bool bIsAuto;

	// セット前
	m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_Saturation, &lMin, &lMax, &lStep, &lDef, &lFlg);
	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_Saturation, &lCur, &bIsAuto );
	qDebug() << "Before Params";
	outPutDebug( lMin, lMax, lStep, lDef, lFlg, lCur, bIsAuto );

	qDebug() << "Satuation set : " << lParam;

#endif

	// セット
	res = m_pclGraphManager->setParam(CC_VideoProcAmp, VideoProcAmp_Saturation, lParam, false);

#ifdef __DEBUG__

	// セット後
	m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_Saturation, &lMin, &lMax, &lStep, &lDef, &lFlg);
	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_Saturation, &lCur, &bIsAuto );
	qDebug() << "After Params";
	outPutDebug( lMin, lMax, lStep, lDef, lFlg, lCur, bIsAuto );

#endif

	if(res != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}

	return IM_ERROR_SUCCESS;

}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	シャープネス
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotSetSharoness(qint32 lParam)
{
	bool res;

	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return IM_ERROR_NOT_OPEN;
	}

#ifdef __DEBUG__ // -------- debug

	long lMin = 0, lMax = 0, lStep = 0, lDef = 0, lFlg = 0, lCur = 0;
	bool bIsAuto;

	// セット前
	m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_Sharpness, &lMin, &lMax, &lStep, &lDef, &lFlg);
	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_Sharpness, &lCur, &bIsAuto );
	qDebug() << "Before Params";
	outPutDebug( lMin, lMax, lStep, lDef, lFlg, lCur, bIsAuto );

	qDebug() << "Sharpness set : " << lParam;

#endif

	// セット
	res = m_pclGraphManager->setParam(CC_VideoProcAmp, VideoProcAmp_Sharpness, lParam, false);

#ifdef __DEBUG__

	// セット後
	m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_Sharpness, &lMin, &lMax, &lStep, &lDef, &lFlg);
	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_Sharpness, &lCur, &bIsAuto );
	qDebug() << "After Params";
	outPutDebug( lMin, lMax, lStep, lDef, lFlg, lCur, bIsAuto );

#endif

	if(res != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}

	return IM_ERROR_SUCCESS;
}


/*! ------------------------------------------------------------------------------------- //
//
//		@brief	ガンマ値
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotSetGamma(qint32 lParam)
{
	bool res;

	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return IM_ERROR_NOT_OPEN;
	}

#ifdef __DEBUG__ // -------- debug

	long lMin = 0, lMax = 0, lStep = 0, lDef = 0, lFlg = 0, lCur = 0;
	bool bIsAuto;

	// セット前
	m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_Gamma, &lMin, &lMax, &lStep, &lDef, &lFlg);
	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_Gamma, &lCur, &bIsAuto );
	qDebug() << "Before Params";
	outPutDebug( lMin, lMax, lStep, lDef, lFlg, lCur, bIsAuto );

	qDebug() << "gamma set : " << lParam;

#endif

	// セット
	res = m_pclGraphManager->setParam(CC_VideoProcAmp, VideoProcAmp_Gamma, lParam, false);

#ifdef __DEBUG__

	// セット後
	m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_Gamma, &lMin, &lMax, &lStep, &lDef, &lFlg);
	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_Gamma, &lCur, &bIsAuto );
	qDebug() << "After Params";
	outPutDebug( lMin, lMax, lStep, lDef, lFlg, lCur, bIsAuto );

#endif

	if(res != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}

	return IM_ERROR_SUCCESS;
}


/*! ------------------------------------------------------------------------------------- //
//
//		@brief	ホワイトバランス（色温度
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotSetWB(qint32 lParam)
{
	bool res;

	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return IM_ERROR_NOT_OPEN;
	}

	// 10の位で四捨五入
	if( (lParam % 100) > 50 )
	{
		lParam = lParam - (lParam % 100);
	}
	else
	{
		lParam = lParam - (lParam % 100) + 100;
	}


#ifdef __DEBUG__
	long lMin = 0, lMax = 0, lStep = 0, lDef = 0, lFlg = 0, lCur = 0;
	bool bIsAuto;
	m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_WhiteBalance, &lMin, &lMax, &lStep, &lDef, &lFlg);
	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_WhiteBalance, &lCur, &bIsAuto );
	qDebug() << "Before Params";
	outPutDebug( lMin, lMax, lStep, lDef, lFlg, lCur, bIsAuto );
#endif

	res = m_pclGraphManager->setParam(CC_VideoProcAmp, VideoProcAmp_WhiteBalance, lParam, false);
	emit sigSetCheckedAutoWB(false);

#ifdef __DEBUG__
	m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_WhiteBalance, &lMin, &lMax, &lStep, &lDef, &lFlg);
	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_WhiteBalance, &lCur, &bIsAuto );
	qDebug() << "After Params";
	outPutDebug( lMin, lMax, lStep, lDef, lFlg, lCur, bIsAuto );
#endif

	if(res != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}

	return IM_ERROR_SUCCESS;

}


/*! ------------------------------------------------------------------------------------- //
//
//		@brief	ホワイトバランス オート
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotSetWBAuto(bool bEnable)
{
	int res = true;

	long lCurrent;
	bool bIsAuto;

	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return IM_ERROR_NOT_OPEN;
	}

	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_WhiteBalance, &lCurrent, &bIsAuto );

	res = m_pclGraphManager->setParam(CC_VideoProcAmp, VideoProcAmp_WhiteBalance, lCurrent, bEnable );

	if(res != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}

	return IM_ERROR_SUCCESS;

}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	逆光補正
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotSetBacklight(qint32 lParam)
{
	bool res;

	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return IM_ERROR_NOT_OPEN;
	}

#ifdef __DEBUG__ // -------- debug

	long lMin = 0, lMax = 0, lStep = 0, lDef = 0, lFlg = 0, lCur = 0;
	bool bIsAuto;

	// セット前
	m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_BacklightCompensation, &lMin, &lMax, &lStep, &lDef, &lFlg);
	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_BacklightCompensation, &lCur, &bIsAuto );
	qDebug() << "Before Params";
	outPutDebug( lMin, lMax, lStep, lDef, lFlg, lCur, bIsAuto );

	qDebug() << "Backlight set : " << lParam;

#endif

	// セット
	res = m_pclGraphManager->setParam(CC_VideoProcAmp, VideoProcAmp_BacklightCompensation, lParam, false);

#ifdef __DEBUG__

	// セット後
	m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_BacklightCompensation, &lMin, &lMax, &lStep, &lDef, &lFlg);
	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_BacklightCompensation, &lCur, &bIsAuto );
	qDebug() << "After Params";
	outPutDebug( lMin, lMax, lStep, lDef, lFlg, lCur, bIsAuto );

#endif

	if(res != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}

	return IM_ERROR_SUCCESS;
}


/*! ------------------------------------------------------------------------------------- //
//
//		@brief	ゲイン
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotSetGain(qint32 lParam)
{
	bool res;

	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return IM_ERROR_NOT_OPEN;
	}

#ifdef __DEBUG__ // -------- debug

	long lMin = 0, lMax = 0, lStep = 0, lDef = 0, lFlg = 0, lCur = 0;
	bool bIsAuto;

	// セット前
	m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_Gain, &lMin, &lMax, &lStep, &lDef, &lFlg);
	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_Gain, &lCur, &bIsAuto );
	qDebug() << "Before Params";
	outPutDebug( lMin, lMax, lStep, lDef, lFlg, lCur, bIsAuto );

	qDebug() << "Gain set : " << lParam;

#endif

	// セット
	res = m_pclGraphManager->setParam(CC_VideoProcAmp, VideoProcAmp_Gain, lParam, false);

#ifdef __DEBUG__

	// セット後
	m_pclGraphManager->getParamRange(CC_VideoProcAmp ,VideoProcAmp_Gain, &lMin, &lMax, &lStep, &lDef, &lFlg);
	m_pclGraphManager->getParam(CC_VideoProcAmp, VideoProcAmp_Gain, &lCur, &bIsAuto );
	qDebug() << "After Params";
	outPutDebug( lMin, lMax, lStep, lDef, lFlg, lCur, bIsAuto );

#endif

	if(res != S_OK){
		return IM_ERROR_COMMAND_FAILURE;	// エラー
	}

	return IM_ERROR_SUCCESS;
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	Volume(0~100)
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotSetVolume(int iParam)
{
#ifdef _AUDIO_MUTE_SET_BY_VOLUME_
	m_iLastVolume = iParam;
	emit sigSetMuteCheck(false);
#endif
	// 値域チェック
	if( iParam < 0 || iParam > 100 )
	{
		return IM_ERROR_INVALID_PARAMETER;			// 無効な値だった
	}

	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return IM_ERROR_NOT_OPEN;
	}

	HRESULT hr;										// 結果格納用
	DOUBLE	dVol;									// グラフマネージャの引数(0.0~1.0)に合わせる用

	dVol = static_cast<double>(iParam) / 100;		// グラフマネージャの引数に合わせる
	hr = m_pclGraphManager->setCurrentVolume(dVol);	// セット

	// 結果チェック
	if(hr != S_OK){
		return IM_ERROR_COMMAND_FAILURE;			// エラー
	}

	return IM_ERROR_SUCCESS;						// 成功
}

/*! ------------------------------------------------------------------------------------- //
//
//		@brief	Mute
//
//  ------------------------------------------------------------------------------------- */
qint32 ClPresenterUVC::slotSetMute(bool bIsEnable)
{
	// ばかよけ
	if(m_pclGraphManager == NULL)
	{
		return IM_ERROR_NOT_OPEN;
	}

#ifndef _AUDIO_MUTE_SET_BY_VOLUME_
	HRESULT hr;										// 結果格納用
	bool	bSetVal;								// グラフマネージャの引数合わせるよう

	//　グラフマネージャ用に値をセット（true falseを反転）
	if( bIsEnable == true )
	{
		bSetVal = false;							// ミュートONならオーディオ無効に
	}
	else
	{
		bSetVal = true;								// ミュートOFFならオーディオ有効に
	}

	// グラフマネージャからセット
	hr = m_pclGraphManager->setCurrentVolume(bSetVal);

	// 結果チェック
	if(hr != S_OK){
		return IM_ERROR_COMMAND_FAILURE;			// エラー
	}

	return IM_ERROR_SUCCESS;						// 成功
#else

	qint32 iParam;
	if( bIsEnable == true )
	{
		iParam = 0.0;
	}
	else
	{
		iParam = m_iLastVolume;
	}

	HRESULT hr;										// 結果格納用
	DOUBLE dVol;

	dVol = static_cast<double>(iParam) / 100;		// グラフマネージャの引数に合わせる
	hr = m_pclGraphManager->setCurrentVolume(dVol);	// セット

	// 結果チェック
	if(hr != S_OK){
		return IM_ERROR_COMMAND_FAILURE;			// エラー
	}

	return IM_ERROR_SUCCESS;						// 成功

#endif
}


//--------------------------------------------------------------------------------
/*!
	@fn			execCommand
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterUVC::execCommand(ClUsbCmd &clCmd)
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
qint32 ClPresenterUVC::getModelName(QVariant &qParam)
{
	qParam = QString("");						//output model name to QVariant

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			getVersion
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterUVC::getVersion(QVariant &qParam)
{
	Q_UNUSED(qParam)
	int res = IM_ERROR_SUCCESS;

	if( res != IM_ERROR_SUCCESS){
		return IM_ERROR_COMMAND_FAILURE;					//エラー返却
	}

	return IM_ERROR_SUCCESS;								//output version to QVariant
}

void ClPresenterUVC::setTranslator()
{
	ClDeviceInfo::setTranslator();

	emit sigSetTextRemoconExpose( WORD_UVC_REMOCON_EXPOSE );		// 露出
	emit sigSetTextRemoconContrast( WORD_UVC_REMOCON_CONTRAST );	// コントラスト
	emit sigSetTextRemoconHue( WORD_UVC_REMOCON_HUE );				// 色相
	emit sigSetTextRemoconSatuation( WORD_UVC_REMOCON_SATUATION );	// 彩度
	emit sigSetTextRemoconSharpness( WORD_UVC_REMOCON_SHARPNESS );	// 鮮明度
	emit sigSetTextRemoconGamma( WORD_UVC_REMOCON_GAMMA );			// ガンマ
	emit sigSetTextRemoconWB( WORD_UVC_REMOCON_WB );				// ホワイトバランス
	emit sigSetTextRemoconBacklight( WORD_UVC_REMOCON_BACKLIGHT );	// 逆光補正
	emit sigSetTextRemoconGain( WORD_UVC_REMOCON_GAIN );			// ゲイン
	emit sigSetTextRemoconVolume( WORD_UVC_REMOCON_VOLUME );		// ボリューム

	if( NULL != m_pclZoomTeleBtn )
	{
		m_pclZoomTeleBtn->setToolTip(WORD_UVC_REMOCON_ZOOM_TELE);
	}
	if( NULL != m_pclZoomWideBtn )
	{
		m_pclZoomWideBtn->setToolTip(WORD_UVC_REMOCON_ZOOM_WIDE);
	}
	if( NULL != m_pclFocusNearBtn )
	{
		m_pclFocusNearBtn->setToolTip(WORD_UVC_REMOCON_FOCUS_NEAR);
	}
	if( NULL != m_pclFocusFarBtn )
	{
		m_pclFocusFarBtn->setToolTip(WORD_UVC_REMOCON_FOCUS_FAR);
	}
	if( NULL != m_pclFocusAutoBtn )
	{
		m_pclFocusAutoBtn->setToolTip(WORD_UVC_REMOCON_FOCUS_AUTO);
	}
	if( NULL != m_pclBrightOpenBtn )
	{
		m_pclBrightOpenBtn->setToolTip(WORD_UVC_REMOCON_BRIGHTNESS_OPEN);
	}
	if( NULL != m_pclBrightCloseBtn )
	{
		m_pclBrightCloseBtn->setToolTip(WORD_UVC_REMOCON_BRIGHTNESS_CLOSE);
	}
	if( NULL != m_pclBrightDefBtn )
	{
		m_pclBrightDefBtn->setToolTip(WORD_UVC_REMOCON_BRIGHTNESS_DEF);
	}
	if( NULL != m_pclWBAuto )
	{
		m_pclWBAuto->setText(WORD_UVC_REMOCON_AUTO);
	}
	if( NULL != m_pclMute )
	{
		m_pclMute->setText(WORD_UVC_REMOCON_MUTE);
	}
}

//--------------------------------------------------------------------------------
/*!
	@fn			readData
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint64	ClPresenterUVC::readData ( char * data, qint64 maxSize )
{
#ifdef __DEBUG__
	qDebug() << sizeof(data) << maxSize;
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
qint64	ClPresenterUVC::writeData ( const char * data, qint64 maxSize )
{
#ifdef __DEBUG__
	qDebug() << sizeof(data) << maxSize;
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
void ClPresenterUVC::convertBGRtoRGB( const unsigned char* bgr,
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

