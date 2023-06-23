#ifndef CLPRESENTERL1EX_WIN32_H
#define CLPRESENTERL1EX_WIN32_H
#include "mac/ClDeviceInfo_mac.h"
#include "ClUsbDriver/ClUsbDriver_global.h"

//--------------------------------------------------------------------------------------
class ClPresenterL1ex : public ClDeviceInfo
{
	Q_OBJECT
public:
	ClPresenterL1ex(qint32 lProductId = PRODUCT_L1EX, QObject *parent = 0);
	virtual ~ClPresenterL1ex();

	virtual qint32	open();														//open for a this device
	virtual qint32	CommandPresenter(qint32 CmdID, QVariant& qParameter);

protected:
	virtual qint32	setupEndpoint();											//Override function

	//V1 Command
	virtual qint32	getPicture(QVariant &qParam);								//get picture from presenter
	virtual qint32	getModelName(QVariant &qParam);								//get model name from device and output to QVariant
	virtual qint32	getVersion(QVariant &qParam);								//get version from device and output to QVariant
	virtual qint32	execCaptureInit();											//CaptureFlag
	virtual qint32	getCaptureStatus(QVariant &Param);							//function of get CaptureFlag

	virtual qint32	externalInputCmd(QVariant &qParam);

	virtual qint64	readData ( char * data, qint64 maxSize );					//
	virtual qint64	writeData ( const char * data, qint64 maxSize );			//
	virtual qint32	reqPicture(eENDPOINT	*peOutEndp, QVariant &param);
	virtual qint32	resPicture(eENDPOINT	*peInEndp, QVariant &param);
	virtual qint32	reqCommand(eENDPOINT	*peOutEndp, QVariant &param);
	virtual qint32	resCommand(eENDPOINT	*peInEndp, QVariant &param);
	virtual bool	analyJpegHeader(const QByteArray qJpgData, QSize &qSize);
	virtual qint32	execCommand(ClUsbCmd &clCmd);

signals:
	void sigFwUpdateProgress(qint32 lParam, qint32 lStatus);	//ファームアップデート進捗状態

private:
	qint32			m_lCmdID;
};
//--------------------------------------------------------------------------------------


#endif // CLPRESENTERL1EX_WIN32_H
