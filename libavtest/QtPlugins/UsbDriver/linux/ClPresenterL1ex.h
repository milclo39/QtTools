#ifndef CLPRESENTERL1EX_H
#define CLPRESENTERL1EX_H
#include "ClUsbDriver/ClUsbDriver_global.h"
#include "linux/ClDeviceInfo.h"

//--------------------------------------------------------------------------------------
class ClPresenterL1ex : public ClDeviceInfo
{
	Q_OBJECT
public:
	ClPresenterL1ex(qint32 lProductId = PRODUCT_L1EX, QObject *parent = 0);
	virtual ~ClPresenterL1ex();

	virtual qint32 CommandPresenter(qint32 CmdID, QVariant& qParameter);

protected:
	virtual qint32 setupEndpoint();						//Override function

	virtual qint32 getPicture(QVariant &qParam);			//get picture from presenter
	virtual qint32 getModelName(QVariant &qParam);			//get model name from device and output to QVariant
	virtual qint32 getVersion(QVariant &qParam);			//get version from device and output to QVariant
	virtual qint32 execCaptureInit();						//CaptureFlag
	virtual qint32 getCaptureStatus(QVariant &Param);		//function of get CaptureFlag
};

//--------------------------------------------------------------------------------------


#endif // CLPRESENTERL1EX_H
