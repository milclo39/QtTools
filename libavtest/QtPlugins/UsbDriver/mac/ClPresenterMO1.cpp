#include "mac/ClPresenterMO1.h"
#include "ClUsbDriver/ClUsbDriver_mac.h"
#include "ClUsbDriver/Command_Presenter.h"
#include "Common/CmnDefine.h"

#define MO_PRODUCT_NAME		("MO-1/MO-1w")
#define INTERVAL_DEFAULT	(-1)

//--------------------------------------------------------------------------------
/*!
	@brief		引数つきコンストラクタ
	@param[in]	pqParent	親クラス
 */
//--------------------------------------------------------------------------------
ClPresenterMO1::ClPresenterMO1( QObject *pqParent ) :
	ClDeviceInfo( PRODUCT_MOBILE, pqParent ),
	m_pclCameraCtrl( NULL )
{
	//接続
	connect( &m_clTimer, SIGNAL(timeout()), this, SLOT(slotTimeout()) );

	//メンバー変数初期化
	m_lCurrentCmd			= -1;
	m_lFocusInterval		= INTERVAL_DEFAULT;
	m_lZoomInterval			= INTERVAL_DEFAULT;
	m_lBrightnessInterval	= INTERVAL_DEFAULT;
	m_bIsConnect			= false;
	
	//カメラ制御のステップを設定 (現状では取得するコマンドが無いため決めうち)
	m_lFocusInterval		= 10;													//現状でのMO-1に設定されているステップ値
	m_lZoomInterval			= 10;													//現状でのMO-1に設定されているステップ値
	m_lBrightnessInterval	= 1;													//現状でのMO-1に設定されているステップ値
	m_lBrightnessDefault	= 6;													//現状でのMO-1に設定されている明るさのデフォルト値
	
	return;
}

//--------------------------------------------------------------------------------
/*!
	@brief		デストラクタ
 */
//--------------------------------------------------------------------------------
ClPresenterMO1::~ClPresenterMO1()
{
	//MO-1クローズ
	close();
	
	//MO-1制御ライブラリを使用して閉じる
	if( NULL != m_pclCameraCtrl )
	{
		deleteMOCameraCtrlClass( m_pclCameraCtrl );
	}
	
	//USB挿抜関連
	if( NULL != m_pUsbDevice ){
		m_pUsbDevice->closeDevice();
		delete m_pUsbDevice;
		m_pUsbDevice = NULL;
	}
	
	return;
}

//--------------------------------------------------------------------------------
/*!
	@brief		開く
	@note		オーバーライド
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterMO1::open()
{	
	//機種をチェック
	if( m_lProductId != PRODUCT_MOBILE )											//MO-1であるかを確認
	{
#ifdef DISPLAY_TRACE
		qDebug("this camera is not mo-1.");
#endif
		return IM_ERROR_NO_DEVICE;													//失敗
	}

	//USBデバイス制御クラス生成/デバイスオープン
	if( NULL == m_pUsbDevice )														//NULLチェック
	{
		//USBデバイス制御クラス生成
		m_pUsbDevice = new ClIoDevice(m_lProductId, false);							//MO-1の場合 (オープンまではさせないので2つ目の引数はfalse)

		//デバイスクラスにクラス設定
		m_pUsbDevice->setDriver(dynamic_cast<ClUsbDriver*>(parent()));
		m_pUsbDevice->setDevinfo(this);
		if( m_pUsbDevice->openDevice() != TRUE ){									//デバイスファイルオープン
#ifdef DISPLAY_TRACE
			qDebug("UsbDevice openDevice error");
#endif
			return IM_ERROR_NO_DEVICE;
		}
	}
	if( NULL == m_pUsbDevice )														//生成できているかを確認
	{
#ifdef DISPLAY_TRACE
		qDebug("UsbDevice create error");
#endif
		return IM_ERROR_FATAL;
	}
	
	//MO-1制御クラス生成
	m_pclCameraCtrl = createMOCameraCtrlClass();
	if( NULL == m_pclCameraCtrl )
	{
#ifdef DISPLAY_TRACE
		qDebug("MOCameraCtrl create error");
#endif
		return IM_ERROR_NO_MEM;
	}

	//MO-1制御ライブラリを使用してオープンする
	if( false == m_pclCameraCtrl->openCamera() )
	{
#ifdef DISPLAY_TRACE
		qDebug("MOCameraCtrl openCamera error");
#endif
		return IM_ERROR_NOT_OPEN;
	}

	//成功したので開いていることが確認できるように設定する
	if( isOpen() == false )															//既に開かれていないかを確認
	{
		//QIODeviceをオープン
		if(QIODevice::open(ReadWrite | Unbuffered) == false){						//QIODeviceのOpen
#ifdef DISPLAY_TRACE
			qDebug("QIODevice open error");
#endif
			return IM_ERROR_NO_DEVICE;
		}
		
		//接続/切断フラグを接続状態に変更
		setConnected(true);															//接続状態(true)にする
	}
	
	return IM_ERROR_SUCCESS;
}

//--------------------------------------------------------------------------------
/*!
	@brief		閉じる
	@note		オーバーライド
 */
//--------------------------------------------------------------------------------
void ClPresenterMO1::close()
{	
	//MO-1制御ライブラリで開いたデバイスを閉じる
	if( NULL != m_pclCameraCtrl )													//生成されているかを確認
	{
		if( true == m_pclCameraCtrl->isOpen() )										//開いているかを確認
		{
			if( false == m_pclCameraCtrl->closeCamera() )							//閉じる
			{
#ifdef DISPLAY_TRACE
				qDebug("MOCameraCtrl closeCamera error");
#endif
				return;
			}
		}
	}
	
	//USB挿抜検出のための処理
	if( NULL != m_pUsbDevice ){
#ifdef DISPLAY_TRACE
		qDebug("close and delete UsbDevice");
#endif
		m_pUsbDevice->closeDevice();
	}

	//成功したので閉じていることが確認できるように設定する
	if( isOpen() == true )															//既に閉じていないかを確認
	{
#ifdef DISPLAY_TRACE
		qDebug("QIODevice close");
#endif
		//QIODeviceをクローズする
		QIODevice::close();															//call for rarent close function
		
		//接続/切断フラグを切断状態に変更
		setConnected(false);														//切断状態(false)にする
	}
	
	return;
}

//--------------------------------------------------------------------------------
/*!
	@brief		コマンド処理
	@note		オーバーライド
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterMO1::CommandPresenter( qint32  CmdID, QVariant  &qParameter )
{
	qint32  lRet = IM_ERROR_COMMAND_FAILURE;

	//オープンチェック
	if( NULL == m_pclCameraCtrl || false == m_bIsConnect )
	{
		stopCmdTimer();																//タイマー終了
		return IM_ERROR_NOT_OPEN;
	}
	
	//判定処理
	switch( CmdID )
	{
		case GET_PICTURE:															//画像取得
		{
			lRet = getPicture( qParameter );
			break;
		}
		case RELEASE_PICTURE:														//取得した画像領域の解放
		{
			lRet = IM_ERROR_SUCCESS;
			break;
		}
		case GET_SIZE_PICTURE:														//直前に取得したサイズの返却
		{
			qParameter = m_iPictureSize;
			lRet = IM_ERROR_SUCCESS;
			break;
		}
		case GET_RECT_PICTURE:														//画像の大きさ取得
		{
			qParameter.setValue<eKINDRECTID>(m_iPictureRect);
			lRet = IM_ERROR_SUCCESS;
			break;
		}
		case SET_RECT_PICTURE:														//画像の大きさ設定
		{
			if(qParameter.type() == QVariant::UserType &&
			   chINRANGE(SIZE_PICTURE_HIGH,qParameter.value<eKINDRECTID>(),RECTID_PICTURE_DEFAULT))
			{
				m_iPictureRect = qParameter.value<eKINDRECTID>();
				lRet = IM_ERROR_SUCCESS;
			}
			break;
		}
		case GET_MODEL_NAME:														//機種名取得
		{
			lRet = getModelName(qParameter);
			break;
		}
		case GET_MODEL_VERSION:														//バージョン取得
		{
			//======================================================//
			//	バージョン取得のコマンドは無いので成功を返しておく	//
			//======================================================//
			lRet = IM_ERROR_SUCCESS;
			break;
		}
		case GET_HEIGHT_PICTURE:													//現在の画像の縦幅
		{
			qParameter = m_usHeight;
			lRet = IM_ERROR_SUCCESS;
			break;
		}
		case GET_WIDTH_PICTURE:														//現在の画像の横幅
		{
			qParameter = m_usWidth;
			lRet = IM_ERROR_SUCCESS;
		}
		//フォーカス
		case FOCUS_AUTO:															//フォーカス(Auto)
		{
			lRet = focusAuto( qParameter );
			break;
		}
		case FOCUS_NEAR:															//フォーカス(Near)
		{
			lRet = focusNear( qParameter );
			break;
		}
		case FOCUS_FAR:																//フォーカス(Far)
		{
			lRet = focusFar( qParameter );
			break;
		}
		case FOCUS_STOP:															//フォーカス(Stop)
		{
			lRet = focusStop( qParameter );
			break;
		}
		//ズーム
		case ZOOM_WIDE:																//ズーム(Wide)
		{
			lRet = zoomWide( qParameter );
			break;
		}
		case ZOOM_TELE:																//ズーム(Tele)
		{
			lRet = zoomTele( qParameter );
			break;
		}
		case ZOOM_STOP:																//ズーム(Stop)
		{
			lRet = zoomStop( qParameter );
			break;
		}
		//明るさ
		case IRIS_OPEN:																//明るさ(Open)
		{
			lRet = irisOpen( qParameter );
			break;
		}
		case IRIS_CLOSE:															//明るさ(Close)
		{
			lRet = irisClose( qParameter );
			break;
		}
		case IRIS_STOP:																//明るさ(Stop)
		{
			lRet = irisStop( qParameter );
			break;
		}
		default:																	//その他
		{
			break;
		}
	}

	return lRet;
}

//--------------------------------------------------------------------------------//
/*!
		@brief	接続状態変更
 */
//--------------------------------------------------------------------------------//
void ClPresenterMO1::setConnected(bool bIsConnect)
{
	m_bIsConnect = bIsConnect;
	
	if( m_bIsConnect == true && isOpen() == false )
	{			
		this->open();
	}
}

//--------------------------------------------------------------------------------
/*!
		@brief	カメラ操作リモコンを返す
 */
//--------------------------------------------------------------------------------
QWidget* ClPresenterMO1::getRemoconWidget()
{
	return new QWidget;
}

//--------------------------------------------------------------------------------
/*!
		@brief	対応解像度を返す
 */
//--------------------------------------------------------------------------------
QStringList ClPresenterMO1::getSupportedResolutions()
{
	QStringList	clRetVal;
	clRetVal.clear();																//リストをクリア	
	if( NULL != m_pclCameraCtrl )													//CommandPresenterを経由しないので確認する
	{
		clRetVal = m_pclCameraCtrl->getResolutionList();							//対応する解像度リストを取得
	}	

	return clRetVal;
}

//--------------------------------------------------------------------------------
/*!
		@brief	現在の解像度を取得
 */
//--------------------------------------------------------------------------------
QString ClPresenterMO1::getCurrentResolution()
{
	QString strResolution;
	strResolution.clear();															//文字列をクリア
	if( NULL != m_pclCameraCtrl )													//CommandPresenterを経由しないので確認する
	{
		strResolution = m_pclCameraCtrl->getResolution();							//現在の解像度を取得
	}
	
	return strResolution;
}

//--------------------------------------------------------------------------------
/*!
		@brief	現在の解像度を設定
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterMO1::setCurrentResolution(QString strResolution)
{
	QStringList	clResolutions;														//引数分解用
	qint32		iWidth	= 0;														//幅格納用
	qint32		iHeight	= 0;														//高さ格納用

	//ばかよけ
	if( NULL == m_pclCameraCtrl )													//CommandPresenterを経由しないので確認する
	{
		return E_FAIL;
	}
	
	// 分解
	clResolutions	=	strResolution.split('x');									//受け取った解像度は文字列なので分解

	// 要素数チェック
	if(clResolutions.count() != 2)													//要素が2つであるかを確認
	{
		return E_FAIL;																//幅・高さで2つのはずなので違う場合は失敗にする
	}

	// 値セット
	iWidth	=	clResolutions.at(0).toInt();										//幅セット
	iHeight	=	clResolutions.at(1).toInt();										//高さセット

	return m_pclCameraCtrl->setResolution(iWidth, iHeight);
}

//--------------------------------------------------------------------------------
/*!
	@brief		画像取得
	@note		オーバーライド
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterMO1::getPicture( QVariant  &qParam )
{
	qint32	lRet = IM_ERROR_SUCCESS;

	//画像取得を試みる
	QByteArray clArray;
	clArray = m_pclCameraCtrl->getPicture();										//画像取得

	if( !clArray.isNull() )															//取得画像が空であるかを確認
	{
		qParam = clArray;															//画像データを格納
		m_usWidth		= m_pclCameraCtrl->getPictureWidth();						//幅を記録
		m_usHeight		= m_pclCameraCtrl->getPictureHeight();						//高さを記録
		m_iPictureSize	= m_pclCameraCtrl->getPictureSize();						//サイズを記録
	}

	return lRet;
}

//--------------------------------------------------------------------------------
/*!
	@brief		機種名取得
	@note		オーバーライド
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterMO1::getModelName( QVariant  &qParam )
{
	qParam = MO_PRODUCT_NAME;														//決め打ちで機種名を返す
	return IM_ERROR_SUCCESS;
}

//--------------------------------------------------------------------------------
/*!
	@brief		タイマー開始
 */
//--------------------------------------------------------------------------------
void ClPresenterMO1::startCmdTimer( qint32 lCmd )
{
	//同じコマンドならスルー
	if( lCmd == m_lCurrentCmd )
	{
		return;
	}

	//タイマーを終了させる
	stopCmdTimer();

	//Interval設定
	if( IRIS_OPEN == lCmd || IRIS_CLOSE == lCmd )
	{
		m_clTimer.setInterval( 500 );												//Winに合わせてIrisの場合は500msに設定
	}
	else
	{
		m_clTimer.setInterval( 100 );												//その他のコマンドは100msに設定
	}

	//開始
	m_lCurrentCmd = lCmd;
	m_clTimer.start();
}
//--------------------------------------------------------------------------------
/*!
	@brief		タイマー終了
 */
//--------------------------------------------------------------------------------
void ClPresenterMO1::stopCmdTimer()
{
	m_clTimer.stop();																//タイマー終了
	m_lCurrentCmd = -1;																//現在のコマンドとして記録していたものを初期化
}
//--------------------------------------------------------------------------------
/*!
	@brief		タイムアウト
 */
//--------------------------------------------------------------------------------
void ClPresenterMO1::slotTimeout()
{
	QVariant qParam;																//使用されていないので適当な定義して使用する

	//オープンチェック
	if( NULL == m_pclCameraCtrl || false == m_bIsConnect )
	{
		stopCmdTimer();																//タイマー終了
		return;
	}

	//判定処理
	switch( m_lCurrentCmd )
	{
		case FOCUS_NEAR:
		{
			focusNear( qParam );
			break;
		}
		case FOCUS_FAR:
		{
			focusFar( qParam );
			break;
		}
		case ZOOM_WIDE:
		{
			zoomWide( qParam );
			break;
		}
		case ZOOM_TELE:
		{
			zoomTele( qParam );
			break;
		}
		case IRIS_OPEN:
		{
			irisOpen( qParam );
			break;
		}
		case IRIS_CLOSE:
		{
			irisClose( qParam );
			break;
		}
		default:
		{
			break;
		}
	}

}

//--------------------------------------------------------------------------------
/*!
	@brief		フォーカス オート
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterMO1::focusAuto( QVariant &qParam )
{
	bool bIsAF	= false;
	
	//AF情報取得
	bIsAF = m_pclCameraCtrl->getAutoFucus();										//AF情報取得
	if( bIsAF == true )
	{
		m_pclCameraCtrl->setAutoFucus( false );										//一度AF設定をOFFにする
	}

	//AF設定
	m_pclCameraCtrl->setAutoFucus( true );											//AF設定をONにする

	return IM_ERROR_SUCCESS;
}

//--------------------------------------------------------------------------------
/*!
	@brief		フォーカス Near
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterMO1::focusNear( QVariant &qParam )
{
	int iValue	= 0;
	int iMin	= 0;
	int iMax	= 0;

	//AF設定をOFFにする
	m_pclCameraCtrl->setAutoFucus( false );											//フォーカスを手動変更するためにAF設定をOFFにする

	//Focus情報取得
	m_pclCameraCtrl->getFocusInf( iValue, iMin, iMax );								//Focus情報取得
	if( iMin <= iValue && iValue < iMax )
	{
		iValue += m_lFocusInterval;													//Focus位置を変更
	}
	else
	{
		stopCmdTimer();																//タイマー終了
		return IM_ERROR_COMMAND_FAILURE;											//値が不正なのでエラーを返す
	}

	//Focus設定
	m_pclCameraCtrl->setFocus( iValue );											//Focus設定

	//タイマー開始
	startCmdTimer( FOCUS_NEAR );

	return IM_ERROR_SUCCESS;
}

//--------------------------------------------------------------------------------
/*!
	@brief		フォーカス Far
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterMO1::focusFar( QVariant &qParam )
{
	int iValue	= 0;
	int iMin	= 0;
	int iMax	= 0;

	//AF設定をOFFにする
	m_pclCameraCtrl->setAutoFucus( false );											//フォーカスを手動変更するためにAF設定をOFFにする

	//Focus情報取得
	m_pclCameraCtrl->getFocusInf( iValue, iMin, iMax );								//Focus情報取得
	if( iMin < iValue && iValue <= iMax )
	{
		iValue -= m_lFocusInterval;													//Focus位置を変更
	}
	else
	{
		stopCmdTimer();																//タイマー終了
		return IM_ERROR_COMMAND_FAILURE;											//値が不正なのでエラーを返す
	}

	//Focus設定
	m_pclCameraCtrl->setFocus( iValue );											//Focus設定

	//タイマー開始
	startCmdTimer( FOCUS_FAR );

	return IM_ERROR_SUCCESS;
}

//--------------------------------------------------------------------------------
/*!
	@brief		フォーカス Stop
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterMO1::focusStop( QVariant &qParam )
{
	stopCmdTimer();																	//タイマー終了
	return IM_ERROR_SUCCESS;
}

//--------------------------------------------------------------------------------
/*!
	@brief		ズーム Tele
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterMO1::zoomTele( QVariant &qParam )
{
	int iValue	= 0;
	int iMin	= 0;
	int iMax	= 0;

	//Zoom情報取得
	m_pclCameraCtrl->getZoomInf( iValue, iMin, iMax );								//Zoom情報取得
	if( iMin <= iValue && iValue < iMax )
	{
		iValue += m_lZoomInterval;													//Zoom位置を変更
	}
	else
	{
		stopCmdTimer();																//タイマー終了
		return IM_ERROR_COMMAND_FAILURE;											//値が不正なのでエラーを返す
	}

	//Zoom設定
	m_pclCameraCtrl->setZoom( iValue );												//Zoom設定

	//タイマー開始
	startCmdTimer( ZOOM_TELE );

	return IM_ERROR_SUCCESS;
}

//--------------------------------------------------------------------------------
/*!
	@brief		ズーム Wide
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterMO1::zoomWide( QVariant &qParam )
{
	int iValue	= 0;
	int iMin	= 0;
	int iMax	= 0;

	//Zoom情報取得
	m_pclCameraCtrl->getZoomInf( iValue, iMin, iMax );								//Zoom情報取得
	if( iMin < iValue && iValue <= iMax )
	{
		iValue -= m_lZoomInterval;													//Zoom位置を変更
	}
	else
	{
		stopCmdTimer();																//タイマー終了
		return IM_ERROR_COMMAND_FAILURE;											//値が不正なのでエラーを返す
	}

	//Zoom設定
	m_pclCameraCtrl->setZoom( iValue );												//Zoom設定

	//タイマー開始
	startCmdTimer( ZOOM_WIDE );

	return IM_ERROR_SUCCESS;
}

//--------------------------------------------------------------------------------
/*!
	@brief		ズーム Stop
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterMO1::zoomStop( QVariant &qParam )
{
	stopCmdTimer();																	//タイマー終了
	return IM_ERROR_SUCCESS;
}

//--------------------------------------------------------------------------------
/*!
	@brief		明るさ オープン
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterMO1::irisOpen( QVariant &qParam )
{
	int iValue	= 0;
	int iMin	= 0;
	int iMax	= 0;

	//Brightness情報取得
	m_pclCameraCtrl->getBrightnessInf( iValue, iMin, iMax );						//Brightness情報取得
	if( iMin <= iValue && iValue < iMax )
	{
		iValue += m_lBrightnessInterval;											//Brightness位置を変更
	}
	else
	{
		stopCmdTimer();																//タイマー終了
		return IM_ERROR_COMMAND_FAILURE;											//値が不正なのでエラーを返す
	}

	//Brightness設定
	m_pclCameraCtrl->setBrightness( iValue );										//Brightness設定

	//タイマー開始
	startCmdTimer( IRIS_OPEN );

	return IM_ERROR_SUCCESS;
}

//--------------------------------------------------------------------------------
/*!
	@brief		明るさ クローズ
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterMO1::irisClose( QVariant &qParam )
{
	int iValue	= 0;
	int iMin	= 0;
	int iMax	= 0;

	//Brightness情報取得
	m_pclCameraCtrl->getBrightnessInf( iValue, iMin, iMax );						//Brightness情報取得
	if( iMin < iValue && iValue <= iMax )
	{
		iValue -= m_lBrightnessInterval;											//Brightness位置を変更
	}
	else
	{
		stopCmdTimer();																//タイマー終了
		return IM_ERROR_COMMAND_FAILURE;											//値が不正なのでエラーを返す
	}

	//Brightness設定
	m_pclCameraCtrl->setBrightness( iValue );										//Brightness設定

	//タイマー開始
	startCmdTimer( IRIS_CLOSE );

	return IM_ERROR_SUCCESS;
}

//--------------------------------------------------------------------------------
/*!
	@brief		明るさ Stop
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterMO1::irisStop( QVariant &qParam )
{
	stopCmdTimer();																	//タイマー終了
	return IM_ERROR_SUCCESS;
}
