#include "ClUsbDriver/ClUsbDriver_global.h"
#include "Common/CmnDefine.h"
#include "win32/ClImDrvCtrl.h"
#include "ClUsbDriver/Command_Presenter.h"
#include "win32/ClDeviceInfo_win.h"
#include "win32/ClPresenterL12.h"
#include "win32/ClPresenterL1ex.h"
#include "win32/ClPresenterL1n.h"
#ifdef OS_WIN_X32
#ifndef _NO_USE_ELVDLIB
#include "win32/ClPresenterCO10.h"
#include "win32/ClPresenterMO.h"
#endif
#endif
#include "ClUsbCmd.h"
#include "ClUsbCmdV2.h"
#include "EL_GUIDs.h"

#define		STREAM_WAIT_MUTEX	(1000)	//!< MutexのtryLockの待ち時間

#if 0
//FPSをログに表示させる場合には有効にする ※普段は無効にしておくこと
#define		DISPLAY_FPS
qreal		g_dTimeSum		= 0;		//合計時間
qreal		g_dCntFrame		= 0;		//合計フレーム数
qreal		g_dTimeBef		= 0;		//前フレームの時間
qreal		g_dTimeOpen		= 0;		//接続時の時間

//--------------------------------------------------------------------------------
/*!
	@fn			displayFPS
	@brief		FPS計測
 */
//--------------------------------------------------------------------------------
void displayFPS()
{
	//現在の時間取得
	QTime clTime = QTime::currentTime();
	qreal dHou	= clTime.hour();
	qreal dMin	= clTime.minute();
	qreal dSec	= clTime.second();
	qreal dMSec	= clTime.msec();
	qreal dTimeCur	= (dHou*60*60*1000) + (dMin*60*1000) + (dSec*1000) + (dMSec);

	//判定
	if(g_dTimeOpen != 0 && g_dTimeBef == 0)
	{
		//表示
		QString clStrLog = QString("First Frame Time : %1 ").arg(dTimeCur - g_dTimeOpen);
		qDebug() << clStrLog;

		//初期化
		g_dTimeSum	= 0;
		g_dCntFrame	= 0;
		g_dTimeOpen = 0;
	}
	else if(g_dTimeSum <= 1000)														//合計時間が1秒以下
	{
		if(g_dTimeBef != 0)
		{
			g_dTimeSum += (dTimeCur - g_dTimeBef);									//FPS計測用の合計時間更新
			g_dCntFrame++;															//FPS計測用のフレームカウント更新
		}
	}
	else																			//合計時間が1秒を超えた
	{
		//表示
		QString clStrLog = QString("FPS: %1, Time: %2 ").arg(g_dCntFrame).arg(g_dTimeSum);
		qDebug() << clStrLog;

		//初期化
		g_dTimeSum	= 0;
		g_dCntFrame	= 0;
	}

	//前フレームの時間更新
	g_dTimeBef = dTimeCur;
}
//--------------------------------------------------------------------------------
/*!
	@fn			initParamFPS
	@brief		FPS計測用パラメータ初期化
 */
//--------------------------------------------------------------------------------
void initParamFPS(QString clStrName)
{
	//初期化
	g_dTimeSum		= 0;															//合計時間
	g_dCntFrame		= 0;															//合計フレーム数
	g_dTimeBef		= 0;															//前フレームの時間
	g_dTimeOpen		= 0;															//接続時の時間

	//初期化時の時間取得
	QTime clTime = QTime::currentTime();
	qreal dHou	= clTime.hour();
	qreal dMin	= clTime.minute();
	qreal dSec	= clTime.second();
	qreal dMSec	= clTime.msec();
	g_dTimeOpen	= (dHou*60*60*1000) + (dMin*60*1000) + (dSec*1000) + (dMSec);

	//表示
	QString clStrLog = QString("[%1] Driver Open Time: %2 ").arg(clStrName).arg(clTime.toString("HH:mm:ss.zz"));
	qDebug() << clStrLog;
}
#endif

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
	//-------------------------------------------
	//	変数初期化
	//-------------------------------------------
	m_bTerminate = false;
	m_bZeroImage = false;

	m_pDevCtrl = a_devctrl;
	m_dSleepTime = 1 / static_cast<double>(30);
	m_iRectId = RECTID_PICTURE_HIGH;

	m_lImgQuality		= 80;
	m_lStreamSoundType	= 0;
	m_lStreamImageType	= 0;
	m_lStreamFRate		= 30;

#if !defined(STREAM_CMD_SPLT)
	//-------------------------------------------
	//	音声出力設定
	//-------------------------------------------
	//音声出力設定
	m_pclAudioOutput	= NULL;
	m_pclAudioStream	= NULL;

	//スリープ設定
	m_clSemSleep.tryAcquire( 1 );						//セマフォ設定
#endif
}
#if !defined(STREAM_CMD_SPLT)
//--------------------------------------------------------------------------------
/*!
	@fn			sleep
	@brief		音声出力用セマフォ
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClSourceImageThread::sleep( quint32  ulMsec )
{
	m_clSemSleep.tryAcquire( 2, ulMsec );
}
#endif
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
	m_dSleepTime = static_cast<double>(1) / static_cast<double>(lRate);
}
//--------------------------------------------------------------------------------
/*!
	@fn			setQuality(qint32)
	@brief		set image quality
	@param		lQuality	:	image quality
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClSourceImageThread::setQuality(qint32 lQuality)
{
	m_lImgQuality = lQuality;
}
//--------------------------------------------------------------------------------
/*!
	@fn			setStreamParam(qint32, qint32, qint32)
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClSourceImageThread::setStreamParam(qint32 lType, qint32 lCompType, qint32 lFrate)
{
	m_lStreamSoundType	= lType;
	m_lStreamImageType	= lCompType;
	m_lStreamFRate		= lFrate;
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
	@fn			getQuality()
	@brief		get image quality
	@return		qint32	:	image quality
 */
//--------------------------------------------------------------------------------
qint32 ClSourceImageThread::getQuality()
{
	return static_cast<qint32>(m_lImgQuality);
}
//--------------------------------------------------------------------------------
/*!
	@fn			getStreamSoundType()
	@brief		get stream sound compression type
	@return		qint32	:	sound compression type
 */
//--------------------------------------------------------------------------------
qint32 ClSourceImageThread::getStreamSoundType()
{
	return static_cast<qint32>(m_lStreamSoundType);
}
//--------------------------------------------------------------------------------
/*!
	@fn			getStreamImageType()
	@brief		get stream image compression type
	@return		qint32	:	image compression type
 */
//--------------------------------------------------------------------------------
qint32 ClSourceImageThread::getStreamImageType()
{
	return static_cast<qint32>(m_lStreamImageType);
}
//--------------------------------------------------------------------------------
/*!
	@fn			getStreamFrate()
	@brief		get stream frame rate
	@return		qint32	:	frame rate
 */
//--------------------------------------------------------------------------------
qint32 ClSourceImageThread::getStreamFrate()
{
	return static_cast<qint32>(m_lStreamFRate);
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
	QStringList qStrListParam;

	qint32      lRetryCnt = 0;

#if !defined(STREAM_CMD_SPLT)
	//-------------------------------------------
	//	ストリーム開始処理
	//-------------------------------------------
	if(m_pDevCtrl->getProductId() == PRODUCT_SESAME)										//ProductIDがSESAMEの場合
	{
		//フォーマット設定
		QAudioFormat clFormat;
		clFormat.setFrequency(44100);														//サンプリング周波数
		clFormat.setChannels(2);															//チャネル数
		clFormat.setSampleSize(16);															//bps
		clFormat.setCodec(AUDIO_INFO_FORMAT_QT);											//フォーマット
		clFormat.setSampleType(QAudioFormat::SignedInt);

		//デフォルト音声出力デバイス情報取得
		QAudioDeviceInfo clInfo = QAudioDeviceInfo::defaultOutputDevice();
		if(clInfo.isNull() == true ||														//取得失敗
		   clInfo.isFormatSupported(clFormat) == false){									//フォーマット未対応
			qDebug("this audio format is not supported");
		}else{
			m_pclAudioOutput = new QAudioOutput(clFormat);
		}

		//ストリーム開始コマンドのパラメータ設定
		qStrListParam.clear();
		qStrListParam.append(QString("%1").arg(m_lStreamSoundType));						//0:NONE, 1:PCM, 2:AAC
		qStrListParam.append(QString("%1").arg(m_lStreamImageType));						//0:JPEG, 1:H.264
		qStrListParam.append(QString("%1").arg(m_lStreamFRate));							//FPS
		qvParam = qStrListParam;															//パラメータ設定

		//ストリーム開始コマンド実行
		lRetryCnt = 0;																		//リトライカウント初期化
		while(lRetryCnt < RETRY_MAX)
		{
			lRes = m_pDevCtrl->CommandPresenter(START_STREAM_V2,qvParam);					//ストリーム取得 開始
			if(lRes == IM_ERROR_SUCCESS){													//成功
				qDebug("stream start command :: success");
				break;
			}else{																			//失敗
				lRetryCnt++;
			}
		}
		if(lRetryCnt >= RETRY_MAX){
			qDebug("stream start command :: failure");
		}
	}
#endif

	//-------------------------------------------
	//	スレッドメイン
	//-------------------------------------------
	lRetryCnt = 0;																			//リトライカウント初期化
	while(m_bTerminate == false)															//終了要求があるまでループ
	{
		dBefore = timer.getAbsoluteTime();													//処理開始時間取得
#if 0
		qvParam.setValue<eKINDRECTID>(m_iRectId);
#endif
#if !defined(STREAM_CMD_SPLT)
		//画像取得
		if(m_pDevCtrl->getProductId() == PRODUCT_SESAME)									//ProductIDがSESAMEの場合
		{
			//ストリーム取得コマンド送信
			lRes = m_pDevCtrl->CommandPresenter(GET_STREAM_V2,qvParam);						//ストリーム取得 (ストリーム取得ではストリームタイプが戻値となる)
			if(lRes != ELMO_STREAM_TYPE_JPG){												//JPG以外のストリームタイプの場合
				if(lRes == ELMO_STREAM_TYPE_SOUND_PCM)										//PCMの場合
				{
					//	音声出力開始
					if( (NULL != m_pclAudioOutput)  && (NULL == m_pclAudioStream) )
					{
						m_pclAudioStream = m_pclAudioOutput->start();
					}

					//データ取り出し
					if(qvParam.type() == QVariant::ByteArray){								//受信データの型確認
						m_qQueue << qvParam;
						while(m_qQueue.count() > 1 ){
							m_qQueue.removeFirst();
						}
					}

					//	音声データ書込み
					QByteArray qArrayAudio = m_qQueue.at(0).toByteArray();					//格納
					if(qArrayAudio.isNull() == false)										//空かを確認
					{
						qint64  llLenData	= qArrayAudio.size();
						qint64  llLenWrite	= 0;
						qint64  llRetry		= 0;
						while( 0 < llLenData )
						{
							llLenWrite = m_pclAudioStream->write(qArrayAudio);
							llLenData -= llLenWrite;
							if( 0 < llLenData )
							{
								llRetry++;
								qArrayAudio.remove( 0, llLenWrite );
								sleep( TIME_SLEEP_FOR_RETRY_SOUND );
							}
						}
					}
				}
				continue;
			}
		}
		else																				//ProductIDがSESAME以外の場合
		{
			qStrListParam.clear();
			qStrListParam.append(QString::number(m_iRectId));
			qStrListParam.append(QString::number(m_lImgQuality));
			qvParam = qStrListParam;
			lRes = m_pDevCtrl->CommandPresenter(GET_PICTURE,qvParam);						//Jpg取得
		}
#else
		//画像取得
		qStrListParam.clear();
		qStrListParam.append(QString::number(m_iRectId));
		qStrListParam.append(QString::number(m_lImgQuality));
		qvParam = qStrListParam;
		lRes = m_pDevCtrl->CommandPresenter(GET_PICTURE,qvParam);							//Jpg取得
#endif
		//結果判定
		if(lRes == IM_ERROR_CANNOT_OUTPUT_IMAGE){											//SDモード
			if(m_bZeroImage == false){
				DBGFL("Start picture size zero");
				emit sigStartZeroImage(m_pDevCtrl->getProductId());
				m_bZeroImage = true;
				m_qQueue.clear();
			}
			continue;
		}else if(lRes == IM_ERROR_DEVICE_REC){												//本体録画中
			if(m_bZeroImage == false){
				DBGFL("Start device rec mode");
				emit sigStartZeroImage(m_pDevCtrl->getProductId());
				m_bZeroImage = true;
				m_qQueue.clear();
			}
			continue;
		}else if(lRes == IM_ERROR_DEVICE_VIDEOREPLAY){										//本体再生中
			if(m_bZeroImage == false){
				DBGFL("Start device movie mode");
				emit sigStartZeroImage(m_pDevCtrl->getProductId());
				m_bZeroImage = true;
				m_qQueue.clear();
			}
			continue;
		}else if(lRes == IM_ERROR_DEVICE_BUSY){												//処理がかぶった場合
			DBGFL("Overlapped command. Document Camera is busy");
			qDebug("retry cnt %d ,max %d, thread flg %d",lRetryCnt ,RETRY_MAX ,m_bTerminate);
			if(lRetryCnt >= RETRY_MAX){
				qDebug("finish thread");
				emit sigGetStreamFail(m_pDevCtrl->getProductId());
				m_bTerminate = true;
				lRetryCnt = 0;
				msleep(100);
			}
			lRetryCnt++;
			continue;
		}else if(lRes < IM_ERROR_SUCCESS){													//その他の失敗
			if(m_pDevCtrl->IsOpenDriver() == false){
				DBGFL("Device not connect");
			}else{
				DBGFL("GET_PICTURE failure");
			}
			emit sigGetStreamFail(m_pDevCtrl->getProductId());
			m_bTerminate = true;
			msleep(100);
			continue;
		}else if(lRes >= IM_ERROR_SUCCESS){													//成功
			if(m_bZeroImage == true){
				DBGFL("End picture size zero");
				emit sigEndZeroImage(m_pDevCtrl->getProductId());
			}
			m_bZeroImage = false;
		}
		lRetryCnt = 0;																		//リトライカウント初期化

		//受信データ確認
		if(qvParam.type() == QVariant::ByteArray){											//受信データの型確認
			m_qQueue << qvParam;
			while(m_qQueue.count() > 1 ){
				m_qQueue.removeFirst();
			}
			emit sigSourceImage(m_pDevCtrl->getProductId(), lRes, m_qQueue);
		}
		dAfter = timer.getAbsoluteTime();													//処理終了時間取得

		//フレームレート調整処理
		if(dBefore != 0 && dAfter != 0)
		{
			lSleepTime = static_cast<qint32>((m_dSleepTime - (dAfter - dBefore)) * 1000);	//sleep時間設定
			if(lSleepTime > 0)
			{
				msleep(static_cast<ulong>(lSleepTime));										//フレームレート調整用sleep
#if 0
				qDebug() << "msleep --- " << static_cast<ulong>(lSleepTime) << m_dSleepTime  << dAfter << dBefore;
#endif
			}
		}
#ifdef DISPLAY_FPS
		displayFPS();																		//FPS計測処理
#endif
	}

	//-------------------------------------------
	//	画像サイズ0の場合の処理
	//-------------------------------------------
	if(m_bZeroImage == true){
		DBGFL("End picture size zero");
		emit sigEndZeroImage(m_pDevCtrl->getProductId());
	}

#if !defined(STREAM_CMD_SPLT)
	//-------------------------------------------
	//	ストリーム終了処理
	//-------------------------------------------
	if(m_pDevCtrl->getProductId() == PRODUCT_SESAME)										//ProductIDがSESAMEの場合
	{
		qint32 lRes = IM_ERROR_SUCCESS;
		QVariant qvParam;

		//音声出力終了処理
		if(m_pclAudioOutput != NULL)
		{
			m_pclAudioOutput->stop();
			delete m_pclAudioOutput;
			m_pclAudioOutput	= NULL;
		}

		//ストリーム終了コマンドのパラメータ設定
		qvParam.setValue<eKINDRECTID>(m_iRectId);											//パラメータ設定

		//ストリーム終了コマンド実行
		lRetryCnt = 0;																		//リトライカウント初期化
		while(lRetryCnt < RETRY_MAX)
		{
			lRes = m_pDevCtrl->CommandPresenter(STOP_STREAM_V2,qvParam);					//ストリーム終了コマンド送信
			if(lRes == IM_ERROR_SUCCESS){													//成功
				qDebug("stream stop command :: success");
				break;
			}else{																			//失敗
				lRetryCnt++;
			}
		}
		if(lRetryCnt >= RETRY_MAX){
			qDebug("stream stop command :: failure");
		}
	}
#endif

	emit sigTerminate(m_pDevCtrl->getProductId());
	DBGFL("Terminate ClSourceImageThread");
}
//--------------------------------------------------------------------------------
/*!
	@fn			slotGetStreamImg()
	@brief		get stream got image
	@return		qint32	:	parameter
 */
//--------------------------------------------------------------------------------
qint32 ClSourceImageThread::slotGetStreamImg(QVariant& qParameter)
{
	if(m_qQueue.isEmpty()){
		return IM_ERROR_COMMAND_FAILURE;
	}
	if(m_qQueue.at(0).isNull() || m_qQueue.at(0).type() != QVariant::ByteArray){
		return IM_ERROR_COMMAND_FAILURE;
	}
	qParameter = m_qQueue.at(0).toByteArray();
	return IM_ERROR_SUCCESS;
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

	m_dSleepTime	= 1 / static_cast<double>(10);
	m_lProductId	= PRODUCT_NONE;
	m_lStatus		= IM_ERROR_SUCCESS;
	m_qJpegArray	= QByteArray();
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
	m_dSleepTime = static_cast<double>(1) / static_cast<double>(lRate);
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
	@fn			slotGetStreamFail(qint32)
	@brief		get Product ID
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
#if 0
				qDebug() << "msleep --- " << static_cast<ulong>(lSleepTime) << m_dSleepTime  << dAfter << dBefore;
#endif
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
	@fn			ClRawImageThread
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
ClRawImageThread::ClRawImageThread(QObject *parent)
	: QThread(parent)
{
	m_bTerminate = false;

	m_dSleepTime	= 1 / static_cast<double>(10);
	m_lProductId	= PRODUCT_NONE;
	m_lStatus		= IM_ERROR_SUCCESS;
	m_qJpegArray	= QByteArray();

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
	m_dSleepTime = static_cast<double>(1) / static_cast<double>(lRate);
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
	@fn			slotSourceImage(qint32, qint32, QVariantList)
	@brief		get Rect ID
 */
//--------------------------------------------------------------------------------
void ClRawImageThread::slotSourceImage(qint32 lProductId, qint32 lStatus, QVariantList queue)
{
	QtDXUTTimer timer;
	static double dDebBefore = 0;

	m_lProductId = lProductId;
	m_lStatus = lStatus;
	if(queue.isEmpty() == false && queue.at(0).type() == QVariant::ByteArray)		//何かしらQByteArrayなデータを受け取った場合
	{
		if(false == this->m_clMutex.tryLock(STREAM_WAIT_MUTEX))						//JPEG領域保護Mutexロック開始を試みる
		{
			return;																	//Mutexが取得できなかった場合は何も起こらなかったことにする
		}
		this->m_qJpegArray = queue.at(0).toByteArray();								//受け取ったQByteArrayをメンバー変数にて保持。暗黙の共有がなされるのでそれほど時間はかからないはず
		this->m_clMutex.unlock();													//JPEG領域保護Mutexを解除
#ifndef SEND_SAME_IMGDATA
		m_bIsUpdateImg = true;														//映像の更新がはいったのでフラグを変更
#endif
		m_bUpdate = true;
		DBGFL("slot event frame rate : %f",dDebBefore - timer.getAbsoluteTime());
		dDebBefore = timer.getAbsoluteTime();
	}
	else
	{
		DBGFL("slotSourceImage Failure");
		m_bTerminate = true;
		m_qJpegArray = QByteArray();
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn			slotStartZeroImage(qint32)
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
	@fn			slotEndZeroImage(qint32)
	@brief		get Rect ID
 */
//--------------------------------------------------------------------------------
void ClRawImageThread::slotEndZeroImage(qint32 lProductId)
{
	emit sigEndZeroImage(lProductId, RAW_STREAM);
}
//--------------------------------------------------------------------------------
/*!
	@fn			slotGetStreamFail(qint32)
	@brief		get Product ID
 */
//--------------------------------------------------------------------------------
void ClRawImageThread::slotGetStreamFail(qint32 lProductId)
{
	m_qJpegArray = QByteArray();
	emit sigGetStreamFail(lProductId, RAW_STREAM);
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
#if 0
				qDebug() << "msleep --- " << static_cast<ulong>(lSleepTime) << m_dSleepTime  << dAfter << dBefore;
#endif
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
				QThread::sleep(5000);
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
	m_bOpen				= false;
	m_pclUseDevice		= NULL;						//set NULL for m_pclUseDevice

	m_lProductId		= PRODUCT_L1EX;				//set default product id
	m_bInitialized		= false;
	m_pclSrcThread		= NULL;						//set NULL for m_pclSrcThread
	m_pclRawThread		= NULL;						//set NULL for m_pclRawThread
	m_pclJpegThread		= NULL;						//set NULL for m_pclJpegThread

	m_pclSendFileThread	= NULL;						//set NULL for m_pclSendFileThread

	m_lStreamSoundType	= 0;
	m_lStreamImageType	= 0;
	m_lStreamFRate		= 30;
}

ClImDrvCtrl::ClImDrvCtrl(qint32 _productId, QObject *parent) :
	QObject(parent)
{
	m_bOpen = false;
	m_pclUseDevice		= NULL;						//set NULL for m_pclUseDevice

	m_lProductId		= _productId;
	m_bInitialized		= false;
	m_pclSrcThread		= NULL;						//set NULL for m_pclSrcThread
	m_pclRawThread		= NULL;						//set NULL for m_pclRawThread
	m_pclJpegThread		= NULL;						//set NULL for m_pclJpegThread

	m_pclSendFileThread	= NULL;						//set NULL for m_pclSendFileThread

	m_lStreamSoundType	= 0;
	m_lStreamImageType	= 0;
	m_lStreamFRate		= 30;
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
	if(m_pclJpegThread != NULL){
		if(m_pclJpegThread->isRunning()){
			m_pclJpegThread->slotLoopTerminate();
#if 0
			m_pclJpegThread->wait(5000);
#else
			m_pclJpegThread->wait();
#endif
		}
		delete m_pclJpegThread;
		m_pclJpegThread = NULL;
	}
	if(m_pclRawThread != NULL){
		if(m_pclRawThread->isRunning()){
			m_pclRawThread->slotLoopTerminate();
#if 0
			m_pclRawThread->wait(5000);
#else
			m_pclRawThread->wait();
#endif
		}
		delete m_pclRawThread;
		m_pclRawThread = NULL;
	}
	if(m_pclSrcThread != NULL){
		if(m_pclSrcThread->isRunning()){
			m_pclSrcThread->slotLoopTerminate();
#if 0
			m_pclSrcThread->wait(20000);
#else
			m_pclSrcThread->wait();
#endif
		}
		delete m_pclSrcThread;
		m_pclSrcThread = NULL;
	}
	if(m_pclSendFileThread != NULL){
		if(m_pclSendFileThread->isRunning()){
			m_pclSendFileThread->stop();
			m_pclSendFileThread->wait(20000);
		}
		delete m_pclSendFileThread;
		m_pclSendFileThread = NULL;
	}

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
bool ClImDrvCtrl::OpenDriver(QString strDevPath)
{
	// IDを指定してプレセンターを開くメソッド
	if(isInitialize() == false){
		return false;
	}
	QMutexLocker qLocker(&m_Mutex);								//start mutex

	DBGFL("OpenDriver win32");

	//Open L-12
	if(m_lProductId == PRODUCT_SESAME){							//look for the same ProductId or ProductId is null
		m_pclUseDevice = new ClPresenterL12(PRODUCT_SESAME,this);
		m_pclUseDevice->open();									//
		if(m_pclUseDevice->isOpen() == true){
#ifdef DISPLAY_FPS
			initParamFPS("L-12");								//FPS計測処理
#endif
			m_bOpen = true;										//set Open flag is true
			return true;										//return success which already opened
		}
	}
	//Open L1ex
	if(m_lProductId == PRODUCT_L1EX){							//look for the same ProductId or ProductId is null
		m_pclUseDevice = new ClPresenterL1ex(PRODUCT_L1EX,this);
		m_pclUseDevice->open();									//
		if(m_pclUseDevice->isOpen() == true){
#ifdef DISPLAY_FPS
			initParamFPS("L-1ex");								//FPS計測処理
#endif
			m_bOpen = true;										//set Open flag is true
			return true;										//return success which already opened
		}
	}
	//Open L1n
	if(m_lProductId == PRODUCT_L1N){							//look for the same ProductId or ProductId is null
		m_pclUseDevice = new ClPresenterL1n(PRODUCT_L1N,this);
		m_pclUseDevice->open();									//
		if(m_pclUseDevice->isOpen() == true){
#ifdef DISPLAY_FPS
			initParamFPS("L-1n");								//FPS計測処理
#endif
			m_bOpen = true;										//set Open flag is true
			return true;										//return success which already opened
		}
	}

	// Open P10s
	if(m_lProductId == PRODUCT_P10S){							//look for the same ProductId or ProductId is null
		m_pclUseDevice = new ClPresenterP10s(PRODUCT_P10S,this);
		m_pclUseDevice->open();							//
		if(m_pclUseDevice->isOpen() == true){
#ifdef DISPLAY_FPS
			initParamFPS("P-10S");								//FPS計測処理
#endif
			m_bOpen = true;										//set Open flag is true
			return true;										//return success which already opened
		}
	}
	//Open P10
	if(m_lProductId == PRODUCT_P10){							//look for the same ProductId or ProductId is null
		m_pclUseDevice = new ClPresenterL1ex(PRODUCT_P10,this);
		m_pclUseDevice->open();									//
		if(m_pclUseDevice->isOpen() == true){
#ifdef DISPLAY_FPS
			initParamFPS("P-10");								//FPS計測処理
#endif
			m_bOpen = true;										//set Open flag is true
			return true;										//return success which already opened
		}
	}
	//Open P30s
	if(m_lProductId == PRODUCT_P30S){							//look for the same ProductId or ProductId is null
		m_pclUseDevice = new ClPresenterP30s(PRODUCT_P30S,this);
		processSleep(5);
		m_pclUseDevice->open();									//
		if(m_pclUseDevice->isOpen() == true){
#ifdef DISPLAY_FPS
			initParamFPS("P-30S");								//FPS計測処理
#endif
			m_bOpen = true;										//set Open flag is true
			return true;										//return success which already opened
		}
	}
	//Open P100
	if(m_lProductId == PRODUCT_P100){							//look for the same ProductId or ProductId is null
		m_pclUseDevice = new ClPresenterP100(PRODUCT_P100,this);
		processSleep(5);
		m_pclUseDevice->open();									//
		if(m_pclUseDevice->isOpen() == true){
#ifdef DISPLAY_FPS
			initParamFPS("P-100");								//FPS計測処理
#endif
			m_bOpen = true;										//set Open flag is true
			return true;										//return success which already opened
		}
	}
	//Open FF1
	if(m_lProductId == PRODUCT_FF1){							//look for the same ProductId or ProductId is null
		m_pclUseDevice = new ClPresenterFF1(PRODUCT_FF1,this);
		m_pclUseDevice->open();									//
		if(m_pclUseDevice->isOpen() == true){
#ifdef DISPLAY_FPS
			initParamFPS("FF-1/HV-110u");						//FPS計測処理
#endif
			m_bOpen = true;										//set Open flag is true
			return true;										//return success which already opened
		}
	}
#ifdef OS_WIN_X32
#ifndef _NO_USE_ELVDLIB
	//Open CO-10
	if(m_lProductId == PRODUCT_CO10){							//look for the same ProductId or ProductId is null
		m_pclUseDevice = new ClPresenterCO10(PRODUCT_CO10,this);
		m_pclUseDevice->open();									//
		processSleep(5);
		if(m_pclUseDevice->isOpen() == true){
#ifdef DISPLAY_FPS
			initParamFPS("CO-10(i-Pochette)");					//FPS計測処理
#endif
			m_bOpen = true;										//set Open flag is true
			return true;										//return success which already opened
		}
	}

	//Open MO-1/MO-1w
	if(m_lProductId == PRODUCT_MOBILE){							//look for the same ProductId or ProductId is null
		m_pclUseDevice = new ClPresenterMO(PRODUCT_MOBILE,strDevPath,this);
		m_pclUseDevice->open();									//
		if(m_pclUseDevice->isOpen() == true){
#ifdef DISPLAY_FPS
			initParamFPS("ELMO UVC/UAC Camera");				//FPS計測処理
#endif
			processSleep(5);
			m_bOpen = true;										//set Open flag is true
			return true;										//return success which already opened
		}
	}

	//Open Other UVC
	if( (m_lProductId >> 16) != 0x09a1 ){						//look for the same ProductId or ProductId is null
		m_pclUseDevice = new ClPresenterUVC(m_lProductId,strDevPath,this);
		m_pclUseDevice->open();									//
		if(m_pclUseDevice->isOpen() == true){
#ifdef DISPLAY_FPS
			initParamFPS("Other UVC/UAC Camera");				//FPS計測処理
#endif
			processSleep(5);
			m_bOpen = true;										//set Open flag is true
			return true;										//return success which already opened
		}
	}
#endif //_NO_USE_ELVDLIB
#endif //OS_WIN_X32
	return false;
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
	}
	m_bOpen = false;											//set Open flag is false

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

	if(lRes <  IM_ERROR_SUCCESS					&&
		lRes != IM_ERROR_CANNOT_OUTPUT_IMAGE	&&
		lRes != IM_ERROR_DEVICE_REC				&&
		lRes != IM_ERROR_DEVICE_VIDEOREPLAY		&&
		lRes != IM_ERROR_DEVICE_BUSY)
	{
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
	@fn			startStream
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClImDrvCtrl::startStream(ClUsbDriver *pclUsbDriver,eKINDSTREAM iKind, qint32 iFrate, qint32 iQuality)
{
	if(isInitialize() == false){									// is not initialized
		return IM_ERROR_NOT_INIT;									// return IM_ERROR_NOT_INIT
	}

	if(m_pclUseDevice == NULL || IsOpenDriver() == false){
		return IM_ERROR_NOT_INIT;
	}

	if(iFrate <= 0 || iFrate > 30){									//fool proof
		iFrate = 30;
	}

	if(iQuality <= 0 || iQuality > 100){							//fool proof
		iQuality = 80;
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
			m_pclSrcThread->setQuality(iQuality);
			m_pclSrcThread->setStreamParam(m_lStreamSoundType, m_lStreamImageType, m_lStreamFRate);
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
					pclUsbDriver,SLOT(slotTerminate(qint32,eKINDSTREAM)));
			connect(m_pclSrcThread,SIGNAL(sigGetStreamFail(qint32)),
					m_pclRawThread,SLOT(slotGetStreamFail(qint32)));
			connect(m_pclRawThread,SIGNAL(sigGetStreamFail(qint32,eKINDSTREAM)),
					pclUsbDriver,SLOT(slotGetStreamFail(qint32,eKINDSTREAM)));
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
					pclUsbDriver,SLOT(slotTerminate(qint32,eKINDSTREAM)));
			connect(m_pclSrcThread,SIGNAL(sigGetStreamFail(qint32)),
					m_pclJpegThread,SLOT(slotGetStreamFail(qint32)));
			connect(m_pclJpegThread,SIGNAL(sigGetStreamFail(qint32,eKINDSTREAM)),
					pclUsbDriver,SLOT(slotGetStreamFail(qint32,eKINDSTREAM)));
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
#if 0
			bRes = m_pclJpegThread->wait(5000);				//wait end of thread at about 5 seconds
#else
			bRes = m_pclJpegThread->wait();					//wait end of thread
#endif
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
#if 0
			bRes = m_pclRawThread->wait(5000);				//wait end of thread at about 5 seconds
#else
			bRes = m_pclRawThread->wait();					//wait end of thread
#endif
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
#if 0
			bRes = m_pclSrcThread->wait(20000);				//wait end of thread at about 20 seconds
#else
			bRes = m_pclSrcThread->wait();					//wait end of thread
#endif
			if(bRes == false){
				DBGFL("quit source thread failure");
			}
//			delete m_pclSrcThread;							//release thread handle
			m_pclSrcThread = NULL;							//set NULL
		}
	}

	return IM_ERROR_SUCCESS;
}


qint32 ClImDrvCtrl::startInputStream()
{
	if( m_pclUseDevice != NULL )
	{
		m_pclUseDevice->startStream();
	}
	return IM_ERROR_SUCCESS;
}

qint32 ClImDrvCtrl::stopInputStream()
{
	// 映像ソースの停止
	if(m_pclUseDevice != NULL)
	{
		m_pclUseDevice->stopStream();
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
	@fn			setQuality
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClImDrvCtrl::setQuality(qint32 lQuality, eKINDSTREAM iKind)
{
	QMutexLocker qLocker(&m_Mutex);							//start mutex
	if(iKind == RAW_STREAM){
		if(m_pclSrcThread != NULL){
			m_pclSrcThread->setQuality(lQuality);			//set image quality for RawThread
		}
	}
	if(iKind == JPEG_STREAM){
		if(m_pclSrcThread != NULL){
			m_pclSrcThread->setQuality(lQuality);			//set image quality for RawThread
		}
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn			getQuality
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClImDrvCtrl::getQuality(eKINDSTREAM iKind)
{
	qint32 lRes = 0;
	QMutexLocker qLocker(&m_Mutex);							//start mutex
	if(iKind == RAW_STREAM){
		if(m_pclSrcThread != NULL){
			lRes = m_pclSrcThread->getQuality();			//get image quality from RawThread
		}
	}
	if(iKind == JPEG_STREAM){
		if(m_pclSrcThread != NULL){
			lRes = m_pclSrcThread->getQuality();			//get image quality from JpegThread
		}
	}
	return lRes;
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
		Sleep(100);
//		_sleep(100);
//		msleep(100);
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
	@fn			GetStreamImg
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClImDrvCtrl::getStreamImg(QVariant& qParameter)
{
	if(m_pclSrcThread == NULL ||
	   m_pclSrcThread->isRunning() == false)
	{
		return IM_ERROR_COMMAND_FAILURE;
	}

	m_pclSrcThread->slotGetStreamImg(qParameter);
	if(qParameter.type() != QVariant::ByteArray || qParameter.toByteArray().isEmpty()){
		return IM_ERROR_COMMAND_FAILURE;
	}
	return IM_ERROR_SUCCESS;
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
//--------------------------------------------------------------------------------
/*!
	@fn			setStreamParam(eKINDSTREAM, qint32, qint32, qint32)
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClImDrvCtrl::setStreamParam(eKINDSTREAM iKind, qint32 lType, qint32 lCompType, qint32 lFrate)
{
	Q_UNUSED(iKind)
	m_lStreamSoundType	= lType;
	m_lStreamImageType	= lCompType;
	m_lStreamFRate		= lFrate;
	return;
}
//--------------------------------------------------------------------------------
/*!
	@fn			getStreamParam(eKINDSTREAM, qint32, qint32, qint32)
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClImDrvCtrl::getStreamParam(eKINDSTREAM iKind, qint32 &lType, qint32 &lCompType, qint32 &lFrate)
{
	Q_UNUSED(iKind)
	lType		= m_lStreamSoundType;
	lCompType	= m_lStreamImageType;
	lFrate		= m_lStreamFRate;
	return;
}

/*! ------------------------------------------------------------------------------

	@brief		カメラ操作リモコンを返す

  ------------------------------------------------------------------------------- */
QWidget*	ClImDrvCtrl::getRemoconWidget()
{
	return m_pclUseDevice->getRemoconWidget();
}

/*! ------------------------------------------------------------------------------

	@brief		言語更新

  ------------------------------------------------------------------------------- */
 void		ClImDrvCtrl::setTranslator()
{
	 m_pclUseDevice->setTranslator();
}

/*! ------------------------------------------------------------------------------

	@brief		対応解像度取得

  ------------------------------------------------------------------------------- */
QStringList	ClImDrvCtrl::getSupportedResolutions()
{
	return m_pclUseDevice->getSupportedResolutions();
}

/*! ------------------------------------------------------------------------------

	@brief		現在の解像度取得

  ------------------------------------------------------------------------------- */
QString		ClImDrvCtrl::getCurrentResolution()
{
	return m_pclUseDevice->getCurrentResolution();
}

/*! ------------------------------------------------------------------------------

	@brief		現在の解像度設定

  ------------------------------------------------------------------------------- */
qint32		ClImDrvCtrl::setCurrentResolution(QString strResolution)
{
	HRESULT hr;

	hr = m_pclUseDevice->setCurrentResolution(strResolution);

	return hr;
}

/*! ------------------------------------------------------------------------------

	@brief		現在のフレームレート設定

  ------------------------------------------------------------------------------- */
qint32		ClImDrvCtrl::setCurrentFrameRate(bool bIsFrameRateMax)
{
	HRESULT hr;

	hr = m_pclUseDevice->setCurrentFrameRate(bIsFrameRateMax);

	return hr;
}

/*! ------------------------------------------------------------------------------

	@brief		再接続

  ------------------------------------------------------------------------------- */
void		ClImDrvCtrl::reopen()
{
	if(m_pclUseDevice != NULL && m_lProductId == PRODUCT_MOBILE)
	{
		m_pclUseDevice->reopen();
	}
}
