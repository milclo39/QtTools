/*--------------------------------------------------------------------------------*/
/*!
    @file	ClStreamView.cpp
    @brief	RTSPストリーミング/USBストリーミング処理
*/
/*--------------------------------------------------------------------------------*/
#include "ClStreamView.h"
#include "CmnValue.h"
#include "ClVisualPresenter.h"
#include <QTimer>
#include <QApplication>

#include "ClCamScanner.h"
#include "ClUsbDriver/Command_Presenter.h"
//--------------------------------------------------------------------------------//
/*!
	@brief		コンストラクタ
*/
//--------------------------------------------------------------------------------//
ClStreamView::ClStreamView(QQuickItem *parent) : QQuickPaintedItem(parent)
{
	setAcceptedMouseButtons(Qt::AllButtons);
	m_pDecoder = NULL;

	ClStreamDecoder::avStreamInit();
	m_pUsbDriver = NULL;
	m_llPts = 0;
}

//--------------------------------------------------------------------------------//
/*!
	@brief		デストラクタ
*/
//--------------------------------------------------------------------------------//
ClStreamView::~ClStreamView()
{
	ClStreamDecoder::avStreamDeinit();
}

//--------------------------------------------------------------------------------//
/*!
	@brief		USBストリーミング初期化
*/
//--------------------------------------------------------------------------------//
void ClStreamView::setup()
{
	ClCamDevice* pclCamDev = ClVisualPresenter::getInstance()->device();
	if (pclCamDev == NULL){
		return;
	}
	stCameraInfo info = pclCamDev->getInfo();
	if (info.con_sta != stCameraInfo::CONNECTED){
		return;
	}
    if(m_pUsbDriver == NULL){
        m_pUsbDriver = new ClUsbDriverForIM4;
        connect(m_pUsbDriver, SIGNAL(sigRawImage(qint32,qint32,QImage)), this, SLOT(slotGetStreamRaw(qint32,qint32,QImage)));
        connect(m_pUsbDriver, SIGNAL(sigErrorNormal(qint32)),this,SLOT(slotDriverError(qint32)));
        connect(m_pUsbDriver, SIGNAL(sigErrorFatal(qint32)),this,SLOT(slotDriverError(qint32)));
        connect(m_pUsbDriver, SIGNAL(sigGetStreamFail(qint32,eKINDSTREAM)),this,SLOT(slotStreamError(qint32,eKINDSTREAM)));
	}
	//ProductIDをINTに変換
#if defined(Q_OS_WIN)
    qint32	lPid = 0;
    int		iIndex1 = 0, iIndex2 = 0;
	QString	strTemp;

	iIndex1 = info.id.indexOf( "pid_", 0, Qt::CaseInsensitive );
	iIndex2 = iIndex1 + 4;
	while( info.id.count() >= iIndex2 )
	{
		if( 0 == isxdigit( info.id[iIndex2].toLatin1() ) )
		{
			break;
		}
		iIndex2++;
	}
	strTemp = info.id.mid( iIndex1 + 4, iIndex2 - iIndex1 - 4 );
	bool bRet = false;
	lPid = strTemp.toLong( &bRet, 16 );

	if( false == bRet ){
		return;
	}
	//ベンダーIDと合わせてデバイスを開く
	m_uiProductId = lPid | 0x09a10000;
#else
    bool bRet = false;
    m_uiProductId = info.id.right(8).toLong( &bRet, 16 );
#endif
    if(IM_ERROR_SUCCESS != m_pUsbDriver->openDevice(m_uiProductId, info.id)){
        return;
    }

	//ストリーム用にPTSを初期化
	m_llPts = 0;

    qint64 llFramerate = 10000000 / 2;
	m_iDuration = llFramerate / 111111 * 1000;
	m_dFramTime = llFramerate / 10000;
}
//--------------------------------------------------------------------------------//
/*!
	@brief		USBストリーミングIF
*/
//--------------------------------------------------------------------------------//
// 開始
void ClStreamView::startLive()
{
	setup();
	if (NULL == m_pUsbDriver){
		return;
	}
    if(m_pUsbDriver->startStream(m_uiProductId, RAW_STREAM, 15, 80) != IM_ERROR_SUCCESS){
		return;
	}
}
// 終了
void ClStreamView::stopLive()
{
	if (NULL == m_pUsbDriver){
		return;
	}
	m_pUsbDriver->stopStream(m_uiProductId, RAW_STREAM);
	disconnect(m_pUsbDriver, SIGNAL(sigRawImage(qint32,qint32,QImage)), this, SLOT(slotGetStreamRaw(qint32,qint32,QImage)));
	disconnect(m_pUsbDriver, SIGNAL(sigErrorNormal(qint32)),this,SLOT(slotDriverError(qint32)));
	disconnect(m_pUsbDriver, SIGNAL(sigErrorFatal(qint32)),this,SLOT(slotDriverError(qint32)));
	disconnect(m_pUsbDriver, SIGNAL(sigGetStreamFail(qint32,eKINDSTREAM)),this,SLOT(slotStreamError(qint32,eKINDSTREAM)));
	delete m_pUsbDriver;
	m_pUsbDriver = NULL;
}
// データ処理スロット
void ClStreamView::slotGetStreamRaw(qint32, qint32, QImage clImg)
{
	slotConvAvImage(clImg, 0);
	emit sigRawStream(clImg, IMG_USB_NORMAL);
}
void ClStreamView::slotConvAvImage(QImage img, qreal)
{
	ClAvImage avImage(img, m_dFramTime);		//AvImage作成
	avImage.setDts(m_llPts);					//DTSとPTSは同じとする
	avImage.setPts(m_llPts);					//
	avImage.setDiffPts(m_iDuration);			//設定転送
	slotImage(avImage);							//画像転送
	m_llPts += m_iDuration;						//PTSにDurationを加算していく
}
// エラー処理
void ClStreamView::slotDriverError(qint32 err)
{
	qDebug() << "slotDriverError:" << err;
}
void ClStreamView::slotStreamError(qint32 err, eKINDSTREAM kind)
{
	qDebug() << "slotStreamError:" << err << kind;
	QTimer::singleShot(1000, this, SLOT(startLive()));	// 1秒後リトライ
}

// ストリーミング開始
void ClStreamView::slotStreamStart(QString strUrl)
{
	if(NULL == m_pDecoder)
	{
		m_pDecoder = new ClStreamDecoder(this);
		connect(m_pDecoder, SIGNAL(sigImage(ClAvImage)), SLOT(slotImage(ClAvImage)));
		connect(m_pDecoder, SIGNAL(sigError()), this, SLOT(slotError()));
		connect(m_pDecoder, SIGNAL(sigChangeResolution(QSize)), this, SLOT(slotChangeResolution(QSize)));
	}
	m_pDecoder->setStream(strUrl);
}
// ストリーミング終了
void ClStreamView::slotStreamEnd()
{
	if(NULL != m_pDecoder)
	{
		disconnect(m_pDecoder, SIGNAL(sigImage(ClAvImage)), this, SLOT(slotImage(ClAvImage)));
		disconnect(m_pDecoder, SIGNAL(sigError()), this, SLOT(slotError()));
		disconnect(m_pDecoder, SIGNAL(sigChangeResolution(QSize)), this, SLOT(slotChangeResolution(QSize)));
		m_pDecoder->stop();
		m_pDecoder->wait();
		delete m_pDecoder;
		m_pDecoder = NULL;
	}
}
// データ処理スロット
void ClStreamView::slotImage(ClAvImage img)
{
    if(true == m_bMirror){
//        QMatrix clRotate; //qt6 remove
        QTransform clRotate;
        clRotate.rotate(180);
        m_imgDraw = img.getImage().transformed(clRotate);
    }
    else{
        m_imgDraw = img.getImage();
    }
    if(true == m_bFreeze){
        return;	// フリーズ中なので更新しない
    }
	QPainter painterRectLine(&m_imgDraw);
	painterRectLine.setPen(QPen(QColor(255, 0, 0, 128), 4));
	painterRectLine.drawRect(QRect());
	update();
}
// エラー処理
void ClStreamView::slotError()
{
	qDebug() << "rtsp error!!!";
	slotStreamEnd();
}
// 解像度変更通知
void ClStreamView::slotChangeResolution(QSize size)
{
	qDebug() << "change resolution" << size;
}
//--------------------------------------------------------------------------------//
/*!
	@brief		描画
*/
//--------------------------------------------------------------------------------//
void ClStreamView::paint(QPainter *painter)
{
	if(false == m_imgDraw.isNull()){
		QSize sizeDraw = m_imgDraw.size().scaled(size().toSize(), Qt::KeepAspectRatio);
		int iX = (width() - sizeDraw.width()) / 2;
		int iY = (height() - sizeDraw.height()) / 2;
		QRect rectDraw(QPoint(iX, iY), sizeDraw);
		painter->drawImage(rectDraw, m_imgDraw);
	}
}
// カメラ選択
void ClStreamView::selectCam()
{
    qint32 lDocCamID = 0;

    if (!ClCamScanner::getInstance()->isScanning(ClCamScannerFactory::SCANNER_TYPE_MAX))
    {
        ClCamScanner::getInstance()->startScan();
    }
    while (!ClCamScanner::getInstance()->isScanning(ClCamScannerFactory::SCANNER_TYPE_MAX))
    {
        QApplication::processEvents();
    }
    while (ClCamScanner::getInstance()->isScanning(ClCamScannerFactory::SCANNER_TYPE_USB_DOC_CAM))
    {
        QApplication::processEvents();
    }
    if (ClCamScanner::getInstance()->isScanning(ClCamScannerFactory::SCANNER_TYPE_MAX))
    {
        ClCamScanner::getInstance()->stopScan();
    }
    foreach (ClCamDevice* pclDev, ClCamScanner::getInstance()->camList())
    {
        stCameraInfo info = pclDev->getInfo();
        if ((info.type == stCameraInfo::DOC_CAM) &&
            (info.conn_type == stCameraInfo::USB || info.conn_type == stCameraInfo::ELUSB))
        {
            //if (info.con_sta != stCameraInfo::DISCONNECT)
            {
                if (true == info.id.contains(DEVICE_PREFIX_TX)) //TX-1/LX-1
                {
                    lDocCamID = 0x09a10020UL;
                }
                else //ELMOカメラ
                {
#if defined(Q_OS_WIN)
                    qint32 index_vid = info.id.indexOf("vid_");
                    qint32 index_pid = info.id.indexOf("pid_");
                    QString strID = info.id.mid(index_vid + 4, 4);
                    strID = strID + info.id.mid(index_pid + 4, 4);
                    lDocCamID = strID.toInt(0, 16);
#else
                    lDocCamID = info.id.right(8).toInt(0, 16);
#endif
                }
                connectCam(pclDev);
                m_nProduct = lDocCamID;
            }
            break;
        }
    }
}

// カメラ接続
void ClStreamView::connectCam(ClCamDevice* pclDev)
{
    ClCamDevice* pCurDev = ClVisualPresenter::getInstance()->device();
    if (nullptr != pCurDev && stCameraInfo::DISCONNECT != pCurDev->getInfo().con_sta)
    {
        return;
    }
    if (nullptr == pclDev && pCurDev == pclDev)
    {
        return;
    }

    if (nullptr != pCurDev)
    {
        disconnect(pCurDev, SIGNAL(connectResult(ClCamDevice::eCONN_RESULT)), this, SLOT(connectResult(ClCamDevice::eCONN_RESULT)));
        disconnect(pCurDev, SIGNAL(stateChanged()), this, SLOT(devStateChanged()));
        stCameraInfo curInfo = pCurDev->getInfo();
        curInfo.handling = false;
        pCurDev->setInfo(curInfo);
    }

//    connect(pclDev, SIGNAL(connectResult(ClCamDevice::eCONN_RESULT)), this, SLOT(connectResult(ClCamDevice::eCONN_RESULT)));
//    connect(pclDev, SIGNAL(stateChanged()), this, SLOT(devStateChanged()));
    stCameraInfo info = pclDev->getInfo();
    info.handling = true;
    pclDev->setInfo(info);

    if (info.inUse)
    {
        pclDev->connect();
    }

//    if(m_pUsbDriver)
//    {
//        m_pUsbDriver->scan(&pclDev);
//    }
}

void ClStreamView::execAf()
{
    if(m_pUsbDriver)
    {
        m_pUsbDriver->slotFocus(m_nProduct, FOCUS_AUTO);
    }
}
void ClStreamView::zoomTele()
{
    if(m_pUsbDriver)
    {
        m_pUsbDriver->slotFocus(m_nProduct, ZOOM_TELE);
    }
}
void ClStreamView::zoomWide()
{
    if(m_pUsbDriver)
    {
        m_pUsbDriver->slotFocus(m_nProduct, ZOOM_WIDE);
    }
}
void ClStreamView::irisOpen()
{
    if(m_pUsbDriver)
    {
        m_pUsbDriver->slotFocus(m_nProduct, IRIS_OPEN);
    }
}
void ClStreamView::irisClose()
{
    if(m_pUsbDriver)
    {
        m_pUsbDriver->slotFocus(m_nProduct, IRIS_CLOSE);
    }
}
void ClStreamView::stopCtrl()
{
    if(m_pUsbDriver)
    {
        m_pUsbDriver->slotFocus(m_nProduct, ZOOM_STOP);
        m_pUsbDriver->slotFocus(m_nProduct, IRIS_STOP);
    }
}
