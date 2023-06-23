#ifndef CLDEVICEINFO_WIN_H
#define CLDEVICEINFO_WIN_H

#include "ClUsbDriver_global.h"
#include "ClUsbCmd.h"
#include "ClUsbCmdV2.h"
#include "UsbDevice.h"
#include "EL_GUIDs.h"
#include "ClUsbDriver/ClUsbDriver_Struct.h"
#include <QTranslator>

//--------------------------------------------------------------------------------------
class ClDeviceInfo : public QIODevice
{
public:
	ClDeviceInfo(qint32 a_ProductId, QObject *object = NULL);
	virtual ~ClDeviceInfo();

	virtual qint32 open();									//open for a this device
	virtual void close();									//close for a this device
	virtual qint32 getVendorId(){ return m_lVendorId; }		//return VendorId
	virtual qint32 getProductId(){ return m_lProductId; }		//return ProductId

	virtual void setTransferTimeout(qint32 timeout){ m_iTimeout = timeout; }
	virtual qint32 getTransferTimeout(){ return m_iTimeout; }

	virtual qint32 CommandPresenter(qint32 CmdID, QVariant& qParameter);

	// リモコン
	virtual QWidget*	getRemoconWidget();

	// 言語
	virtual void		setTranslator();

	// ストリーム停止
	virtual void		startStream(){}
	virtual void		stopStream(){}

	// 解像度
	virtual QStringList	getSupportedResolutions();
	virtual QString		getCurrentResolution();
	virtual qint32		setCurrentResolution(QString strResolution);

	// フレームレート
	virtual qint32		setCurrentFrameRate(bool bIsFrameRateMax);

	virtual void		reopen(){}

protected:
	ClDeviceInfo(){}

	virtual qint64	readData ( char * data, qint64 maxSize );
	virtual qint64	writeData ( const char * data, qint64 maxSize );

	virtual qint32 reqPicture(eENDPOINT	*peOutEndp, QVariant &param);
	virtual qint32 resPicture(eENDPOINT	*peInEndp, QVariant &param);
	virtual qint32 reqCommand(eENDPOINT	*peOutEndp, QVariant &param);
	virtual qint32 resCommand(eENDPOINT	*peInEndp, QVariant &param);

	qint32						m_iTimeout;				//time to time-out
	qint32						m_lVendorId;			//vendor ID of device
	qint32						m_lProductId;			//product ID of device

	eENDPOINT	*m_pOutCmdEndp;		//end point descriptor for command out
	eENDPOINT	*m_pInCmdEndp;		//end point descriptor for command in
	eENDPOINT	*m_pOutDataEndp;	//end point descriptor for jpeg data out
	eENDPOINT	*m_pInDataEndp;		//end point descriptor for jpeg data in

	virtual qint32 setupEndpoint();							//Override function

	virtual qint32 getPicture(QVariant &qParam);			//get picture from presenter
	virtual qint32 execCommand(ClUsbCmd &clCmd);			//execute some command with device
	virtual qint32 getModelName(QVariant &qParam);			//get model name from device and output to QVariant
	virtual qint32 getVersion(QVariant &qParam);			//get version from device and output to QVariant
	virtual qint32 execCaptureInit();						//CaptureFlag
	virtual qint32 getCaptureStatus(QVariant &Param);		//function of get CaptureFlag

	qint32					m_iPictureSize;					//return of picture size that get just before that
	eKINDRECTID				m_iPictureRect;					//specify of picture size
	quint16					m_usHeight;						//Height
	quint16					m_usWidth;						//Width

	ClIoDevice *			m_pUsbDevice;					//Usb Device

	QTranslator*			m_pclTrans;						// 言語

#define CHECK_MAX_NUM	(300)								//
#define CHECK_CHAR_FF	((qint8)0xff)						//
#define CHECK_CHAR_C0	((qint8)0xc0)						//
#define CHECK_CHAR_D8	((qint8)0xd8)						//
#define JPEG_OFFSET_Y	(5)									//
#define JPEG_OFFSET_X	(7)									//

#define JPEG_PICTURESIZE		(65536)						//

	virtual bool analyJpegHeader(const QByteArray qJpgData, QSize &qSize);
	virtual bool changeJpegHeader(QByteArray &qJpgData, QSize qSize);

};
//--------------------------------------------------------------------------------------


#endif // CLDEVICEINFO_H
