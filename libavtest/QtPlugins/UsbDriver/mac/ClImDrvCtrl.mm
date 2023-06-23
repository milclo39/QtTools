#include "mac/ClImDrvCtrl.h"
#include "ClUsbDriver/ClUsbDriver_global.h"
#include "Common/CmnDefine.h"
#include "ClUsbDriver/Command_Presenter.h"
#include "mac/ClDeviceInfo_mac.h"
#include "mac/ClPresenterL1ex.h"
#include "mac/ClPresenterL1n.h"
#include "mac/ClPresenterL12.h"

#ifndef _NO_USE_ELVDLIB
#ifdef _USE_CO10
#include "mac/ClPresenterCo10.h"
#endif
#ifdef _USE_MO1
#include "mac/ClPresenterMO1.h"
#endif
#endif

#include "ClUsbCmd.h"
#include "ClUsbCmdV2.h"
#include <Cocoa/Cocoa.h>

#define		STREAM_WAIT_MUTEX	(1000)	//!< MutexのtryLockの待ち時間




//--------------------------------------------------------------------------------
/*!
    @class		ClSourceImageThread
    @brief
 */
//--------------------------------------------------------------------------------
/*!
    @fn			ClSourceImageThread
    @brief
    @return		NONE
 */
//--------------------------------------------------------------------------------
ClSourceImageThread::ClSourceImageThread(QObject *parent, ClImDrvCtrl *a_devctrl)
    : QThread(parent)
{
    m_bTerminate = false;
    m_bZeroImage = false;

    m_pDevCtrl = a_devctrl;
    m_dSleepTime = 1 / static_cast<double>(30);
    m_lImgQuality = JPEG_QUARITY_DEF;
    m_iRectId = RECTID_PICTURE_HIGH;
    m_lLoopCount = 0;
}
//--------------------------------------------------------------------------------
/*!
    @fn			slotLoopTerminate
    @brief
    @return		NONE
 */
//--------------------------------------------------------------------------------
void ClSourceImageThread::slotLoopTerminate()
{
    m_bTerminate = true;															//set true for end of loop
}
//--------------------------------------------------------------------------------
/*!
    @fn			setFrate(qint32)
    @brief		set framerate
    @param		lRate	:	framerate
    @return		NONE
 */
//--------------------------------------------------------------------------------
void ClSourceImageThread::setFrate(qint32 lRate)
{
    m_dSleepTime = 1 / static_cast<double>(lRate);
}
//--------------------------------------------------------------------------------
/*!
    @fn			setRectId()
    @brief		set Rect ID
    @param		eKINDRECTID	:	Rect ID
 */
//--------------------------------------------------------------------------------
void ClSourceImageThread::setRectId(eKINDRECTID iRectId)
{
    m_iRectId = iRectId;
}
//--------------------------------------------------------------------------------
/*!
    @fn			getRectId()
    @brief		get Rect ID
    @return		eKINDRECTID	:	Rect ID
 */
//--------------------------------------------------------------------------------
eKINDRECTID ClSourceImageThread::getRectId()
{
    return m_iRectId;
}
//--------------------------------------------------------------------------------
/*!
    @brief		最新画像取得
*/
//--------------------------------------------------------------------------------
qint32 ClSourceImageThread::getStreamingImage(QVariant &clParameter)
{
    //パラメータチェック
    if(clParameter.type() != QVariant::StringList)
    {
        return IM_ERROR_COMMAND_FAILURE;
    }
    QStringList clParamList = clParameter.toStringList();
    qint32 lRectId = clParamList.at(0).toInt();
    qint32 lQuality = clParamList.at(1).toInt();
    if(lRectId != getRectId() ||
       lQuality != getQuality())
    {
        return IM_ERROR_COMMAND_FAILURE;
    }

    //画像がいるかチェック
    if(m_qData.isEmpty() == true)
    {
        return IM_ERROR_COMMAND_FAILURE;
    }
    clParameter = m_qData;
    return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
    @fn			getFrate()
    @brief		get framerate
    @return		qint32	:	framerate
 */
//--------------------------------------------------------------------------------
qint32 ClSourceImageThread::getFrate()
{
    return static_cast<qint32>(1 / m_dSleepTime);
}
//--------------------------------------------------------------------------------
/*!
    @fn			run
    @brief
    @return		NONE
 */
//--------------------------------------------------------------------------------
void ClSourceImageThread::run()
{
    qint32 lRes = IM_ERROR_SUCCESS;
    QVariant qvParam;
    QtDXUTTimer timer;
    double dBefore = 0, dAfter = 0;
    qint32 lSleepTime = 0;
#ifndef _NO_USE_ELVDLIB
#ifdef _USE_CO10
    NSAutoreleasePool *nsPool = [[NSAutoreleasePool alloc] init];							//CO-10で必要
#endif
#endif
    while(m_bTerminate == false)															//終了要求があるまでループ
    {
        dBefore = timer.getAbsoluteTime();													//処理開始時間取得

		QStringList clParamList;
		clParamList.append(QString::number(m_iRectId));
		clParamList.append(QString::number(m_lImgQuality));
		qvParam = clParamList;
		lRes = m_pDevCtrl->CommandPresenter(GET_PICTURE,qvParam);							//Jpg取得
        if(lRes == IM_ERROR_CANNOT_OUTPUT_IMAGE){
            if(m_bZeroImage == false){
                DBGFL("Start picture size zero");
                m_qData = QByteArray();
                emit sigStartZeroImage(m_pDevCtrl->getProductId());
                m_bZeroImage = true;
            }
            continue;
        }
        else if(lRes == IM_ERROR_DEVICE_BUSY)
        {
            m_lLoopCount++;
            if(m_lLoopCount > RETRY_MAX)
            {
                emit sigGetStreamFail(m_pDevCtrl->getProductId());
                break;
            }
            continue;		//本体側で待っているらしいので待たない
        }
        else if(lRes == IM_ERROR_NOT_OPEN)
        {
            //ここに入ってくるときは既にカメラが抜けてるときなのでclose処理を待つ
            while(m_bTerminate == false)
            {
                msleep(10);
            }
            break;
        }
        else if(lRes != IM_ERROR_SUCCESS){
            DBGFL("GET_PICTURE failure");
            emit sigGetStreamFail(m_pDevCtrl->getProductId());
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
            m_qData = qvParam.toByteArray();
            emit sigSourceImage(m_pDevCtrl->getProductId(), lRes, m_qData);
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
#ifndef _NO_USE_ELVDLIB
#ifdef _USE_CO10
    [nsPool release];
#endif
#endif
}
//--------------------------------------------------------------------------------
/*!
    @class		ClJpegImageThread
    @brief
 */
//--------------------------------------------------------------------------------
/*!
    @fn			ClJpegImageThread
    @brief
    @return		NONE
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
    @fn			slotLoopTerminate
    @brief
    @return		NONE
 */
//--------------------------------------------------------------------------------
void ClJpegImageThread::slotLoopTerminate()
{
    m_bTerminate = true;															//set true for end of loop
}
//--------------------------------------------------------------------------------
/*!
    @fn			setFrate(qint32)
    @brief		set framerate
    @param		lRate	:	framerate
    @return		NONE
 */
//--------------------------------------------------------------------------------
void ClJpegImageThread::setFrate(qint32 lRate)
{
    m_dSleepTime = 1 / static_cast<double>(lRate);
}
//--------------------------------------------------------------------------------
/*!
    @fn			getFrate()
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
    @fn			slotSourceImage()
    @brief		get Rect ID
 */
//--------------------------------------------------------------------------------
void ClJpegImageThread::slotSourceImage(qint32 lProductId, qint32 lStatus, QByteArray data)
{
    QtDXUTTimer timer;
    static double dDebBefore = 0;

    m_lProductId = lProductId;
    m_lStatus = lStatus;
    if(data.isEmpty() == false){
        m_qJpegArray = data;
        m_bUpdate = true;

        DBGFL("slot event frame rate : %f",dDebBefore - timer.getAbsoluteTime());
        dDebBefore = timer.getAbsoluteTime();
    }
}
//--------------------------------------------------------------------------------
/*!
    @fn			slotStartZeroImage()
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
    @fn			slotEndZeroImage()
    @brief		get Rect ID
 */
//--------------------------------------------------------------------------------
void ClJpegImageThread::slotEndZeroImage(qint32 lProductId)
{
    emit sigEndZeroImage(lProductId, JPEG_STREAM);
}
//--------------------------------------------------------------------------------
/*!
    @brief		画像取得失敗
*/
//--------------------------------------------------------------------------------
void ClJpegImageThread::slotGetStreamFail(qint32 lProductId)
{
    m_qJpegArray = QByteArray();
    emit sigGetStreamFail(lProductId, JPEG_STREAM);
}
//--------------------------------------------------------------------------------
/*!
    @fn			run
    @brief
    @return		NONE
 */
//--------------------------------------------------------------------------------
void ClJpegImageThread::run()
{
    QtDXUTTimer timer;
    double dBefore = 0, dAfter = 0;
    qint32 lSleepTime = 0;
    m_bUpdate = false;
#ifndef _NO_USE_ELVDLIB
#ifdef _USE_CO10
    NSAutoreleasePool *nsPool = [[NSAutoreleasePool alloc] init];							//CO-10で必要
#endif
#endif
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
#ifndef _NO_USE_ELVDLIB
#ifdef _USE_CO10
    [nsPool release];
#endif
#endif
}
//--------------------------------------------------------------------------------
/*!
    @class		ClRawImageThread
    @brief
 */
//--------------------------------------------------------------------------------
/*!
    @fn			ClRawImageThread
    @brief
    @return		NONE
 */
//--------------------------------------------------------------------------------
ClRawImageThread::ClRawImageThread(QObject *parent)
    : QThread(parent)
{
    m_bTerminate = false;

    m_dSleepTime = 1 / static_cast<double>(10);
    m_lProductId = PRODUCT_NONE;
    m_lStatus = IM_ERROR_SUCCESS;

#ifndef SEND_SAME_IMGDATA
	m_bIsUpdateImg = false;															//フラグの初期化
#endif
}
//--------------------------------------------------------------------------------
/*!
    @fn			slotLoopTerminate
    @brief
    @return		NONE
 */
//--------------------------------------------------------------------------------
void ClRawImageThread::slotLoopTerminate()
{
    m_bTerminate = true;															//set true for end of loop
}
//--------------------------------------------------------------------------------
/*!
    @fn			setFrate(qint32)
    @brief		set framerate
    @param		lRate	:	framerate
    @return		NONE
 */
//--------------------------------------------------------------------------------
void ClRawImageThread::setFrate(qint32 lRate)
{
    m_dSleepTime = 1 / static_cast<double>(lRate);
}
//--------------------------------------------------------------------------------
/*!
    @fn			getFrate()
    @brief		get framerate
    @return		qint32	:	framerate
 */
//--------------------------------------------------------------------------------
qint32 ClRawImageThread::getFrate()
{
    return static_cast<qint32>(1 / m_dSleepTime);
}
//--------------------------------------------------------------------------------
/*!
    @fn			slotSourceImage()
    @brief		get Rect ID
 */
//--------------------------------------------------------------------------------
void ClRawImageThread::slotSourceImage(qint32 lProductId, qint32 lStatus, QByteArray data)
{
    QtDXUTTimer timer;
    static double dDebBefore = 0;

    m_lProductId = lProductId;
    m_lStatus = lStatus;
    if(data.isEmpty() == false)									//何かしら有効なQByateArrayを受け取った場合
    {
        if(false == this->m_clMutex.tryLock(STREAM_WAIT_MUTEX))	//JPEG領域保護Mutexロック開始を試みる
        {
            return;												//Mutexが取得できなかった場合は何も起こらなかったことにする
        }
        this->m_qJpegArray = data;								//受け取ったQByteArrayをメンバー変数にて保持。暗黙の共有がなされるのでそれほど時間はかからないはず
        this->m_clMutex.unlock();								//JPEG領域ほぼMutexを解除
#ifndef SEND_SAME_IMGDATA
		m_bIsUpdateImg = true;														//映像の更新がはいったのでフラグを変更
#endif
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
    @fn			slotStartZeroImage()
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
    @fn			slotEndZeroImage()
    @brief		get Rect ID
 */
//--------------------------------------------------------------------------------
void ClRawImageThread::slotEndZeroImage(qint32 lProductId)
{
    emit sigEndZeroImage(lProductId, RAW_STREAM);
}
//--------------------------------------------------------------------------------
/*!
    @brief		画像取得失敗
*/
//--------------------------------------------------------------------------------
void ClRawImageThread::slotGetStreamFail(qint32 lProductId)
{
    m_qJpegArray = QByteArray();
    emit sigGetStreamFail(lProductId, JPEG_STREAM);
}
//--------------------------------------------------------------------------------
/*!
    @fn			run
    @brief
    @return		NONE
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

        if(m_qImage.isNull() == false && m_qJpegArray.isNull() == false){
#ifdef SEND_SAME_IMGDATA
			emit sigRawImage(m_lProductId, m_lStatus, m_qImage);
#else
			if( m_bIsUpdateImg == false )
			{
				//前に送信したものと同じっぽい場合は何もしない
			}
			else
			{
				emit sigRawImage(m_lProductId, m_lStatus, m_qImage);
				m_bIsUpdateImg = false;
			}
#endif
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
    @class		ClSendFileThread
    @brief
 */
//--------------------------------------------------------------------------------
/*!
    @fn			ClSendFileThread(ClPresenterL12)
    @brief		コンストラクタ
 */
//--------------------------------------------------------------------------------
ClSendFileThread::ClSendFileThread(QObject *parent, ClImDrvCtrl *a_devctrl)
    : QThread(parent)
{
    m_bStopFlg = false;
    m_pDevCtrl = a_devctrl;
    connect(m_pDevCtrl, SIGNAL(sigFwUpdateProgress(qint32, qint32)),
            this, SIGNAL(sigFwUpdateProgress(qint32,qint32)));
}
//--------------------------------------------------------------------------------
/*!
    @fn			setFileName(QString)
    @brief		ファイル名設定、サイズ、データ読込
 */
//--------------------------------------------------------------------------------
void ClSendFileThread::setFileName(QString qStrFileName)
{
    QFile qFile(qStrFileName);
    if(qFile.open(QFile::ReadOnly))
    {
        m_lImgLength = qFile.size();
        QDataStream out(&qFile);
        quint8 cGet;
        while(!out.atEnd())
        {
            out >> cGet;
            m_qArrayFile.append(cGet);
        }
        qFile.close();																//ファイルを閉じる
    }
}
//--------------------------------------------------------------------------------
/*!
    @fn			stop()
    @brief		スレッド終了
 */
//--------------------------------------------------------------------------------
void ClSendFileThread::stop()
{
    m_bStopFlg = true;
}
//--------------------------------------------------------------------------------
/*!
    @fn			run
    @brief		メイン
 */
//--------------------------------------------------------------------------------
void ClSendFileThread::run()
{
    QVariant	qParam;
    qint32		lImgLength = (qint32)m_lImgLength;
    qint32		lStart = 0;
    qint32		lArraySize = 0;
    qint32		lOtherLength = 0;
    qint32		lRet = 0;

    m_bStopFlg = false;

    while(m_bStopFlg == false)
    {
        if(lImgLength > 0)
        {
            qDebug("send file --- %d %d",lImgLength, m_qArrayFile.length());
            lOtherLength = sizeof(ELMO_PACKET_HEADER_V2) + ELMO_FILEUP_DUMMY;
            if( lImgLength > ELMO_MAXPACKETSIZE_V2 - lOtherLength - ELUSB_WT_SENDLENGTH_ADJUST)
            {
                lArraySize = ELMO_MAXPACKETSIZE_V2 - lOtherLength - ELUSB_WT_SENDLENGTH_ADJUST;
            }
            else if((lImgLength + lOtherLength) % ELUSB_WT_USBPACKET_LENGTH == 0)
            {
                lArraySize = lImgLength - ELUSB_WT_SENDLENGTH_ADJUST;
            }
            else
            {
                lArraySize = lImgLength;
            }
            QByteArray qArrayRemove = m_qArrayFile.remove(0, lStart);
            QByteArray qArrayTmp = qArrayRemove.remove(lArraySize, lImgLength - lArraySize);
            qParam = qArrayTmp;
            lRet = m_pDevCtrl->CommandPresenter(UPLOAD_FILE_V2, qParam);
            if(lRet < IM_ERROR_SUCCESS){
                qDebug("Command Error Send File--- 0x%x",lRet);
                m_bStopFlg = true;
            }
            //最後のデータ送信後はちょっと待ち
            if(lImgLength - lArraySize == 0){
                msleep(5000);
            }
            lStart = lArraySize;									//読込地点を変更
            lImgLength -= lArraySize;
        }else{
            qParam = QByteArray();
            lRet = m_pDevCtrl->CommandPresenter(UPLOAD_FILE_V2, qParam);
            if(lRet < IM_ERROR_SUCCESS){
                qDebug("Command Error Res Progress --- 0x%x",lRet);
                m_bStopFlg = true;
            }
            if(lRet == 1)
            {
                qDebug("Thread Finish");
                m_bStopFlg = true;
            }
        }
    }
    m_bStopFlg = true;
}
//--------------------------------------------------------------------------------
/*!
    @class		ClImDrvCtrl
    @brief
 */
//--------------------------------------------------------------------------------
/*!
    @fn			ClImDrvCtrl
    @brief		Constructor
 */
//--------------------------------------------------------------------------------
ClImDrvCtrl::ClImDrvCtrl()
{
    m_pclUseDevice		= NULL;						//set NULL for m_pclUseDevice

    m_lProductId		= PRODUCT_L1EX;				//set default product id
    m_bInitialized		= false;
    m_pclSrcThread		= NULL;						//set NULL for m_pclSrcThread
    m_pclRawThread		= NULL;						//set NULL for m_pclRawThread
    m_pclJpegThread		= NULL;						//set NULL for m_pclJpegThread
}

ClImDrvCtrl::ClImDrvCtrl(qint32 _productId, QObject *parent) :
    QObject(parent)
{
    m_pclUseDevice		= NULL;						//set NULL for m_pclUseDevice

    m_lProductId		= _productId;
    m_bInitialized		= false;
    m_pclSrcThread		= NULL;						//set NULL for m_pclSrcThread
    m_pclRawThread		= NULL;						//set NULL for m_pclRawThread
    m_pclJpegThread		= NULL;						//set NULL for m_pclJpegThread
}
//--------------------------------------------------------------------------------
/*!
    @fn			~ClImDrvCtrl
    @brief
    @return		NONE
 */
//--------------------------------------------------------------------------------
ClImDrvCtrl::~ClImDrvCtrl()
{
    CloseDriver();
}
//--------------------------------------------------------------------------------
/*!
    @fn			initialize
    @brief
    @return
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
    @fn			isInitialize
    @brief
    @return
 */
//--------------------------------------------------------------------------------
bool ClImDrvCtrl::isInitialize()
{
    return m_bInitialized;
}

//--------------------------------------------------------------------------------
/*!
    @fn			chkAllDeviceInfo
    @brief		this function is able to show all devices, this is for debug
    @return		QString
 */
//--------------------------------------------------------------------------------
QString ClImDrvCtrl::chkAllDeviceInfo()
{
    QString strResult;
/*
        Image Mate for L-12では機種の切り替えが無いため不必要？ 実装は後回しにする
*/
    strResult = QString("I make soon --- chkAllDeviceInfo 2011.02.06");

    return strResult;
}
//--------------------------------------------------------------------------------
/*!
    @fn			OpenDriver
    @brief
    @return		NONE
 */
//--------------------------------------------------------------------------------
bool ClImDrvCtrl::OpenDriver()
{
    //Source for Windows
    //		機種切り替えが無いのでリスト不要？ ひとまず無しで作成

    // IDを指定してプレセンターを開くメソッド
    if(isInitialize() == false){
        return false;
    }
    QMutexLocker qLocker(&m_Mutex);								//start mutex

    DBGFL("OpenDriver win32");

    bool bRes = false;
    //Open L-12
    if(m_lProductId == PRODUCT_SESAME){			//look for the same ProductId or ProductId is null
        m_pclUseDevice = new ClPresenterL12(PRODUCT_SESAME,parent());
        m_pclUseDevice->open();									//
        if(m_pclUseDevice->isOpen() == true){
            bRes = true;
        }
    }
    //Open L1ex
    else if(m_lProductId == PRODUCT_L1EX){							//look for the same ProductId or ProductId is null
        m_pclUseDevice = new ClPresenterL1ex(PRODUCT_L1EX,parent());
        m_pclUseDevice->open();									//
        if(m_pclUseDevice->isOpen() == true){
            bRes = true;
        }
    }
    //Open L1n
    else if(m_lProductId == PRODUCT_L1N){							//look for the same ProductId or ProductId is null
        m_pclUseDevice = new ClPresenterL1n(PRODUCT_L1N,parent());
        m_pclUseDevice->open();									//
        if(m_pclUseDevice->isOpen() == true){
            bRes = true;
        }
    }
    // Open P10s
    else if(m_lProductId == PRODUCT_P10S){							//look for the same ProductId or ProductId is null
        m_pclUseDevice = new ClPresenterP10s(PRODUCT_P10S,parent());
        m_pclUseDevice->open();							//
        if(m_pclUseDevice->isOpen() == true){
            bRes = true;
        }
    }
    //Open P10
    else if(m_lProductId == PRODUCT_P10){							//look for the same ProductId or ProductId is null
        m_pclUseDevice = new ClPresenterL1ex(PRODUCT_P10,parent());
        m_pclUseDevice->open();							//
        if(m_pclUseDevice->isOpen() == true){
            bRes = true;
        }
    }
    //Open P30s
    else if(m_lProductId == PRODUCT_P30S){							//look for the same ProductId or ProductId is null
        m_pclUseDevice = new ClPresenterP30s(PRODUCT_P30S,parent());
        m_pclUseDevice->open();							//
        if(m_pclUseDevice->isOpen() == true){
            bRes = true;
        }
    }
    //Open P100
    else if(m_lProductId == PRODUCT_P100){							//look for the same ProductId or ProductId is null
        m_pclUseDevice = new ClPresenterP100(PRODUCT_P100,parent());
        m_pclUseDevice->open();							//
        if(m_pclUseDevice->isOpen() == true){
            bRes = true;
        }
    }
    //Open FF1
    else if(m_lProductId == PRODUCT_FF1){							//look for the same ProductId or ProductId is null
        m_pclUseDevice = new ClPresenterFF1(PRODUCT_FF1,parent());
        m_pclUseDevice->open();							//
        if(m_pclUseDevice->isOpen() == true){
            bRes = true;
        }
    }
#ifndef _NO_USE_ELVDLIB
#ifdef _USE_CO10
    //CO-10
    else if(m_lProductId == PRODUCT_CO10)
    {
        m_pclUseDevice = new ClPresenterCo10(PRODUCT_CO10, parent());
        m_pclUseDevice->open();
        if(m_pclUseDevice->isOpen() == true)
        {
            bRes = true;
        }
    }
#endif
#ifdef _USE_MO1
    //MO-1
    else if( PRODUCT_MOBILE == m_lProductId )
    {
        m_pclUseDevice = new ClPresenterMO1( parent() );
        m_pclUseDevice->open();
        if( m_pclUseDevice->isOpen() == true)
        {
            bRes = true;
        }
    }
#endif
#endif
    if(bRes == true)
    {
        ClUsbDriver *pclDriver = dynamic_cast<ClUsbDriver*>(parent());
        pclDriver->open(m_lProductId);
    }
    return bRes;
}
//--------------------------------------------------------------------------------
/*!
    @fn			CloseDriver
    @brief
    @return		NONE
 */
//--------------------------------------------------------------------------------
bool ClImDrvCtrl::CloseDriver()
{
    // 書画カメラを閉じる
    if(isInitialize() == false){
        return false;
    }

    QMutexLocker qLocker(&m_Mutex);								//start mutex

    DBGFL("CloseDriver win32");
    if(IsOpenDriver() == true){
        m_pclUseDevice->close();
#if 1 //elmo add
    }else if(NULL != m_pclUseDevice){
        m_pclUseDevice->close();
#endif
    }

    return true;
}
//--------------------------------------------------------------------------------
/*!
    @fn			IsOpenDriver
    @brief
    @return		NONE
 */
//--------------------------------------------------------------------------------
bool ClImDrvCtrl::IsOpenDriver()
{
    // 指定したIDの書画カメラがOPENされているか判定する
    if(isInitialize() == false){
        return false;
    }

    DBGFL("IsOpenDriver win32");

    if(m_pclUseDevice != NULL){
        bool bRes = m_pclUseDevice->isOpen();					//check for already opened device]
        if(bRes == true){										//if device is not opened
            return true;										//retuan true when is already opened device
        }
    }
    return false;
}
//--------------------------------------------------------------------------------
/*!
    @fn			CommandPresenter
    @brief
    @return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClImDrvCtrl::CommandPresenter(qint32 CmdID, QVariant& qParameter)
{
    qint32 lRes = IM_ERROR_NOT_OPEN;							//未接続

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

    if(m_pclUseDevice->isConnect() == false)
    {
        return IM_ERROR_NOT_OPEN;
    }

    if(lRes != IM_ERROR_SUCCESS && lRes != IM_ERROR_CANNOT_OUTPUT_IMAGE){
        qLocker.unlock();
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
    @fn			startStream
    @brief
    @return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClImDrvCtrl::startStream(ClUsbDriver *pclUsbDriver,eKINDSTREAM iKind, qint32 iFrate, qint32 lQuality)
{
    if(isInitialize() == false){									// is not initialized
        return IM_ERROR_NOT_INIT;									// return IM_ERROR_NOT_INIT
    }

    if(iFrate <= 0 || iFrate > 30){									//fool proof
        iFrate = 30;
    }


    if(iKind == RAW_STREAM || iKind == JPEG_STREAM)					//for V1 command
    {
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
            m_pclSrcThread->setQuality(lQuality);
            m_pclSrcThread->start();
        }
        if(iKind == RAW_STREAM && m_pclRawThread == NULL){
            m_pclRawThread = new ClRawImageThread(this);
            connect(m_pclSrcThread,SIGNAL(sigStartZeroImage(qint32)),
                    m_pclRawThread,SLOT(slotStartZeroImage(qint32)));
            connect(m_pclSrcThread,SIGNAL(sigEndZeroImage(qint32)),
                    m_pclRawThread,SLOT(slotEndZeroImage(qint32)));
            connect(m_pclSrcThread,SIGNAL(sigSourceImage(qint32, qint32, QByteArray)),
                    m_pclRawThread,SLOT(slotSourceImage(qint32, qint32, QByteArray)), Qt::DirectConnection);
            connect(m_pclRawThread,SIGNAL(sigRawImage(qint32, qint32, QImage)),
                    pclUsbDriver,SIGNAL(sigRawImage(qint32, qint32, QImage)));
            connect(m_pclRawThread,SIGNAL(sigStartZeroImage(qint32,eKINDSTREAM)),
                    pclUsbDriver,SLOT(slotStartZeroImage(qint32,eKINDSTREAM)));
            connect(m_pclRawThread,SIGNAL(sigEndZeroImage(qint32,eKINDSTREAM)),
                    pclUsbDriver,SLOT(slotEndZeroImage(qint32,eKINDSTREAM)));
            connect(m_pclRawThread,SIGNAL(sigTerminate(qint32,eKINDSTREAM)),
                    pclUsbDriver,SIGNAL(sigTerminate(qint32,eKINDSTREAM)));
            connect(m_pclSrcThread,SIGNAL(sigGetStreamFail(qint32)),
                    m_pclRawThread,SLOT(slotGetStreamFail(qint32)));
            connect(m_pclRawThread,SIGNAL(sigGetStreamFail(qint32,eKINDSTREAM)),
                    pclUsbDriver,SIGNAL(sigGetStreamFail(qint32,eKINDSTREAM)));
            m_pclRawThread->setFrate(iFrate);
            m_pclRawThread->start();
        }
        if(iKind == JPEG_STREAM && m_pclJpegThread == NULL){
            m_pclJpegThread = new ClJpegImageThread(this);
            connect(m_pclSrcThread,SIGNAL(sigStartZeroImage(qint32)),
                    m_pclJpegThread,SLOT(slotStartZeroImage(qint32)));
            connect(m_pclSrcThread,SIGNAL(sigEndZeroImage(qint32)),
                    m_pclJpegThread,SLOT(slotEndZeroImage(qint32)));
            connect(m_pclSrcThread,SIGNAL(sigSourceImage(qint32, qint32, QByteArray)),
                    m_pclJpegThread,SLOT(slotSourceImage(qint32, qint32, QByteArray)));
            connect(m_pclJpegThread,SIGNAL(sigJpegImage(qint32, qint32, QByteArray)),
                    pclUsbDriver,SIGNAL(sigJpegImage(qint32, qint32, QByteArray)));
            connect(m_pclJpegThread,SIGNAL(sigStartZeroImage(qint32,eKINDSTREAM)),
                    pclUsbDriver,SLOT(slotStartZeroImage(qint32,eKINDSTREAM)));
            connect(m_pclJpegThread,SIGNAL(sigEndZeroImage(qint32,eKINDSTREAM)),
                    pclUsbDriver,SLOT(slotEndZeroImage(qint32,eKINDSTREAM)));
            connect(m_pclJpegThread,SIGNAL(sigTerminate(qint32,eKINDSTREAM)),
                    pclUsbDriver,SIGNAL(sigTerminate(qint32,eKINDSTREAM)));
            connect(m_pclSrcThread,SIGNAL(sigGetStreamFail(qint32)),
                    m_pclJpegThread,SLOT(slotGetStreamFail(qint32)));
            connect(m_pclJpegThread,SIGNAL(sigGetStreamFail(qint32,eKINDSTREAM)),
                    pclUsbDriver,SIGNAL(sigGetStreamFail(qint32,eKINDSTREAM)));
            m_pclJpegThread->setFrate(iFrate);
            m_pclJpegThread->start();
        }
    }
	else
	{
        return IM_ERROR_COMMAND_FAILURE;
	}

    return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
    @fn			stopStream
    @brief
    @return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClImDrvCtrl::stopStream(eKINDSTREAM iKind)
{
    if(isInitialize() == false){							// is not initialized
        return IM_ERROR_NOT_INIT;							// return IM_ERROR_NOT_INIT
    }

    QMutexLocker qLocker(&m_DelMutex);						//start mutex
    if(isStreaming(iKind) == true){							//if is already stream
        if(iKind == JPEG_STREAM){							// jpg steam
            Q_ASSERT(m_pclJpegThread);
            bool bRes = false;
            m_pclJpegThread->slotLoopTerminate();
			bRes = m_pclJpegThread->wait(5000);				//wait end of thread at about 5 seconds
            if(bRes == false){
                DBGFL("quit jpeg thread failure");
            }
//			delete m_pclJpegThread;							//release thread handle
            m_pclJpegThread = NULL;							//set NULL
        }
        if(iKind == RAW_STREAM){							// raw steam
            Q_ASSERT(m_pclRawThread);
            bool bRes = false;
            m_pclRawThread->slotLoopTerminate();
			bRes = m_pclRawThread->wait(5000);				//wait end of thread at about 5 seconds
            if(bRes == false){
                DBGFL("quit raw thread failure");
            }
//			delete m_pclRawThread;							//release thread handle
            m_pclRawThread = NULL;							//set NULL
        }
        if(m_pclJpegThread == NULL && m_pclRawThread == NULL &&
           m_pclSrcThread != NULL){
            bool bRes = false;
            m_pclSrcThread->slotLoopTerminate();
			bRes = m_pclSrcThread->wait(20000);				//wait end of thread at about 20 seconds
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
    @fn			isStartStream
    @brief
    @return		bool
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
    @fn			setFrameRate
    @brief
    @return		NONE
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
    @fn			getFrameRate
    @brief
    @return		NONE
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
    @brief		画質設定
*/
//--------------------------------------------------------------------------------
void ClImDrvCtrl::setQuality(qint32 lQuality, eKINDSTREAM eKind)
{
    QMutexLocker clLocker(&m_Mutex);
    if(eKind == RAW_STREAM ||
       eKind == JPEG_STREAM)
    {
        if(m_pclSrcThread != NULL)
        {
            m_pclSrcThread->setQuality(lQuality);
        }
    }
}
//--------------------------------------------------------------------------------
/*!
    @brief		画質取得
*/
//--------------------------------------------------------------------------------
qint32 ClImDrvCtrl::getQuality(eKINDSTREAM eKind)
{
    qint32 lRes = 0;
    QMutexLocker clLocker(&m_Mutex);
    if(eKind == RAW_STREAM ||
       eKind == JPEG_STREAM)
    {
        if(m_pclSrcThread != NULL)
        {
            lRes = m_pclSrcThread->getQuality();
        }
    }
    return lRes;
}
//--------------------------------------------------------------------------------
/*!
    @brief		ストリームから画像取得
*/
//--------------------------------------------------------------------------------
qint32 ClImDrvCtrl::getStreamingImage(QVariant &clParam)
{
    if(m_pclSrcThread == NULL ||
       m_pclSrcThread->isRunning() == false)
    {
        return IM_ERROR_COMMAND_FAILURE;
    }

    return m_pclSrcThread->getStreamingImage(clParam);
}
//--------------------------------------------------------------------------------
/*!
    @fn			processSleep
    @brief
 */
//--------------------------------------------------------------------------------
void ClImDrvCtrl::processSleep(quint32 seconds)
{
    QtDXUTTimer timer;
    double dBefore = 0;

    dBefore = timer.getAbsoluteTime();	//処理開始時間取得
    do{
        usleep(100);
        qApp->processEvents();
    }while((timer.getAbsoluteTime() - dBefore) < static_cast<double>(seconds));
}
//--------------------------------------------------------------------------------
/*!
    @fn			slotSourceTerminate
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
    @fn			startSendFile
    @brief
    @return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClImDrvCtrl::startSendFile(ClUsbDriver *pclUsbDriver, QVariant &qParam)
{
    if(isInitialize() == false){									// is not initialized
        return IM_ERROR_NOT_INIT;									// return IM_ERROR_NOT_INIT
    }

    if(m_pclUseDevice == NULL || IsOpenDriver() == false){
        return IM_ERROR_NOT_INIT;
    }

    if(qParam.type() != QVariant::String){
        return IM_ERROR_INVALID_PARAMETER;
    }

    if(m_pclSrcThread == NULL){
        m_pclSendFileThread = new ClSendFileThread(this,this);
        connect(m_pclUseDevice, SIGNAL(sigFwUpdateProgress(qint32,qint32)),
                            m_pclSendFileThread, SIGNAL(sigFwUpdateProgress(qint32,qint32)));
        connect(m_pclSendFileThread, SIGNAL(sigFwUpdateProgress(qint32,qint32)),
                            pclUsbDriver, SIGNAL(sigFwUpdateProgress(qint32,qint32)));
        m_pclSendFileThread->setFileName(qParam.toString());
        m_pclSendFileThread->start();
    }
    return IM_ERROR_SUCCESS;
}

//================================================================================
//									UVC
//================================================================================
//--------------------------------------------------------------------------------
/*!
	@fn			getRemoconWidget
	@brief		リモコンを返す
	@return		NONE
 */
//--------------------------------------------------------------------------------
QWidget* ClImDrvCtrl::getRemoconWidget()
{
	return m_pclUseDevice->getRemoconWidget();
}
//--------------------------------------------------------------------------------
/*!
	@fn			setTranslator
	@brief		言語更新
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClImDrvCtrl::setTranslator()
{
	m_pclUseDevice->setTranslator();
}
//--------------------------------------------------------------------------------
/*!
	@fn			getSupportedResolutions
	@brief		対応している解像度を取得
	@return		NONE
 */
//--------------------------------------------------------------------------------
QStringList	ClImDrvCtrl::getSupportedResolutions()
{
	return m_pclUseDevice->getSupportedResolutions();
}
//--------------------------------------------------------------------------------
/*!
	@fn			getCurrentResolution
	@brief		現在の解像度を取得
	@return		NONE
 */
//--------------------------------------------------------------------------------
QString		ClImDrvCtrl::getCurrentResolution()
{
	return m_pclUseDevice->getCurrentResolution();
}
//--------------------------------------------------------------------------------
/*!
	@fn			setCurrentResolution
	@brief		現在の解像度に設定
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32		ClImDrvCtrl::setCurrentResolution(QString strResolution)
{
	HRESULT hr;

	hr = m_pclUseDevice->setCurrentResolution(strResolution);

	return hr;
}
