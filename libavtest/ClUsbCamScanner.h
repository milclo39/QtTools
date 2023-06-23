#ifndef CLUSBCAMSCANNER_H
#define CLUSBCAMSCANNER_H
#include "AbsCamScanner.h"
#include "ClCamFilter.h"
#include "ClL12Presenter.h"

class QTimer;
class QCameraInfo;

class ClUsbCamScanner : public AbsCamScanner
{
    Q_OBJECT
private:
    ClCamFilter* m_pclCamFilter;                    ///< Camera filter
    QTimer *m_scanTimer;                            ///< Scan timer
    bool m_isScanning;                              ///< Is scanning
	QList<stCameraInfo> m_curInfoList;				///< Current USB camera list
	ClUsbDriverForIM4  *m_pelPresenter;				///<

protected slots:
    void scan();                                    ///< Scan

public:
    ClUsbCamScanner(
            ClCamFilter* pclCamFilter = NULL);      ///< Constructor
    virtual ~ClUsbCamScanner();                     ///< Destructor
    bool start();                                   ///< Start scanning
    void stop();                                    ///< Stop scanning
    bool isScanning() const;                        ///< Is scanning
	bool exist(ClCamDevice*);						 ///< Exist device
};

#endif // CLUSBCAMSCANNER_H
