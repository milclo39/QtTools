#include "ClCamScanner.h"
#include "ClLogger.h"
//#include "ClWifiDocCamScanner.h"

#include <QMap>
#include <QtNetwork/QtNetwork>

#define DEBUG

#ifndef DEBUG
#undef LOG
#define LOG(x)
#endif

ClCamScanner ClCamScanner::m_instance;

ClCamScanner::ClCamScanner(QObject *parent) : QObject(parent)
{
    m_scannerList = NULL;
    m_camBundle.clear();
    m_orderedList.clear();
    m_isRunning = false;
}

ClCamScanner::~ClCamScanner()
{
    if (!m_scannerList)
    {
        return;
    }

    for(qint32 scanType = 0; scanType < ClCamScannerFactory::SCANNER_TYPE_MAX;
            ++scanType)
    {
        AbsCamScanner* scanner = m_scannerList[scanType];
        if (NULL != scanner)
        {
            switch (scanType)
            {
                case ClCamScannerFactory::SCANNER_TYPE_USB_DOC_CAM:
                case ClCamScannerFactory::SCANNER_TYPE_WIFI_DOC_CAM:
                {
                    disconnect(scanner, SIGNAL(sigDeviceScanned(ClCamDevice*)),
                            this, SLOT(docCamScanned(ClCamDevice*)));
                    disconnect(scanner, SIGNAL(sigReqConnect(ClCamDevice*)),
                            this, SLOT(slotReqConnect(ClCamDevice*)));
                    disconnect(scanner, SIGNAL(sigWiFiQuality(QString,qint32)),
                            this, SIGNAL(sigWiFiQuality(QString,qint32)));
                    break;
                }
                case ClCamScannerFactory::SCANNER_TYPE_WEBCAM:
                    disconnect(scanner, SIGNAL(sigDeviceScanned(ClCamDevice*)),
                            this, SLOT(webcamScanned(ClCamDevice*)));
#if 0 //他社UVCカメラは挿抜で接続しないのでコメントアウト
                    disconnect(scanner, SIGNAL(sigReqConnect(ClCamDevice*)),
                            this, SLOT(slotReqConnect(ClCamDevice*)));
#endif
                break;

                default:
                    break;
            }

            delete scanner;
            scanner = NULL;
        }
    }

    delete[] m_scannerList;
    clear();
}

void ClCamScanner::initialize()
{
    if (NULL == m_scannerList)
    {
        ClCamScannerFactory scanFactory;

        m_scannerList = new AbsCamScanner*[ClCamScannerFactory::SCANNER_TYPE_MAX];

        for(qint32 scanType = 0; scanType < ClCamScannerFactory::SCANNER_TYPE_MAX;
                ++scanType)
        {
            AbsCamScanner* scanner = scanFactory.getInstance(
                        static_cast<ClCamScannerFactory::eSCANNER_TYPE>(scanType));
            m_scannerList[scanType] = scanner;

            if (NULL != scanner)
            {
                switch (scanType)
                {
                    case ClCamScannerFactory::SCANNER_TYPE_USB_DOC_CAM:
                    case ClCamScannerFactory::SCANNER_TYPE_WIFI_DOC_CAM:
                    {
                        connect(scanner, SIGNAL(sigDeviceScanned(ClCamDevice*)),
                                SLOT(docCamScanned(ClCamDevice*)));
                        connect(scanner, SIGNAL(sigReqConnect(ClCamDevice*)),
                                SLOT(slotReqConnect(ClCamDevice*)));
                        connect(scanner, SIGNAL(sigWiFiQuality(QString,qint32)),
                                SIGNAL(sigWiFiQuality(QString,qint32)));
                        break;
                    }
                    case ClCamScannerFactory::SCANNER_TYPE_WEBCAM:
                        connect(scanner, SIGNAL(sigDeviceScanned(ClCamDevice*)),
                                SLOT(webcamScanned(ClCamDevice*)));
#if 0 //他社UVCカメラは挿抜で接続しないのでコメントアウト
                        connect(scanner, SIGNAL(sigReqConnect(ClCamDevice*)),
                                SLOT(slotReqConnect(ClCamDevice*)));
#endif
                    break;

                    default:
                        break;
                }
            }
        }
    }
}

void ClCamScanner::webcamScanned(ClCamDevice* pclDev)
{
    stCameraInfo camInfo = pclDev->getInfo();
#if 0
    if( true == camInfo.id.contains(DEVICE_PREFIX_TX) ||    //TX-1/LX-1
        //true == camInfo.id.contains(DEVICE_PREFIX_MX) ||    //MX-1
        true == camInfo.id.contains(DEVICE_PREFIX_MO)       //MO-1/L-12i(UVC)/L-12iD(UVC)
      )
#else
#if defined(Q_OS_WIN)
    if( true == camInfo.id.contains("vid_09a1") )
#else
    if( true == camInfo.id.right(8).contains("09a1") )
#endif
#endif
    {
        camInfo.type = stCameraInfo::DOC_CAM;
    }
    else
    {
#if defined(MEASURE_AVGFRANMERATE)
        camInfo.type = stCameraInfo::DOC_CAM; //レンダラーでのライブをさせるために他社製品もELMO製品として扱う
#else
        camInfo.type = stCameraInfo::WEB_CAM;
#endif
    }
    pclDev->setInfo(camInfo);

    addCamera(pclDev);
}

void ClCamScanner::docCamScanned(ClCamDevice* pclDev)
{
    stCameraInfo camInfo = pclDev->getInfo();
    camInfo.type = stCameraInfo::DOC_CAM;
    pclDev->setInfo(camInfo);

    addCamera(pclDev);
}

void ClCamScanner::slotReqConnect(ClCamDevice* pclDev)
{
    QMutexLocker locker(&m_mutex);
    stCameraInfo camInfo = pclDev->getInfo();

    QMap<QString, ClCamDevice*>::Iterator camIt = m_camBundle.find(camInfo.id);

    if (camIt != m_camBundle.end())
    {
        ClCamDevice* pCamDev = camIt.value();
        ClCamDevice::stCameraInfo curInfo = pCamDev->getInfo();

        curInfo.scannedTime = camInfo.scannedTime;

        if ((QString::number(curInfo.address).compare(QString::number(camInfo.address)) == 0)
                && (curInfo.inUse == camInfo.inUse))
        {
            pCamDev->setInfo(curInfo);

            camInfo.tag = NULL;
            pclDev->setInfo(camInfo);

            delete pclDev;
            return;
        }

        if (!curInfo.inUse && camInfo.inUse)
        {
            LOG("Device " << curInfo.name << ":" << QHostAddress(curInfo.address).toString() << " is discoverable again.");
        }

        curInfo.address = camInfo.address;
        curInfo.tag = camInfo.tag;
        curInfo.inUse = camInfo.inUse;
        pCamDev->setInfo(curInfo);

        camInfo.tag = NULL;
        pclDev->setInfo(camInfo);

        delete pclDev;

        emit sigReqConnect(pCamDev);
    }
    else
    {
        m_camBundle.insert(camInfo.id, pclDev);
        if (camInfo.type == stCameraInfo::WEB_CAM)
        {
            m_orderedList.push_back(pclDev);
        }
        else
        {
            if (camInfo.conn_type == stCameraInfo::USB || camInfo.conn_type == stCameraInfo::ELUSB)
            {
                m_orderedList.push_front(pclDev);
                emit sigReqConnect(pclDev);
            }
            else
            {
                QList<ClCamDevice*>::iterator loopIt = m_orderedList.begin();
                bool foundWebcam = false;

                for(;loopIt != m_orderedList.end(); ++loopIt)
                {
                    if ((*loopIt)->getInfo().type == stCameraInfo::WEB_CAM)
                    {
                        m_orderedList.insert(loopIt, pclDev);
                        foundWebcam = true;
                        break;
                    }
                }

                if (!foundWebcam)
                {
                    m_orderedList.append(pclDev);
                }
            }
        }
    }
}

void ClCamScanner::addCamera(ClCamDevice* pclDev)
{
    QMutexLocker locker(&m_mutex);
    stCameraInfo camInfo = pclDev->getInfo();

    QMap<QString, ClCamDevice*>::Iterator camIt = m_camBundle.find(camInfo.id);

    if (camIt != m_camBundle.end())
    {
        ClCamDevice* pCamDev = camIt.value();
        ClCamDevice::stCameraInfo curInfo = pCamDev->getInfo();

        curInfo.scannedTime = camInfo.scannedTime;

        if ((QString::number(curInfo.address).compare(QString::number(camInfo.address)) == 0)
                && (curInfo.inUse == camInfo.inUse))
        {
            pCamDev->setInfo(curInfo);

            camInfo.tag = NULL;
            pclDev->setInfo(camInfo);

            delete pclDev;
            return;
        }

        if (!curInfo.inUse && camInfo.inUse)
        {
            LOG("Device " << curInfo.name << ":" << QHostAddress(curInfo.address).toString() << " is discoverable again.");
        }

        curInfo.address = camInfo.address;
        curInfo.tag = camInfo.tag;
        curInfo.inUse = camInfo.inUse;
        pCamDev->setInfo(curInfo);

        camInfo.tag = NULL;
        pclDev->setInfo(camInfo);

        delete pclDev;
    }
    else
    {
        m_camBundle.insert(camInfo.id, pclDev);
        if (camInfo.type == stCameraInfo::WEB_CAM)
        {
            m_orderedList.push_back(pclDev);
        }
        else
        {
            if (camInfo.conn_type == stCameraInfo::USB || camInfo.conn_type == stCameraInfo::ELUSB)
            {
                m_orderedList.push_front(pclDev);
            }
            else
            {
                QList<ClCamDevice*>::iterator loopIt = m_orderedList.begin();
                bool foundWebcam = false;

                for(;loopIt != m_orderedList.end(); ++loopIt)
                {
                    if ((*loopIt)->getInfo().type == stCameraInfo::WEB_CAM)
                    {
                        m_orderedList.insert(loopIt, pclDev);
                        foundWebcam = true;
                        break;
                    }
                }

                if (!foundWebcam)
                {
                    m_orderedList.append(pclDev);
                }
            }
        }
    }

    emit sigCamListUpdated();
}

void ClCamScanner::clear()
{
    for(QMap<QString, ClCamDevice*>::Iterator camIt = m_camBundle.begin(); camIt != m_camBundle.end(); ++camIt)
    {
        ClCamDevice* pCamDev = camIt.value();
        delete pCamDev;
    }

    m_camBundle.clear();
    m_orderedList.clear();
}

void ClCamScanner::doScan()
{
    initialize();

    for(qint32 scanType = 0; scanType < ClCamScannerFactory::SCANNER_TYPE_MAX;
            ++scanType)
    {
        AbsCamScanner* scanner = m_scannerList[scanType];
        if ((NULL != scanner) && (!scanner->isScanning()))
        {
            scanner->start();
        }
    }
}

void ClCamScanner::startScan()
{
    LOG("Request to start scanning");
    doScan();
    m_isRunning = true;
}

void ClCamScanner::stopScan()
{
    LOG("Request stop scanning");
    m_isRunning = false;
    for(qint32 scanType = 0; scanType < ClCamScannerFactory::SCANNER_TYPE_MAX;
            ++scanType)
    {
        AbsCamScanner* scanner = m_scannerList[scanType];
        if (NULL != scanner)
        {
            scanner->stop();
        }
    }
}

ClCamScanner* ClCamScanner::getInstance()
{
    return &m_instance;
}

bool ClCamScanner::isScanning(ClCamScannerFactory::eSCANNER_TYPE scanType)
{
    if (ClCamScannerFactory::SCANNER_TYPE_MAX == scanType)
    {
        return m_isRunning;
    }

    AbsCamScanner* scanner = m_scannerList[scanType];
    if (NULL != scanner)
    {
        return scanner->isScanning();
    }

    return false;
}

void ClCamScanner::addDev(ClCamDevice* pclDev)
{
    QMutexLocker locker(&m_mutex);

    m_camBundle.insert(pclDev->getInfo().id, pclDev);
}

bool ClCamScanner::exist(ClCamDevice *pCamDev)
{
	for(qint32 iScan = 0; iScan < ClCamScannerFactory::SCANNER_TYPE_MAX; iScan++){
		AbsCamScanner* scanner = m_scannerList[iScan];
		if(NULL != scanner && scanner->exist(pCamDev)){
			return true;
		}
	}
	return false;
}
#if defined(Q_OS_MAC)
void ClCamScanner::removeCam(stCameraInfo info)
{
    for(int i = 0; i < m_orderedList.count(); i++)
    {
        stCameraInfo curInfo = m_orderedList.at(i)->getInfo();
        if(curInfo.id == info.id && curInfo.scannedTime == info.scannedTime)
        {
            ClCamDevice *pCamDev = m_orderedList.at(i);
            info.tag = NULL;
            pCamDev->setInfo(info);
            m_orderedList.removeAt(i);
        }
    }
    if(m_camBundle.keys().contains(info.id))
    {
        stCameraInfo curInfo = m_camBundle.value(info.id)->getInfo();
        if(curInfo.id == info.id && curInfo.scannedTime == info.scannedTime)
        {
            ClCamDevice *pCamDev = m_camBundle.value(info.id);
            info.tag = NULL;
            pCamDev->setInfo(info);
            m_camBundle.remove(info.id);
        }
    }
    emit sigCamListUpdated();
}
#endif
