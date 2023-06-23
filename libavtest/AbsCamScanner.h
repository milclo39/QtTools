#ifndef ABSCAMSCANNER_H
#define ABSCAMSCANNER_H

#include <QTimer>

class ClCamDevice;

class AbsCamScanner : public QObject
{
    Q_OBJECT
private:
    QString m_name;                                 ///< Scanner name

public:
    explicit AbsCamScanner(QObject *parent = nullptr);

signals:
    void sigDeviceScanned(ClCamDevice*);
    void sigReqConnect(ClCamDevice*);
    void sigWiFiQuality(QString, qint32);

public slots:
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual bool isScanning() const = 0;
	virtual bool exist(ClCamDevice *pCamDev);
public:
    QString name() const;                           ///< Get name
    void setName(const QString &name);              ///< Set name
};

#endif // ABSCAMSCANNER_H
