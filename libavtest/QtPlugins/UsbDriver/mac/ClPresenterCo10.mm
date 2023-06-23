#include "mac/ClPresenterCo10.h"
#include "ClUsbDriver/ClUsbDriver_mac.h"
#include "ClUsbDriver/Command_Presenter.h"
#include "Common/CmnDefine.h"
#include <Cocoa/Cocoa.h>
#include <VisualPresenter/VisualPresenterDevice.h>

@class VPV2;
//--------------------------------------------------------------------------------//
//ClMacObject
//--------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------//
/*!
	@brief		Objective-Cのオブジェクト群
*/
//--------------------------------------------------------------------------------//
class ClMacObject
{
public:
	ClMacObject();
	~ClMacObject();
	bool initialize(ClPresenterCo10 *pclDevInfo);

	void setPresenter(VisualPresenterDevice *pclDevice){m_pclDevice = pclDevice;}
	VisualPresenterDevice *getPresenter(){return m_pclDevice;}

private:
	NSAutoreleasePool *m_pclPool;
	VPV2 *m_pclVpv2;
	VisualPresenterDevice *m_pclDevice;
};
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		コンストラクタ
*/
//--------------------------------------------------------------------------------//
ClMacObject::ClMacObject()
{
	m_pclPool = NULL;
	m_pclVpv2 = NULL;
	m_pclDevice = NULL;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		デストラクタ
*/
//--------------------------------------------------------------------------------//
ClMacObject::~ClMacObject()
{
	if(m_pclDevice != NULL)
	{
		[m_pclDevice release];
	}
	if(m_pclVpv2 != NULL)
	{
		[m_pclVpv2 release];
	}
	if(m_pclPool != NULL)
	{
		[m_pclPool release];
	}
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		初期化
*/
//--------------------------------------------------------------------------------//
bool ClMacObject::initialize(ClPresenterCo10 *pclDevInfo)
{
	//これがないとエラーが出るのでとりあえず追加
	m_pclPool = [[NSAutoreleasePool alloc] init];
	if(m_pclPool == NULL)
	{
		return false;
	}

	m_pclVpv2 = [VPV2 alloc];
	if(m_pclVpv2 == NULL)
	{
		[m_pclPool release];
		return false;
	}
	[m_pclVpv2 init];
	m_pclDevice = [VisualPresenterDevice alloc];
	if(m_pclDevice == NULL)
	{
		[m_pclPool release];
		[m_pclVpv2 release];
		return false;
	}
	[m_pclVpv2 setDevInfo:pclDevInfo];
	[m_pclVpv2 setObject:this];

	[m_pclDevice initWithDelegate:m_pclVpv2];

	return true;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
//VPV2
//--------------------------------------------------------------------------------//
@interface VPV2 : NSObject
{
@private
		ClPresenterCo10 *m_pclDevInfo;
		ClMacObject *m_pclObject;
}
- (void)setDevInfo:(ClPresenterCo10*)pclDevInfo;
- (void)setObject:(ClMacObject*)pclObject;
@end

@implementation VPV2


//--------------------------------------------------------------------------------//
/*!
	@brief		制御クラス設定
*/
//--------------------------------------------------------------------------------//
- (void)setDevInfo:(ClPresenterCo10*)pclDevInfo;
{
	m_pclDevInfo = pclDevInfo;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		Objective-cクラス管理クラス登録
*/
//--------------------------------------------------------------------------------//
- (void)setObject:(ClMacObject*)pclObject;
{
	m_pclObject = pclObject;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------//
- (void)PresenterV2_SDK_NewCameraDetected:(VisualPresenterDevice *)device;
{
	m_pclDevInfo->checkRestart();
	return;
	{
		//再起動
		NSURL *url = [NSURL fileURLWithPath:[[NSBundle mainBundle] bundlePath]];
		LSLaunchURLSpec launchSpec;
		launchSpec.appURL = (CFURLRef)url;
		launchSpec.itemURLs = NULL;
		launchSpec.passThruParams = NULL;
		launchSpec.launchFlags = kLSLaunchDefaults | kLSLaunchNewInstance;
		launchSpec.asyncRefCon = NULL;
		LSOpenFromURLSpec(&launchSpec, NULL);
		[[NSApplication sharedApplication] terminate:self];
	}
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------//
- (void)PresenterV2_SDK_USBDeviceAppeared:(VisualPresenterDevice *)device;
{
	if(m_pclObject->getPresenter() != device)
	{
		delete m_pclObject->getPresenter();
		m_pclObject->setPresenter(device);
	}

	[m_pclObject->getPresenter() PresenterV2_SDK_StartPreviewVideo];
	m_pclDevInfo->setConnected(true);
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------//
- (void)PresenterV2_SDK_USBDeviceDisappeared:(VisualPresenterDevice *)device;
{
	m_pclDevInfo->setConnected(false);
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------//
- (void)PresenterV2_SDK_MSDCDeviceAppeared:(VisualPresenterDevice *)device;
{
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------//
- (void)PresenterV2_SDK_MSDCDeviceDisappeared:(VisualPresenterDevice *)device;
{
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------//
- (void)PresenterV2_SDK_DidFinishRecording:(id)sender toOutputFileAtURL:(NSURL *)outputFileURL dueToError:(NSError *)error;
{
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------//
- (void)PresenterV2_SDK_PropertyPageWillClose:(NSNotification *)notification;
{
}
//--------------------------------------------------------------------------------//
@end

//--------------------------------------------------------------------------------//
//ClCommandThread
//--------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------//
/*!
	@brief		コンストラクタ
*/
//--------------------------------------------------------------------------------//
ClCommandThread::ClCommandThread()
{
	m_bIsStop = false;
	m_lSendCommand = 0;
	m_pclObject = NULL;
	m_pclCommandMutex = NULL;
	m_lFocusValue = -1;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		メイン処理
*/
//--------------------------------------------------------------------------------//
void ClCommandThread::run()
{
	VisualPresenterDevice *pclPresenter = m_pclObject->getPresenter();
	if(pclPresenter == NULL ||
	   m_pclCommandMutex == NULL)
	{
		return;
	}

	while(m_bIsStop == false)
	{
		m_pclCommandMutex->lock();
		switch(m_lSendCommand)
		{
		case ZOOM_WIDE:												//拡大
			{
				[pclPresenter PresenterV2_SDK_OneTrigger:kCmdSetZoomMinusBtn];
				break;
			}
		case ZOOM_TELE:												//縮小
			{
				[pclPresenter PresenterV2_SDK_OneTrigger:kCmdSetZoomPlusBtn];
				break;
			}
		case IRIS_OPEN:												//開く
			{
				[pclPresenter PresenterV2_SDK_OneTrigger:kCmdSetBRTPlusBtn];
				break;
			}
		case IRIS_CLOSE:											//閉じる
			{
				[pclPresenter PresenterV2_SDK_OneTrigger:kCmdSetBRTMinusBtn];
				break;
			}
		case FOCUS_NEAR:											//近く
			{
				execFocus(10);
				break;
			}
		case FOCUS_FAR:												//遠く
			{
				execFocus(-10);
				break;
			}
		default:
			{
				return;
			}
		}
		m_pclCommandMutex->unlock();
		msleep(10);
	}
	m_bIsStop = false;		//停止フラグおろしておく
	m_lSendCommand = 0;		//送信コマンドクリア
	m_lFocusValue = -1;		//フォーカス値初期化
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		フォーカス実行
*/
//--------------------------------------------------------------------------------//
void ClCommandThread::execFocus(qint32 lAdd)
{
	VisualPresenterDevice *pclPresenter = m_pclObject->getPresenter();
	if(pclPresenter == NULL)
	{
		return;
	}

	DEVICE_ERROR error = DEVICE_NO_ERROR;

	//現在のフォーカス値取得
	if(m_lFocusValue == -1)
	{
		error = [pclPresenter PresenterV2_SDK_GetFocus:&m_lFocusValue];
	}

	//設定
	if(error == DEVICE_NO_ERROR)
	{
		qint32 lNewValue = m_lFocusValue + lAdd;
		if(lNewValue < 0)
		{
			lNewValue = 0;
		}
		else if(lNewValue > 1023)
		{
			lNewValue = 1023;
		}

		if(lNewValue != m_lFocusValue)
		{
			[pclPresenter PresenterV2_SDK_SetFocus:lNewValue];
			m_lFocusValue = lNewValue;
		}
	}
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
//ClPresenterCo10
//--------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------//
/*!
	@brief		コンストラクタ
*/
//--------------------------------------------------------------------------------//
ClPresenterCo10::ClPresenterCo10(qint32 lProductId, QObject *parent) :
		ClDeviceInfo(lProductId, parent)
{
	m_pclObject = NULL;
	m_bIsConnect = false;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		デストラクタ
*/
//--------------------------------------------------------------------------------//
ClPresenterCo10::~ClPresenterCo10()
{
	if(m_clThread.isRunning() == true)
	{
		m_clThread.stop();
		m_clThread.wait();
	}
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		オープン
*/
//--------------------------------------------------------------------------------//
qint32 ClPresenterCo10::open()
{
	Class clCo10;
	clCo10 = NSClassFromString(@"VisualPresenterDevice");
	if(clCo10 == NULL)
	{
		return IM_ERROR_NOT_INIT;
	}

	if(m_pclObject != NULL)
	{
		return IM_ERROR_SUCCESS;
		delete m_pclObject;
	}

	m_pclObject = new ClMacObject();
	if(m_pclObject == NULL)
	{
		return IM_ERROR_NOT_INIT;
	}
	if(m_pclObject->initialize(this) == false)
	{
		return IM_ERROR_NOT_INIT;
	}
	m_clThread.setObject(m_pclObject);
	m_clThread.setMutex(&m_clCommandMutex);

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		コマンド送信
*/
//--------------------------------------------------------------------------------//
qint32 ClPresenterCo10::CommandPresenter(qint32 CmdID, QVariant &qParameter)
{
	VisualPresenterDevice *pclPresenter = m_pclObject->getPresenter();
	if(pclPresenter == NULL)
	{
		return IM_ERROR_NOT_INIT;
	}

	m_clCommandMutex.lock();
	qint32 lRes = IM_ERROR_SUCCESS;
	switch(CmdID)
	{
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
			qParameter = m_iPictureSize;
			lRes = IM_ERROR_SUCCESS;
			break;
		}
	case GET_RECT_PICTURE:										//画像の大きさ取得
		{
			qParameter.setValue<eKINDRECTID>(m_iPictureRect);
			lRes = IM_ERROR_SUCCESS;
			break;
		}
	case SET_RECT_PICTURE:										//画像の大きさ設定
		{
			if(qParameter.type() == QVariant::UserType &&
			   chINRANGE(SIZE_PICTURE_HIGH,qParameter.value<eKINDRECTID>(),RECTID_PICTURE_DEFAULT))
			{
				m_iPictureRect = qParameter.value<eKINDRECTID>();
				lRes = IM_ERROR_SUCCESS;
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
			qParameter = m_usHeight;
			lRes = IM_ERROR_SUCCESS;
			break;
		}
	case GET_WIDTH_PICTURE:										//現在の画像の横幅
		{
			qParameter = m_usWidth;
			lRes = IM_ERROR_SUCCESS;
			break;
		}
	case ZOOM_WIDE:			//no break							//拡大
	case ZOOM_TELE:												//縮小
		{
			if(m_clThread.isRunning() == false ||
			   (m_clThread.isRunning() == true &&
				(m_clThread.getCommand() == ZOOM_WIDE ||
				 m_clThread.getCommand() == ZOOM_TELE)))
			{
				m_clThread.setCommand(CmdID);
				if(m_clThread.isRunning() == false)
				{
					m_clThread.start();
				}
			}
			break;
		}
	case IRIS_OPEN:			//no break							//開く
	case IRIS_CLOSE:											//閉じる
		{
			if(m_clThread.isRunning() == false ||
			   (m_clThread.isRunning() == true &&
				(m_clThread.getCommand() == IRIS_OPEN ||
				 m_clThread.getCommand() == IRIS_CLOSE)))
			{
				m_clThread.setCommand(CmdID);
				if(m_clThread.isRunning() == false)
				{
					m_clThread.start();
				}
			}
			break;
		}
	case FOCUS_NEAR:		//no break							//近く
	case FOCUS_FAR:												//遠く
		{
			if(m_clThread.isRunning() == false ||
			   (m_clThread.isRunning() == true &&
				(m_clThread.getCommand() == FOCUS_NEAR ||
				 m_clThread.getCommand() == FOCUS_FAR)))
			{
				m_clThread.setCommand(CmdID);
				if(m_clThread.isRunning() == false)
				{
					m_clThread.start();
				}
			}
			break;
		}
	case ZOOM_STOP:												//ストップ
		{
			if(m_clThread.isRunning() == true &&
			   (m_clThread.getCommand() == ZOOM_WIDE ||
				m_clThread.getCommand() == ZOOM_TELE))
			{
				m_clThread.stop();
			}
			break;
		}
	case IRIS_STOP:												//ストップ
		{
			if(m_clThread.isRunning() == true &&
			   (m_clThread.getCommand() == IRIS_OPEN ||
				m_clThread.getCommand() == IRIS_CLOSE))
			{
				m_clThread.stop();
			}
			break;
		}
	case FOCUS_STOP:											//ストップ
		{
			if(m_clThread.isRunning() == true &&
			   (m_clThread.getCommand() == FOCUS_NEAR ||
				m_clThread.getCommand() == FOCUS_FAR))
			{
				m_clThread.stop();
			}
			break;
		}
	case IRIS_INITIALIZE:										//初期位置
		{
			[pclPresenter PresenterV2_SDK_SetBrightness:50];
			break;
		}
	case FOCUS_AUTO:											//自動
		{
			[pclPresenter PresenterV2_SDK_OneTrigger:kCmdSetAFBtn];
			break;
		}
	/* --------------------------------------------------------- +/
						外部入力コマンド
	/+ --------------------------------------------------------- */
	case EXTERNAL_INPUT:										//外部入力コマンド V1
		{
			break;
		}
	default:
		lRes = IM_ERROR_INVALID_PARAMETER;						//対応したコマンドがない
		break;
	}
	m_clCommandMutex.unlock();
	return lRes;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		接続状態変更
*/
//--------------------------------------------------------------------------------//
void ClPresenterCo10::setConnected(bool bIsConnect)
{
	m_bIsConnect = bIsConnect;

	ClUsbDriver *pclDriver = dynamic_cast<ClUsbDriver*>(parent());
	if(pclDriver != NULL)
	{
		if(m_bIsConnect == true &&
		   QIODevice::open(QIODevice::ReadWrite) == true)
		{
			pclDriver->open(m_lProductId);
		}
		else
		{
			QIODevice::close();
			pclDriver->close(m_lProductId);
		}
	}
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		再起動確認
*/
//--------------------------------------------------------------------------------//
void ClPresenterCo10::checkRestart()
{
	ClUsbDriver *pclDriver = dynamic_cast<ClUsbDriver*>(parent());
	if(pclDriver != NULL)
	{
		pclDriver->checkRestart();
	}
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		画像取得
*/
//--------------------------------------------------------------------------------//
qint32 ClPresenterCo10::getPicture(QVariant &qParam)
{
	NSAutoreleasePool *nsPool = [[NSAutoreleasePool alloc] init];

	VisualPresenterDevice *pclPresenter = m_pclObject->getPresenter();
	if(pclPresenter == NULL)
	{
		return IM_ERROR_NOT_INIT;
	}

	NSImage *sourceImage;
	DEVICE_ERROR error = DEVICE_NO_ERROR;
	error = [pclPresenter PresenterV2_SDK_CaptureImage:&sourceImage];
	if(error != DEVICE_NO_ERROR)
	{
		return IM_ERROR_CAPTURE_FAILURE;
	}

	//JPEGに変換
	NSData *tiffData = [sourceImage TIFFRepresentation];
	NSBitmapImageRep *bmpImageRep = [NSBitmapImageRep imageRepWithData:tiffData];
	NSDictionary *property = [NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithFloat:0.5],
							NSImageCompressionFactor, nil];
	NSData *jpgData = [bmpImageRep representationUsingType:NSJPEGFileType properties:property];

	const char *buffer = reinterpret_cast<const char*>([jpgData bytes]);	//データポインタ取得
	NSUInteger length = [jpgData length];									//データサイズ取得
	qParam = QByteArray(reinterpret_cast<const char*>(buffer), length);	//バッファにコピー

	//画像の幅、高さ取得
	QSize clSize;
	analyJpegHeader(qParam.toByteArray(), clSize);

	m_usHeight = clSize.height();
	m_usWidth = clSize.width();
	m_iPictureSize = length;

	[nsPool release];

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		カメラ名取得
*/
//--------------------------------------------------------------------------------//
qint32 ClPresenterCo10::getModelName(QVariant &qParam)
{
	qParam = QString("i-Pochette");
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		カメラバージョン取得
*/
//--------------------------------------------------------------------------------//
qint32 ClPresenterCo10::getVersion(QVariant &qParam)
{
	VisualPresenterDevice *pclPresenter = m_pclObject->getPresenter();
	if(pclPresenter == NULL)
	{
		return IM_ERROR_NOT_INIT;
	}

	//バッファ初期化
	UInt8 ucVersion[256];
	memset(ucVersion, NULL, sizeof(ucVersion));
	//バージョン取得
	DEVICE_ERROR error;
	error = [pclPresenter PresenterV2_SDK_QueryFirmwareVersion:ucVersion];
	if(error != DEVICE_NO_ERROR)
	{
		return IM_ERROR_COMMAND_FAILURE;
	}

	QStringList strVersionList;
	strVersionList.append(QString(reinterpret_cast<const char*>(ucVersion)));
	qParam = strVersionList;

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------//
