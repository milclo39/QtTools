#ifndef CLL12PRESENTER_H
#define CLL12PRESENTER_H

#include "ClUsbDriver/ClUsbDriver.h"
#include "ClCamDevice.h"

//--------------------------------------------------------------------------------------
class ClUsbDriverForIM4 : public ClUsbDriver
{
	Q_OBJECT
public:
	explicit ClUsbDriverForIM4(QWidget *parent = NULL);
	virtual ~ClUsbDriverForIM4();

	virtual qint32 initialize();								//initialize
    virtual qint32 scan(ClCamDevice **ppCamDev);

	virtual qint32 openDevice(qint32 lProductId,
								QString  strDevPath);						//open for a device
	virtual qint32 startStream(qint32 lProductId, eKINDSTREAM iKind,qint32 iFrate, qint32 iQuality);
	virtual qint32 stopStream(qint32 lProductId, eKINDSTREAM iKind);

	//camera controll
	virtual void slotZoom(qint32 lProductId, qint32 command);						//is received when wanna control zoom
	virtual void slotFocus(qint32 lProductId, qint32 command);						//is received when wanna control focus
	virtual void slotIris(qint32 lProductId, qint32 command);						//is received when wanna control iris
	//extend(L-1ex,SDC330)
	virtual void slotRotate(qint32 lProductId, qint32 command);						//is received when wanna roatate picture

};

//--------------------------------------------------------------------------------------


#endif // CLL12PRESENTER_H
