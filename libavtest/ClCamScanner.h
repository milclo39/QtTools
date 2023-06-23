//***************************************************************************//
//***************************************************************************//
/*!
    @file		ClCamScanner.h
    @author		tmson
    @brief		Scan all available camera
*/
//***************************************************************************//
//***************************************************************************//
#ifndef CLCAMSCANNER_H
#define CLCAMSCANNER_H

#include "ClCamDevice.h"
#include "ClCamScannerFactory.h"

#include <QObject>
#include <QMap>
#include <QList>

class QTimer;
class AbsCamScanner;

class ClCamScanner : public QObject
{
    Q_OBJECT
protected:
    explicit ClCamScanner(QObject *parent = 0);     ///< Constructor
    virtual ~ClCamScanner();                        ///< Destructor
    void initialize();                              ///< Initialize member

protected slots:
    void webcamScanned(ClCamDevice*);               ///< Webcam scanned
    void docCamScanned(ClCamDevice*);               ///< Doc Camera scanned
    void doScan();                                  ///< Do scan

    void slotReqConnect(ClCamDevice*);

signals:
    void sigCamListUpdated();                       ///< On camera list updated
    void sigWebcamScanned(ClCamDevice*);            ///< Webcam scanned
    void sigReqConnect(ClCamDevice*);               ///< Request connect to device
    void sigWiFiQuality(QString clStrDeviceID, qint32 lSignalLevel);
	void sigCamListChanged();

public slots:
    void startScan();                               ///< Scan camera list
    void stopScan();                                ///< Stop scaning

public:
    static ClCamScanner* getInstance();             ///< Get instance
    inline const QMap<QString, ClCamDevice*>& camBundle() const;
                                                    ///< Get camera map
    inline const QList<ClCamDevice*>& camList() const;
                                                    ///< Get camera list
    bool isScanning(ClCamScannerFactory::eSCANNER_TYPE);
                                                    /// Is scanning
    void addDev(ClCamDevice*);                      ///< Add device
    void addCamera(ClCamDevice*);                   ///< Add camera
    void clear();                                   ///< Clear all
	bool exist(ClCamDevice*pCamDev);				///<

#if defined(Q_OS_MAC)
    void removeCam(stCameraInfo info);
#endif

private:
    static ClCamScanner m_instance;                 ///< Instance
    AbsCamScanner** m_scannerList;                  ///< Scanner list
    QMap<QString, ClCamDevice*> m_camBundle;        ///< Camera map
    QList<ClCamDevice*> m_orderedList;              ///< Camera ordered list
    QMutex m_mutex;                                 ///< Mutex
    bool m_isRunning;                               ///< Is running
};

const QMap<QString, ClCamDevice*>& ClCamScanner::camBundle() const
{
    return m_camBundle;
}

const QList<ClCamDevice*>& ClCamScanner::camList() const
{
    return m_orderedList;
}
#endif // CLCAMSCANNER_H
