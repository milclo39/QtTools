#include "ClCamDevice.h"
#include "ClVisualPresenter.h"
//#include "ClWifiDocCamScanner.h"
#include "ClCamScanner.h"
#include "ClLogger.h"
//#include "ClLiveViewController.h"
#include <QtNetwork>

#define DEBUG

#ifndef DEBUG
#undef LOG
#define LOG(x)
#endif

ClCamDevice* ClCamDevice::m_p_connectInstance = NULL;

ClCamDevice::ClCamDevice(QObject *parent):
    QObject(parent),
    m_connectTimer(NULL)
{
    m_isBusy = false;
    m_connectTimer = new QTimer();
    QObject::connect(m_connectTimer, SIGNAL(timeout()),
            this, SLOT(doConnect()));
}

ClCamDevice::~ClCamDevice()
{
    if (NULL != m_connectTimer)
    {
        QObject::disconnect(m_connectTimer, SIGNAL(timeout()), this, SLOT(doConnect()));
        delete m_connectTimer;
        m_connectTimer = NULL;
    }

    m_info.clear();
}

void ClCamDevice::connect()
{
    if (m_p_connectInstance)
    {
        m_p_connectInstance->doDisconnect();
    }

    LOG("Request connect to " << m_info.name);

    m_info.con_sta = stCameraInfo::CONNECTING;
    m_p_connectInstance = this;
    ClVisualPresenter* presenter = ClVisualPresenter::getInstance();
    presenter->switchDevice(this);
    emit stateChanged();

    m_connectTimer->setInterval(1000);
    m_connectTimer->stop();
    m_connectTimer->start();
}

void ClCamDevice::connectNow()
{
    LOG("connectNow");
    connect();
    doConnect();
}

void ClCamDevice::connectOnly()
{
    if (!m_info.handling)
    {
        return;
    }
    m_info.con_sta = stCameraInfo::CONNECTING;
    m_p_connectInstance = this;
    m_connectTimer->setInterval(1000);
    m_connectTimer->stop();
    m_connectTimer->start();
}

void ClCamDevice::stopConnect()
{
    m_connectTimer->stop();
}

void ClCamDevice::doDisconnect()
{
    LOG("Disconnect with " << m_info.name);
    stopConnect();
    switch (m_info.conn_type)
    {
        case stCameraInfo::WIFI:
            m_socket.close();
            break;
        default:
            break;
    }
    m_p_connectInstance->m_info.con_sta = stCameraInfo::DISCONNECT;
    emit disconnected();
    emit stateChanged();
}

bool ClCamDevice::isConnected()
{
/*
    if (ClLiveViewController::getInstance()->getCtl())
    {
        if (!ClLiveViewController::getInstance()->getCtl()->isStreaming())
        {
            LOG("Frame dropped. Consider disconnected");
            return false;
        }
        else
        {
            return true;
        }
    }
*/
    switch (m_info.conn_type)
    {
        case stCameraInfo::WIFI:
            if (m_socket.state() != QTcpSocket::ConnectedState)
            {
                LOG("Socket disconnected");
                return false;
            }

            return true;

        case stCameraInfo::USB:
			//NO break
		case stCameraInfo::ELUSB:
			return ClCamScanner::getInstance()->exist(this);

        default:
            break;
    }

    return false;
}

void ClCamDevice::doConnect()
{
    if (m_isBusy)
    {
        return;
    }
    m_isBusy = true;

    LOG("Connecting to " << m_info.name);
    bool connected = false;

    do
    {
        switch(m_info.type)
        {
            case stCameraInfo::DOC_CAM:
                switch (m_info.conn_type)
                {
                    case stCameraInfo::WIFI:
                        m_socket.connectToHost(QHostAddress(m_info.address), 554);

                        connected = m_socket.waitForConnected(3000);
                        if (!connected)
                        {
                            LOG("Error: " << m_socket.errorString());
                            break;
                        }
                        break;

					case stCameraInfo::USB:				//for UVC device
						connected = true;
						break;
					case stCameraInfo::ELUSB:			//for Elmo Command device
						connected = true;
						break;
					default:
                        connected = true;
                        break;
                }
            break;

            default:
                connected = true;
                break;
        }
    }
    while (false);

    if (connected)
    {
        LOG("Connected to " << m_info.name);
        m_info.con_sta = stCameraInfo::CONNECTED;
        m_connectTimer->stop();
        emit connectResult(CONN_RESULT_OK);
        emit stateChanged();
    }
    else
    {
        LOG("Cannot connect to " << m_info.name);
        m_info.con_sta = stCameraInfo::DISCONNECT;
        m_connectTimer->stop();
        emit connectResult(CONN_RESULT_NG);
        emit stateChanged();
    }

    m_isBusy = false;
}

