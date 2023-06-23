#ifndef CLVISUALPRESENTER_H
#define CLVISUALPRESENTER_H

#include <QTimer>
#include <QSize>
#include <QMutex>
class ClCamDevice;
class QTimer;

class ClVisualPresenter: public QObject
{
    Q_OBJECT
signals:
    void devChanged();
    void disconnected();

protected:
    ClVisualPresenter();

protected slots:
    void keepAlive();
    void onDeviceChanged();

public slots:
    void slotResolution(QString type, QSize size);
    void slotClose();

public:
    static ClVisualPresenter* getInstance();
    ClCamDevice* device();
    void switchDevice(ClCamDevice*);

private:
    static ClVisualPresenter m_instance;            ///< Instance
    ClCamDevice* m_pclDevice;                       ///< Current device
    QTimer m_keepAliveTimer;                        ///< Keep alive timer
    QMutex m_mutex;                                 ///< Lock mutex
};

#endif // CLVISUALPRESENTER_H
