#ifndef CLPRESENTERL1N_WIN32_H
#define CLPRESENTERL1N_WIN32_H
#include "mac/ClDeviceInfo_mac.h"
#include "ClUsbDriver_global.h"

//--------------------------------------------------------------------------------------
class ClPresenterL1n : public ClDeviceInfo
{
	Q_OBJECT
public:
	ClPresenterL1n(qint32 lProductId = PRODUCT_L1N, QObject *parent = 0);
	virtual ~ClPresenterL1n();

	virtual qint32	open();
	virtual qint32	CommandPresenter(qint32 CmdID, QVariant& qParameter);

protected:
	virtual qint32	setupEndpoint();											//Override function

	virtual qint64	readData ( char * data, qint64 maxSize );
	virtual qint64	writeData ( const char * data, qint64 maxSize );
	virtual qint32	reqPicture(eENDPOINT	*peOutEndp, QVariant &param);
	virtual qint32	resPicture(eENDPOINT	*peInEndp, QVariant &param);
	virtual qint32	reqCommand(eENDPOINT	*peOutEndp, QVariant &param);
	virtual qint32	resCommand(eENDPOINT	*peInEndp, QVariant &param);
	virtual bool	analyJpegHeader(const QByteArray qJpgData, QSize &qSize);
	virtual qint32	execCommand(ClUsbCmd &clCmd);

	virtual qint32	getPicture(QVariant &qParam);								//get picture from presenter
	virtual qint32	getModelName(QVariant &qParam);								//get model name from device and output to QVariant
	virtual qint32	getVersion(QVariant &qParam);								//get version from device and output to QVariant
	virtual qint32	externalInputCmd(QVariant &qParam);							//external input command
};
//--------------------------------------------------------------------------------------
class ClPresenterP10s : public ClPresenterL1n
{
	Q_OBJECT
public:
	ClPresenterP10s(qint32 lProductId = PRODUCT_P10S, QObject *parent = 0)
		:ClPresenterL1n(lProductId,parent){}

protected:
	virtual qint32	getModelName(QVariant &qParam);									//get model name from device and output to QVariant

};
//--------------------------------------------------------------------------------------
class ClPresenterP30s : public ClPresenterL1n
{
	Q_OBJECT
public:
	ClPresenterP30s(qint32 lProductId = PRODUCT_P30S, QObject *parent = 0)
		:ClPresenterL1n(lProductId,parent){}

protected:
	virtual qint32	getModelName(QVariant &qParam);									//get model name from device and output to QVariant

};
//--------------------------------------------------------------------------------------
class ClPresenterP100 : public ClPresenterL1n
{
	Q_OBJECT
public:
	ClPresenterP100(qint32 lProductId = PRODUCT_P100, QObject *parent = 0)
		:ClPresenterL1n(lProductId,parent){}

protected:
	virtual qint32	getModelName(QVariant &qParam);									//get model name from device and output to QVariant

};
//--------------------------------------------------------------------------------------
class ClPresenterFF1 : public ClPresenterL1n
{
	Q_OBJECT
public:
	ClPresenterFF1(qint32 lProductId = PRODUCT_FF1, QObject *parent = 0)
		:ClPresenterL1n(lProductId,parent){}

protected:
	virtual qint32	getModelName(QVariant &qParam);									//get model name from device and output to QVariant

};
//--------------------------------------------------------------------------------------




#endif // CLPRESENTERL1N_WIN32_H
