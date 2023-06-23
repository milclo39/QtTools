#include "mac/ClPresenterMO1.h"
#include "ClUsbDriver/ClUsbDriver_mac.h"
#include "ClUsbDriver/Command_Presenter.h"
#include "Common/CmnDefine.h"
#include "MOCameraLib/MOCameraLib.h"

#import <Cocoa/Cocoa.h>
#import <QTKit/QTKit.h>

#define MO_PRODUCT_NAME						("ELMO UVC/UAC Camera")					//!< 機種名 変更：L-12iのUVC/UACモードにも対応するため、一般的な名前にする
#define INTERVAL_DEFAULT					(-1)									//!< Stepの値の初期値
#define CAMPARAM_DEFAULT					(-1)									//!< 保持する値(ズーム値など)の初期値
#define EXP_CREATE_RESOLUTION_STRING(x,y)	( QString("%1 x %2").arg(x).arg(y) )	//!< 解像度を表す文字列の作成

#if 1
#define	DEFAULT_AUDIO_USE															//デフォルトのオーディオ入力を使用する場合は有効にする
#endif


//--------------------------------------------------------------------------------
//	ヘッダーで定義するとなぜかコンパイルが通らないので、暫定的にグローバルで定義
//--------------------------------------------------------------------------------
static MOCameraLib				*g_pclMOCameraCtrl	= nil;							//MO-1制御用

resolution_t*					g_pResolutionList	= nil;							//resokution list

QTCaptureSession*				g_pclCaptureSession	= nil;							//capture session

QTCaptureDeviceInput*			g_pclAudioDeviceIn	= nil;							//capture device input (audio)
QTCaptureAudioPreviewOutput*	g_pclCaptureAudioOut= nil;							//capture device output (audio)


//--------------------------------------------------------------------------------
/*!
	@brief		ClMODeviceCtrl
 */
//--------------------------------------------------------------------------------
@interface ClMODeviceCtrl : NSObject
{
	ClPresenterMO1 *m_pclCamCtrl;
}
@end

@implementation ClMODeviceCtrl
//MO-1接続
- (void)MOOpenProc:(id)refCon
{
#ifdef DISPLAY_TRACE
	NSLog( @"call back ::: open" );
#endif
	if( m_pclCamCtrl )																//nilチェック
	{
		m_pclCamCtrl->MOCameraLibCall( true );										//開く
	}
}
//MO-1切断
- (void)MOCloseProc:(id)refCon
{
#ifdef DISPLAY_TRACE
	NSLog( @"call back ::: close" );
#endif
	if( m_pclCamCtrl )																//nilチェック
	{
		m_pclCamCtrl->MOCameraLibCall( false );										//閉じる
	}
}
//生成元を記憶
- (void)SetIns:(void*)refCon
{
	if( nil != refCon )																//nilチェック
	{
		m_pclCamCtrl = (ClPresenterMO1 *)refCon;									//生成元を記憶
	}
}
@end


//--------------------------------------------------------------------------------
/*!
	@brief		USB Device Add Remove CallBack
 */
//--------------------------------------------------------------------------------
void cb_MOCameraLibCall(void *refCon, BOOL Add_Remove )
{
#ifdef DISPLAY_TRACE
	qDebug("call back ::: start");
#endif
	
	//----------------------------------------------------------------------//
	//	Pool準備
	//----------------------------------------------------------------------//
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];						//pool初期化

	//----------------------------------------------------------------------//
	//	ClPresenterMO1を取得
	//----------------------------------------------------------------------//
	ClPresenterMO1 *pclCamCtrl = (ClPresenterMO1 *)refCon;							//生成元を記憶

	//----------------------------------------------------------------------//
	//	関数呼び出し
	//----------------------------------------------------------------------//
	ClMODeviceCtrl *pclDevCtrl = [[ClMODeviceCtrl alloc] init];
	[pclDevCtrl SetIns:refCon];
	if( Add_Remove )																//接続された場合
	{
#ifdef DISPLAY_TRACE
		qDebug("call back ::: connect");
#endif
		//オープン済みかどうかを確認
		if( true == pclCamCtrl->isOpen() )
		{
			return;
		}

		//待ってからオープン処理実行
		[pclDevCtrl performSelector:@selector(MOOpenProc:)							//サンプルソースにあわせて待ち時間7sec
									withObject:nil
									afterDelay:7];
	}
	else																			//切断された場合
	{
#ifdef DISPLAY_TRACE
		qDebug("call back ::: disconnect");
#endif
		//クローズ済みかどうかを確認
		if( false == pclCamCtrl->isOpen() )
		{
			return;
		}

		//待ってからクローズ処理実行
		[pclDevCtrl performSelector:@selector(MOCloseProc:)							//サンプルソースにあわせて待ち時間10msec
									withObject:nil
									afterDelay:0.01];
	}
	
	//----------------------------------------------------------------------//
	//	Pool解放
	//----------------------------------------------------------------------//
	[pool release];																	//pool解放

#ifdef DISPLAY_TRACE
	qDebug("call back ::: end");
#endif
}

//--------------------------------------------------------------------------------
/*!
	@brief		引数つきコンストラクタ
	@param[in]	pqParent	親クラス
 */
//--------------------------------------------------------------------------------
ClPresenterMO1::ClPresenterMO1( QObject *pqParent ) :
	ClDeviceInfo( PRODUCT_MOBILE, pqParent )
{
	//----------------------------------------------------------------------//
	//	接続
	//----------------------------------------------------------------------//
	connect( &m_clTimer, SIGNAL(timeout()), this, SLOT(slotTimeout()) );

	//----------------------------------------------------------------------//
	//	メンバー変数初期化
	//----------------------------------------------------------------------//
	m_lCurrentCmd			= -1;
	m_lFocusInterval		= INTERVAL_DEFAULT;
	m_lZoomInterval			= INTERVAL_DEFAULT;
	m_lBrightnessInterval	= INTERVAL_DEFAULT;
	m_bIsConnect			= false;
	
	//----------------------------------------------------------------------//
	//	カメラ制御のステップを設定 (現状では取得するコマンドが無いため決めうち)
	//----------------------------------------------------------------------//
	//step
	m_lFocusInterval		= 10;													//現状でのMO-1に設定されているフォーカスのステップ値
	m_lZoomInterval			= 10;													//現状でのMO-1に設定されているズームのステップ値
	m_lBrightnessInterval	= 1;													//現状でのMO-1に設定されている明るのステップ値
	//default
	m_lFocusDefault			= 43;													//現状でのMO-1に設定されているフォーカスのデフォルト値 (default:43,min:0,max:255)
	m_lZoomDefault			= 0;													//現状でのMO-1に設定されているズームのデフォルト値 (default:0,min:0,max:255)
	m_lBrightnessDefault	= 6;													//現状でのMO-1に設定されている明るさのデフォルト値 (default:6,min:0,max:14)
	//current
	m_lFocusCurrent			= CAMPARAM_DEFAULT;										//現在設定されているフォーカス値
	m_lZoomCurrent			= CAMPARAM_DEFAULT;										//現在設定されているズーム値
	m_lBrightnessCurrent	= CAMPARAM_DEFAULT;										//現在設定されている明るさ

	//----------------------------------------------------------------------//
	//	MO-1制御用メンバー変数 初期化
	//----------------------------------------------------------------------//
	m_pcImageData			= nil;													//image data
	m_iCurrentResolution	= 0;													//resolution

	//----------------------------------------------------------------------//
	//	画像データのメモリ確保
	//----------------------------------------------------------------------//
	m_pcImageData = (char*) malloc( 1920 * 1080 * 4 );								//サンプルが決め打ちなのでひとまず決め打ちにした

	//----------------------------------------------------------------------//
	//	MO-1制御用クラスの初期化
	//----------------------------------------------------------------------//
	init();																			//初期化
	
	return;
}

//--------------------------------------------------------------------------------
/*!
	@brief		デストラクタ
 */
//--------------------------------------------------------------------------------
ClPresenterMO1::~ClPresenterMO1()
{
	//----------------------------------------------------------------------//
	//	MO-1を閉じる
	//----------------------------------------------------------------------//
	close();
	
	//----------------------------------------------------------------------//
	//	MO-1制御メンバー変数解放
	//----------------------------------------------------------------------//
	//画像データ解放
	if( m_pcImageData )																//画像データ
	{
		free( m_pcImageData );														//解放
		m_pcImageData = nil;														//初期化
	}
	//解像度リスト解放
	if( g_pResolutionList )															//解像度リスト
	{
		free( g_pResolutionList );													//解放
		g_pResolutionList = nil;													//初期化
	}

	//----------------------------------------------------------------------//
	//	オーディオ
	//----------------------------------------------------------------------//
	//オーディオデバイス入力終了処理
	if( g_pclAudioDeviceIn )														//オーディオデバイス入力
	{
		[g_pclAudioDeviceIn release];												//解放
		g_pclAudioDeviceIn = nil;													//初期化
	}
	//オーディオ出力の終了処理
	if( g_pclCaptureAudioOut )														//オーディオ出力
	{
		[g_pclCaptureAudioOut release];												//解放
		g_pclCaptureAudioOut = nil;													//初期化
	}

	return;
}

//--------------------------------------------------------------------------------
/*!
 @brief		USB挿抜の定義
 */
//--------------------------------------------------------------------------------
void ClPresenterMO1::MOCameraLibCall( bool Add_Remove )
{
	//----------------------------------------------------------------------//
	//	UsbDriver生成
	//----------------------------------------------------------------------//
	ClUsbDriver *pclDriver = dynamic_cast<ClUsbDriver*>(parent());
	if(pclDriver == NULL)
	{
		return;
	}
	
	//----------------------------------------------------------------------//
	//	Connect/Disconnect 判定処理
	//----------------------------------------------------------------------//
	if( Add_Remove )
	{
#ifdef DISPLAY_TRACE
		qDebug("call back ::: usb driver open");
#endif
		if( IM_ERROR_SUCCESS != this->open() )										//実際に開く
		{
#ifdef DISPLAY_TRACE
			qDebug("call back ::: usb driver open ::: error");						//失敗
#endif
			pclDriver->close( PRODUCT_MOBILE );										//失敗したのでクローズ処理実行
		}
		else																		
		{
#ifdef DISPLAY_TRACE
			qDebug("call back ::: usb driver open ::: success");					//成功
#endif
			pclDriver->open( PRODUCT_MOBILE );										//成功したので開いたシグナル発信
		}
	}
	else
	{
#ifdef DISPLAY_TRACE
		qDebug("call back ::: usb driver close");
#endif
		pclDriver->close( PRODUCT_MOBILE );
	}
}

//--------------------------------------------------------------------------------
/*!
 @brief		初期化
 */
//--------------------------------------------------------------------------------
void ClPresenterMO1::init()
{
	//----------------------------------------------------------------------//
	//	Pool準備
	//----------------------------------------------------------------------//
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];						//pool初期化

	//----------------------------------------------------------------------//
	//	Frameworkの初期化
	//----------------------------------------------------------------------//
	g_pclMOCameraCtrl =																//Framework初期化
			[[MOCameraLib alloc] initWithCallback:cb_MOCameraLibCall Param:this];
	if( g_pclMOCameraCtrl == nil )													//nilかを確認
	{
#ifdef DISPLAY_TRACE
		qDebug("MOCameraLib init error");
#endif
		return;																		//失敗
	}
	else
	{
#ifdef DISPLAY_TRACE
		qDebug("MOCameraLib init success");
#endif
	}

	//----------------------------------------------------------------------//
	//	Pool解放
	//----------------------------------------------------------------------//
	[pool release];																	//pool解放
}

//--------------------------------------------------------------------------------
/*!
	@brief		セッション開始
 */
//--------------------------------------------------------------------------------
void ClPresenterMO1::startSession()
{
	//----------------------------------------------------------------------//
	//	セッション再開
	//----------------------------------------------------------------------//
	if(g_pclCaptureSession)															//nilかを確認
	{
		if(![g_pclCaptureSession isRunning])										//セッションが停止しているかを確認
		{
			[g_pclCaptureSession startRunning];										//セッションを開始
		}
	}
}

//--------------------------------------------------------------------------------
/*!
	@brief		セッション停止
 */
//--------------------------------------------------------------------------------
void ClPresenterMO1::stopSession()
{
	//----------------------------------------------------------------------//
	//	セッションを一度停止
	//----------------------------------------------------------------------//
	if(g_pclCaptureSession)															//nilかを確認
	{
		if([g_pclCaptureSession isRunning])											//セッションは動いているかを確認
		{
			[g_pclCaptureSession stopRunning];										//セッションが動いていたら一端停止する
		}
	}
}

//--------------------------------------------------------------------------------
/*!
	@brief		Audio デバイス
 */
//--------------------------------------------------------------------------------
bool ClPresenterMO1::openAudioDevice()
{
	BOOL	success = NO;
	NSError	*error	= nil;

	//----------------------------------------------------------------------//
	//	Pool準備
	//----------------------------------------------------------------------//
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];						//pool初期化

	//----------------------------------------------------------------------//
	//	セッションを一度停止
	//----------------------------------------------------------------------//
	stopSession();																	//セッション停止	

#ifndef DEFAULT_AUDIO_USE
	//======================================================================//
	//	オーディオデバイスをMO-1/ELMOで指定する場合
	//======================================================================//
	//----------------------------------------------------------------------//
	//	使用可能なオーディオ入力デバイスを取得
	//----------------------------------------------------------------------//
	NSArray *devices	= nil;													//デバイスリスト
	NSUInteger index	= 0;													//インデックス
	NSUInteger devCnt	= 0;													//デバイスリストに登録されている数
	//現状のオーディオデバイスを取得
	devices =																	//デバイスリスト取得
		[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeSound];
	//登録されているオーディオ入力デバイスの個数を取得
	devCnt = [devices count];													//デバイス数取得
	//オーディオ入力デバイス確認処理
	QTCaptureDevice *audioDevice = nil;											//オーディオデバイス
	for( index = 0; index < devCnt; index++ )
	{
		//オーディオデバイス取り出し
		audioDevice = [devices objectAtIndex:index];							//指定indexのデバイス取得

		//デバイス名で判定
		NSString *strCurrent= nil;
		strCurrent = [audioDevice localizedDisplayName];
#ifdef DISPLAY_TRACE
		NSLog(@"audio device name [%d / %d]: %@",index+1,devCnt,strCurrent);	//現在のオーディオデバイス名
#endif
		NSRange range_MO1 = [strCurrent rangeOfString:@"MO-1"];
		NSRange range_ELMO = [strCurrent rangeOfString:@"ELMO"];
		if( range_MO1.location != NSNotFound || range_ELMO.location != NSNotFound )
		{
			break;																//MO-1/ELMOが見つかったので検索終了
		}
		else
		{
			audioDevice = nil;													//この後の確認でひっかかるように初期化
		}
	}
	if( audioDevice == nil )													//nilかを確認
	{
		[pool release];															//解放
		startSession();															//セッション再開
		return false;															//失敗
	}
	//----------------------------------------------------------------------//
	//	オーディオデバイスを開く
	//----------------------------------------------------------------------//
	success = [audioDevice open:&error];										//開く
	if(!success)
	{
		qDebug("audio device open error");
		audioDevice = nil;														//初期化
		[pool release];															//解放
		startSession();															//セッション再開
		return false;															//失敗
	}

	//----------------------------------------------------------------------//
	//	オーディオデバイス名の判定
	//----------------------------------------------------------------------//
	NSString *strDeviceName= nil;
	strDeviceName = [audioDevice localizedDisplayName];
#ifdef DISPLAY_TRACE
	NSLog(@"audio device name : %@",strDeviceName);								//オープンされたオーディオ名
#endif
	NSRange rangeCheck_MO1 = [strDeviceName rangeOfString:@"MO-1"];
	NSRange rangeCheck_ELMO = [strDeviceName rangeOfString:@"ELMO"];
	if( rangeCheck_MO1.location == NSNotFound && rangeCheck_ELMO.location == NSNotFound )
	{
		qDebug("audio device open error");
		[pool release];															//解放
		startSession();															//セッション再開
		return false;															//失敗
	}
#else
	//======================================================================//
	//	デフォルトで設定されているオーディオ入力デバイスを使用する場合
	//======================================================================//
	//----------------------------------------------------------------------//
	//	デフォルトのオーディオ入力を取得
	//----------------------------------------------------------------------//
	QTCaptureDevice *audioDevice =
		[QTCaptureDevice defaultInputDeviceWithMediaType:QTMediaTypeSound];
	if( audioDevice == nil )													//nilかを確認
	{
		qDebug("audio device is nil");
		[pool release];															//解放
		startSession();															//セッション再開
		return false;															//失敗
	}

	//----------------------------------------------------------------------//
	//	オーディオデバイスを開く
	//----------------------------------------------------------------------//
	success = [audioDevice open:&error];										//開く
	if(!success)
	{
		qDebug("audio device open error");
		audioDevice = nil;														//初期化
		[pool release];															//解放
		startSession();															//セッション再開
		return false;															//失敗
	}
#endif //DEFAULT_AUDIO_USE
	//----------------------------------------------------------------------//
	//	オーディオ入力として登録
	//----------------------------------------------------------------------//
	if(audioDevice)
	{
#ifdef DISPLAY_TRACE
		NSString *strAudioName	= [audioDevice localizedDisplayName];
		NSLog(@"reg audio device name : %@",strAudioName);						//登録するオーディオ名
#endif
		//オーディオ入力
		g_pclAudioDeviceIn =
			[[QTCaptureDeviceInput alloc] initWithDevice:audioDevice];
		success =
			[g_pclCaptureSession addInput:g_pclAudioDeviceIn error:&error];
		if(!success)
		{
			qDebug("audio device input add error");
			[pool release];														//解放
			startSession();														//セッション再開
			return false;														//失敗
		}
		//オーディオ出力
		g_pclCaptureAudioOut = [[QTCaptureAudioPreviewOutput alloc] init];
#ifndef DEFAULT_AUDIO_USE
		[g_pclCaptureAudioOut setVolume:1.0];									//ひとまず音量はmaxにしておく
#else
		QTCaptureDevice *pclAudioDevice = [g_pclAudioDeviceIn device];
		NSString *strUniqueID = [pclAudioDevice uniqueID];
		NSString *strAudioName	= [audioDevice localizedDisplayName];
		NSLog(@"audio device -> unique id : %@ , display name : %@",strUniqueID, strAudioName);
		NSRange rangeCheckIn = [strUniqueID rangeOfString:@"AppleHDAEngineInput:1"];
		if( rangeCheckIn.location != NSNotFound )								//内蔵マイクかもしれない場合(外部マイクも含まれる)
		{
#if 0 //要望により音量0にはしないことになった
			qDebug("elmo audio device is not found. set volume is min");
			[g_pclCaptureAudioOut setVolume:0.00];								//音量をminにする
#else
			qDebug("elmo audio device is not found. set volume is 0.03");
			[g_pclCaptureAudioOut setVolume:0.03];								//内蔵マイク/内蔵スピーカーで激しくハウリングしない程度の音量にしておく
#endif
		}
		else																	//その他(USBなど)
		{
			qDebug("elmo audio device is found. set volume max");
			[g_pclCaptureAudioOut setVolume:1.00];								//音量をmaxにする
		}
#endif //DEFAULT_AUDIO_USE
		usleep(500000);															//これがないと2回目以降高確率で固まる(挿抜を素早くやるとどのみち固まる)
		success =
			[g_pclCaptureSession addOutput:g_pclCaptureAudioOut error:&error];
		if(!success)
		{
			qDebug("audio device output add error");
			[pool release];														//解放
			startSession();														//セッション再開
			return false;														//失敗
		}
	}

	//----------------------------------------------------------------------//
	//	セッション再開
	//----------------------------------------------------------------------//
	startSession();																	//セッション開始

	//----------------------------------------------------------------------//
	//	Pool解放
	//----------------------------------------------------------------------//
	[pool release];																	//解放

	return true;
}

//--------------------------------------------------------------------------------
/*!
	@brief		開く
	@note		オーバーライド
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterMO1::open()
{
	//----------------------------------------------------------------------//
	//	状態確認
	//----------------------------------------------------------------------//
	if( m_bIsConnect == true )
	{
		return IM_ERROR_SUCCESS;
	}
	
	//----------------------------------------------------------------------//
	//	ばかよけ
	//----------------------------------------------------------------------//
	if( g_pclMOCameraCtrl == nil )													//ばかよけ
	{
		qDebug("MOCameraLib is not initialize");
		return IM_ERROR_NOT_OPEN;
	}
	
	//----------------------------------------------------------------------//
	//	Pool準備
	//----------------------------------------------------------------------//
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];						//pool初期化

	//----------------------------------------------------------------------//
	//	開く
	//----------------------------------------------------------------------//
	if( [g_pclMOCameraCtrl OpenMO_1] )
	{
#ifdef DISPLAY_TRACE
		qDebug("open mo-1 error");
#endif
		g_pclCaptureSession = nil;													//セッション初期化
		g_pclAudioDeviceIn	= nil;													//オーディオデバイス入力初期化
		g_pclCaptureAudioOut= nil;													//オーディオ出力初期化
		[pool release];																//pool解放
		return false;
	}
	else
	{
#ifdef DISPLAY_TRACE
		qDebug("open mo-1 success");
#endif		
		g_pclCaptureSession	= g_pclMOCameraCtrl->captureSession;					//capture session
	}

	//----------------------------------------------------------------------//
	//	Audio Device 設定
	//----------------------------------------------------------------------//
	if( false == openAudioDevice() )												//Audio Device 関連準備
	{
#ifdef DISPLAY_TRACE
		qDebug("audio setting error");												//オーディオは無くても動くので失敗してもそのまま続行
#endif
	}

	//----------------------------------------------------------------------//
	//	成功したので開いていることが確認できるように設定する
	//----------------------------------------------------------------------//
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

	//----------------------------------------------------------------------//
	//	Pool解放
	//----------------------------------------------------------------------//
	[pool release];																	//pool解放

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
	//----------------------------------------------------------------------//
	//	状態確認
	//----------------------------------------------------------------------//
	if( m_bIsConnect == false )
	{
		return;
	}

	//----------------------------------------------------------------------//
	//	Pool準備
	//----------------------------------------------------------------------//
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];						//pool初期化

	//----------------------------------------------------------------------//
	//	セッション
	//----------------------------------------------------------------------//
	if( g_pclCaptureSession )
	{
		//----------------------------------------------------------------------//
		//	セッションを停止
		//----------------------------------------------------------------------//
		stopSession();																//停止

		//----------------------------------------------------------------------//
		//	セッションに登録している入出力デバイスを消す
		//----------------------------------------------------------------------//
		[g_pclCaptureSession removeInput: [g_pclCaptureSession inputs]];			//デバイス入力を削除
		[g_pclCaptureSession removeOutput: [g_pclCaptureSession outputs]];			//デバイス出力を削除

		//----------------------------------------------------------------------//
		//	オーディオ
		//----------------------------------------------------------------------//
		//オーディオの終了処理
		if( g_pclAudioDeviceIn )													//nilかを確認
		{
			[g_pclAudioDeviceIn release];											//解放
			g_pclAudioDeviceIn = nil;												//初期化
		}
		//オーディオ出力の終了処理
		if( g_pclCaptureAudioOut )														//オーディオ出力
		{
			[g_pclCaptureAudioOut release];												//解放
			g_pclCaptureAudioOut = nil;													//初期化
		}
		
		//----------------------------------------------------------------------//
		//	セッションを再開
		//----------------------------------------------------------------------//
		startSession();																//開始
	}

	//----------------------------------------------------------------------//
	//	MO-1を閉じる
	//----------------------------------------------------------------------//
	if( [g_pclMOCameraCtrl CloseMO_1] )
	{
		qDebug("close mo-1 error");
	}
	else
	{
		qDebug("close mo-1 success");
	}

	//----------------------------------------------------------------------//
	//	MO-成功したので閉じていることが確認できるように設定する
	//----------------------------------------------------------------------//
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

	//----------------------------------------------------------------------//
	//	Pool解放
	//----------------------------------------------------------------------//
	[pool release];																	//pool解放

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
	if( nil == g_pclMOCameraCtrl || false == m_bIsConnect )
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
        case IRIS_INITIALIZE:                                                       //明るさ(Initialize) L-12i対応のため追加 2013.01.23
        {
            lRet = irisInitialize( qParameter );
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
	QStringList clListResolution;

	//ばかよけ
	if( g_pclMOCameraCtrl == nil )													//ばかよけ
	{
		return QStringList();
	}

	//Pool準備
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];						//pool初期化

	//解像度リストの個数を取得
	UInt32	count = 0;
	[g_pclMOCameraCtrl GetResolutionList:(resolution_t*)&count];					//リストの数を取得

	//解像度リストを取得
	g_pResolutionList =																//リストのサイズに合わせて領域確保
		(resolution_t*)malloc( sizeof(resolution_t) + sizeof(g_pResolutionList->SizeInfo) * ( count - 1 ) );
	g_pResolutionList->Count = count;												//リストを本取得するために個数を設定(これがないとリスト取得できない)
	[g_pclMOCameraCtrl GetResolutionList:g_pResolutionList];						//解像度リストを取得

	for( UInt32 i = 0; i < count; i++ )												//リストの作成
	{
		UInt32 width	= g_pResolutionList->SizeInfo[i].width;						//幅
		UInt32 height	= g_pResolutionList->SizeInfo[i].height;					//高さ
		QString clStrRes= EXP_CREATE_RESOLUTION_STRING( width, height );			//文字列に変換
		if(false == clListResolution.contains(clStrRes)){
			clListResolution.append( clStrRes );										//リストに登録
		}
#ifdef DISPLAY_TRACE
		qDebug() << "Supported Resolution :: " << clStrRes;
#endif
	}

	//Pool解放
	[pool release];																	//pool解放

	return clListResolution;
}

//--------------------------------------------------------------------------------
/*!
		@brief	現在の解像度を取得
 */
//--------------------------------------------------------------------------------
QString ClPresenterMO1::getCurrentResolution()
{
	QString strResolution;
	UInt32 width, height;

	//ばかよけ
	if( g_pclMOCameraCtrl == nil )													//ばかよけ
	{
		return QString();
	}

	//解像度を取得
	[g_pclMOCameraCtrl GetResolution:&width Height:&height];						//現在設定されている解像度を取得

	//文字列に変換
	strResolution = EXP_CREATE_RESOLUTION_STRING( width, height );					//文字列に変換

#ifdef DISPLAY_TRACE
	qDebug() << "Get Current Resolution :: " << strResolution;
#endif

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
	UInt32		width,height;
	qint32		iWidth	= 0;														//幅格納用
	qint32		iHeight	= 0;														//高さ格納用

	//ばかよけ
	if( g_pclMOCameraCtrl == nil )													//ばかよけ
	{
		return false;
	}

	//現在の解像度を確認
	QString clCurrent = getCurrentResolution();
	if( strResolution == clCurrent )												//Mac OS のUVCデバイスドライバで覚えている値がばかになっている場合に備えて確認
	{
#ifdef DISPLAY_TRACE
		qDebug("this resolution is already set.");
#endif
		//同じであった場合は一端デフォルトに設定しておく
		if( clCurrent != "640 x 480" ){												//デフォルトでない場合
			width	= 640;															//デフォルトの幅
			height	= 480;															//デフォルトの高さ
		}else{																		//デフォルトであった場合
			width	= 1280;															//MO-1で対応しているデフォルト以外の幅 (値は対応しているものなら何でもよい)
			height	= 800;															//MO-1で対応しているデフォルト以外の高さ (値は対応しているものなら何でもよい)
		}

		//設定
		[g_pclMOCameraCtrl SetResolution:width Height:height];						//解像度設定 (あくまでMac OSの覚えている値が間違っていた時の対策なので成功/失敗は問わない)
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

	//変換
	width	= (UInt32)iWidth;														//幅を格納
	height	= (UInt32)iHeight;														//高さを格納

#ifdef DISPLAY_TRACE
	qDebug() << "Set Resolution :: " << width << height;
#endif

	//解像度の設定
	if( [g_pclMOCameraCtrl SetResolution:width Height:height] )						//解像度設定
	{
		return false;																//失敗
	}
	else
	{
		return true;																//成功
	}
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

	//画像を取得
	QByteArray clArray;
	UInt32 width, height;

	//ばかよけ
	if( g_pclMOCameraCtrl == nil )													//ばかよけ
	{
		return IM_ERROR_CAPTURE_FAILURE;
	}

	//画像の取得
	if( ![g_pclMOCameraCtrl GetImage:m_pcImageData Width:&width Height:&height] )
	{
		//Pool準備
		NSAutoreleasePool *nsPool = [[NSAutoreleasePool alloc] init];

		//バッファ準備
		unsigned char *buff[ 4 ] = {0};
		buff[ 0 ] = (unsigned char*)m_pcImageData;

		//Bitmap生成
		NSBitmapImageRep  *bitmap = nil;
		bitmap =				  [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:buff
								   pixelsWide:width
								   pixelsHigh:height
								   bitsPerSample:8
								   samplesPerPixel:4
								   hasAlpha:YES
								   isPlanar:NO
								   colorSpaceName:NSCalibratedRGBColorSpace
								   bitmapFormat:NSAlphaFirstBitmapFormat
								   bytesPerRow:(4 * width)
								   bitsPerPixel:32];
		if( bitmap == nil )
		{
			qDebug("bitmap is nil");
			qParam.clear();
			m_usWidth		= 0;
			m_usHeight		= 0;
			m_iPictureSize	= 0;
			return lRet;
		}
		[bitmap autorelease];

		//JPGに変換
		NSData *jpgData;
		NSDictionary *properties;
#if 0
		BOOL interlaced;
		properties = [NSDictionary
					  dictionaryWithObject:[NSNumber numberWithBool:interlaced]
					  forKey:NSImageInterlaced];
#else
		properties = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithFloat:0.5],
					  NSImageCompressionFactor, nil];
#endif
		jpgData = nil;
		jpgData = [bitmap representationUsingType:NSJPEGFileType properties:properties];

		const char *buffer_jpg = reinterpret_cast<const char*>([jpgData bytes]);	//データポインタ取得
		NSUInteger length = [jpgData length];										//データサイズ取得
		clArray = QByteArray(reinterpret_cast<const char*>(buffer_jpg), length);	//バッファにコピー			

		//画像サイズ等を記録
		qParam			= clArray;													//画像データを格納
		m_usWidth		= width;													//幅
		m_usHeight		= height;													//高さ
		m_iPictureSize	= length;													//サイズ						

#ifdef DISPLAY_TRACE
		//デスクトップにJPG保存
		NSMutableString *path;
		path = [NSMutableString stringWithString:NSHomeDirectory()];
		[path appendString:@"/Desktop/test.jpg"];
		[jpgData writeToFile:path atomically:YES];
#endif

		//Pool解放
		[nsPool release];
	}
	else
	{
		qParam.clear();
		m_usWidth		= 0;
		m_usHeight		= 0;
		m_iPictureSize	= 0;
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
	if( nil == g_pclMOCameraCtrl || false == m_bIsConnect )
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
	Q_UNUSED(qParam)																//未使用定義

	bool bIsAF	= false;															//AF状態格納用

	//AF情報取得
	bIsAF = this->getAutoFucus();													//AF情報取得
	if( bIsAF == true )
	{
		this->setAutoFucus( false );												//一度AF設定をOFFにする
		usleep(1000);																//待ち(※環境によってAFが効かなくなることがあるための対処)
	}

	//AF設定
	this->setAutoFucus( true );														//AF設定をONにする

	return IM_ERROR_SUCCESS;
}

//--------------------------------------------------------------------------------
/*!
	@brief		フォーカス Near
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterMO1::focusNear( QVariant &qParam )
{
	Q_UNUSED(qParam)																//未使用定義

	int iValue	= 0;																//現在の値格納
	int iMin	= 0;																//Minimum
	int iMax	= 0;																//Maximum

	//AF設定をOFFにする
	this->setAutoFucus( false );													//フォーカスを手動変更するためにAF設定をOFFにする

	//Focus情報取得
	this->getFocusInf( iValue, iMin, iMax );										//Focus情報取得
	if( iMin <= iValue && iValue < iMax )
	{
		iValue += m_lFocusInterval;													//Focus位置を変更
        if(iValue > iMax)iValue = iMax;                                             //範囲超えたら境界値に設定　2013.01.24追加
	}
    else
	{
        stopCmdTimer();																//念のためタイマー終了
        return IM_ERROR_SUCCESS;                                                    //既に境界値・範囲外の場合はなにもせず抜ける。2013.01.24修正
	}

	//Focus設定
	this->setFocus( iValue );														//Focus設定

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
	Q_UNUSED(qParam)																//未使用定義

	int iValue	= 0;																//現在の値格納
	int iMin	= 0;																//Minimum
	int iMax	= 0;																//Maximum

	//AF設定をOFFにする
	this->setAutoFucus( false );													//フォーカスを手動変更するためにAF設定をOFFにする

	//Focus情報取得
	this->getFocusInf( iValue, iMin, iMax );										//Focus情報取得
	if( iMin < iValue && iValue <= iMax )
	{
		iValue -= m_lFocusInterval;													//Focus位置を変更
        if(iValue < iMin)iValue = iMin;                                             //範囲超えたら境界値に設定　2013.01.24追加
	}
	else
	{
        stopCmdTimer();																//念のためタイマー終了
        return IM_ERROR_SUCCESS;                                                    //既に境界値・範囲外の場合はなにもせず抜ける。2013.01.24修正
	}

	//Focus設定
	this->setFocus( iValue );														//Focus設定

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
	Q_UNUSED(qParam)																//未使用定義

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
	Q_UNUSED(qParam)																//未使用定義

	int iValue	= 0;																//現在の値格納
	int iMin	= 0;																//Minimum
	int iMax	= 0;																//Maximum

	//Zoom情報取得
	this->getZoomInf( iValue, iMin, iMax );											//Zoom情報取得
	if( iMin <= iValue && iValue < iMax )
	{
		iValue += m_lZoomInterval;													//Zoom位置を変更
        if(iValue > iMax)iValue = iMax;                                             //範囲超えたら境界値に設定　2013.01.24追加
    }
	else
	{
        stopCmdTimer();                                                             //念のためタイマー終了
        return IM_ERROR_SUCCESS;                                                    //既に境界値・範囲外の場合はなにもせず抜ける。2013.01.24修正
	}

	//Zoom設定
	this->setZoom( iValue );														//Zoom設定

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
	Q_UNUSED(qParam)																//未使用定義

	int iValue	= 0;																//現在の値格納
	int iMin	= 0;																//Minimum
	int iMax	= 0;																//Maximum

	//Zoom情報取得
	this->getZoomInf( iValue, iMin, iMax );											//Zoom情報取得
	if( iMin < iValue && iValue <= iMax )
	{
		iValue -= m_lZoomInterval;													//Zoom位置を変更
        if(iValue < iMin)iValue = iMin;                                             //範囲超えたら境界値に設定　2013.01.24追加
    }
	else
	{
        stopCmdTimer();																//念のためタイマー終了
        return IM_ERROR_SUCCESS;                                                    //既に境界値・範囲外の場合はなにもせず抜ける。2013.01.24修正
	}

	//Zoom設定
	this->setZoom( iValue );														//Zoom設定

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
	Q_UNUSED(qParam)																//未使用定義

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
	Q_UNUSED(qParam)																//未使用定義

	int iValue	= 0;																//現在の値格納
	int iMin	= 0;																//Minimum
	int iMax	= 0;																//Maximum

	//Brightness情報取得
	this->getBrightnessInf( iValue, iMin, iMax );									//Brightness情報取得
	if( iMin <= iValue && iValue < iMax )
	{
		iValue += m_lBrightnessInterval;											//Brightness位置を変更
        if(iValue > iMax)iValue = iMax;                                             //範囲超えたら境界値に設定　2013.01.24追加
	}
	else
	{
        stopCmdTimer();                                                             //念のためタイマー終了
        return IM_ERROR_SUCCESS;                                                    //既に境界値・範囲外の場合はなにもせず抜ける。2013.01.24修正
    }

	//Brightness設定
	this->setBrightness( iValue );													//Brightness設定

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
	Q_UNUSED(qParam)																//未使用定義

	int iValue	= 0;																//現在の値格納
	int iMin	= 0;																//Minimum
	int iMax	= 0;																//Maximum

	//Brightness情報取得
	this->getBrightnessInf( iValue, iMin, iMax );									//Brightness情報取得
	if( iMin < iValue && iValue <= iMax )
	{
		iValue -= m_lBrightnessInterval;											//Brightness位置を変更
        if(iValue < iMin)iValue = iMin;                                             //範囲超えたら境界値に設定　2013.01.24追加
	}
	else
	{
        stopCmdTimer();																//念のためタイマー終了
        return IM_ERROR_SUCCESS;                                                    //既に境界値・範囲外の場合はなにもせず抜ける。2013.01.24修正
    }

	//Brightness設定
	this->setBrightness( iValue );													//Brightness設定

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
	Q_UNUSED(qParam)																//未使用定義

    stopCmdTimer();     //タイマー終了
	return IM_ERROR_SUCCESS;
}

//--------------------------------------------------------------------------------
/*!
    @brief		明るさ Initialize
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterMO1::irisInitialize( QVariant &qParam )
{
    Q_UNUSED(qParam)																//未使用定義

    //Brightnessをデフォルトに設定
    this->setBrightness( m_lBrightnessDefault );

    return IM_ERROR_SUCCESS;
}

//--------------------------------------------------------------------------------
/*!
	@brief		AFの取得
 */
//--------------------------------------------------------------------------------
bool ClPresenterMO1::getAutoFucus()
{
	BOOL bValue;

	//ばかよけ
	if( g_pclMOCameraCtrl == nil )													//ばかよけ
	{
		return false;
	}

	//AF取得
	[g_pclMOCameraCtrl GetAutoFocus:&bValue];										//AF設定の取得

	if(bValue == NO)
	{
		return false;
	}
	else
	{
		return true;
	}
}

//--------------------------------------------------------------------------------
/*!
	@brief		AFの設定
 */
//--------------------------------------------------------------------------------
void ClPresenterMO1::setAutoFucus( bool bValue )
{
	//ばかよけ
	if( g_pclMOCameraCtrl == nil )													//ばかよけ
	{
		return;
	}

	//AF設定
	[g_pclMOCameraCtrl SetAutoFocus:bValue];										//AF設定の設定
}

//--------------------------------------------------------------------------------
/*!
	@brief		Focusの取得
 */
//--------------------------------------------------------------------------------
void ClPresenterMO1::getFocusInf( int &iValue, int &iMin, int &iMax )
{
	int value,min,max;

	//ばかよけ
	if( g_pclMOCameraCtrl == nil )													//ばかよけ
	{
		return;
	}

	//フォーカス取得
	[g_pclMOCameraCtrl GetFocusInfo:&value Min:&min Max:&max];						//Focus情報の取得
	iValue	= value;																//現在の値
	iMin	= min;																	//Minimum
	iMax	= max;																	//Maximum
}

//--------------------------------------------------------------------------------
/*!
	@brief		Focusの設定
 */
//--------------------------------------------------------------------------------
void ClPresenterMO1::setFocus( int iValue )
{
	//ばかよけ
	if( g_pclMOCameraCtrl == nil )													//ばかよけ
	{
		return;
	}

	//フォーカス設定
	[g_pclMOCameraCtrl SetFocus:iValue];											//Focusの設定
}

//--------------------------------------------------------------------------------
/*!
	@brief		Brightnessの取得
 */
//--------------------------------------------------------------------------------
void ClPresenterMO1::getBrightnessInf( int &iValue, int &iMin, int &iMax )
{
	int value,min,max;

	//ばかよけ
	if( g_pclMOCameraCtrl == nil )													//ばかよけ
	{
		return;
	}

	//明るさ取得
	[g_pclMOCameraCtrl GetBrightnessInfo:&value Min:&min Max:&max];					//Brightness情報の取得
	iValue	= value;																//現在の値
	iMin	= min;																	//Minimum
	iMax	= max;																	//Maximum
}

//--------------------------------------------------------------------------------
/*!
	@brief		Brightnessの設定
 */
//--------------------------------------------------------------------------------
void ClPresenterMO1::setBrightness( int iValue )
{
	//ばかよけ
	if( g_pclMOCameraCtrl == nil )													//ばかよけ
	{
		return;
	}

	//明るさ設定
	[g_pclMOCameraCtrl SetBrightness:iValue];										//Brightnessの設定
}

//--------------------------------------------------------------------------------
/*!
	@brief		Zoomの取得
 */
//--------------------------------------------------------------------------------
void ClPresenterMO1::getZoomInf( int &iValue, int &iMin, int &iMax )
{
	int value,min,max;

	//ばかよけ
	if( g_pclMOCameraCtrl == nil )													//ばかよけ
	{
		return;
	}

	//ズーム取得
	[g_pclMOCameraCtrl GetZoomInfo:&value Min:&min Max:&max];						//Zoom情報の取得
	iValue	= value;																//現在の値
	iMin	= min;																	//Minimum
	iMax	= max;																	//Maximum
}

//--------------------------------------------------------------------------------
/*!
	@brief		Zoomの設定
 */
//--------------------------------------------------------------------------------
void ClPresenterMO1::setZoom( int iValue )
{
	//ばかよけ
	if( g_pclMOCameraCtrl == nil )													//ばかよけ
	{
		return;
	}

	//ズーム設定
	[g_pclMOCameraCtrl SetZoom:iValue];												//Zoomの設定
}
