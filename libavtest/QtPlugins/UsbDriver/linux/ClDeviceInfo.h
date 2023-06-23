#ifndef CLDEVICEINFO_H
#define CLDEVICEINFO_H

#include "ClUsbDriver/ClUsbDriver_global.h"
#include "ClUsbCmd.h"


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

	virtual QString getDeviceInfo(libusb_device *a_pDevice);							//

	void setContext(libusb_context *ctx){ m_ctx = ctx; }

protected:
	ClDeviceInfo(){}

	virtual qint64	readData ( char * data, qint64 maxSize );
	virtual qint64	writeData ( const char * data, qint64 maxSize );

	virtual qint32 reqPicture(const libusb_endpoint_descriptor *outEndp, QVariant &param);
	virtual qint32 resPicture(const libusb_endpoint_descriptor *pInEndp, QVariant &param);
	virtual qint32 reqPictureV2(const libusb_endpoint_descriptor *outEndp, QVariant &param);
	virtual qint32 resPictureV2(const libusb_endpoint_descriptor *pInEndp, QVariant &param);

	virtual qint32 reqStreamV2(const libusb_endpoint_descriptor *pOutEndp, QVariant &qParam);
	virtual qint32 resStreamV2(const libusb_endpoint_descriptor *pInEndp, QVariant &qParam);

	virtual qint32 reqCommand(const libusb_endpoint_descriptor *outEndp, QVariant &param);
	virtual qint32 resCommand(const libusb_endpoint_descriptor *pInEndp, QVariant &param);

	virtual qint32 resetDevice();

	qint32						m_iTimeout;				//time to time-out
	qint32						m_lVendorId;			//vendor ID of device
	qint32						m_lProductId;			//product ID of device

	libusb_context				*m_ctx;
	libusb_device_handle		*m_pHandle;				//handle of device
	libusb_device				*m_pDevice;				//itself device

	libusb_device_descriptor	*m_pDescriptor;			//descriptor
	libusb_config_descriptor	*m_pConfig;				//config descriptor

	struct libusb_transfer		*m_pTransfer;

	qint32 detachKernelDriver();						//
	qint32 attachKernelDriver();						//

	const libusb_endpoint_descriptor	*m_pOutCmdEndp;		//end point descriptor for command out
	const libusb_endpoint_descriptor	*m_pInCmdEndp;		//end point descriptor for command in
	const libusb_endpoint_descriptor	*m_pOutDataEndp;	//end point descriptor for jpeg data out
	const libusb_endpoint_descriptor	*m_pInDataEndp;		//end point descriptor for jpeg data in

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

#define CHECK_MAX_NUM	(300)								//
#define CHECK_CHAR_FF	((qint8)0xff)						//
#define CHECK_CHAR_C0	((qint8)0xc0)						//
#define CHECK_CHAR_D8	((qint8)0xd8)						//
#define JPEG_OFFSET_Y	(5)									//
#define JPEG_OFFSET_X	(7)									//

	virtual bool analyJpegHeader(const QByteArray qJpgData, QSize &qSize);
	virtual bool changeJpegHeader(QByteArray &qJpgData, QSize qSize);

#define JPEG_PICTURESIZE		512					//

	union unENDPADDRESS{								//structure of end point address
		quint8 cData;
		struct{
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
			quint8 edpNumber	:	4;					//number of end point
			quint8 reserved		:	3;					//
			quint8 direction	:	1;					//direction of end point
#else
			quint8 direction	:	1;					//direction of end point
			quint8 reserved		:	3;					//
			quint8 edpNumber	:	4;					//number of end point
#endif
		};
	};

	union unENDPATTRIBUTE{								//structure of end point attribute
		quint8 cData;
		struct{
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
			quint8 transferType	:	2;					//transfer type
			quint8 isoSyncType	:	2;					//ISO sync type
			quint8 isoUsageType	:	2;					//ISO usage type
			quint8 reserved		:	2;
#else
			quint8 reserved		:	2;
			quint8 isoUsageType	:	2;					//ISO usage type
			quint8 isoSyncType	:	2;					//ISO sync type
			quint8 transferType	:	2;					//transfer type
#endif
		};
	};

	union unDESCATTRIBUTE{								//structure of descriptor attribute
		quint8 cData;
		struct{
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
			quint8 reserved		:	5;					//reserved
			quint8 remotewkup	:	1;					//remote wake up
			quint8 selfpower	:	1;					//self power supply
			quint8 reservedbit	:	1;					//reserved
#else
			quint8 reservedbit	:	1;					//reserved
			quint8 selfpower	:	1;					//self power supply
			quint8 remotewkup	:	1;					//remote wake up
			quint8 reserved		:	5;					//reserved
#endif
		};
	};

#define ENDPOINT_IN			0x01
#define ENDPOINT_OUT		0x00

	QString printEndpointDescriptor(const libusb_endpoint_descriptor *pEndpt);			//
	QString printInterfaceDescriptor(const libusb_interface_descriptor *pInterdesc);	//
	QString printInterface(const libusb_interface *pInterface);							//
	QString printDeviceDescriptor(const libusb_device_descriptor *pDescriptor);			//
	QString printConfigDescriptor(const libusb_config_descriptor *pConfig);				//
	QString printStringDescriptor(libusb_device_handle *pHandle,
								  const libusb_device_descriptor *pDescriptor);			//
	QString getDeviceClass(quint8 bDeviceClass);		//
	QString getVendorString(ushort usVendorId);			//
	QString getDescriptorType(quint8 bDescType);		//
	QString getUsbType(quint16 bcdUSB);					//
	QString getEndpointAddress(quint8 address);			//
	QString getEndpointAttribute(quint8 attribute);		//
	QString getDescriptorAttribute(quint8 attribute);	//

};
//--------------------------------------------------------------------------------------


#endif // CLDEVICEINFO_H
