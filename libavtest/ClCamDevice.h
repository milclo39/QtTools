//***************************************************************************//
//***************************************************************************//
/*!
    @file		ClCamDevice.h
    @author		tmson
    @brief		Camera device
*/
//***************************************************************************//
//***************************************************************************//
#ifndef CLCAMDEVICE_H
#define CLCAMDEVICE_H

#include <QTimer>
//#include <QCameraInfo>
#include <QTcpSocket>
#include <QDateTime>
#include <QMutex>

#if defined(Q_OS_WIN)
#define DEVICE_PREFIX_TX	"vid_09a1&pid_0020"
#define DEVICE_PREFIX_MO	"vid_09a1&pid_001f"
#if 1 //MX-1
#define DEVICE_PREFIX_MX	"vid_09a1&pid_0021"
#else //CX-3
#define DEVICE_PREFIX_MX	"vid_04b4&pid_00c3"
#endif
#else
#define DEVICE_PREFIX_TX    "09a10020"
#define DEVICE_PREFIX_MO	"09a1001f"
#if 1 //MX-1
#define DEVICE_PREFIX_MX	"09a10021"
#else //CX-3
#define DEVICE_PREFIX_MX	"04b400c3"
#endif
#endif
#define TXLX_PRODUCTNUM			0x20
#define MX_PRODUCTNUM			0x21
#define PX_PRODUCTNUM			0x22
#define MO_PRODUCTNUM			0x1f
#define MAMO_PRODUCTNUM			0x25
#define MXP2_PRODUCTNUM         0x26
#define OX1_PRODUCTNUM          0x27
#define L12F_PRODUCTNUM         0x28
#define L12W_PRODUCTNUM         0x29
#define MXP3_PRODUCTNUM         0x2A
#define MXPA_PRODUCTNUM         0x30
#define L12Z_PRODUCTNUM         0x2C

#define MODEL_NAME_MX1  "MX-1"
#define MODEL_NAME_MXP  "MX-P"
#define MODEL_NAME_MXP2 "MX-P2"
#define MODEL_NAME_MXP3 "MX-P3"
#define VIEW_NAME_MX1   "MX-1/MX-P"
#define VIEW_NAME_MAMO  "MA-1/MO-2"


class ClCamDevice : public QObject
{
    Q_OBJECT
public:
    //***************************************************************************//
    // Connect result - START
    //***************************************************************************//
    enum eCONN_RESULT
    {
        CONN_RESULT_OK,
        CONN_RESULT_NG
    };
    //***************************************************************************//
    // Connect result - END
    //***************************************************************************//

    //***************************************************************************//
    // Camera info - START
    //***************************************************************************//
    struct stCameraInfo
    {
        enum
        {
            DISCONNECT,
            CONNECTING,
            CONNECTED,
        } con_sta;

        enum
        {
            DOC_CAM,
            WEB_CAM,

            MAX_CAM
        } type;

        enum
        {
            WIFI,
			USB,			//for UVC device
			ELUSB,			//for ELMO Command device

            MAX_CON
        } conn_type;

        QString id;
        QString name;
        QString serial;
        quint32 address;
        QDateTime scannedTime;
        bool handling;
        bool retrying;
        bool inUse;
        void* tag;

        stCameraInfo()
        {
            con_sta = DISCONNECT;
            type = MAX_CAM;
            conn_type = MAX_CON;
            address = 0;
            inUse = false;
            handling = false;
            retrying = false;
            tag = NULL;
        }

        stCameraInfo& operator =(const stCameraInfo& other)
        {
            con_sta = other.con_sta;
            type= other.type;
            conn_type = other.conn_type;
            id = other.id;
            name= other.name;
            serial = other.serial;
            address = other.address;
            inUse = other.inUse;
            handling = other.handling;
            retrying = other.retrying;
            tag = other.tag;
            scannedTime = other.scannedTime;

            return *this;
        }

        void clear()
        {
            switch (conn_type)
            {
                case stCameraInfo::USB:
                    if (NULL != tag)
                    {
//                        delete static_cast<QCameraInfo*>(tag);
                        delete tag;
                    }
                    break;
                default:
                    break;
            }
        }
    };
    //***************************************************************************//
    // Camera info - END
    //***************************************************************************//

public:
    explicit ClCamDevice(QObject *parent = 0);
    virtual ~ClCamDevice();

signals:
    void stateChanged();
    void connectResult(ClCamDevice::eCONN_RESULT);
    void disconnected();

public slots:
    void connect();
    void connectNow();
    void connectOnly();
    void stopConnect();
    void doDisconnect();

protected slots:
    void doConnect();

private:
    stCameraInfo m_info;                            ///< Is connecting
    static ClCamDevice* m_p_connectInstance;        ///< Connecting instance
    QTimer *m_connectTimer;                         ///< Connect timer
    bool m_isBusy;                                  ///< Is busy flag
    QTcpSocket m_socket;                            ///< Tcp socket

public:
    static inline ClCamDevice* getConnectInstance();///< Get connect instance;
    inline const stCameraInfo& getInfo() const;     ///< Get info
    inline void setInfo(const stCameraInfo& info);  ///< Set info
    bool isConnected();                             ///< Check if connection is alive
};

typedef struct ClCamDevice::stCameraInfo stCameraInfo;
//***************************************************************************//
// Inline definiton
//***************************************************************************//
ClCamDevice* ClCamDevice::getConnectInstance()
{
    return m_p_connectInstance;
}

// Get info
const stCameraInfo& ClCamDevice::getInfo() const
{
    return m_info;
}

// Set info
void ClCamDevice::setInfo(const stCameraInfo& info)
{
    m_info = info;
}

#endif // CLCAMDEVICE_H
