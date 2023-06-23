#include "ClUsbDriver/ClUsbDriver_global.h"
#include "Common/CmnDefine.h"
#include "linux/ClImDrvCtrl.h"
#include "ClUsbDriver/Command_Presenter.h"
#include "linux/ClDeviceInfo.h"
#include "linux/ClPresenterL1ex.h"
#include "linux/ClPresenterL1n.h"
#ifdef Q_OS_LINUX
#include "linux/ClPresenterWebcam.h"
#endif
#include "ClUsbCmd.h"

#define		STREAM_WAIT_MUTEX	(1000)	//!< MutexのtryLockの待ち時間




//--------------------------------------------------------------------------------
/*!
	@class		ClSourceImageThread
	@brief
 */
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
ClSourceImageThread::ClSourceImageThread(QObject *parent, ClImDrvCtrl *a_devctrl)
	: QThread(parent)
{
	m_bTerminate = false;
	m_bZeroImage = false;

	m_pDevCtrl = a_devctrl;
	m_dSleepTime = 1 / static_cast<double>(30);
	m_iRectId = RECTID_PICTURE_HIGH;
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
void ClSourceImageThread::slotLoopTerminate()
{
	m_bTerminate = true;															//set true for end of loop
}
//--------------------------------------------------------------------------------
/*!
	@brief		set framerate
	@param		lRate framerate
 */
//--------------------------------------------------------------------------------
void ClSourceImageThread::setFrate(qint32 lRate)
{
	m_dSleepTime = 1 / static_cast<double>(lRate);
}
//--------------------------------------------------------------------------------
/*!
	@brief		set Rect ID
	@param		iRectId
 */
//--------------------------------------------------------------------------------
void ClSourceImageThread::setRectId(eKINDRECTID iRectId)
{
	m_iRectId = iRectId;
}
//--------------------------------------------------------------------------------
/*!
	@brief		get Rect ID
	@retval		RECTID_UNSUPPORT
	@retval		RECTID_PICTURE_HIGH
	@retval		RECTID_PICTURE_MID
	@retval		RECTID_PICTURE_LOW

 */
//--------------------------------------------------------------------------------
eKINDRECTID ClSourceImageThread::getRectId()
{
	return m_iRectId;
}
//--------------------------------------------------------------------------------
/*!
	@brief		get framerate
	@retval		1-30 framerate
 */
//--------------------------------------------------------------------------------
qint32 ClSourceImageThread::getFrate()
{
	return static_cast<qint32>(1 / m_dSleepTime);
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
void ClSourceImageThread::run()
{
	qint32 lRes = IM_ERROR_SUCCESS;
	QVariant qvParam;
	QtDXUTTimer timer;
	double dBefore = 0, dAfter = 0;
	qint32 lSleepTime = 0;

	while(m_bTerminate == false)															//終了要求があるまでループ
	{

		dBefore = timer.getAbsoluteTime();	//処理開始時間取得

		qvParam.setValue<eKINDRECTID>(m_iRectId);
		lRes = m_pDevCtrl->CommandPresenter(GET_PICTURE,qvParam);
		if(lRes == IM_ERROR_CANNOT_OUTPUT_IMAGE){
			if(m_bZeroImage == false){
				DBGFL("Start picture size zero");
				emit sigStartZeroImage(m_pDevCtrl->getProductId());
				m_bZeroImage = true;
			}
			continue;
		}else if(lRes != IM_ERROR_SUCCESS){
			DBGFL("GET_PICTURE failure");
			m_bTerminate = true;
			continue;
		}else if(lRes == IM_ERROR_SUCCESS){
			if(m_bZeroImage == true){
				DBGFL("End picture size zero");
				emit sigEndZeroImage(m_pDevCtrl->getProductId());
			}
			m_bZeroImage = false;
		}
		if(qvParam.type() == QVariant::ByteArray){
			m_qQueue << qvParam.toByteArray();
			while(m_qQueue.count() > 1 ){
				m_qQueue.removeFirst();
			}

			emit sigSourceImage(m_pDevCtrl->getProductId(), lRes, m_qQueue);

		}
		dAfter = timer.getAbsoluteTime();	//処理終了時間取得

		if(dBefore != 0 && dAfter != 0)
		{
			lSleepTime = static_cast<qint32>((m_dSleepTime - (dAfter - dBefore)) * 1000);	//sleep時間設定
			if(lSleepTime > 0)
			{
				msleep(static_cast<ulong>(lSleepTime));		//フレームレート調整用sleep
			}
		}
	}
	if(m_bZeroImage == true){
		DBGFL("End picture size zero");
		emit sigEndZeroImage(m_pDevCtrl->getProductId());
	}
	emit sigTerminate(m_pDevCtrl->getProductId());
	DBGFL("Terminate ClSourceImageThread");
}
//--------------------------------------------------------------------------------
/*!
	@class		ClJpegImageThread
	@brief
 */
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
ClJpegImageThread::ClJpegImageThread(QObject *parent)
	: QThread(parent)
{
	m_bTerminate = false;

	m_dSleepTime = 1 / static_cast<double>(10);
	m_lProductId = PRODUCT_NONE;
	m_lStatus = IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
void ClJpegImageThread::slotLoopTerminate()
{
	m_bTerminate = true;															//set true for end of loop
}
//--------------------------------------------------------------------------------
/*!
	@brief		set framerate
	@param		lRate	:	framerate
 */
//--------------------------------------------------------------------------------
void ClJpegImageThread::setFrate(qint32 lRate)
{
	m_dSleepTime = 1 / static_cast<double>(lRate);
}
//--------------------------------------------------------------------------------
/*!
	@brief		get framerate
	@return		qint32	:	framerate
 */
//--------------------------------------------------------------------------------
qint32 ClJpegImageThread::getFrate()
{
	return static_cast<qint32>(1 / m_dSleepTime);
}
//--------------------------------------------------------------------------------
/*!
	@brief		get Rect ID
 */
//--------------------------------------------------------------------------------
void ClJpegImageThread::slotSourceImage(qint32 lProductId, qint32 lStatus, QVariantList queue)
{
	QtDXUTTimer timer;
	static double dDebBefore = 0;

	m_lProductId = lProductId;
	m_lStatus = lStatus;
	if(queue.isEmpty() == false && queue.at(0).type() == QVariant::ByteArray){
		m_qJpegArray = queue.at(0).toByteArray();
		m_bUpdate = true;

		DBGFL("slot event frame rate : %f",dDebBefore - timer.getAbsoluteTime());
		dDebBefore = timer.getAbsoluteTime();
	}
}
//--------------------------------------------------------------------------------
/*!
	@brief		get Rect ID
 */
//--------------------------------------------------------------------------------
void ClJpegImageThread::slotStartZeroImage(qint32 lProductId)
{
	m_qJpegArray = QByteArray();
	emit sigStartZeroImage(lProductId, JPEG_STREAM);
}
//--------------------------------------------------------------------------------
/*!
	@brief		get Rect ID
 */
//--------------------------------------------------------------------------------
void ClJpegImageThread::slotEndZeroImage(qint32 lProductId)
{
	emit sigEndZeroImage(lProductId, JPEG_STREAM);
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
void ClJpegImageThread::run()
{
	QtDXUTTimer timer;
	double dBefore = 0, dAfter = 0;
	qint32 lSleepTime = 0;
	m_bUpdate = false;

	while(m_bTerminate == false)															//終了要求があるまでループ
	{
		dBefore = timer.getAbsoluteTime();	//処理開始時間取得
		if(m_qJpegArray.isNull() == false && m_bUpdate == true){
			m_bUpdate = false;
			emit sigJpegImage(m_lProductId, m_lStatus, m_qJpegArray);
		}else{
			/*!
				配信のタイミング時に配信できない場合、次の配信タイミングまで待つとかなり遅れて配信してしまう。
				その為、次のタイミングですぐに送れるようにここで待機する
			*/
			msleep(1);
			qApp->processEvents();
			continue;
		}
		dAfter = timer.getAbsoluteTime();	//処理終了時間取得

		if(dBefore != 0 && dAfter != 0)
		{
			lSleepTime = static_cast<qint32>((m_dSleepTime - (dAfter - dBefore)) * 1000);	//sleep時間設定
			if(lSleepTime > 0)
			{
				msleep(static_cast<ulong>(lSleepTime));		//フレームレート調整用sleep
			}
		}
	}
	emit sigTerminate(m_lProductId,JPEG_STREAM);
	DBGFL("Terminate JpegImageThread");
}
//--------------------------------------------------------------------------------
/*!
	@class		ClRawImageThread
	@brief
 */
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
ClRawImageThread::ClRawImageThread(QObject *parent)
	: QThread(parent)
{
	m_bTerminate = false;

	m_dSleepTime = 1 / static_cast<double>(10);
	m_lProductId = PRODUCT_NONE;
	m_lStatus = IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
void ClRawImageThread::slotLoopTerminate()
{
	m_bTerminate = true;															//set true for end of loop
}
//--------------------------------------------------------------------------------
/*!
	@brief		set framerate
	@param		lRate	:	framerate
 */
//--------------------------------------------------------------------------------
void ClRawImageThread::setFrate(qint32 lRate)
{
	m_dSleepTime = 1 / static_cast<double>(lRate);
}
//--------------------------------------------------------------------------------
/*!
	@brief		get framerate
 */
//--------------------------------------------------------------------------------
qint32 ClRawImageThread::getFrate()
{
	return static_cast<qint32>(1 / m_dSleepTime);
}
//--------------------------------------------------------------------------------
/*!
	@brief		get Rect ID
 */
//--------------------------------------------------------------------------------
void ClRawImageThread::slotSourceImage(qint32 lProductId, qint32 lStatus, QVariantList queue)
{
	QtDXUTTimer timer;
	static double dDebBefore = 0;

	m_lProductId = lProductId;
	m_lStatus = lStatus;
	if(queue.isEmpty() == false && queue.at(0).type() == QVariant::ByteArray)	//何かしらQByteArrayなデータを受け取った場合
	{
		if(false == this->m_clMutex.tryLock(STREAM_WAIT_MUTEX))					//JPEG領域保護Mutexロック開始を試みる
		{
			return;																//Mutexが取得できなかった場合は何も起こらなかったことにする
		}
		this->m_qJpegArray = queue.at(0).toByteArray();							//受け取ったQByteArrayをメンバー変数にて保持。暗黙の共有がなされるのでそれほど時間はかからないはず
		this->m_clMutex.unlock();												//JPEG領域ほぼMutexを解除

		m_bUpdate = true;

		DBGFL("slot event frame rate : %f",dDebBefore - timer.getAbsoluteTime());
		dDebBefore = timer.getAbsoluteTime();
	}else{
		DBGFL("slotSourceImage Failure");
		m_bTerminate = true;
		m_qJpegArray = QByteArray();
	}
}
//--------------------------------------------------------------------------------
/*!
	@brief		get Rect ID
 */
//--------------------------------------------------------------------------------
void ClRawImageThread::slotStartZeroImage(qint32 lProductId)
{
	m_qJpegArray = QByteArray();
	emit sigStartZeroImage(lProductId, RAW_STREAM);
}
//--------------------------------------------------------------------------------
/*!
	@brief		get Rect ID
 */
//--------------------------------------------------------------------------------
void ClRawImageThread::slotEndZeroImage(qint32 lProductId)
{
	emit sigEndZeroImage(lProductId, RAW_STREAM);
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
void ClRawImageThread::run()
{
	QtDXUTTimer timer;
	double dBefore = 0, dAfter = 0;
	qint32 lSleepTime = 0;
	m_bUpdate = false;

	while(m_bTerminate == false)									//終了要求があるまでループ
	{

		dBefore = timer.getAbsoluteTime();							//処理開始時間取得
		if(m_qJpegArray.isNull() == false && m_bUpdate == true)		//JPEGデータが存在していて、新しいデータを受信していた場合
		{
			this->m_bUpdate = false;								//更新必要フラグを解除

			if(false == this->m_clMutex.tryLock(STREAM_WAIT_MUTEX))	//JPEG領域保護Mutexロック開始を試みる
			{
				continue;											//Mutex取得に失敗した場合はループの頭からやり直し
			}
			QByteArray qWork(this->m_qJpegArray);					//メンバー変数にて保持しているQByteArrayを元に作業領域を生成。この時点では暗黙の共有が為されるため、それほど重くはならないはず(つまりMutex占有時間はそれほど無いはず)
			this->m_clMutex.unlock();								//JPEG領域保護Mutex解放

			bool bRes = m_qImage.loadFromData(qWork,"JPG");			//QByteArraで受け取ったJPEGデータをQImageに変換。ここでJPEGに対するデコードが発生する
			if(bRes == false)
			{
				DBGFL("loadFromData Failure");
				m_bTerminate = true;
			}
		}
		else if(m_qJpegArray.isNull() == true && m_qImage.isNull() == false)
		{
			m_qImage = QImage();
		}

		if(m_qImage.isNull() == false){
			emit sigRawImage(m_lProductId, m_lStatus, m_qImage);
		}
		dAfter = timer.getAbsoluteTime();	//処理終了時間取得

		if(dBefore != 0 && dAfter != 0)
		{
			lSleepTime = static_cast<qint32>((m_dSleepTime - (dAfter - dBefore)) * 1000);	//sleep時間設定
			if(lSleepTime > 0)
			{
				msleep(static_cast<ulong>(lSleepTime));		//フレームレート調整用sleep
			}
		}
	}
	emit sigTerminate(m_lProductId,RAW_STREAM);
	DBGFL("Terminate RawImageThread");
}

//--------------------------------------------------------------------------------
/*!
	@class		ClImDrvCtrl
	@brief
 */
//--------------------------------------------------------------------------------
/*!
	@brief		Constructor
 */
//--------------------------------------------------------------------------------
ClImDrvCtrl::ClImDrvCtrl()
{
	m_bOpen = false;
	m_ctx = NULL;
	m_pclUseDevice = NULL;						//set NULL for m_pclUseDevice

	m_lProductId = PRODUCT_L1EX;				//set default product id
	m_bInitialized = false;
	m_pclSrcThread = NULL;						//set NULL for m_pclSrcThread
	m_pclRawThread = NULL;						//set NULL for m_pclRawThread
	m_pclJpegThread = NULL;						//set NULL for m_pclJpegThread
}

ClImDrvCtrl::ClImDrvCtrl(qint32 _productId, libusb_context *ctx, QObject *parent) :
    QObject(parent)
{
	m_bOpen = false;
	m_ctx = ctx;
	m_pclUseDevice = NULL;						//set NULL for m_pclUseDevice

	m_lProductId = _productId;
	m_bInitialized = false;
	m_pclSrcThread = NULL;						//set NULL for m_pclSrcThread
	m_pclRawThread = NULL;						//set NULL for m_pclRawThread
	m_pclJpegThread = NULL;						//set NULL for m_pclJpegThread
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
ClImDrvCtrl::~ClImDrvCtrl()
{
	CloseDriver();
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
qint32 ClImDrvCtrl::initialize()
{
	if(m_bInitialized == true){
		return IM_ERROR_SUCCESS;
	}


	m_bInitialized = true;
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
bool ClImDrvCtrl::isInitialize()
{
	return m_bInitialized;
}

//--------------------------------------------------------------------------------
/*!
	@brief		this function is able to show all devices, this is for debug
	@return		QString
 */
//--------------------------------------------------------------------------------
QString ClImDrvCtrl::chkAllDeviceInfo()
{
	QString strResult;
	ssize_t	devCount = 0;									//Number of device
	libusb_device **ppDevList = NULL;							//device all list
	if(isInitialize() == false){							//if does not initialization
		return "not initialized";							//return commet of error
	}
	QMutexLocker qLocker(&m_Mutex);								//start mutex

	if(IsOpenDriver() == true){									//if already opened
		return "aleady opend";								//return comment of error
	}

	devCount = libusb_get_device_list(m_ctx,&ppDevList);		//getting for device and write a list
	if(devCount <= 0){
		if(devCount == LIBUSB_ERROR_NO_MEM){
			strResult = "libusb_get_device_list : LIBUSB_ERROR_NO_MEM";
		}else{
			strResult = "libusb_get_device_list was Zero or other error";
		}
		libusb_free_device_list(ppDevList,1);					//release device list
		return strResult;
	}

	for(qint32 i = 0; i < devCount; i++ ){
		ClDeviceInfo workDevice(PRODUCT_NONE);
		strResult += workDevice.getDeviceInfo(ppDevList[i]);
	}

	libusb_free_device_list(ppDevList,1);					//release device list
	return strResult;
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
bool ClImDrvCtrl::OpenDriver()
{
	ssize_t	devCount = 0;									//Number of device
	libusb_device **ppDevList = NULL;							//device all list

	// IDを指定してプレセンターを開くメソッド
	if(isInitialize() == false){
		return false;
	}
	QMutexLocker qLocker(&m_Mutex);								//start mutex

	DBGFL("OpenDriver linux");

	devCount = libusb_get_device_list(m_ctx,&ppDevList);		//getting for device and write a list
	if(devCount <= 0){
		if(devCount == LIBUSB_ERROR_NO_MEM){
			DBGFL("libusb_get_device_list : LIBUSB_ERROR_NO_MEM");
		}else{
			DBGFL("libusb_get_device_list was Zero or other error");
		}
		libusb_free_device_list(ppDevList,1);					//release device list
		return false;
	}

	//Open L1ex
	if(m_lProductId == PRODUCT_L1EX){							//look for the same ProductId or ProductId is null
		m_pclUseDevice = new ClPresenterL1ex(PRODUCT_L1EX,this);
		m_pclUseDevice->open();									//
		if(m_pclUseDevice->isOpen() == true){
			libusb_free_device_list(ppDevList,1);				//release some memories of device list.
			m_bOpen = true;										//set Open flag is true
			return true;										//return success which already opened
		}
	}
	//Open L1n
	if(m_lProductId == PRODUCT_L1N){							//look for the same ProductId or ProductId is null
		m_pclUseDevice = new ClPresenterL1n(PRODUCT_L1N,this);
		m_pclUseDevice->open();									//
		if(m_pclUseDevice->isOpen() == true){
			libusb_free_device_list(ppDevList,1);				//release some memories of device list.
			m_bOpen = true;										//set Open flag is true
			return true;										//return success which already opened
		}
	}
	// Open P10s
	if(m_lProductId == PRODUCT_P10S){							//look for the same ProductId or ProductId is null
		m_pclUseDevice = new ClPresenterP10s(PRODUCT_P10S,this);
		m_pclUseDevice->open();							//
		if(m_pclUseDevice->isOpen() == true){
			libusb_free_device_list(ppDevList,1);					//release some memories of device list.
			m_bOpen = true;										//set Open flag is true
			return true;										//return success which already opened
		}
	}
	//Open P10
	if(m_lProductId == PRODUCT_P10){							//look for the same ProductId or ProductId is null
		m_pclUseDevice = new ClPresenterL1ex(PRODUCT_P10,this);
		m_pclUseDevice->open();							//
		if(m_pclUseDevice->isOpen() == true){
			libusb_free_device_list(ppDevList,1);					//release some memories of device list.
			m_bOpen = true;										//set Open flag is true
			return true;										//return success which already opened
		}
	}
	//Open P30s
	if(m_lProductId == PRODUCT_P30S){							//look for the same ProductId or ProductId is null
		m_pclUseDevice = new ClPresenterP30s(PRODUCT_P30S,this);
		processSleep(5);
		m_pclUseDevice->open();							//
		if(m_pclUseDevice->isOpen() == true){
			libusb_free_device_list(ppDevList,1);					//release some memories of device list.
			m_bOpen = true;										//set Open flag is true
			return true;										//return success which already opened
		}
	}
	//Open P100
	if(m_lProductId == PRODUCT_P100){							//look for the same ProductId or ProductId is null
		m_pclUseDevice = new ClPresenterP100(PRODUCT_P100,this);
		processSleep(5);
		m_pclUseDevice->open();							//
		if(m_pclUseDevice->isOpen() == true){
			libusb_free_device_list(ppDevList,1);					//release some memories of device list.
			m_bOpen = true;										//set Open flag is true
			return true;										//return success which already opened
		}
	}
	//Open FF1
	if(m_lProductId == PRODUCT_FF1){							//look for the same ProductId or ProductId is null
		m_pclUseDevice = new ClPresenterFF1(PRODUCT_FF1,this);
		m_pclUseDevice->open();							//
		if(m_pclUseDevice->isOpen() == true){
			libusb_free_device_list(ppDevList,1);					//release some memories of device list.
			m_bOpen = true;										//set Open flag is true
			return true;										//return success which already opened
		}
	}


	libusb_free_device_list(ppDevList,1);						//release some memories of device list.

	return false;
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
bool ClImDrvCtrl::CloseDriver()
{
	// 書画カメラを閉じる
	if(isInitialize() == false){
		return false;
	}

	QMutexLocker qLocker(&m_Mutex);								//start mutex

	DBGFL("CloseDriver linux");
	if(IsOpenDriver() == true){
		m_pclUseDevice->close();
	}
	m_bOpen = false;											//set Open flag is false

	return true;
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
bool ClImDrvCtrl::IsOpenDriver()
{
	// 指定したIDの書画カメラがOPENされているか判定する
	if(isInitialize() == false){
		return false;
	}

	DBGFL("IsOpenDriver linux");

	if(m_bOpen == true){										//if m_pclUseDevice is not NULL
		bool bRes = m_pclUseDevice->isOpen();					//check for already opened device
		if(bRes == true){										//if device is not opened
			return true;										//retuan true when is already opened device
		}
	}

	return false;
}
//--------------------------------------------------------------------------------
/*!
	@brief	コマンドの実行
 */
//--------------------------------------------------------------------------------
qint32 ClImDrvCtrl::CommandPresenter(qint32 CmdID, QVariant& qParameter)
{
	qint32 lRes = IM_ERROR_NOT_OPEN;						//未接続

	// 書画カメラに対するコマンドを実行する
																//DLL初期化エラー
	if(isInitialize() == false){
		return IM_ERROR_NOT_INIT;
	}
	if(IsOpenDriver() == false){
		return IM_ERROR_NOT_OPEN;
	}
	QMutexLocker qLocker(&m_Mutex);								//start mutex
	lRes = m_pclUseDevice->CommandPresenter(CmdID, qParameter);

	if(lRes != IM_ERROR_SUCCESS && lRes != IM_ERROR_CANNOT_OUTPUT_IMAGE){
		qLocker.unlock();
		CloseDriver();
	}
	if(lRes == IM_ERROR_SUCCESS){
		if(CmdID == SET_RECT_PICTURE){
			if(m_pclSrcThread != NULL){
				m_pclSrcThread->setRectId(qParameter.value<eKINDRECTID>());
			}
		}
	}
	return lRes;
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
qint32 ClImDrvCtrl::startStream(ClUsbDriver *pclUsbDriver,eKINDSTREAM iKind, qint32 iFrate)
{
	if(isInitialize() == false){							// is not initialized
		return IM_ERROR_NOT_INIT;							// return IM_ERROR_NOT_INIT
	}

	if(iFrate <= 0 || iFrate > 30){							//fool proof
		iFrate = 30;
	}

	if(m_pclSrcThread == NULL){
		m_pclSrcThread = new ClSourceImageThread(this,this);
		QVariant qParam;
		qint32 lRes = CommandPresenter(GET_RECT_PICTURE,qParam);
		if(lRes != IM_ERROR_SUCCESS){
			m_pclSrcThread->setRectId(RECTID_UNSUPPORT);
		}else{
			m_pclSrcThread->setRectId(qParam.value<eKINDRECTID>());
		}
		connect(m_pclSrcThread,SIGNAL(sigTerminate(qint32)),
				this,SLOT(slotSourceTerminate(qint32)));
		m_pclSrcThread->setFrate(iFrate);
		m_pclSrcThread->start();
	}
	if(iKind == RAW_STREAM && m_pclRawThread == NULL){
		m_pclRawThread = new ClRawImageThread(this);
		connect(m_pclSrcThread,SIGNAL(sigStartZeroImage(qint32)),
				m_pclRawThread,SLOT(slotStartZeroImage(qint32)));
		connect(m_pclSrcThread,SIGNAL(sigEndZeroImage(qint32)),
				m_pclRawThread,SLOT(slotEndZeroImage(qint32)));
		connect(m_pclSrcThread,SIGNAL(sigSourceImage(qint32, qint32, QVariantList)),
				m_pclRawThread,SLOT(slotSourceImage(qint32, qint32, QVariantList)));
		connect(m_pclRawThread,SIGNAL(sigRawImage(qint32, qint32, QImage)),
				pclUsbDriver,SIGNAL(sigRawImage(qint32, qint32, QImage)));
		connect(m_pclRawThread,SIGNAL(sigStartZeroImage(qint32,eKINDSTREAM)),
				pclUsbDriver,SLOT(slotStartZeroImage(qint32,eKINDSTREAM)));
		connect(m_pclRawThread,SIGNAL(sigEndZeroImage(qint32,eKINDSTREAM)),
				pclUsbDriver,SLOT(slotEndZeroImage(qint32,eKINDSTREAM)));
		connect(m_pclRawThread,SIGNAL(sigTerminate(qint32,eKINDSTREAM)),
				pclUsbDriver,SIGNAL(sigTerminate(qint32,eKINDSTREAM)));
		m_pclRawThread->setFrate(iFrate);
		m_pclRawThread->start();
	}
	if(iKind == JPEG_STREAM && m_pclJpegThread == NULL){
		m_pclJpegThread = new ClJpegImageThread(this);
		connect(m_pclSrcThread,SIGNAL(sigStartZeroImage(qint32)),
				m_pclJpegThread,SLOT(slotStartZeroImage(qint32)));
		connect(m_pclSrcThread,SIGNAL(sigEndZeroImage(qint32)),
				m_pclJpegThread,SLOT(slotEndZeroImage(qint32)));
		connect(m_pclSrcThread,SIGNAL(sigSourceImage(qint32, qint32, QVariantList)),
				m_pclJpegThread,SLOT(slotSourceImage(qint32, qint32, QVariantList)));
		connect(m_pclJpegThread,SIGNAL(sigJpegImage(qint32, qint32, QByteArray)),
				pclUsbDriver,SIGNAL(sigJpegImage(qint32, qint32, QByteArray)));
		connect(m_pclJpegThread,SIGNAL(sigStartZeroImage(qint32,eKINDSTREAM)),
				pclUsbDriver,SLOT(slotStartZeroImage(qint32,eKINDSTREAM)));
		connect(m_pclJpegThread,SIGNAL(sigEndZeroImage(qint32,eKINDSTREAM)),
				pclUsbDriver,SLOT(slotEndZeroImage(qint32,eKINDSTREAM)));
		connect(m_pclJpegThread,SIGNAL(sigTerminate(qint32,eKINDSTREAM)),
				pclUsbDriver,SIGNAL(sigTerminate(qint32,eKINDSTREAM)));
		m_pclJpegThread->setFrate(iFrate);
		m_pclJpegThread->start();
	}

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
qint32 ClImDrvCtrl::stopStream(eKINDSTREAM iKind)
{
	if(isInitialize() == false){							// is not initialized
		return IM_ERROR_NOT_INIT;							// return IM_ERROR_NOT_INIT
	}

	QMutexLocker qLocker(&m_DelMutex);						//start mutex
	if(isStreaming(iKind) == true){							//if is already stream
		if(iKind == JPEG_STREAM){
			Q_ASSERT(m_pclJpegThread);
			bool bRes = false;
			m_pclJpegThread->slotLoopTerminate();
			bRes = m_pclJpegThread->wait(5000);				//wait end of thread at about 10 seconds
			if(bRes == false){
				DBGFL("quit jpeg thread failure");
			}
//			delete m_pclJpegThread;							//release thread handle
			m_pclJpegThread = NULL;							//set NULL
		}
		if(iKind == RAW_STREAM){
			Q_ASSERT(m_pclRawThread);
			bool bRes = false;
			m_pclRawThread->slotLoopTerminate();
			bRes = m_pclRawThread->wait(5000);				//wait end of thread at about 10 seconds
			if(bRes == false){
				DBGFL("quit raw thread failure");
			}
//			delete m_pclRawThread;							//release thread handle
			m_pclRawThread = NULL;							//set NULL
		}
		if(m_pclJpegThread == NULL && m_pclRawThread == NULL && m_pclSrcThread != NULL){
			bool bRes = false;
			m_pclSrcThread->slotLoopTerminate();
			bRes = m_pclSrcThread->wait(20000);				//wait end of thread at about 10 seconds
			if(bRes == false){
				DBGFL("quit source thread failure");
			}
//			delete m_pclSrcThread;							//release thread handle
			m_pclSrcThread = NULL;							//set NULL
		}
	}
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
bool ClImDrvCtrl::isStreaming(eKINDSTREAM iKind)
{
	bool bRes = false;
	if(iKind == RAW_STREAM){
		if(m_pclRawThread != NULL){
			bRes = m_pclRawThread->isRunning();				//check for already runs thread
		}
	}
	if(iKind == JPEG_STREAM){
		if(m_pclJpegThread != NULL){
			bRes = m_pclJpegThread->isRunning();			//check for already runs thread
		}
	}
	return bRes;

}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
void ClImDrvCtrl::setFrameRate(qint32 lRate, eKINDSTREAM iKind)
{
	QMutexLocker qLocker(&m_Mutex);								//start mutex
	if(iKind == RAW_STREAM){
		if(m_pclRawThread != NULL){
			m_pclRawThread->setFrate(lRate);				//set frame rate for RawThread
		}
	}
	if(iKind == JPEG_STREAM){
		if(m_pclJpegThread != NULL){
			m_pclJpegThread->setFrate(lRate);				//set frame rate for RawThread
		}
	}
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
qint32 ClImDrvCtrl::getFrameRate(eKINDSTREAM iKind)
{
	qint32 lRes = 0;
	QMutexLocker qLocker(&m_Mutex);								//start mutex
	if(iKind == RAW_STREAM){
		if(m_pclRawThread != NULL){
			lRes = m_pclRawThread->getFrate();				//get frame rate from RawThread
		}
	}
	if(iKind == JPEG_STREAM){
		if(m_pclJpegThread != NULL){
			lRes = m_pclJpegThread->getFrate();				//get frame rate from JpegThread
		}
	}
	return lRes;
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
void ClImDrvCtrl::processSleep(quint32 seconds)
{
	QtDXUTTimer timer;
	double dBefore = 0;

	dBefore = timer.getAbsoluteTime();						//処理開始時間取得
	do{
		usleep(100000);
		qApp->processEvents();
	}while((timer.getAbsoluteTime() - dBefore) < static_cast<double>(seconds));
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
void ClImDrvCtrl::slotSourceTerminate(qint32)
{
	stopStream(RAW_STREAM);
	stopStream(JPEG_STREAM);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClWebCamCtrl
	@brief
 */
//--------------------------------------------------------------------------------
/*!
	@brief		Constructor
 */
//--------------------------------------------------------------------------------
ClWebCamCtrl::ClWebCamCtrl()
{
}

ClWebCamCtrl::ClWebCamCtrl(qint32 _productId, libusb_context *ctx, QObject *parent) :
	ClImDrvCtrl(_productId,ctx,parent)
{
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
ClWebCamCtrl::~ClWebCamCtrl()
{
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
bool ClWebCamCtrl::OpenDriver()
{
#ifdef Q_OS_LINUX
	//Open WebCam
	m_pclUseDevice = new ClPresenterWebcam(m_lProductId,this);
	m_pclUseDevice->open();									//
	if(m_pclUseDevice->isOpen() == true){
		m_bOpen = true;										//set Open flag is true
		return true;										//return success which already opened
	}
#endif
	return false;
}
//--------------------------------------------------------------------------------
