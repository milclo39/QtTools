#include "ClVisualPresenter.h"
#include "ClCamDevice.h"
//#include "ClUsbCamScanner.h"
//#include "ClWifiDocCamScanner.h"
#include "ClLogger.h"

#include <QTimer>

#define DEBUG

#ifndef DEBUG
#undef LOG
#define LOG(x)
#endif

ClVisualPresenter ClVisualPresenter::m_instance;

ClVisualPresenter::ClVisualPresenter()
    : QObject(NULL)
,   m_pclDevice(NULL)
{
}

void ClVisualPresenter::keepAlive()
{
    QMutexLocker locker(&m_mutex);

    if (NULL == m_pclDevice)
    {
        return;
    }

    stCameraInfo newInfo = m_pclDevice->getInfo();

    if (stCameraInfo::CONNECTED != newInfo.con_sta)
    {
        return;
    }

    bool connected = m_pclDevice->isConnected();

    if (!connected)
    {
        LOG("KeepAlive fail: " << QString(newInfo.name) << " disconnected");
        newInfo.con_sta = stCameraInfo::DISCONNECT;
        newInfo.inUse = false;
        m_pclDevice->setInfo(newInfo);
        emit devChanged();
    }
}

void ClVisualPresenter::onDeviceChanged()
{
    stCameraInfo camInfo = m_pclDevice->getInfo();
    switch(camInfo.con_sta)
    {
        case stCameraInfo::DISCONNECT:
            if (!camInfo.handling)
            {
                break;
            }
			camInfo = m_pclDevice->getInfo();
			camInfo.retrying = true;
			camInfo.handling = false;
			m_pclDevice->setInfo(camInfo);
			break;

        case stCameraInfo::CONNECTED:
            camInfo = m_pclDevice->getInfo();
            camInfo.retrying = false;
            camInfo.handling = true;
            m_pclDevice->setInfo(camInfo);
            break;

        default:
            break;
    }
}

void ClVisualPresenter::slotResolution(QString type, QSize size)
{
    Q_UNUSED(type)
    Q_UNUSED(size)
    if (NULL != m_pclDevice)
    {
        m_pclDevice->connectOnly();
    }
}

void ClVisualPresenter::slotClose()
{
    if (NULL != m_pclDevice && stCameraInfo::CONNECTED == m_pclDevice->getInfo().con_sta)
    {
        m_pclDevice->doDisconnect();
    }
}

ClVisualPresenter *ClVisualPresenter::getInstance()
{
    return &m_instance;
}

ClCamDevice *ClVisualPresenter::device()
{
    return m_pclDevice;
}

void ClVisualPresenter::switchDevice(ClCamDevice* pclDev)
{
    LOG("Switch visual device to " << QString(pclDev->getInfo().name));
    QMutexLocker locker(&m_mutex);
    if (NULL != m_pclDevice)
    {
        disconnect(m_pclDevice, SIGNAL(stateChanged()), this, SIGNAL(devChanged()));
        disconnect(this, SIGNAL(devChanged()), this, SLOT(onDeviceChanged()));
        disconnect(m_pclDevice, SIGNAL(disconnected()), this, SIGNAL(disconnected()));
    }

    m_pclDevice = pclDev;
    connect(m_pclDevice, SIGNAL(stateChanged()), this, SIGNAL(devChanged()));
    connect(this, SIGNAL(devChanged()), this, SLOT(onDeviceChanged()));
    connect(m_pclDevice, SIGNAL(disconnected()), this, SIGNAL(disconnected()));

    if (!m_keepAliveTimer.isActive())
    {
        connect(&m_keepAliveTimer, SIGNAL(timeout()), this, SLOT(keepAlive()));
        m_keepAliveTimer.setInterval(3000);
        m_keepAliveTimer.start();
    }
}
