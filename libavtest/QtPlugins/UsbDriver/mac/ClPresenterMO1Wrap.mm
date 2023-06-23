#include <QPixmap>

#if 0
#include "ClPresenterMO1Wrap.hpp"													//MO-1用Frameworkのヘッダをインクルードするとhppだとエラーになる
#else
#include "ClPresenterMO1Wrap.h"														//MO-1用Frameworkを使うにあたり拡張子をhの変更
#endif
#include "MOCameraLib/MOCameraLib.h"

#define EXP_CREATE_RESOLUTION_STRING(x,y)	( QString("%1 x %2").arg(x).arg(y) )	//!< 解像度を表す文字列の作成

static MOCameraLib *g_pclMOCameraCtrl = nil;										//MO-1制御用

//--------------------------------------------------------------------------------
/*!
 @brief		pool
 */
//--------------------------------------------------------------------------------
class QNSAutoReleasePool
{
private:
	void *m_pool;
public:
	QNSAutoReleasePool();
	~QNSAutoReleasePool();
	inline void *handle() const { return m_pool; }
};
QNSAutoReleasePool::QNSAutoReleasePool() : m_pool([[NSAutoreleasePool alloc] init])
{
}
QNSAutoReleasePool::~QNSAutoReleasePool()
{
	[static_cast<NSAutoreleasePool*>(m_pool) release];
}


//--------------------------------------------------------------------------------
/*!
	@brief		MO-1/MO-1w制御クラス定義
 */
//--------------------------------------------------------------------------------
class ClMOCameraCtrl : public IMOCameraCtrl
{
public:
	ClMOCameraCtrl( void ) ;
	~ClMOCameraCtrl( void );

	virtual bool			openCamera( void );										//open
	virtual bool			closeCamera( void );									//close

	virtual bool			isOpen(){return m_bIsOpen;}								//check open

	virtual QByteArray		getPicture( void );										//get picture

	virtual QStringList		getResolutionList( void );								//get resolution list
	virtual QString			getResolution();										//get current resolution
	virtual bool			setResolution( int iWidth, int iHeight );				//set current resolution

	virtual bool			getAutoFucus( void );									//get auto focus state (false:off,true:on)
	virtual void			setAutoFucus( bool bValue );							//set auto focus state (false:off,true:on)

	virtual void			getFocusInf( int &iValue, int &iMin, int &iMax );		//get focus information
	virtual void			setFocus( int iValue );									//set focus

	virtual void			getBrightnessInf( int &iValue, int &iMin, int &iMax );	//get brightness information
	virtual void			setBrightness( int iValue );							//set brightness

	virtual void			getZoomInf( int &iValue, int &iMin, int &iMax );		//get zoom information
	virtual void			setZoom( int iValue );									//set zoom

private:
	void					init();													//initialzie

	void					startSession();											//start capture session
	void					stopSession();											//stop capture session

	bool					openAudioDevice();										//open audio device

private:
	int						m_iCurrentResolution;									//current resolution
	resolution_t*			m_pResolutionList;										//resokution list

	QTCaptureSession*		m_pclCaptureSession;									//capture session
	QTCaptureDevice*		m_pclVideoDevice;										//capture device (video)
	QTCaptureDeviceInput*	m_pclVideoDeviceIn;										//capture device input (video)
	QTCaptureDeviceInput*	m_pclAudioDeviceIn;										//capture device input (audio)
	QTCaptureAudioPreviewOutput* m_pclCaptureAudioOut;								//capture device output (audio)
};


//--------------------------------------------------------------------------------
/*!
	@brief		MO-1/MO-1w制御クラス生成
 */
//--------------------------------------------------------------------------------
IMOCameraCtrl* createMOCameraCtrlClass()
{
	return new ClMOCameraCtrl;
}


//--------------------------------------------------------------------------------
/*!
	@brief		MO-1/MO-1w制御クラス削除
 */
//--------------------------------------------------------------------------------
void deleteMOCameraCtrlClass( IMOCameraCtrl *pclMOCameraCtrl )
{
	if( pclMOCameraCtrl )
	{
		delete pclMOCameraCtrl;
		pclMOCameraCtrl = NULL;
	}
	return;
}


//********************************************************************************
/*!
	@class		MO-1/MO-1制御クラス
*/
//********************************************************************************
//--------------------------------------------------------------------------------
/*!
	@brief		コンストラクタ
 */
//--------------------------------------------------------------------------------
ClMOCameraCtrl::ClMOCameraCtrl()
{
	//初期化
	m_bIsOpen			= false;													//is open flag

	m_pcImageData		= nil;														//image data

	m_iCurrentResolution= 0;														//resolution
	m_pResolutionList	= nil;														//resolution list

	m_pclCaptureSession	= nil;														//capture session
	m_pclVideoDevice	= nil;														//capture device (video)
	m_pclVideoDeviceIn	= nil;														//capture device input (video)
	m_pclAudioDeviceIn	= nil;														//capture device input (audio)
	m_pclCaptureAudioOut= nil;														//capture device output (audio)

	//初期化
	init();																			//初期化	

	//画像データのメモリ確保
	m_pcImageData = (char*) malloc( 1920 * 1080 * 4 );								//サンプルが決め打ちなのでひとまず決め打ちにした
}

//--------------------------------------------------------------------------------
/*!
	@brief		デストラクタ
 */
//--------------------------------------------------------------------------------
ClMOCameraCtrl::~ClMOCameraCtrl()
{
	//----------------------------------------------------------------------//
	//	制御関連
	//----------------------------------------------------------------------//
#if 0																				//これがあると_NSAutoreleaseNoPoolの警告がでる
	//MO-1制御解放
	if( g_pclMOCameraCtrl )															//MOCameraLib
	{
		[g_pclMOCameraCtrl release];												//解放
		g_pclMOCameraCtrl = nil;													//初期化
	}
#endif	
	//画像データ解放
	if( m_pcImageData )																//画像データ
	{
		free( m_pcImageData );														//解放
		m_pcImageData = nil;														//初期化
	}
	//解像度リスト解放
	if( m_pResolutionList )															//解像度リスト
	{
		free( m_pResolutionList );													//解放
		m_pResolutionList = nil;													//初期化
	}

	//----------------------------------------------------------------------//
	//	ビデオ
	//----------------------------------------------------------------------//
	//ビデオデバイス入力終了処理
	if( m_pclVideoDeviceIn )														//ビデオデバイス入力
	{
		[m_pclVideoDeviceIn release];												//解放
		m_pclVideoDeviceIn = nil;													//初期化
	}

	//----------------------------------------------------------------------//
	//	オーディオ
	//----------------------------------------------------------------------//
	//オーディオデバイス入力終了処理
	if( m_pclAudioDeviceIn )														//オーディオデバイス入力
	{
		[m_pclAudioDeviceIn release];												//解放
		m_pclAudioDeviceIn = nil;													//初期化
	}
	//オーディオ出力の終了処理
	if( m_pclCaptureAudioOut )														//オーディオ出力
	{
		[m_pclCaptureAudioOut release];												//解放
		m_pclCaptureAudioOut = nil;													//初期化
	}
}

//--------------------------------------------------------------------------------
/*!
 @brief		初期化
 */
//--------------------------------------------------------------------------------
void ClMOCameraCtrl::init()
{
	//----------------------------------------------------------------------//
	//	Frameworkの初期化
	//----------------------------------------------------------------------//
	g_pclMOCameraCtrl = [[MOCameraLib alloc] init];									//Framework初期化
	if( g_pclMOCameraCtrl == nil )													//nilかを確認
	{
#ifdef DISPLAY_TRACE
		qDebug("MOCameraLib init error");
#endif		
		return;																		//失敗
	}
}

//--------------------------------------------------------------------------------
/*!
	@brief		セッション開始
 */
//--------------------------------------------------------------------------------
void ClMOCameraCtrl::startSession()
{
	//----------------------------------------------------------------------//
	//	セッション再開
	//----------------------------------------------------------------------//
	if(m_pclCaptureSession)															//nilかを確認
	{
		if(![m_pclCaptureSession isRunning])										//セッションが停止しているかを確認
		{
			[m_pclCaptureSession startRunning];										//セッションを開始
		}
	}
}

//--------------------------------------------------------------------------------
/*!
	@brief		セッション停止
 */
//--------------------------------------------------------------------------------
void ClMOCameraCtrl::stopSession()
{
	//----------------------------------------------------------------------//
	//	セッションを一度停止
	//----------------------------------------------------------------------//
	if(m_pclCaptureSession)															//nilかを確認
	{
		if([m_pclCaptureSession isRunning])											//セッションは動いているかを確認
		{
			[m_pclCaptureSession stopRunning];										//セッションが動いていたら一端停止する
		}
	}
}

//--------------------------------------------------------------------------------
/*!
	@brief		オーディオ設定
 */
//--------------------------------------------------------------------------------
bool ClMOCameraCtrl::openAudioDevice()
{
	BOOL	success = NO;
	NSError	*error	= nil;
	UInt32	retryCnt= 0;

	//----------------------------------------------------------------------//
	//	Pool準備
	//----------------------------------------------------------------------//
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];						//pool初期化

	//----------------------------------------------------------------------//
	//	セッションを一度停止
	//----------------------------------------------------------------------//
	stopSession();																	//セッション停止

	//----------------------------------------------------------------------//
	//	ビデオ
	//----------------------------------------------------------------------//
	//ビデオデバイス終了処理
	if(m_pclVideoDevice)															//nilかを確認
	{	
		[m_pclVideoDevice release];													//解放
		m_pclVideoDevice = nil;														//初期化
	}
	//ビデオデバイス生成
	m_pclVideoDevice =																//muxedでビデオデバイス生成
		[QTCaptureDevice defaultInputDeviceWithMediaType:QTMediaTypeMuxed];
	success = [m_pclVideoDevice open:&error];										//開く
	if(!success)
	{
		qDebug("video device [muxed] open error");									//注意：snow leopardでは対応していないので失敗するが問題は無い
		m_pclVideoDevice = nil;														//初期化
	}
	if(m_pclVideoDevice)															//nilかを確認
	{
		m_pclVideoDeviceIn =														//ビデオデバイス入力生成
			[[QTCaptureDeviceInput alloc] initWithDevice:m_pclVideoDevice];
		usleep(500000);																//追加前に少し待つ (これがないと追加時に固まりやすくなる)
		success =																	//セッションに追加
			[m_pclCaptureSession addInput:m_pclVideoDeviceIn error:&error];
	}
	
	//----------------------------------------------------------------------//
	//	オーディオ
	//----------------------------------------------------------------------//
	if(![m_pclVideoDevice hasMediaType:QTMediaTypeSound] &&							//タイプがSoundでないかを確認
	   ![m_pclVideoDevice hasMediaType:QTMediaTypeMuxed])							//タイプがMuxedでないかを確認
	{
		//----------------------------------------------------------------------//
		//	使用可能なオーディオ入力デバイスを取得
		//----------------------------------------------------------------------//
#if 1	//オーディオデバイスをMO-1で指定する場合
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
		while( 1 )
		{
			//オーディオデバイス取り出し
			audioDevice = [devices objectAtIndex:index];							//指定indexのデバイス取得
			index++;																//indexをインクリメント

			//デバイス名で判定
			NSString *strCurrent= nil;
			strCurrent = [audioDevice localizedDisplayName];			
#ifdef DISPLAY_TRACE
			NSLog(@"audio device name [%d / %d]: %@",index,devCnt,strCurrent);		//現在のオーディオデバイス名
#endif
			NSRange range = [strCurrent rangeOfString:@"MO-1"];
			if( range.location != NSNotFound )										//MO-1/MO-1wも考慮してMO-1を含むデバイスを検出
			{				
				break;																//MO-1が見つかったので検索終了
			}
			else
			{
				audioDevice = nil;													//この後の確認でひっかかるように初期化
				if( index == devCnt )
				{
					if( retryCnt >= 5 )												//リトライ上限を超えた
					{
						startSession();												//セッション再開
						return false;												//失敗
					}
					usleep(1000000);												//MO-1が無かったのでちょっと待つ
					devices =														//デバイスリスト取得
						[QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeSound];
					devCnt = [devices count];										//デバイス数取得
					index = 0;														//index初期化
					retryCnt++;														//リトライカウント更新
				}
			}
		}
#else	//環境設定->サウンド->入力 で選択されているデバイスを使用する場合
		QTCaptureDevice *audioDevice = 
			[QTCaptureDevice defaultInputDeviceWithMediaType:QTMediaTypeSound];
#endif
		if( audioDevice == nil )													//nilかを確認
		{
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
			startSession();															//セッション再開
			return false;															//失敗
		}

		//----------------------------------------------------------------------//
		//	オーディオ名称の判定
		//----------------------------------------------------------------------//
		NSString *strDeviceName= nil;
		strDeviceName = [audioDevice localizedDisplayName];	
#ifdef DISPLAY_TRACE
		NSLog(@"audio device name : %@",strDeviceName);								//オープンされたオーディオ名
#endif
		NSRange rangeCheck = [strDeviceName rangeOfString:@"MO-1"];
		if( rangeCheck.location == NSNotFound )										//名称にMO-1が入っていない場合
		{
			qDebug("audio device open error");
			startSession();															//セッション再開
			return false;															//失敗
		}

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
			m_pclAudioDeviceIn = 
				[[QTCaptureDeviceInput alloc] initWithDevice:audioDevice];
			success = 
				[m_pclCaptureSession addInput:m_pclAudioDeviceIn error:&error];
			if(!success)
			{
				qDebug("audio device input add error");
				startSession();														//セッション再開
				return false;														//失敗
			}
			
			//オーディオ出力
			m_pclCaptureAudioOut = [[QTCaptureAudioPreviewOutput alloc] init];
			[m_pclCaptureAudioOut setVolume:1.0];									//ひとまず音量はmaxにしておく	
			usleep(500000);															//これがないと2回目以降高確率で固まる(挿抜を素早くやるとどのみち固まる)
			success = 
				[m_pclCaptureSession addOutput:m_pclCaptureAudioOut error:&error];
			if(!success)
			{
				qDebug("audio device output add error");
				startSession();														//セッション再開
				return false;														//失敗
			}
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
	@brief		オープン
 */
//--------------------------------------------------------------------------------
bool ClMOCameraCtrl::openCamera()
{
	//----------------------------------------------------------------------//
	//	ばかよけ
	//----------------------------------------------------------------------//
	if( g_pclMOCameraCtrl == nil )													//ばかよけ
	{
		qDebug("MOCameraLib is not initialize");		
		return false;
	}

	//----------------------------------------------------------------------//
	//	Pool準備
	//----------------------------------------------------------------------//
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];						//pool初期化

	//----------------------------------------------------------------------//
	//	MO-1を開く
	//----------------------------------------------------------------------//
	if( [g_pclMOCameraCtrl OpenMO_1:0] )											//MO-1を開く
	{
#ifdef DISPLAY_TRACE
		qDebug("open mo-1 error");
#endif
		m_pclCaptureSession = nil;													//セッション初期化
		m_pclVideoDevice	= nil;													//ビデオデバイス初期化
		m_pclVideoDeviceIn	= nil;													//ビデオデバイス入力初期化
		m_pclAudioDeviceIn	= nil;													//オーディオデバイス入力初期化
		m_pclCaptureAudioOut= nil;													//オーディオ出力初期化
		return false;
	}
	else
	{
		//保持
		m_pclCaptureSession	= g_pclMOCameraCtrl->captureSession;					//capture session
		m_pclVideoDevice	= g_pclMOCameraCtrl->videoDevice;						//capture device
	}

	//----------------------------------------------------------------------//
	//	Audio設定
	//----------------------------------------------------------------------//
	if( false == openAudioDevice() )												//Audio関連準備
	{
#ifdef DISPLAY_TRACE
		qDebug("audio setting error");												//オーディオは無くても動くので失敗してもそのまま続行
#endif
	}

	//----------------------------------------------------------------------//
	//	Pool解放
	//----------------------------------------------------------------------//
	[pool release];																	//pool解放

	//----------------------------------------------------------------------//
	//	フラグ変更
	//----------------------------------------------------------------------//
	m_bIsOpen = true;
	
	return true;
}

//--------------------------------------------------------------------------------
/*!
 @brief		クローズ
 */
//--------------------------------------------------------------------------------
bool ClMOCameraCtrl::closeCamera()
{
	//----------------------------------------------------------------------//
	//	Pool準備
	//----------------------------------------------------------------------//
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];						//pool初期化
	
	//----------------------------------------------------------------------//
	//	セッション
	//----------------------------------------------------------------------//
	if( m_pclCaptureSession )
	{
		if( [m_pclCaptureSession isRunning] )										//状態を確認
		{
			//セッション停止
			[m_pclCaptureSession stopRunning];										//停止する
			
			//セッションに追加していた入出力デバイスを削除
			[m_pclCaptureSession removeInput: [m_pclCaptureSession inputs]];		//デバイス入力を削除
			[m_pclCaptureSession removeOutput: [m_pclCaptureSession outputs]];		//デバイス出力を削除
		}
	}

	//----------------------------------------------------------------------//
	//	ビデオ
	//----------------------------------------------------------------------//
	//ビデオデバイス終了処理
	if( m_pclVideoDevice )															//nilかを確認
	{
		[m_pclVideoDevice release];													//解放
		m_pclVideoDevice = nil;														//初期化
	}
	//ビデオデバイス入力終了処理
	if( m_pclVideoDeviceIn )														//nilかを確認
	{		
		[m_pclVideoDeviceIn release];												//解放
		m_pclVideoDeviceIn = nil;													//初期化
	}

	//----------------------------------------------------------------------//
	//	オーディオ
	//----------------------------------------------------------------------//
	//オーディオデバイス入力終了処理
	if( m_pclAudioDeviceIn )														//nilかを確認
	{
		[m_pclAudioDeviceIn release];												//解放
		m_pclAudioDeviceIn = nil;													//初期化
	}
	
	//----------------------------------------------------------------------//
	//	Pool解放
	//----------------------------------------------------------------------//
	[pool release];																	//pool解放

	//----------------------------------------------------------------------//
	//	フラグ変更
	//----------------------------------------------------------------------//
	m_bIsOpen = false;

	return true;
}

//--------------------------------------------------------------------------------
/*!
	@brief		対応解像度リストの取得
 */
//--------------------------------------------------------------------------------
QStringList ClMOCameraCtrl::getResolutionList()
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
	m_pResolutionList =																//リストのサイズに合わせて領域確保
		(resolution_t*)malloc( sizeof(resolution_t) + sizeof(m_pResolutionList->SizeInfo) * ( count - 1 ) );
	m_pResolutionList->Count = count;												//リストを本取得するために個数を設定(これがないとリスト取得できない)
	[g_pclMOCameraCtrl GetResolutionList:m_pResolutionList];						//解像度リストを取得

	for( UInt32 i = 0; i < count; i++ )												//リストの作成
	{
		UInt32 width	= m_pResolutionList->SizeInfo[i].width;						//幅
		UInt32 height	= m_pResolutionList->SizeInfo[i].height;					//高さ
		QString clStrRes= EXP_CREATE_RESOLUTION_STRING( width, height );			//文字列に変換
		clListResolution.append( clStrRes );										//リストに登録
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
	@brief		設定されている解像度の取得
 */
//--------------------------------------------------------------------------------
QString ClMOCameraCtrl::getResolution()
{
	QString clStrRes;
	UInt32 width, height;

	//ばかよけ
	if( g_pclMOCameraCtrl == nil )													//ばかよけ
	{
		return QString();
	}

	//解像度を取得
	[g_pclMOCameraCtrl GetResolution:&width Height:&height];						//現在設定されている解像度を取得

	//文字列に変換
	clStrRes = EXP_CREATE_RESOLUTION_STRING( width, height );						//文字列に変換

#ifdef DISPLAY_TRACE
	qDebug() << "Get Current Resolution :: " << clStrRes;
#endif

	return clStrRes;
}

//--------------------------------------------------------------------------------
/*!
	@brief		解像度の設定
 */
//--------------------------------------------------------------------------------
bool ClMOCameraCtrl::setResolution( int iWidth, int iHeight )
{
	UInt32 width,height;
	width	= (UInt32)iWidth;														//幅を格納
	height	= (UInt32)iHeight;														//高さを格納

	//ばかよけ
	if( g_pclMOCameraCtrl == nil )													//ばかよけ
	{
		return false;
	}

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
	@brief		画像の取得
 */
//--------------------------------------------------------------------------------
QByteArray ClMOCameraCtrl::getPicture()
{
	//画像を取得
	QByteArray clArray;
	UInt32 width, height;
	
	//ばかよけ
	if( g_pclMOCameraCtrl == nil )													//ばかよけ
	{
		return QByteArray();
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
		NSBitmapImageRep  *bitmap = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:buff
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
		[bitmap autorelease];

		//JPGに変換
		NSData *jpgData;
		NSDictionary *properties;
		BOOL interlaced;
		properties = [NSDictionary
					  dictionaryWithObject:[NSNumber numberWithBool:interlaced]
					  forKey:NSImageInterlaced];
		jpgData = [bitmap representationUsingType:NSJPEGFileType properties:properties];

		const char *buffer_jpg = reinterpret_cast<const char*>([jpgData bytes]);	//データポインタ取得
		NSUInteger length = [jpgData length];										//データサイズ取得
		clArray = QByteArray(reinterpret_cast<const char*>(buffer_jpg), length);	//バッファにコピー
		
		//画像サイズ等を記録
		m_usWidth		= width;
		m_usHeight		= height;
		m_iPictureSize	= length;
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
	
	return clArray;
}

//--------------------------------------------------------------------------------
/*!
	@brief		AFの取得
 */
//--------------------------------------------------------------------------------
bool ClMOCameraCtrl::getAutoFucus()
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
void ClMOCameraCtrl::setAutoFucus( bool bValue )
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
void ClMOCameraCtrl::getFocusInf( int &iValue, int &iMin, int &iMax )
{
	int value,min,max;

	//ばかよけ
	if( g_pclMOCameraCtrl == nil )													//ばかよけ
	{
		return;
	}

	//フォーカス取得
	[g_pclMOCameraCtrl GetFocusInfo:&value Min:&min Max:&max];						//Focus情報の取得
	iValue	= value;
	iMin	= min;
	iMax	= max;
}

//--------------------------------------------------------------------------------
/*!
	@brief		Focusの設定
 */
//--------------------------------------------------------------------------------
void ClMOCameraCtrl::setFocus( int iValue )
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
void ClMOCameraCtrl::getBrightnessInf( int &iValue, int &iMin, int &iMax )
{
	int value,min,max;

	//ばかよけ
	if( g_pclMOCameraCtrl == nil )													//ばかよけ
	{
		return;
	}

	//明るさ取得
	[g_pclMOCameraCtrl GetBrightnessInfo:&value Min:&min Max:&max];					//Brightness情報の取得
	iValue	= value;
	iMin	= min;
	iMax	= max;
}

//--------------------------------------------------------------------------------
/*!
	@brief		Brightnessの設定
 */
//--------------------------------------------------------------------------------
void ClMOCameraCtrl::setBrightness( int iValue )
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
void ClMOCameraCtrl::getZoomInf( int &iValue, int &iMin, int &iMax )
{
	int value,min,max;

	//ばかよけ
	if( g_pclMOCameraCtrl == nil )													//ばかよけ
	{
		return;
	}

	//ズーム取得
	[g_pclMOCameraCtrl GetZoomInfo:&value Min:&min Max:&max];						//Zoom情報の取得
	iValue	= value;
	iMin	= min;
	iMax	= max;
}

//--------------------------------------------------------------------------------
/*!
	@brief		Zoomの設定
 */
//--------------------------------------------------------------------------------
void ClMOCameraCtrl::setZoom( int iValue )
{
	//ばかよけ
	if( g_pclMOCameraCtrl == nil )													//ばかよけ
	{
		return;
	}

	//ズーム設定
	[g_pclMOCameraCtrl SetZoom:iValue];												//Zoomの設定
}
