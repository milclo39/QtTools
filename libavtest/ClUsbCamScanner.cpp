#include "ClUsbCamScanner.h"
#include "ClCamDevice.h"
#include "ClLogger.h"

#include <QCamera>
//#include <QCameraInfo>
#include <QTimer>
#include <QDateTime>

#define DEBUG

#ifndef DEBUG
#undef LOG
#define LOG(x)
#endif

ClUsbCamScanner::ClUsbCamScanner(ClCamFilter* pclCamFilter):
	AbsCamScanner(NULL),
	m_pelPresenter(NULL),
	m_scanTimer(NULL)
{
    m_curInfoList.clear();
    m_isScanning = false;
    m_pclCamFilter = pclCamFilter;
    m_scanTimer = new QTimer();
    m_scanTimer->setInterval(3000);
    connect(m_scanTimer, SIGNAL(timeout()), SLOT(scan()));
}

ClUsbCamScanner::~ClUsbCamScanner()
{
    if (NULL != m_scanTimer)
    {
        disconnect(m_scanTimer, SIGNAL(timeout()), this, SLOT(scan()));
        delete m_scanTimer;
        m_scanTimer = NULL;
    }
    if (NULL != m_pclCamFilter)
    {
        delete m_pclCamFilter;
        m_pclCamFilter = NULL;
    }
}

void ClUsbCamScanner::scan()
{
	bool bReqConnect = false;
	QList<stCameraInfo> infoList;
    m_isScanning = true;

	try{
		//一旦全デバイスをクリア
		foreach (stCameraInfo info, m_curInfoList)
        {
            ClCamDevice* pCamDev = new ClCamDevice;
            info.inUse = false;
            pCamDev->setInfo(info);

			emit sigDeviceScanned(pCamDev);
		}
#if 0
        foreach (const QCameraInfo &cameraInfo, QCameraInfo::availableCameras())
        {
            if (!m_isScanning)
            {
                break;
            }

            if (NULL != m_pclCamFilter)
            {
                if (!m_pclCamFilter->filter(cameraInfo.deviceName()))
                {
                    continue;
                }
            }

            LOG("Scanned[UVC]:" << name() << ":" << cameraInfo.description() << " " << cameraInfo.deviceName());

            ClCamDevice* pCamDev = new ClCamDevice;

            stCameraInfo info;
            info.id = cameraInfo.deviceName();
            info.name = cameraInfo.description();
            info.tag = new QCameraInfo(cameraInfo);
            info.conn_type = stCameraInfo::USB;
            info.inUse = true;
            info.scannedTime = QDateTime::currentDateTime();
			info.type = stCameraInfo::DOC_CAM;

            pCamDev->setInfo(info);
			infoList.push_back(info);

			//新規登録か確認
			bool bFound = false;
			foreach(const stCameraInfo curInfo, m_curInfoList){
				if(info.id == curInfo.id){
					//すでに登録されていた
					bFound = true;				//発見フラグ
					emit sigDeviceScanned(pCamDev);
					break;
				}
			}
			//まだ登録されていないDeviceだった
			if(false == bFound && false == bReqConnect){
				bReqConnect = true;
				emit sigReqConnect(pCamDev);	//接続イベントを送る
			}
		}
#endif
		//L-12 for application mode
		if(NULL != m_pelPresenter){
			ClCamDevice *pCamDev = NULL;
			m_pelPresenter->scan(&pCamDev);
			if (NULL != pCamDev && NULL != m_pclCamFilter)
			{
				if (!m_pclCamFilter->filter(pCamDev->getInfo().id))
				{
					delete pCamDev;
					pCamDev = NULL;
				}
			}
			if(NULL != pCamDev){
				LOG("Scanned[APP]:" << name() << ":" << pCamDev->getInfo().name << " " << pCamDev->getInfo().id);
				infoList.push_back(pCamDev->getInfo());
				//新規登録か確認
				bool bFound = false;
				foreach(const stCameraInfo curInfo, m_curInfoList){
					if(pCamDev->getInfo().id == curInfo.id){
						//すでに登録されていた
						bFound = true;				//発見フラグ
						emit sigDeviceScanned(pCamDev);
						break;
					}
				}
				//まだ登録されていないDeviceだった
				if(false == bFound && false == bReqConnect){
					bReqConnect = true;
					emit sigReqConnect(pCamDev);	//接続イベントを送る
				}
			}
		}
#if defined(Q_OS_MAC)
        {
            foreach(const stCameraInfo befInfo, m_curInfoList){
                bool res = false;
                foreach(const stCameraInfo curInfo, infoList){
                    if(befInfo.id == curInfo.id)
                    {
                        res = true;
                    }
                }
                if(res == false)
                {
                    printf("[disconnect >>> %s]",befInfo.id.toLocal8Bit().constData());
                    ClCamScanner::getInstance()->removeCam(befInfo);
                }
            }
        }
#endif
		m_curInfoList.clear();
		m_curInfoList = infoList;				//現在のリストに更新
	}
	catch(...){
		// no nothing
	}

	m_isScanning = false;
}

bool ClUsbCamScanner::start()
{
    LOG("Start scanning " << name());
#if defined(Q_OS_MAC)
    if (NULL != m_pclCamFilter && m_pclCamFilter->filter("09a10021")) //DocCamFilterの場合のみClUsbDriverForIM4生成
#endif
    {
        m_pelPresenter = new ClUsbDriverForIM4(NULL);
        m_pelPresenter->initialize();
    }
#if 0
	m_isScanning = true;
#else
	this->scan();
#endif
    m_scanTimer->start();
    return true;
}

void ClUsbCamScanner::stop()
{
    LOG("Stop scanning " << name());
    m_isScanning = false;
    m_scanTimer->stop();

	delete m_pelPresenter;
    m_pelPresenter = NULL;
}

bool ClUsbCamScanner::isScanning() const
{
    return m_isScanning;
}

bool ClUsbCamScanner::exist(ClCamDevice* pclDev)
{
    stCameraInfo info = pclDev->getInfo();

	foreach (const stCameraInfo curInfo, m_curInfoList)
	{
		if (info.id.compare(curInfo.id) == 0)
		{
			return true;
		}
	}
    return false;
}

