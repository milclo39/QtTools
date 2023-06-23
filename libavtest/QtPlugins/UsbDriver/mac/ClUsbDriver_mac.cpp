#include "mac/ClImDrvCtrl.h"
#include "ClUsbDriver/Command_Presenter.h"
#include "ClUsbDriver/ClUsbDriver_mac.h"
#include "ClUsbDriver/ClUsbDriver_global.h"

#if 1 //elmo add
static libusb_context *g_ctx = NULL;
#endif

//--------------------------------------------------------------------------------//
/*!
    @brief		コンストラクタ
*/
//--------------------------------------------------------------------------------//
ClUsbDriver::ClUsbDriver(QWidget *pclParent) : QWidget(pclParent)
{
    m_bIsInitialized = false;
    m_bZeroImage = false;

    //プロダクトIDリスト初期化
    m_clProductList.append(PRODUCT_P10S);
    m_clProductList.append(PRODUCT_L1N);
    m_clProductList.append(PRODUCT_FF1);
    m_clProductList.append(PRODUCT_P30S);
    m_clProductList.append(PRODUCT_P100);
    m_clProductList.append(PRODUCT_L1EX);
    m_clProductList.append(PRODUCT_P10);
    m_clProductList.append(PRODUCT_MC1EX);
    m_clProductList.append(PRODUCT_SESAME);
#ifdef _USE_CO10
    m_clProductList.append(PRODUCT_CO10);
#endif
#ifdef _USE_MO1
    m_clProductList.append(PRODUCT_MOBILE);
#endif

    qRegisterMetaType<eKINDSTREAM>("eKINDSTREAM");

    m_qStrSendFilePath.clear();
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		デストラクタ
*/
//--------------------------------------------------------------------------------//
ClUsbDriver::~ClUsbDriver()
{
    if(isInitialize() == true)
    {
        closeDevice();
    }
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		初期化
*/
//--------------------------------------------------------------------------------//
qint32 ClUsbDriver::initialize()
{
    if(m_bIsInitialized == false)
    {
        m_bIsInitialized = true;
#if 0 //elmo del
        for(int i = 0; i < m_clProductList.count(); i++)
        {
            qint32 lProductId = m_clProductList.at(i);
            openDevice(lProductId);
        }
#else //elmo add
        g_ctx = NULL;
        if(libusb_init(&g_ctx) < 0)
        {
            printf("ERROR: libusb_init");
            return IM_ERROR_FATAL;
        }
        libusb_set_debug(g_ctx, 3);
#endif
    }

    return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------//

#if 1 //elmo add
//--------------------------------------------------------------------------------//
/*!
    @brief		デバイズ確認
*/
//--------------------------------------------------------------------------------//
qint32 ClUsbDriver::checkDeviceAll()
{
    if(isInitialize() == false)
    {
        return IM_ERROR_NOT_INIT;
    }
    libusb_device **devices;
    int count = libusb_get_device_list(NULL, &devices);
    if(count < 0)
    {
        return IM_ERROR_FATAL;
    }
    for(int i = 0; i < count; i++)
    {
        libusb_device_descriptor desc;
        libusb_device *device = devices[i];
        if(libusb_get_device_descriptor(device, &desc) < 0)
        {
            break;
        }
        int id = desc.idVendor << 16 | desc.idProduct;
        if(m_clProductList.contains(id))
        {
            return id;
        }
    }
    libusb_free_device_list(devices, 1);

    return IM_ERROR_NO_DEVICE;
}
//--------------------------------------------------------------------------------//
#endif

//--------------------------------------------------------------------------------//
/*!
    @brief		オープン
    @attention	内部用
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::open(qint32 lProductId)
{
    emit sigOpenDevice(lProductId);		//デバイスオープン通知
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		クローズ
    @attention	内部用
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::close(qint32 lProductId)
{
    closeDevice(lProductId);
    emit sigCloseDevice(lProductId);
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		再起動確認
    @attention	内部用
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::checkRestart()
{
#ifndef _USE_CO10
	emit sigCheckRestartCo10();
#endif
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		初期化確認
*/
//--------------------------------------------------------------------------------//
bool ClUsbDriver::isInitialize()
{
    return m_bIsInitialized;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		デバイスオープン確認
*/
//--------------------------------------------------------------------------------//
bool ClUsbDriver::isOpened(qint32 lProductId)
{
    if(isInitialize() == false)
    {
        return false;
    }

    ClImDrvCtrl *pclImCtrl = NULL;
    if(lProductId == 0)
    {
        QList<qint32> clKeys = m_ImDrvCtrlList.keys();
        for(int i = 0; i < clKeys.count(); i++)
        {
            pclImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList.value(clKeys.at(i)));
            if(pclImCtrl != NULL)
            {
                if(pclImCtrl->IsOpenDriver() == true)
                {
                    return true;
                }
            }
        }
    }
    else
    {
        pclImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList.value(lProductId));
        if(pclImCtrl != NULL)
        {
            return pclImCtrl->IsOpenDriver();
        }
    }
    return false;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		ストリーミング中か
*/
//--------------------------------------------------------------------------------//
bool ClUsbDriver::isStreaming(qint32 lProductId, eKINDSTREAM iKind)
{
    qint32 lRes = chkInitOpen(lProductId);
    if(lRes != IM_ERROR_SUCCESS)
    {
        return false;
    }

    ClImDrvCtrl *pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList.value(lProductId));
    if(pImCtrl != NULL)
    {
        return pImCtrl->isStreaming(iKind);
    }
    return false;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		デバイスオープン
*/
//--------------------------------------------------------------------------------//
qint32 ClUsbDriver::openDevice(qint32 lProductId)
{
    //処理が必要か確認
    if(isInitialize() == false)
    {
        return IM_ERROR_NOT_INIT;
    }
    if(isOpened(lProductId) == true)
    {
        return IM_ERROR_ALREADY_OPENED;
    }

    //ドライバコントローラ初期化
    qint32 lRes = IM_ERROR_SUCCESS;
    ClImDrvCtrl *pclImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList.value(lProductId, NULL));
    if(pclImCtrl == NULL)
    {
        pclImCtrl = new ClImDrvCtrl(lProductId, parent());
        if(pclImCtrl == NULL)
        {
            return IM_ERROR_NOT_OPEN;
        }
        else if(pclImCtrl->initialize() != IM_ERROR_SUCCESS)
        {
            delete pclImCtrl;
            return IM_ERROR_NOT_INIT;
        }
        pclImCtrl->setParent(this);

        //ドライバオープン
        if(pclImCtrl->OpenDriver() == false)
        {
            lRes = IM_ERROR_NOT_OPEN;
        }
        m_ImDrvCtrlList.insert(lProductId, pclImCtrl);	//リストに追加
    }

    return lRes;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		デバイスクローズ
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::closeDevice(qint32 lProductId)
{
    ClImDrvCtrl *pImCtrl = NULL;

    if(isInitialize() == false)
    {
        return;
    }
    else if(isOpened(lProductId) == false)
    {
        return;
    }

    if(lProductId == 0)
    {
        QList<qint32> clKeyList = m_ImDrvCtrlList.keys();
        for(int i = 0; i < clKeyList.count(); i++)
        {
            pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList.value(clKeyList.at(i), NULL));
            if(pImCtrl != NULL)
            {
                //ストリーム停止
                stopStream(pImCtrl->getProductId(), JPEG_STREAM);
                stopStream(pImCtrl->getProductId(), RAW_STREAM);

                pImCtrl->CloseDriver();
                m_ImDrvCtrlList.remove(clKeyList.at(i));
                delete pImCtrl;
            }
        }
    }
    else
    {
        //ストリーム停止
        stopStream(lProductId, JPEG_STREAM);
        stopStream(lProductId, RAW_STREAM);

        pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList.value(lProductId));
        if(pImCtrl != NULL)
        {
            pImCtrl->CloseDriver();
        }
    }
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		ストリーム開始
*/
//--------------------------------------------------------------------------------//
qint32 ClUsbDriver::startStream(qint32 lProductId, eKINDSTREAM iKind, qint32 iFrate, qint32 lQuality)
{
    qint32 lRes = chkInitOpen(lProductId);
    if(lRes != IM_ERROR_SUCCESS)
    {
        return lRes;
    }

    ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux

    if(iFrate <= 0 || iFrate > 30){							//fool proof
        iFrate = 30;
    }

    if(lQuality <= 0 || lQuality > 100)
    {
        lQuality = JPEG_QUARITY_DEF;
    }

    pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
    if(pImCtrl == NULL){									//if pImCtrl is NULL
        return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
    }
    return pImCtrl->startStream(this,iKind,iFrate,lQuality);
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		ストリーム停止
*/
//--------------------------------------------------------------------------------//
qint32 ClUsbDriver::stopStream(qint32 lProductId, eKINDSTREAM iKind)
{
    qint32 lRes = chkInitOpen(lProductId);
    if(lRes != IM_ERROR_SUCCESS)
    {
        return lRes;
    }

    ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux

    pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
    if(pImCtrl == NULL){									//if pImCtrl is NULL
        return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
    }
    return pImCtrl->stopStream(iKind);
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		JPEGキャプチャ
*/
//--------------------------------------------------------------------------------//
qint32 ClUsbDriver::captureJpeg(qint32 lProductId, QByteArray &clData, eKINDSTREAM eKind,
                                QVariant clQuality, eKINDRECTID eRectId)
{
    qint32 lRes = chkInitOpen(lProductId);
    if(lRes != IM_ERROR_SUCCESS)
    {
        return lRes;
    }

    ClImDrvCtrl *pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList.value(lProductId));
    if(pImCtrl == NULL)
    {
        return IM_ERROR_NOT_OPEN;
    }

    QStringList clParamList;	//送信パラメータリスト
    //画像サイズ設定
    qint32 lRectId = RECTID_PICTURE_DEFAULT;
    if(eRectId == RECTID_PICTURE_DEFAULT)
    {
        QVariant clParam;
        lRes = pImCtrl->CommandPresenter(GET_RECT_PICTURE, clParam);
        if(lRes == IM_ERROR_SUCCESS)
        {
            if(clParam.type() != QVariant::UserType)
            {
                lRectId = RECTID_PICTURE_DEFAULT;
            }
        }
        else
        {
            lRectId = RECTID_PICTURE_DEFAULT;
        }
    }
    else
    {
        lRectId = eRectId;
    }
    clParamList.append(QString::number(lRectId));	//RectIdをパラメータリストに追加
    //画質設定
    qint32 lQuality = JPEG_QUARITY_DEF;
    if(clQuality.toString().isEmpty() == true)
    {
        if(clQuality.toInt() > 0)
        {
            lQuality = clQuality.toInt();
        }
    }
    else
    {
        if(clQuality.toString().toInt() > 0)
        {
            lQuality = clQuality.toString().toInt();
        }
    }
    clParamList.append(QString::number(lQuality));

    //コマンド設定
    qint32 lCommand = 0;
    if(eKind == JPEG_STREAM)
    {
        lCommand = GET_PICTURE;
    }

    //ストリームの最新画像取得
    QVariant clParam(clParamList);
    if(lCommand == GET_PICTURE)
    {
        lRes = pImCtrl->getStreamingImage(clParam);
    }
    if(lRes == IM_ERROR_SUCCESS)
    {
        clData = QByteArray(clParam.toByteArray().constData(), clParam.toByteArray().size());		//データコピー
        return lRes;
    }

    if(lCommand != 0)
   {
        lRes = pImCtrl->CommandPresenter(lCommand, clParam);	//コマンド送信

        //データコピー
        if(lRes == IM_ERROR_SUCCESS)
        {
            if(clParam.type() == QVariant::ByteArray)
            {
                clData = QByteArray(clParam.toByteArray().constData(), clParam.toByteArray().size());
            }
        }
    }
    else
    {
        lRes = IM_ERROR_INVALID_PARAMETER;	//不正値
    }

    return lRes;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		Rawイメージ取得
*/
//--------------------------------------------------------------------------------//
qint32 ClUsbDriver::captureRaw(qint32 lProductId, QImage &clData, eKINDSTREAM eKind,
                               QVariant clQuality, eKINDRECTID eRectId)
{
    qint32 lRes = IM_ERROR_SUCCESS;

    QByteArray clJpegData;
    eKINDSTREAM eJpgStream = JPEG_STREAM;
    lRes = captureJpeg(lProductId, clJpegData, eJpgStream, clQuality, eRectId);
    if(lRes == IM_ERROR_SUCCESS)
    {
        if(clData.loadFromData(clJpegData, "jpg") == false)
        {
            lRes = IM_ERROR_DECORD;
        }
    }
    return lRes;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		カメラ操作コマンド実行
*/
//--------------------------------------------------------------------------------//
qint32 ClUsbDriver::execCameraControl(qint32 lProductId, qint32 command)
{
    qint32 lRes = chkInitOpen(lProductId);
    if(lRes != IM_ERROR_SUCCESS)
    {
        return false;
    }

    ClImDrvCtrl *pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList.value(lProductId));
    if(pImCtrl == NULL)
    {
        return IM_ERROR_NOT_OPEN;
    }
    QVariant param(command);
    return pImCtrl->CommandPresenter(command, param);
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		初期化、オープン確認
*/
//--------------------------------------------------------------------------------//
qint32 ClUsbDriver::chkInitOpen(qint32 lProductId)
{
    if(isInitialize() == false)
    {
        return IM_ERROR_NOT_INIT;
    }
    if(isOpened(lProductId) == false)
    {
        if(openDevice(lProductId) != IM_ERROR_SUCCESS)
        {
            return IM_ERROR_NOT_OPEN;
        }
    }
    return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		ストリーム開始
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotStartStream(qint32 lProductId, eKINDSTREAM kind, qint32 lFrate, qint32 lQuality)
{
    qint32 lRes = startStream(lProductId, kind, lFrate, lQuality);
    if(lRes != IM_ERROR_SUCCESS)
    {
        emit sigErrorNormal(lRes);
    }
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		ストリーム停止
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotStopStream(qint32 lProductId, eKINDSTREAM kind)
{
    if(isStreaming(lProductId, kind) == true)
    {
        qint32 lRes = stopStream(lProductId, kind);
        if(lRes != IM_ERROR_SUCCESS)
        {
            emit sigErrorNormal(lRes);
        }
    }
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		フレームレート取得
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotGetFRate(qint32 lProductId, eKINDSTREAM kind, qint32 &lFrate)
{
    if(isStreaming(lProductId, kind) == true)
    {
        ClImDrvCtrl *pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList.value(lProductId));
        if(pImCtrl == NULL)
        {
            emit sigErrorNormal(IM_ERROR_NOT_OPEN);
            return;
        }
        lFrate = pImCtrl->getFrameRate(kind);
    }
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		フレームレート設定
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotSetFRate(qint32 lProductId, eKINDSTREAM kind, qint32 lFrate)
{
    if(isStreaming(lProductId, kind) == true)
    {
        ClImDrvCtrl *pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList.value(lProductId));
        if(pImCtrl == NULL)
        {
            emit sigErrorNormal(IM_ERROR_NOT_OPEN);
            return;
        }
        pImCtrl->setFrameRate(lFrate, kind);
    }
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		画質取得
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotGetQuality(qint32 lProductId, eKINDSTREAM kind, qint32 &lQuality)
{
    ClImDrvCtrl *pImCtrl = NULL;
    if(isStreaming(lProductId, kind) == true)
    {
        pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);
        if(pImCtrl == NULL)
        {
            emit sigErrorNormal(IM_ERROR_NOT_OPEN);
            return;
        }
        lQuality = pImCtrl->getQuality(kind);
    }
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		画質設定
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotSetQuality(qint32 lProductId, eKINDSTREAM kind, qint32 lQuality)
{
    ClImDrvCtrl *pImCtrl = NULL;
    if(isStreaming(lProductId, kind) == true)
    {
        pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);
        if(pImCtrl == NULL)
        {
            emit sigErrorNormal(IM_ERROR_NOT_OPEN);
            return;
        }
        pImCtrl->setQuality(lQuality, kind);
    }
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		キャプチャ
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotCapture(qint32 lProductId, eKINDSTREAM kind, qint32 &lStatus,
                              QVariant &qParam, eKINDRECTID lRectId)
{
    qint32 lRes = chkInitOpen(lProductId);
    if(lRes != IM_ERROR_SUCCESS)
    {
        return;
    }

    switch(kind)
    {
	case JPEG_STREAM:
        {
            QByteArray clData;
            lRes = captureJpeg(lProductId, clData, kind, qParam, lRectId);
            if(lRes == IM_ERROR_SUCCESS)
            {
                qParam = clData;
            }
            else if(lRes == IM_ERROR_CANNOT_OUTPUT_IMAGE)
            {
                emit sigZeroCapture(lProductId);
            }
            else
            {
                emit sigErrorNormal(lRes);
            }
            break;
        }
	case RAW_STREAM:
        {
            QImage clImage;
            lRes = captureRaw(lProductId, clImage, kind, qParam, lRectId);
            if(lRes == IM_ERROR_SUCCESS)
            {
                qParam = clImage;
            }
            else if(lRes == IM_ERROR_CANNOT_OUTPUT_IMAGE)
            {
                emit sigZeroCapture(lProductId);
            }
            else
            {
                emit sigErrorNormal(lRes);
            }
            break;
        }
    default:
        {
            emit sigErrorFatal(IM_ERROR_FATAL);
            break;
        }
    }
    lStatus = lRes;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		カメラ名取得
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotGetPresenterName(qint32 lProductId, QString &qName)
{
    qint32 lRet = chkInitOpen(lProductId);
    if(lRet != IM_ERROR_SUCCESS)
    {
        if(lRet == IM_ERROR_NOT_OPEN)
        {
            emit sigErrorNormal(lRet);
        }
        return;
    }
    ClImDrvCtrl *pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList.value(lProductId));
    if(pImCtrl == NULL)
    {
        return;
    }

    QVariant clParam;
    lRet = pImCtrl->CommandPresenter(GET_MODEL_NAME, clParam);
    if(lRet != IM_ERROR_SUCCESS)
    {
        emit sigErrorNormal(lRet);
    }
    else if(clParam.type() == QVariant::String)
    {
        qName = clParam.toString();
    }
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		カメラバージョン取得
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotGetPresenterVersion(qint32 lProductId, QStringList &qVerlist)
{
    qint32 lRet = chkInitOpen(lProductId);
    if(lRet != IM_ERROR_SUCCESS)
    {
        if(lRet == IM_ERROR_NOT_OPEN)
        {
            emit sigErrorNormal(lRet);
        }
        return;
    }
    ClImDrvCtrl *pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList.value(lProductId));
    if(pImCtrl == NULL)
    {
        return;
    }

    QVariant clParam;
    lRet = pImCtrl->CommandPresenter(GET_MODEL_VERSION, clParam);
    if(lRet != IM_ERROR_SUCCESS)
    {
        emit sigErrorNormal(lRet);
    }
    else if(clParam.type() == QVariant::StringList)
    {
        qVerlist = clParam.toStringList();
    }
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		接続カメラリスト取得
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotGetPresenterList(QVariantList &qSendlist)
{
    QList<qint32> clKeys = m_ImDrvCtrlList.keys();
    for(int i = 0; i < clKeys.count(); i++)
    {
        ClImDrvCtrl *pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList.value(clKeys.at(i)));
        if(pImCtrl != NULL &&
           pImCtrl->IsOpenDriver() == true)
        {
            qSendlist.append(pImCtrl->getProductId());
        }
    }
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		画像サイズ取得
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotGetPictureRect(qint32 lProductId, QSize &qSize)
{
    qint32 lRet = chkInitOpen(lProductId);
    if(lRet != IM_ERROR_SUCCESS)
    {
        if(lRet == IM_ERROR_NOT_OPEN)
        {
            emit sigErrorNormal(lRet);
        }
        return;
    }
    ClImDrvCtrl *pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList.value(lProductId));
    if(pImCtrl == NULL)
    {
        return;
    }

    qint32 lWidth = 0, lHeight = 0;

    QVariant clParam;
    //幅取得
    lRet = pImCtrl->CommandPresenter(GET_WIDTH_PICTURE, clParam);
    if(lRet != IM_ERROR_SUCCESS ||
       clParam.type() != QVariant::Int)
    {
        emit sigErrorNormal(IM_ERROR_OTHER);
    }
    else
    {
        lWidth = clParam.toInt();
    }
    //高さ取得
    lRet = pImCtrl->CommandPresenter(GET_HEIGHT_PICTURE, clParam);
    if(lRet != IM_ERROR_SUCCESS ||
       clParam.type() != QVariant::Int)
    {
        emit sigErrorNormal(IM_ERROR_OTHER);
    }
    else
    {
        lHeight = clParam.toInt();
    }

    //戻り値に設定
    qSize.setWidth(lWidth);
    qSize.setHeight(lHeight);
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		画像バイト数取得
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotGetPictureSize(qint32 lProductId, qint32 &lPictSize)
{
    qint32 lRet = chkInitOpen(lProductId);
    if(lRet != IM_ERROR_SUCCESS)
    {
        if(lRet == IM_ERROR_NOT_OPEN)
        {
            emit sigErrorNormal(lRet);
        }
        return;
    }
    ClImDrvCtrl *pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList.value(lProductId));
    if(pImCtrl == NULL)
    {
        return;
    }

    QVariant clParam;
    lRet = pImCtrl->CommandPresenter(GET_SIZE_PICTURE, clParam);
    if(lRet != IM_ERROR_SUCCESS ||
       clParam.type() != QVariant::Int)
    {
        emit sigErrorNormal(IM_ERROR_OTHER);
    }
    else
    {
        lPictSize = clParam.toInt();
    }
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		画像サイズID取得
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotGetRectId(qint32 lProductId, eKINDRECTID &lRectId)
{
    qint32 lRet = chkInitOpen(lProductId);
    if(lRet != IM_ERROR_SUCCESS)
    {
        if(lRet == IM_ERROR_NOT_OPEN)
        {
            emit sigErrorNormal(lRet);
        }
        return;
    }
    ClImDrvCtrl *pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList.value(lProductId));
    if(pImCtrl == NULL)
    {
        return;
    }

    QVariant clParam;
    lRet = pImCtrl->CommandPresenter(GET_RECT_PICTURE, clParam);
    if(lRet != IM_ERROR_SUCCESS ||
       clParam.type() != QVariant::UserType)
    {
        emit sigErrorNormal(IM_ERROR_OTHER);
    }
    else
    {
        lRectId = clParam.value<eKINDRECTID>();
    }
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		画像サイズID設定
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotSetRectId(qint32 lProductId, eKINDRECTID lRectId)
{
    qint32 lRet = chkInitOpen(lProductId);
    if(lRet != IM_ERROR_SUCCESS)
    {
        if(lRet == IM_ERROR_NOT_OPEN)
        {
            emit sigErrorNormal(lRet);
        }
        return;
    }
    ClImDrvCtrl *pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList.value(lProductId));
    if(pImCtrl == NULL)
    {
        return;
    }

    QVariant clParam;
    clParam.setValue(lRectId);
    lRet = pImCtrl->CommandPresenter(SET_RECT_PICTURE, clParam);
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		ズーム
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotZoom(qint32 lProductId, qint32 command)
{
    qint32 lRet = execCameraControl(lProductId, command);
    if(lRet != IM_ERROR_SUCCESS)
    {
        emit sigErrorNormal(lRet);
    }
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		フォーカス
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotFocus(qint32 lProductId, qint32 command)
{
    qint32 lRet = execCameraControl(lProductId, command);
    if(lRet != IM_ERROR_SUCCESS)
    {
        emit sigErrorNormal(lRet);
    }
}
//--------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------//
/*!
    @brief		アイリス
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotIris(qint32 lProductId, qint32 command)
{
    qint32 lRet = execCameraControl(lProductId, command);
    if(lRet != IM_ERROR_SUCCESS)
    {
        emit sigErrorNormal(lRet);
    }
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		回転
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotRotate(qint32 lProductId, qint32 command)
{
    qint32 lRet = execCameraControl(lProductId, command);
    if(lRet != IM_ERROR_SUCCESS)
    {
        emit sigErrorNormal(lRet);
    }
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		ネガポジ
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotPosiNega(qint32 lProductId, qint32 command)
{
    qint32 lRet = execCameraControl(lProductId, command);
    if(lRet != IM_ERROR_SUCCESS)
    {
        emit sigErrorNormal(lRet);
    }
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		カラー/モノクロ
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotColorBw(qint32 lProductId, qint32 command)
{
    qint32 lRet = execCameraControl(lProductId, command);
    if(lRet != IM_ERROR_SUCCESS)
    {
        emit sigErrorNormal(lRet);
    }
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		スローシャッター
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotSlowShutter(qint32 lProductId, qint32 command)
{
    qint32 lRet = execCameraControl(lProductId, command);
    if(lRet != IM_ERROR_SUCCESS)
    {
        emit sigErrorNormal(lRet);
    }
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		表示モード
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotImageMode(qint32 lProductId, qint32 command)
{
    qint32 lRet = execCameraControl(lProductId, command);
    if(lRet != IM_ERROR_SUCCESS)
    {
        emit sigErrorNormal(lRet);
    }
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		顕微鏡モード
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotMicroscopeMode(qint32 lProductId, qint32 command)
{
    qint32 lRet = execCameraControl(lProductId, command);
    if(lRet != IM_ERROR_SUCCESS)
    {
        emit sigErrorNormal(lRet);
    }
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		画像取得不可
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotStartZeroImage(qint32 lProductId, eKINDSTREAM eKind)
{
    if(m_bZeroImage == true)
    {
        return;
    }
    m_bZeroImage = true;
    emit sigStartZeroImage(lProductId, eKind);
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		画像取得可能
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotEndZeroImage(qint32 lProductId, eKINDSTREAM eKind)
{
    if(m_bZeroImage == false)
    {
        return;
    }
    m_bZeroImage = false;
    emit sigEndZeroImage(lProductId, eKind);
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		ズーム値設定
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotSetVariantZoomV2(qint32 lProductId, qint32 lParam)
{
    setValueCommand(lProductId, SET_VARIANT_ZOOM_V2, lParam);
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		ズーム値取得
*/
//--------------------------------------------------------------------------------//
void ClUsbDriver::slotGetZoomStatusV2(qint32 lProductId, qint32 &lParam)
{
    QVariant clParam;
    if(getValueCommand(lProductId, GET_ZOOM_STATUS_V2, clParam) == IM_ERROR_SUCCESS)
    {
        lParam = clParam.toInt();
    }
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		設定書き込み
*/
//--------------------------------------------------------------------------------//
qint32 ClUsbDriver::setValueCommand(qint32 lProductId, qint32 lCommand, QVariant clParam)
{
    qint32 lRes = chkInitOpen(lProductId);
    if(lRes != IM_ERROR_SUCCESS)
    {
        return IM_ERROR_NOT_OPEN;
    }

    ClImDrvCtrl *pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);
    if(pImCtrl == NULL)
    {
        return IM_ERROR_NOT_OPEN;
    }
    lRes = pImCtrl->CommandPresenter(lCommand, clParam);
    if(lRes != IM_ERROR_SUCCESS)
    {
        emit sigErrorNormal(lRes);
    }
    return lRes;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
    @brief		設定値取得
*/
//--------------------------------------------------------------------------------//
qint32 ClUsbDriver::getValueCommand(qint32 lProductId, qint32 lCommand, QVariant &clParam)
{
    qint32 lRes = chkInitOpen(lProductId);
    if(lRes != IM_ERROR_SUCCESS)
    {
        return IM_ERROR_NOT_OPEN;
    }

    ClImDrvCtrl *pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);
    if(pImCtrl == NULL)
    {
        return IM_ERROR_NOT_OPEN;
    }
    lRes = pImCtrl->CommandPresenter(lCommand, clParam);
    if(lRes != IM_ERROR_SUCCESS)
    {
        emit sigErrorNormal(lRes);
    }
    return lRes;
}
//--------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------//
/*!
    @brief		画像ファイル情報送信
*/
//--------------------------------------------------------------------------------//
qint32 ClUsbDriver::slotSendImgFileInfoV2(qint32 lProductId, qint32 command, QVariant &clParam)
{
    Q_UNUSED(lProductId);
    Q_UNUSED(command);
    Q_UNUSED(clParam)
    return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------//
/*!
    @brief		ファームファイル情報送信
*/
//--------------------------------------------------------------------------------//
qint32 ClUsbDriver::slotSendFwFileInfoV2(qint32 lProductId, qint32 command, QVariant &clParam)
{
    qint32 lRes = IM_ERROR_SUCCESS;
    ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
    if(isInitialize() == false)								// is not initialized
    {
        return IM_ERROR_NOT_INIT;
    }
    if(isOpened(lProductId) == false)
    {
        if(openDevice(lProductId) != IM_ERROR_SUCCESS){
            return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
        }
    }
    pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
    if(pImCtrl == NULL){									//if pImCtrl is NULL
        return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
    }
    lRes = pImCtrl->CommandPresenter(SEND_UPFW_INFO_V2,clParam);

    //送信ファイルのパスを記憶
    if(lRes == IM_ERROR_SUCCESS && clParam.type() == QVariant::StringList){
        m_qStrSendFilePath = clParam.toStringList().at(0);
        //qDebug() << "copy file path" << m_qStrSendFilePath;
    }

    return lRes;
}

//--------------------------------------------------------------------------------//
/*!
    @brief		ファイルデータ送信
*/
//--------------------------------------------------------------------------------//
qint32 ClUsbDriver::slotUploadFileV2(qint32 lProductId, qint32 command, QVariant &clParam)
{
    qint32 lRes = IM_ERROR_SUCCESS;
    ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
    if(isInitialize() == false)								// is not initialized
    {
        return IM_ERROR_NOT_INIT;
    }
    if(isOpened(lProductId) == false)
    {
        if(openDevice(lProductId) != IM_ERROR_SUCCESS){
            return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
        }
    }
    pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
    if(pImCtrl == NULL){									//if pImCtrl is NULL
        return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
    }

    if(m_qStrSendFilePath.isNull()){						//ファイル情報送信済みか確認
        return IM_ERROR_COMMAND_FAILURE;
    }
    clParam = m_qStrSendFilePath;
    lRes = pImCtrl->startSendFile(this, clParam);
    m_qStrSendFilePath.clear();								//一度送ったら消去

    return lRes;
}

//================================================================================
//										UVC
//================================================================================
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetRemoconWidget(qint32 lProductId, QWidget** pclDestRemoconWidget)
	@brief	リモコン取得
	@param	lProductId				:	プロダクトID
			pclDestRemoconWidget	:	格納先
 */
//--------------------------------------------------------------------------------
qint32	ClUsbDriver::slotGetRemoconWidget(qint32 lProductId, QWidget** pclDestRemoconWidget)
{

	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false)								// is not initialized
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId) != IM_ERROR_SUCCESS)
		{
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}

	*pclDestRemoconWidget = pImCtrl->getRemoconWidget();

	return IM_ERROR_SUCCESS;
}

//--------------------------------------------------------------------------------
/*!
	@fn		slotGetSupportedResolutions(QStringList* clDestResolutions)
	@brief	対応解像度取得
	@param	clDestResolutions		:	格納先
 */
//--------------------------------------------------------------------------------
qint32	ClUsbDriver::slotGetSupportedResolutions(qint32 lProductId, QStringList& clDestResolutions)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false)								// is not initialized
	{
		return IM_ERROR_NOT_INIT;
	}
    if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId) != IM_ERROR_SUCCESS)
		{
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}

	clDestResolutions = pImCtrl->getSupportedResolutions();

	return IM_ERROR_SUCCESS;
}

//--------------------------------------------------------------------------------
/*!
	@fn		slotGetCurrentResolution(qint32 lProductId, QString& clResolution)
	@brief	現在の解像度を取得
	@param	lProductId				:	プロダクトID
	@param	clResolution			:	解像度
 */
//--------------------------------------------------------------------------------
qint32	ClUsbDriver::slotGetCurrentResolution(qint32 lProductId, QString& clResolution)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false)								// is not initialized
	{
		return IM_ERROR_NOT_INIT;
	}
    if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId) != IM_ERROR_SUCCESS)
		{
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}

	clResolution = pImCtrl->getCurrentResolution();

	return IM_ERROR_SUCCESS;
}

//--------------------------------------------------------------------------------
/*!
	@fn		slotSetCurrentResolution(qint32 lProductId, QString clResolution)
	@brief	現在の解像度を設定
	@param	lProductId				:	プロダクトID
	@param	clResolution			:	解像度
 */
//--------------------------------------------------------------------------------
qint32	ClUsbDriver::slotSetCurrentResolution(qint32 lProductId, QString clResolution)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false)								// is not initialized
	{
		return IM_ERROR_NOT_INIT;
	}
    if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId) != IM_ERROR_SUCCESS)
		{
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}

	clResolution = pImCtrl->setCurrentResolution(clResolution);

	return IM_ERROR_SUCCESS;
}
