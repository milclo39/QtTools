#include "ClUsbDriver/ClUsbDriver_global.h"
#include "Common/CmnDefine.h"
#include "ClUsbDriver/ClUsbDriver_linux.h"
#include "ClUsbDriver/Command_Presenter.h"
#include "linux/ClDeviceInfo.h"
#include "vndrlist.h"
#include "qtdxuttimer.h"
#include "ClUsbCmd.h"
#include "ClUsbCmdV2.h"
#include "ElmoStruct.h"
#include "ElmoStructV2.h"


#define		ELUSB_WT_USBPACKET_LENGTH	(512)	//512バイト境界
#define		ELUSB_WT_SENDLENGTH_ADJUST	(32)	//512バイト境界 対策

//--------------------------------------------------------------------------------
/*!
	@fn			ClDeviceInfo
	@brief
 */
//--------------------------------------------------------------------------------
ClDeviceInfo::ClDeviceInfo(qint32 a_ProductId, QObject *object) : QIODevice(object)
{
	m_lVendorId = VENDOR_ELMO;						//set default VENDOR_ID
	m_lProductId = a_ProductId;
	m_pConfig = NULL;
	m_pHandle = NULL;
	m_pDevice = NULL;
	m_pDescriptor = NULL;
	m_iTimeout = 5000;							//time to time-out

	m_pInCmdEndp = NULL;										//set NULL to inEndp
	m_pOutCmdEndp = NULL;										//set NULL to outEndp
	m_pInDataEndp = NULL;										//set NULL to inEndp
	m_pOutDataEndp = NULL;										//set NULL to outEndp

	m_pTransfer = NULL;

	m_iPictureSize = 0;
	m_iPictureRect = RECTID_PICTURE_DEFAULT;
	m_usHeight = 0;
	m_usWidth = 0;

	m_ctx = NULL;
}
//--------------------------------------------------------------------------------
/*!
	@fn			~ClDeviceInfo
	@brief
 */
//--------------------------------------------------------------------------------
ClDeviceInfo::~ClDeviceInfo()
{
	close();
	m_pInCmdEndp = NULL;										//set NULL to inEndp
	m_pOutCmdEndp = NULL;										//set NULL to outEndp
	m_pInDataEndp = NULL;										//set NULL to inEndp
	m_pOutDataEndp = NULL;										//set NULL to outEndp
}
//--------------------------------------------------------------------------------
/*!
	@fn			open
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::open()
{
	if(isOpen() == true){
		return LIBUSB_SUCCESS;
	}
	m_pHandle = libusb_open_device_with_vid_pid(m_ctx,m_lVendorId,m_lProductId);	//get the handle
	if(m_pHandle != NULL){
		qint32 lRetRes = libusb_reset_device(m_pHandle);	//reset for device
		if(lRetRes != LIBUSB_SUCCESS){						//if not success
			return lRetRes;									//return error number
		}

		m_pDevice = libusb_get_device(m_pHandle);			//get the underlying device for a handle
		if(m_pDevice != NULL){
			if(detachKernelDriver() != LIBUSB_SUCCESS){		//
				close();
				return LIBUSB_ERROR_NO_DEVICE;
			}
			qint32 lEpRes = setupEndpoint();			//is overrided setupEndpoint of call
			if(lEpRes != LIBUSB_SUCCESS){				//
				return lEpRes;							//
			}
			if(QIODevice::open(ReadWrite | Unbuffered) == false){	//
				if(m_pConfig != NULL){
					libusb_free_config_descriptor(m_pConfig);	//release config descriptor
					m_pConfig = NULL;							//
				}
				libusb_close(m_pHandle);						//the device is closeing
				attachKernelDriver();							//
				m_pHandle = NULL;								//set NULL for handle
				if(m_pDescriptor != NULL){
					delete m_pDescriptor;
					m_pDescriptor = NULL;
				}
				return LIBUSB_ERROR_NO_DEVICE;
			}
		}else{
			libusb_close(m_pHandle);					//Its closed handle because was able to get device
			return LIBUSB_ERROR_NO_DEVICE;
		}
	}else{
		return LIBUSB_ERROR_NO_DEVICE;
	}
	return LIBUSB_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			close
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClDeviceInfo::close()
{
	if(isOpen() == true){								//if available device
		if(m_pConfig != NULL){
			libusb_free_config_descriptor(m_pConfig);	//release config descriptor
			m_pConfig = NULL;							//
		}
		libusb_close(m_pHandle);						//the device is closeing
		attachKernelDriver();							//
		m_pHandle = NULL;								//set NULL for handle
		if(m_pDescriptor != NULL){
			delete m_pDescriptor;
			m_pDescriptor = NULL;
		}
		QIODevice::close();								//call for rarent close function
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn			detachKarnelDriver
	@brief
	@return		ref
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::detachKernelDriver()
{
	int res = LIBUSB_SUCCESS;
	if(m_pHandle != NULL){
		if(libusb_kernel_driver_active(m_pHandle,NULL) == 1){			//if a kernel driver is active
			res = libusb_detach_kernel_driver(m_pHandle,NULL);
			if(res != LIBUSB_SUCCESS){								//if retuen is not success
				DBGFL("failed detach kernel driver : %d",res);
			}
		}
	}
	return res;
}
//--------------------------------------------------------------------------------
/*!
	@fn			attachKarnelDriver
	@brief
	@return		ref
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::attachKernelDriver()
{
	int res = LIBUSB_SUCCESS;
	if(m_pHandle != NULL){
		if(libusb_kernel_driver_active(m_pHandle,NULL) == 0){			//if a kernel driver is not actived
			res = libusb_attach_kernel_driver(m_pHandle,NULL);			//do kernel device to active
			if(res != LIBUSB_SUCCESS){								//if retuen is not success
				DBGFL("failed detach kernel driver");
			}
		}
	}
	return res;
}
//--------------------------------------------------------------------------------
/*!
	@fn			getDeviceInfo
	@brief
	@return		ref
 */
//--------------------------------------------------------------------------------
QString ClDeviceInfo::getDeviceInfo(libusb_device *a_pDevice)
{
	Q_ASSERT(a_pDevice);
	QString strResult;
	int res = LIBUSB_SUCCESS;
	libusb_config_descriptor *a_pConfig = NULL;
	libusb_device_handle *a_pHandle = NULL;
	libusb_device_descriptor *a_pDescriptor = new libusb_device_descriptor();
	res = libusb_get_device_descriptor(a_pDevice,a_pDescriptor);
	if(res != LIBUSB_SUCCESS){
		delete a_pDescriptor;
		return strResult + "libusb_get_device_descriptor failure\n";
	}
	strResult += QString().sprintf("bus number : %d\n",libusb_get_bus_number(a_pDevice));
	strResult += QString().sprintf("device address : %d\n",libusb_get_device_address(a_pDevice));

	strResult += printDeviceDescriptor(a_pDescriptor);

	res = libusb_get_active_config_descriptor(a_pDevice,&a_pConfig);
	if(res != LIBUSB_SUCCESS){
		delete a_pDescriptor;
		return  strResult + "libusb_get_active_config_descriptor failure\n";
	}
	strResult += printConfigDescriptor(a_pConfig);
	res = libusb_open(a_pDevice, &a_pHandle);
	if(res == LIBUSB_SUCCESS){
		strResult += printStringDescriptor(a_pHandle,a_pDescriptor);
		libusb_close(a_pHandle);
		a_pHandle = NULL;
	}
	libusb_free_config_descriptor(a_pConfig);
	delete a_pDescriptor;
	strResult += "\n";
	return strResult;
}
//--------------------------------------------------------------------------------
/*!
	@fn			printEndpointDescriptor
	@brief
	@return		ref
 */
//--------------------------------------------------------------------------------
QString ClDeviceInfo::printEndpointDescriptor(const libusb_endpoint_descriptor *pEndpt)
{
	QString strResult;

	strResult += "-------------show Endpoint  Descriptor--------------\n";
	strResult += QString("Descriptor Type : ") + getDescriptorType(pEndpt->bDescriptorType);
	strResult += QString("Address : ") + getEndpointAddress(pEndpt->bEndpointAddress);
	strResult += QString("Attributes : ") + getEndpointAttribute(pEndpt->bmAttributes);
	strResult += QString().sprintf("wMaxPacketSize : %d\n",pEndpt->wMaxPacketSize);
	strResult += QString().sprintf("Interval for polling endpoint : %d\n",pEndpt->bInterval);
	strResult += QString().sprintf("bRefresh * For audio devices only * : %d\n",pEndpt->bRefresh);
	strResult += QString().sprintf("bSynchAddress * For audio devices only * : %d\n",pEndpt->bSynchAddress);

	if(pEndpt->extra != NULL){
		strResult += QString().sprintf("Extra descriptors : %s\n",pEndpt->extra);
		strResult += QString().sprintf("extra_length : %d\n",pEndpt->extra_length);
	}
	return strResult;
}

//--------------------------------------------------------------------------------
/*!
	@fn			printInterfaceDescriptor
	@brief
	@return		ref
 */
//--------------------------------------------------------------------------------
QString ClDeviceInfo::printInterfaceDescriptor(const libusb_interface_descriptor *pInterdesc)
{
	QString strResult;

	strResult += "-------------show Interface Descriptor--------------\n";
	strResult += QString("Descriptor Type : ") + getDescriptorType(pInterdesc->bDescriptorType);
	strResult += QString().sprintf("bInterfaceNumber : %d\n",pInterdesc->bInterfaceNumber);
	strResult += QString().sprintf("bAlternateSetting : %d\n",pInterdesc->bAlternateSetting);
	strResult += QString("CLASS Type : ") + getDeviceClass(pInterdesc->bInterfaceClass);
	strResult += QString("Sub Class Type : ") + getDeviceClass(pInterdesc->bInterfaceSubClass);
	strResult += QString().sprintf("bInterfaceProtocol : %d\n",pInterdesc->bInterfaceProtocol);
	strResult += QString().sprintf("Index of string descriptor : %d\n",pInterdesc->iInterface);
	strResult += QString().sprintf("bNumEndpoints : %d\n",pInterdesc->bNumEndpoints);
	for(qint32 i = 0; i < pInterdesc->bNumEndpoints; i++ ){
		strResult += printEndpointDescriptor(&pInterdesc->endpoint[i]);
	}
	if(pInterdesc->extra != NULL){
		strResult += QString().sprintf("Extra descriptors : %s\n",pInterdesc->extra);
		strResult += QString().sprintf("extra_length : %d\n",pInterdesc->extra_length);
	}
	return strResult;
}

//--------------------------------------------------------------------------------
/*!
	@fn			printInterfac
	@brief
	@return		ref
 */
//--------------------------------------------------------------------------------
QString ClDeviceInfo::printInterface(const libusb_interface *pInterface)
{
	QString strResult;

	strResult += "-------------show Interface--------------\n";
	strResult += QString().sprintf("num_altsetting : %d\n",pInterface->num_altsetting);
	for(qint32 i = 0; i < pInterface->num_altsetting; i++ ){
		strResult += printInterfaceDescriptor(&pInterface->altsetting[i]);
	}
	return strResult;
}

//--------------------------------------------------------------------------------
/*!
	@fn			printConfigDescriptor
	@brief
	@return		ref
 */
//--------------------------------------------------------------------------------
QString ClDeviceInfo::printConfigDescriptor(const libusb_config_descriptor *pConfig)
{
	QString strResult;

	strResult += "-------------show Config Descriptor--------------\n";
	strResult += QString("Descriptor Type : ") + getDescriptorType(pConfig->bDescriptorType);
	strResult += QString().sprintf("wTotalLength : %d\n",pConfig->wTotalLength);
	strResult += QString().sprintf("bConfigurationValue : %d\n",pConfig->bConfigurationValue);
	strResult += QString().sprintf("iConfiguration : %d\n",pConfig->iConfiguration);
	strResult += QString("bmAttributes : ") + getDescriptorAttribute(pConfig->bmAttributes);
	strResult += QString().sprintf("MaxPower : %d\n",pConfig->MaxPower);
	strResult += QString().sprintf("bNumInterfaces : %d\n",pConfig->bNumInterfaces);

	for(qint32 i = 0; i < pConfig->bNumInterfaces; i++ ){
		strResult += printInterface(&pConfig->interface[i]);
	}

	if(pConfig->extra != NULL){
		strResult += QString().sprintf("Extra descriptors : %s\n",pConfig->extra);
		strResult += QString().sprintf("extra_length : %d\n",pConfig->extra_length);
	}
	return strResult;
}

//--------------------------------------------------------------------------------
/*!
	@fn			printDeviceDescriptor
	@brief
	@return		ref
 */
//--------------------------------------------------------------------------------
QString ClDeviceInfo::printDeviceDescriptor(const libusb_device_descriptor *pDescriptor)
{
	QString strResult;

	strResult += "-------------show Device Descriptor--------------\n";

	strResult += QString("Descriptor Type : ") + getDescriptorType(pDescriptor->bDescriptorType);

	strResult += QString("USB Type : ") + getUsbType(pDescriptor->bcdUSB);

	strResult += QString("CLASS Type : ") + getDeviceClass(pDescriptor->bDeviceClass);

	strResult += QString().sprintf("Device subclass : %d\n",pDescriptor->bDeviceSubClass);
	strResult += QString().sprintf("Device protocol : %d\n",pDescriptor->bDeviceProtocol);

	strResult += QString("Vendor name : ") + getVendorString(pDescriptor->idVendor);
	strResult += QString().sprintf("Product ID : 0x%04x\n",pDescriptor->idProduct);

	strResult += QString().sprintf("Release Number : 0x%04x\n",pDescriptor->bcdDevice);

	strResult += QString().sprintf("manufacturer : 0x%x\n",pDescriptor->iManufacturer);
	strResult += QString().sprintf("iProduct : 0x%x\n",pDescriptor->iProduct);
	strResult += QString().sprintf("iSerialNumber : 0x%x\n",pDescriptor->iSerialNumber);
	strResult += QString().sprintf("bNumConfigurations : 0x%x\n",pDescriptor->bNumConfigurations);

	return strResult;
}
//--------------------------------------------------------------------------------
/*!
	@fn			printStringDescriptor
	@brief
	@return		ref
 */
//--------------------------------------------------------------------------------
QString ClDeviceInfo::printStringDescriptor(libusb_device_handle *pHandle,const libusb_device_descriptor *pDescriptor)
{
	QString strResult;
	uchar ucBuffer[512];
	qint32 res = LIBUSB_SUCCESS;
	strResult += "-------------show String Descriptor--------------\n";

	res = libusb_get_string_descriptor_ascii(pHandle, pDescriptor->iSerialNumber,ucBuffer,sizeof(ucBuffer));
	if(res < LIBUSB_SUCCESS){
		return strResult + "libusb_get_device_descriptor failure\n";
	}
	strResult += QString(reinterpret_cast<char*>(ucBuffer));
	strResult += "\n";
	return strResult;
}

//--------------------------------------------------------------------------------
/*!
	@fn			getDescriptorType
	@brief
	@return		ref
 */
//--------------------------------------------------------------------------------
QString ClDeviceInfo::getDescriptorType(quint8 bDescType)
{
	QString strResult;

	switch(bDescType){
	case LIBUSB_DT_DEVICE:
		strResult = "DEVICE";
		break;
	case LIBUSB_DT_CONFIG:
		strResult = "CONFIG";
		break;
	case LIBUSB_DT_STRING:
		strResult = "STRING";
		break;
	case LIBUSB_DT_INTERFACE:
		strResult = "INTERFACE";
		break;
	case LIBUSB_DT_ENDPOINT:
		strResult = "ENDPOINT";
		break;
	case LIBUSB_DT_HID:
		strResult = "HID";
		break;
	case LIBUSB_DT_REPORT:
		strResult = "REPORT";
		break;
	case LIBUSB_DT_PHYSICAL:
		strResult = "PHYSICAL";
		break;
	case LIBUSB_DT_HUB:
		strResult = "HUB";
		break;
	default:
		strResult = "Unknown";
		break;
	}
	strResult += "\n";
	return strResult;
}
//--------------------------------------------------------------------------------
/*!
	@fn			getUsbType
	@brief
	@return		ref
 */
//--------------------------------------------------------------------------------
QString ClDeviceInfo::getUsbType(quint16 bcdUSB)
{
	QString strResult;

	switch(bcdUSB){
	case 0x110:
		strResult = "USB 1.1";
		break;
	case 0x200:
		strResult = "USB 2.0";
		break;
	case 0x300:
		strResult = "USB 3.0";
		break;
	default:
		strResult = "Unknown";
		break;
	}
	strResult += "\n";
	return strResult;
}

//--------------------------------------------------------------------------------
/*!
	@fn			getDeviceClass
	@brief
	@return		ref
 */
//--------------------------------------------------------------------------------
QString ClDeviceInfo::getVendorString(ushort usVendorId)
{
	QString strResult = "Unknown";

	for(qint32 i = 0; USBVendorIDs[i].usVendorID != 0x0; i++){
		if(USBVendorIDs[i].usVendorID == usVendorId){
			strResult = USBVendorIDs[i].szVendor;
			break;
		}
	}
	strResult += QString().sprintf("(0x%04x)\n",usVendorId);
	return strResult;
}
//--------------------------------------------------------------------------------
/*!
	@fn			getDeviceClass
	@brief
	@return		ref
 */
//--------------------------------------------------------------------------------
QString ClDeviceInfo::getDeviceClass(quint8 bDeviceClass)
{
	QString strResult;

	switch(bDeviceClass){
	case LIBUSB_CLASS_PER_INTERFACE:
		strResult = "PER_INTERFACE";
		break;
	case LIBUSB_CLASS_AUDIO:
		strResult = "AUDIO";
		break;
	case LIBUSB_CLASS_COMM:
		strResult = "COMM";
		break;
	case LIBUSB_CLASS_HID:
		strResult = "HID";
		break;
	case LIBUSB_CLASS_PRINTER:
		strResult = "PRINTER";
		break;
	case LIBUSB_CLASS_DATA:
		strResult = "DATA";
		break;
	case LIBUSB_CLASS_HUB:
		strResult = "HUB";
		break;
	case LIBUSB_CLASS_MASS_STORAGE:
		strResult = "MASS STORAGE";
		break;
	case LIBUSB_CLASS_PTP:
		strResult = "PTP";
		break;
	case LIBUSB_CLASS_APPLICATION:
		strResult = "APPLICATION";
		break;
	case LIBUSB_CLASS_VENDOR_SPEC:
		strResult = "VENDOR SPECIAL";
		break;
	case LIBUSB_CLASS_WIRELESS:
		strResult = "WIRELESS";
		break;
	default:
		strResult = "Unknown";
		break;
	}
	strResult += "\n";
	return strResult;
}
//--------------------------------------------------------------------------------
/*!
	@fn			getEndpointAddress
	@brief
	@return		ref
 */
//--------------------------------------------------------------------------------
QString ClDeviceInfo::getEndpointAddress(quint8 address)
{
	QString strResult;
	unENDPADDRESS endAddr;

	endAddr.cData = address;
	switch(address & LIBUSB_ENDPOINT_DIR_MASK){
	case LIBUSB_ENDPOINT_IN:
		strResult += "ENDPOINT IN";
		break;
	case LIBUSB_ENDPOINT_OUT:
		strResult += "ENDPOINT OUT";
		break;
	default:
		strResult += "ENDPOINT DRECTION UNKNOWN";
		break;
	}
	strResult += "\n";
	strResult += QString().sprintf("Endpoint number : %d\n",endAddr.edpNumber);

	strResult += "\n";
	return strResult;
}
//--------------------------------------------------------------------------------
/*!
	@fn			getEndpointAttribute
	@brief
	@return		ref
 */
//--------------------------------------------------------------------------------
QString ClDeviceInfo::getEndpointAttribute(quint8 attribute)
{
	QString strResult;
	unENDPATTRIBUTE endAttr;

	endAttr.cData = attribute;

	quint8 transferType = endAttr.transferType;
	quint8 isoSyncType = endAttr.isoSyncType;
	quint8 isoUsageType = endAttr.isoUsageType;

	switch(transferType){
	case LIBUSB_TRANSFER_TYPE_CONTROL:
		strResult += "TRANSFER TYPE CONTROL";
		break;
	case LIBUSB_TRANSFER_TYPE_INTERRUPT:
		strResult += "TRANSFER TYPE INTERRUPT";
		break;
	case LIBUSB_TRANSFER_TYPE_BULK:
		strResult += "TRANSFER TYPE BULK";
		break;
	case LIBUSB_TRANSFER_TYPE_ISOCHRONOUS:
		strResult += "TRANSFER TYPE ISOCHRONOUS";
		break;
	default:
		strResult += "TRANSFER TYPE UNKNOWN";
		break;
	}
	strResult += "\n";

	switch(isoSyncType){
	case LIBUSB_ISO_SYNC_TYPE_NONE:
		strResult += "ISO SYNC TYPE NONE";
		break;
	case LIBUSB_ISO_SYNC_TYPE_ASYNC:
		strResult += "ISO SYNC TYPE ASYNC";
		break;
	case LIBUSB_ISO_SYNC_TYPE_ADAPTIVE:
		strResult += "ISO SYNC TYPE ADAPTIVE";
		break;
	case LIBUSB_ISO_SYNC_TYPE_SYNC:
		strResult += "ISO SYNC TYPE SYNC";
		break;
	default:
		strResult = "TRANSFER TYPE UNKNOWN";
		break;
	}
	strResult += "\n";

	switch(isoUsageType){
	case LIBUSB_ISO_USAGE_TYPE_DATA:
		strResult += "ISO USAGE TYPE DATA";
		break;
	case LIBUSB_ISO_USAGE_TYPE_FEEDBACK:
		strResult += "ISO USAGE TYPE FEEDBACK";
		break;
	case LIBUSB_ISO_USAGE_TYPE_IMPLICIT:
		strResult += "ISO USAGE TYPE IMPLICIT";
		break;
	default:
		strResult += "TRANSFER TYPE UNKNOWN";
		break;
	}

	strResult += "\n";
	return strResult;
}
//--------------------------------------------------------------------------------
/*!
	@fn			getDescriptorAttribute
	@brief
	@return		ref
 */
//--------------------------------------------------------------------------------
QString ClDeviceInfo::getDescriptorAttribute(quint8 attribute)
{
	QString strResult;
	unDESCATTRIBUTE attr;

	attr.cData = attribute;

	if(attr.remotewkup == 1){
		strResult += "remote wake up\n";
	}
	if(attr.selfpower == 1){
		strResult += "Self power supply\n";
	}
	if(strResult.isEmpty() == true){
		strResult = "\n";
	}
	return strResult;
}
//--------------------------------------------------------------------------------
/*!
	@fn			readData
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint64	ClDeviceInfo::readData ( char * data, qint64 maxSize )
{
	qint32 lReqLength = 0;
	qint32 lRet = 0;
	if(isOpen() == false){
		return -1;
	}
	if(maxSize > INT_MAX){
		DBG("got error from writeData");
		return -1;
	}
	lReqLength = static_cast<qint32>(maxSize);

	ClUsbCmd clUsb(data,lReqLength);							//

	if(clUsb.getCode() == EL_GET_JPEG_DATA){					//if code is Jpeg data

		QVariant qParam(lReqLength);
		lRet = resPicture(m_pInDataEndp, qParam);
		if(lRet > 0 && qParam.type() == QVariant::List){
			QVariantList qVarList = qParam.toList();
			if(qVarList.at(0).type() == QVariant::Size){
				QSize qSize = qVarList.at(0).toSize();
				clUsb.setParam2(qSize.width());
				clUsb.setParam3(qSize.height());
			}
			if(qVarList.at(1).type() == QVariant::ByteArray){
				QByteArray *pqArray = reinterpret_cast<QByteArray*>(clUsb.getParam1());
				*pqArray = qVarList.at(1).toByteArray();
			}
		}
		if(lRet >= 0){										//
			QByteArray qCmdArray;
			QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
			qWork.setByteOrder(QDataStream::LittleEndian);	//
			qWork << clUsb.getPagetType()						//set PacketType
				<< clUsb.getTransId()							//set Trans ID
				<< clUsb.getDataSize()							//set DataSize
				<< clUsb.getCode()								//set code
				<< qint16(0x00)									//set dummy (Reserved)
				<< clUsb.getParam1()							//set Param1
				<< clUsb.getParam2()							//set Param2
				<< clUsb.getParam3()							//set Param3
				<< clUsb.getParam4()							//set Param4
				<< clUsb.getParam5();							//set Param5
			memcpy(data, qCmdArray.data(), qCmdArray.length());
		}
	}else{
		QVariant qParam(lReqLength);
		lRet = resCommand(m_pInCmdEndp, qParam);
		if(lRet > 0){
			qint64 llWork = lRet;
			if(maxSize < lRet){
				llWork = maxSize;
			}
			if(qParam.type() == QVariant::ByteArray){
				QByteArray qCmdArray = qParam.toByteArray();
				memcpy(data, qCmdArray.data(), llWork);			//copy
#if 0  // for USB packet moniter in qDebug
				QByteArray qDbgArray(data,llWork);
				for(int i = 0; i < qDbgArray.length(); i=i+2){
					DBGFL("qDbgArray : %02x %02x",QChar(qDbgArray.at(i)).unicode(),QChar(qDbgArray.at(i+1)).unicode());
				}
#endif
			}
		}
	}
	if(lRet < 0){											//guess, Disconnected USB?
		qint32 a_lRet = 0;
		a_lRet = libusb_reset_device(m_pHandle);
		if(a_lRet != LIBUSB_SUCCESS){
			DBGFL( "got error from libusb_reset_device : %d",a_lRet);
		}
		lRet = -1;
	}
	return lRet;
}
//--------------------------------------------------------------------------------
/*!
	@fn			writeData
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint64	ClDeviceInfo::writeData ( const char * data, qint64 maxSize )
{
	qint32 lReqLength = 0;
	qint32 lRet = 0;

	if(isOpen() == false){
		return -1;
	}
	if(maxSize > INT_MAX){
		DBGFL("got error from writeData");
		return -1;
	}
	lReqLength = static_cast<qint32>(maxSize);
	QByteArray qCmdArray(data,lReqLength);
	QVariant qParam(qCmdArray);

	ClUsbCmd clUsb(data,lReqLength);							//
	if(clUsb.getCode() == EL_GET_JPEG_DATA){					//if code is Jpeg data
		lRet = reqPicture(m_pOutDataEndp, qParam);
	}else{
		lRet = reqCommand(m_pOutCmdEndp, qParam);
	}
	if(lRet < 0){
		resetDevice();
	}
	return lRet;
}
//--------------------------------------------------------------------------------
/*!
	@fn			resetDevice
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::resetDevice()
{
	qint32 a_lRet = 0;
	a_lRet = libusb_reset_device(m_pHandle);
	if(a_lRet != LIBUSB_SUCCESS){
		DBGFL("got error from libusb_reset_device : %d",a_lRet);
	}
	return a_lRet;
}

//--------------------------------------------------------------------------------
/*!
	@fn			setupEndpoint
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::setupEndpoint()
{
	return LIBUSB_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			reqPicture
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::reqPicture(const libusb_endpoint_descriptor *pOutEndp, QVariant &param)
{
	qint32 lRet = 0;

	Q_ASSERT(pOutEndp);
	lRet = reqCommand(pOutEndp, param);
	return lRet;
}
//--------------------------------------------------------------------------------
/*!
	@fn			resPicture
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::resPicture(const libusb_endpoint_descriptor *pInEndp, QVariant &qParam)
{
	qint32 lActual_length = 0;
	qint32 lRet = 0;
	qint32 lJpegLength = 0;
	char pcBuffer[JPEG_PICTURESIZE];									//Jpeg temporary buffer
	QByteArray qJpegData;
	QSize qSize;
	Q_ASSERT(pInEndp);

	lRet = resCommand(pInEndp, qParam);
	if(lRet < 0){
		return -1;
	}
	if(qParam.type() == QVariant::ByteArray){
		QByteArray qCmdArray = qParam.toByteArray();
		ClUsbCmd clUsb(qCmdArray.data(),qCmdArray.length());							//
		if(clUsb.getParam1() == 0){										//if the Param1 is zero, it may be an SD mode
			return 0;													//return for zero
		}
		qSize.setWidth(clUsb.getParam2());
		qSize.setHeight(clUsb.getParam3());

		qint32 siHeader = sizeof(ELMO_PACKET_HEADER);
		lJpegLength = clUsb.getParam1();
		qint32 lRestLength = lJpegLength;
		PELMO_PACKET_HEADER pstHeader;
		while(0 < lRestLength){
			DBGFL("RestLength : %d",lRestLength);
			qint32 lreqLength = JPEG_PICTURESIZE;
			qint32 lBlockSize = 0;
			qint32 lRes = 0;
			qint32 lActual_count = 0;
			do{
				lRes = libusb_bulk_transfer(m_pHandle,
											pInEndp->bEndpointAddress,
											(uchar *)pcBuffer,
											lreqLength,
											&lActual_length,
											m_iTimeout);
				if(lRes < 0){
					DBGFL("got error from libusb_bulk_transfer : %d",lRes);
					return lRes;
				}
				if(lActual_count++ > 5){
					DBGFL("lActual_length size is zero : %d",lActual_length);
					return LIBUSB_ERROR_BUSY;
				}
			}while(lActual_length == 0);
			pstHeader = reinterpret_cast<PELMO_PACKET_HEADER>(pcBuffer);
			lBlockSize = pstHeader->DataSize;
			if(pstHeader->PacketType != ELMO_PKT_DAT){
				DBGFL("got unjust PacketType : %d",pstHeader->PacketType);
				return IM_ERROR_OTHER;
			}
			if(lBlockSize <= 0){
				DBGFL("got unjust block size : %d",lBlockSize);
				return IM_ERROR_OTHER;
			}
			DBGFL("lBlockSize : %d",lBlockSize);

			lRestLength -= lBlockSize;
			lBlockSize -= (lActual_length - siHeader);
			qJpegData.append(pcBuffer + siHeader, lActual_length - siHeader);
			while(0 < lBlockSize){
				if(lBlockSize < JPEG_PICTURESIZE){
					lreqLength = lBlockSize;
				}
				qint32 lActual_count = 0;
				do{
					lRes = libusb_bulk_transfer(m_pHandle,
												pInEndp->bEndpointAddress,
												(uchar *)pcBuffer,
												lreqLength,
												&lActual_length,
												m_iTimeout);
					if(lRes < 0){
						DBGFL("got error from libusb_bulk_transfer : %d",lRes);
						return lRes;
					}
					if(lActual_count++ > 5){
						DBGFL("lActual_length size is zero : %d",lActual_length);
						return LIBUSB_ERROR_BUSY;
					}
				}while(lActual_length == 0);
				qJpegData.append(pcBuffer, lActual_length);
				lBlockSize -= lActual_length;
			}
		}
	}
	QVariantList qVarList;
	qVarList << qSize;
	qVarList << qJpegData;
	qParam.clear();
	qParam = qVarList;

	return lJpegLength;
}
//--------------------------------------------------------------------------------
/*!
	@fn			reqCommand
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::reqCommand(const libusb_endpoint_descriptor *pOutEndp, QVariant &param)
{
	qint32 lActual_length = 0;								//
	qint32 lRet = 0;										//

	Q_ASSERT(pOutEndp);
	if(param.type() != QVariant::ByteArray){				//
		return -1;
	}
	QByteArray qCmdArray = param.toByteArray();				//
#if 0  // for USB packet moniter in qDebug
	QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
	DBGFL("maxSize : %d",maxSize);
	for(int i = 0; i < qCmdArray.length(); i=i+2){
		DBGFL("data2 : %02x %02x",QChar(qCmdArray.at(i)).unicode(),QChar(qCmdArray.at(i+1)).unicode());
	}
	DBGFL(QChar(pOutEndp->bEndpointAddress).unicode());
#endif
	qint32 lActual_count = 0;
	do{
		lRet = libusb_bulk_transfer(m_pHandle,
									pOutEndp->bEndpointAddress,
									(uchar *)qCmdArray.data(),
									qCmdArray.length(),
									&lActual_length,
									m_iTimeout);
		if(lRet < 0){
			DBGFL("got error from libusb_bulk_transfer : %d",lRet);
			return lRet;
		}
		if(lActual_count++ > 5){
			DBGFL("lActual_length size is zero : %d",lActual_length);
			return LIBUSB_ERROR_BUSY;
		}
	}while(lActual_length == 0);

	if(
		(lActual_length > ELUSB_WT_USBPACKET_LENGTH)		&&
		(lActual_length % ELUSB_WT_USBPACKET_LENGTH == 0)	&&
		(lActual_length != ELMO_MAXPACKETSIZE)				)
	{
		lRet = libusb_bulk_transfer(m_pHandle,
									pOutEndp->bEndpointAddress,
									(uchar *)NULL,
									0,
									&lActual_length,
									m_iTimeout);
	}
	if(lRet < 0){
		DBGFL("got error from libusb_bulk_transfer : %d",lRet);
		// no error
	}

	return lActual_length;
}

//--------------------------------------------------------------------------------
/*!
	@fn			resCommand
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::resCommand(const libusb_endpoint_descriptor *pInEndp, QVariant &param)
{
	qint32 lActual_length = 0;
	qint32 lRet = 0;
	char pucRecvData[128];


	Q_ASSERT(pInEndp);
	if(param.type() != QVariant::Int){				//
		return -1;
	}
	qint32 lLength = param.toInt();					//
	qint32 lActual_count = 0;
	do{
		lRet = libusb_bulk_transfer(m_pHandle,
									pInEndp->bEndpointAddress,
									(uchar *)pucRecvData,
									lLength,
									&lActual_length,
									m_iTimeout);
		if(lRet < 0){
			DBGFL("got error from libusb_bulk_transfer : %d",lRet);
			return lRet;
		}
		if(lActual_count++ > 5){
			DBGFL("lActual_length size is zero : %d",lActual_length);
			return LIBUSB_ERROR_BUSY;
		}
	}while(lActual_length == 0);
	QByteArray qResArray(pucRecvData, lActual_length);
	param.clear();
	param.setValue<QByteArray>(qResArray);
	return lActual_length;
}
//--------------------------------------------------------------------------------
/*!
	@fn			analyJpegHeader
	@brief
	@return		bool
 */
//--------------------------------------------------------------------------------
bool ClDeviceInfo::analyJpegHeader(const QByteArray qJpgData, QSize &qSize)
{
	typedef union{
		struct{
			quint8	ucH;
			quint8	ucL;
		};
		quint16	usPoint;
	}uniPOINT;

	if(qJpgData.at(0) != CHECK_CHAR_FF || qJpgData.at(1) != CHECK_CHAR_D8){
		return false;
	}

	for(qint32 i = 0; i < qJpgData.length(); i++){
		if(qJpgData.at(i) == CHECK_CHAR_FF && qJpgData.at(i + 1) == CHECK_CHAR_C0){
			uniPOINT uniHeight;
			uniPOINT uniWidth;
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
			uniHeight.ucH = qJpgData.at(i + JPEG_OFFSET_Y + 1);
			uniHeight.ucL = qJpgData.at(i + JPEG_OFFSET_Y);
			uniWidth.ucH = qJpgData.at(i + JPEG_OFFSET_X + 1);
			uniWidth.ucL = qJpgData.at(i + JPEG_OFFSET_X);
#else
			uniHeight.ucH = qJpgData.at(i + JPEG_OFFSET_Y);
			uniHeight.ucL = qJpgData.at(i + JPEG_OFFSET_Y + 1);
			uniWidth.ucH = qJpgData.at(i + JPEG_OFFSET_X);
			uniWidth.ucL = qJpgData.at(i + JPEG_OFFSET_X + 1);
#endif
			qSize.setHeight(uniHeight.usPoint);
			qSize.setWidth(uniWidth.usPoint);
			return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------------------
/*!
	@fn			changeJpegHeader
	@brief
	@return		bool
 */
//--------------------------------------------------------------------------------
bool ClDeviceInfo::changeJpegHeader(QByteArray &qJpgData, QSize qSize)
{
	typedef union{
		struct{
			quint8	ucH;
			quint8	ucL;
		};
		quint16	usPoint;
	}uniPOINT;

	if(qJpgData.at(0) != CHECK_CHAR_FF || qJpgData.at(1) != CHECK_CHAR_D8){
		return false;
	}

	for(qint32 i = 0; i < qJpgData.length(); i++){
		if(qJpgData.at(i) == CHECK_CHAR_FF && qJpgData.at(i + 1) == CHECK_CHAR_C0){
			uniPOINT uniHeight;
			uniHeight.usPoint = quint16(qSize.height());
			uniPOINT uniWidth;
			uniWidth.usPoint = quint16(qSize.width());
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
			*(qJpgData.data() + i + JPEG_OFFSET_Y + 1) = uniHeight.ucH;
			*(qJpgData.data() + i + JPEG_OFFSET_Y) = uniHeight.ucL;
			*(qJpgData.data() + i + JPEG_OFFSET_X + 1) = uniWidth.ucH;
			*(qJpgData.data() + i + JPEG_OFFSET_X) = uniWidth.ucL;

#else
			qJpgData.at(i + JPEG_OFFSET_Y) = uniHeight.ucH;
			qJpgData.at(i + JPEG_OFFSET_Y + 1) = uniHeight.ucL;
			qJpgData.at(i + JPEG_OFFSET_X) = uniWidth.ucH;
			qJpgData.at(i + JPEG_OFFSET_X + 1) = uniWidth.ucL;
#endif
			return true;
		}
	}
	return false;
}
//--------------------------------------------------------------------------------
/*!
	@fn			reqPictureV2
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::reqPictureV2(const libusb_endpoint_descriptor *pOutEndp, QVariant &qParam)
{
	qint32 lRet = 0;													//コマンドの成功/失敗 判定用

	Q_ASSERT(pOutEndp);													//
	lRet = reqCommand(pOutEndp, qParam);									//コマンド要求

	return lRet;
}
//--------------------------------------------------------------------------------
/*!
	@fn			resPictureV2
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::resPictureV2(const libusb_endpoint_descriptor *pInEndp, QVariant &qParam)
{
	qint32 lRet = 0;													//コマンドの成功/失敗 判定用

	qint32 lActual_length = 0;											//受信データ長
	qint32 lJpegLength = 0;												//画像データ長
	char pcBuffer[ELMO_MAXPACKETSIZE_V2];								//画像データ 一時格納
	QByteArray qJpegData;												//画像データ 返信用
	QSize qSize(0,0);													//コマンド仕様上サイズは入らないため(0,0)を格納
	Q_ASSERT(pInEndp);													//

	lRet = resCommand(pInEndp, qParam);									//画像情報を受信
	if(lRet < 0){														//成功/失敗判定
		return -1;														//0未満はエラー値なので抜ける
	}
	//変数の宣言
	qint32 lRes = 0;
	qint32 lActual_count	= 0;										//読込回数
	qint32 lreqLength		= 0;										//読みいくサイズ
	qint32 lBlockSize		= 0;										//パケットのサイズ
	qint32 lRestLength		= 0;										//画像の残り読込サイズ
	qint32 siHeader			= sizeof(ELMO_PACKET_HEADER_V2);			//Header部分のサイズ
	PELMO_PACKET_HEADER_V2	pstHeader;									//ヘッダー抽出用
	PELMO_PACKET_V2			sReturn;
	PELMO_COMMAND_V2		pRetPacket;

	//SessionID等をセット
	if(qParam.type() == QVariant::ByteArray)
	{
		//取得した情報を送信時の形に直す
		sReturn		= (PELMO_PACKET_V2)qParam.toByteArray().constData();//
		pRetPacket	= (PELMO_COMMAND_V2)sReturn->Data;					//
		m_lSessionID	= pRetPacket->Param0;							//SessionID 格納
		lJpegLength		= pRetPacket->Param1;							//Jpgの画像サイズ
		lRestLength		= lJpegLength;									//Jpgの画像サイズ

		//1枚分になるまで要求＋取得を繰り返す
		while(0 < lRestLength){

			//要求 -----------------------------------------------------
			ClPictCmdV2 clPict(m_iPictureRect);							//this variable is for send
			clPict.setSessionBit();										//SessinBit set
			clPict.clrFinishBit();										//Finish Bit clear
			clPict.setParam0(m_lSessionID);								//set sessin id
			clPict.setParam1(0);										//set param 1
			clPict.setParam2(0);										//set param 2
			clPict.setParam3(0);										//set param 3
			clPict.setParam4(0);										//set param 4
			clPict.setParam5(0);										//set param 5
			QByteArray qArrayReq;
			QDataStream qWork(&qArrayReq,QIODevice::ReadWrite);
			qWork.setByteOrder(QDataStream::LittleEndian);	//
			qWork
				<< clPict.getHeaderVer()								//set PacketType
				<< clPict.getTransId()									//set Trans ID
				<< clPict.getCommand()									//set code
				<< clPict.getDataSize()									//set DataSize
				<< clPict.getParam0()									//set Param0
				<< clPict.getParam1()									//set Param1
				<< clPict.getParam2()									//set Param2
				<< clPict.getParam3()									//set Param3
				<< clPict.getParam4()									//set Param4
				<< clPict.getParam5();									//set Param5
			QVariant qParamReq(qArrayReq);
			lRet = reqCommand(m_pOutDataEndp, qParamReq);				//画像要求
			if(lRet < 0){
				return -1;
			}

			//取得 -----------------------------------------------------
			DBGFL("RestLength : %d",lRestLength);
			//サイズを計算
			if(lRestLength > ELMO_MAXPACKETSIZE_V2 - siHeader - ELMO_CMD_SESSION_SIZE)	//画像サイズが読込MAXより大きい場合
			{
				lreqLength	= ELMO_MAXPACKETSIZE_V2;
			}
			else
			{
				lreqLength	= lRestLength + siHeader + ELMO_CMD_SESSION_SIZE;
			}
			//Headerを含む1回目のデータを読み込み
			do{
				lRes = libusb_bulk_transfer(m_pHandle,
											pInEndp->bEndpointAddress,
											(uchar *)pcBuffer,
											lreqLength,
											&lActual_length,
											m_iTimeout);
				if(lRes < 0){
					DBGFL("got error from libusb_bulk_transfer : %d",lRes);
					return lRes;
				}
				if(lActual_count++ > 5){
					DBGFL("lActual_length size is zero : %d",lActual_length);
					return IM_ERROR_DEVICE_BUSY;
				}
			}while(lActual_length == 0);
			//パケットのData部分のサイズを取得
			pstHeader = reinterpret_cast<PELMO_PACKET_HEADER_V2>(pcBuffer);
			lBlockSize = pstHeader->DataSize;
			//データサイズが0なら抜ける
			if(lBlockSize <= 0){
				DBGFL("got unjust block size : %d",lBlockSize);
				return IM_ERROR_OTHER;
			}
			DBGFL("lBlockSize : %d",lBlockSize);
			//受信したOpCodeが0でないければ失敗なので抜ける
			if(pstHeader->OpCode != IM_ERROR_SUCCESS){
				DBGFL("got OpCode : %d",pstHeader->OpCode);
				return IM_ERROR_OTHER;
			}

			//画像サイズから1回目に受信したデータ内の画像サイズを引き、残りの画像サイズを計算
			lRestLength -= (lBlockSize - ELMO_CMD_SESSION_SIZE);
			//パケット内の残りの画像サイズを取得
			lBlockSize -= (lActual_length - siHeader);
			//返信用変数へコピー
			qJpegData.append(pcBuffer + siHeader + ELMO_CMD_SESSION_SIZE, lActual_length - siHeader - ELMO_CMD_SESSION_SIZE);
			//残りのパケットをサイズになるまで読み、返信用変数へ格納
			while(0 < lBlockSize){
				if(lBlockSize > ELMO_MAXPACKETSIZE_V2)					//画像サイズが読込MAXより大きい場合
				{
					lreqLength	= ELMO_MAXPACKETSIZE_V2;
				}
				else
				{
					lreqLength	= lBlockSize;
				}
				lActual_count = 0;
				do{
					lRes = libusb_bulk_transfer(m_pHandle,
												pInEndp->bEndpointAddress,
												(uchar *)pcBuffer,
												lreqLength,
												&lActual_length,
												m_iTimeout);
					if(lRes < 0){
						DBGFL("got error from libusb_bulk_transfer : %d",lRes);
						return lRes;
					}
					if(lActual_count++ > 5){
						DBGFL("lActual_length size is zero : %d",lActual_length);
						return IM_ERROR_DEVICE_BUSY;
					}
				}while(lActual_length == 0);
				qJpegData.append(pcBuffer, lActual_length);
				lBlockSize -= lActual_length;
			}
			//Finish Bitがたった場合にループを抜ける
			clPict.setPacketType(pstHeader->HeaderVer);					//Finish Bitでの判定に使用するためここで格納
			if(clPict.getFinishBit() == 1)
			{
				break;
			}
			//----------------------------------------------------------
		}
	}
	QVariantList qVarList;
	qVarList << qSize;
	qVarList << qJpegData;
	qParam.clear();
	qParam = qVarList;

	return lJpegLength;
}
//--------------------------------------------------------------------------------
/*!
	@fn			reqStreamV2
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::reqStreamV2(const libusb_endpoint_descriptor *pOutEndp, QVariant &qParam)
{
	qint32 lRet = 0;

	Q_ASSERT(pOutEndp);
	lRet = reqCommand(pOutEndp, qParam);
	return lRet;
}
//--------------------------------------------------------------------------------
/*!
	@fn			resStreamV2
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::resStreamV2(const libusb_endpoint_descriptor *pInEndp, QVariant &qParam)
{
	//ストリームは64 byteづつ来るためELMO_MAXPACKETSIZE_V2ではなく、ELMO_MAXPACKETSIZEを使用
	qint32 lActual_length = 0;
	qint32 lJpegLength = 0;
	char pcBuffer[ELMO_MAXPACKETSIZE];
	QByteArray qJpegData;
	QSize qSize(0,0);													//コマンド仕様上サイズは入らないため(0,0)を格納
	Q_ASSERT(pInEndp);

	//使用変数の宣言
	qint32 lRes				= 0;
	qint32 iReadMax			= ELMO_MAXPACKETSIZE;
	qint32 lreqLength		= iReadMax;
	qint32 lBlockSize		= 0;
	qint32 lActual_count	= 0;
	PELMO_PACKET_HEADER_V2 pstHeader;

	//Header込みのデータ受信 -------------------------------------------
	do{
		lRes = libusb_bulk_transfer(m_pHandle,
									pInEndp->bEndpointAddress,
									(uchar *)pcBuffer,
									lreqLength,
									&lActual_length,
									m_iTimeout);
		if(lRes < 0){
			DBGFL("got error from libusb_bulk_transfer : %d",lRes);
			return lRes;
		}
		if(lActual_count++ > 5){
			DBGFL("lActual_length size is zero : %d",lActual_length);
			return IM_ERROR_DEVICE_BUSY;
		}
	}while(lActual_length == 0);
	pstHeader = reinterpret_cast<PELMO_PACKET_HEADER_V2>(pcBuffer);
	lBlockSize = pstHeader->DataSize;
	if(lBlockSize <= 0){
		DBGFL("got unjust block size : %d",lBlockSize);
		return IM_ERROR_OTHER;
	}
	DBGFL("lBlockSize : %d",lBlockSize);
	if(pstHeader->OpCode != IM_ERROR_SUCCESS){
		DBGFL("got OpCode : %d",pstHeader->OpCode);
		return IM_ERROR_OTHER;
	}

	//ストリーム情報格納 -------------------------------------------------------
	//コマンドV2の形に変換
	PELMO_PACKET_V2		sReturn  = reinterpret_cast<PELMO_PACKET_V2>(pcBuffer);
	PELMO_COMMAND_V2	sCommand = reinterpret_cast<PELMO_COMMAND_V2>(sReturn->Data);
	m_lSessionID	= sCommand->Param0;									//SessionID 格納
	m_lFileType		= sCommand->Param1;									//ファイルタイプ（0:jpg,1:H.264,2:sound)
	m_lTimeStamp	= sCommand->Param2;									//タイムスタンプ
	qint32 siHeader = sizeof(ELMO_PACKET_HEADER_V2);					//Header部分のサイズ
	qint32 lOtherStreamSize = ELMO_STREAM_OTHER_IMG + siHeader;			//Header込みデータの中の画像データ以外のサイズ
	lJpegLength		= lBlockSize - ELMO_STREAM_OTHER_IMG;				//1回目受信データサイズからストリーム情報（SessionIDなど）を抜いたものが画像サイズ
	qSize.setWidth(m_lFileType);										//ファイルタイプ格納

	//ストリーム終了コマンドの返信かを確認 -------------------------------------
	ClStartStreamCmdV2 UsbStream(pcBuffer,lActual_length);
#if 0
	if(UsbStream.getFinishBit() == 1)									//FinishBitが立っている場合
#else
	if(lActual_length == 32)
#endif
	{
		QDataStream qWorkStream(&qJpegData, QIODevice::WriteOnly);
		qWorkStream << UsbStream.getHeaderVer()
					<< UsbStream.getTransId()
					<< UsbStream.getCommand()
					<< UsbStream.getDataSize()
					<< UsbStream.getParam0()
					<< UsbStream.getParam1()
					<< UsbStream.getParam2()
					<< UsbStream.getParam3()
					<< UsbStream.getParam4()
					<< UsbStream.getParam5();
		lJpegLength = lBlockSize + siHeader;
		lBlockSize = 0;
#if defined(FLG_DEBUG_TRACE)
		PELMO_PACKET_V2  sReturn	= (PELMO_PACKET_V2)pcBuffer;
		PELMO_COMMAND_V2 pRetPacket	= (PELMO_COMMAND_V2)sReturn->Data;
		qDebug() << "responce stream";
		qDebug() << "getHeaderVer\t"	<< sReturn->Header.DataSize;
		qDebug() << "getTransId\t"		<< sReturn->Header.TransID;
		qDebug() << "getCommand\t"		<< sReturn->Header.OpCode;
		qDebug() << "getDataSize\t"		<< sReturn->Header.DataSize;
		qDebug() << "getParam0\t"		<< pRetPacket->Param0;
		qDebug() << "getParam1\t"		<< pRetPacket->Param1;
		qDebug() << "getParam2\t"		<< pRetPacket->Param2;
		qDebug() << "getParam3\t"		<< pRetPacket->Param3;
		qDebug() << "getParam4\t"		<< pRetPacket->Param4;
		qDebug() << "getParam5\t"		<< pRetPacket->Param5;
		qDebug() << "m_lSessionID\t"	<< m_lSessionID;
#endif
	}
	else																//通常のストリーム受信の場合
	{
		lBlockSize -= (lActual_length - siHeader);
		qJpegData.append(pcBuffer + lOtherStreamSize,
						 lActual_length - lOtherStreamSize );
	}

	//パケットサイズになるまで取得を繰り返す -----------------------------------
	//  ストリームは画像1枚分を1パケットで送信する
	while(0 < lBlockSize){
		//読込に行くサイズを設定
		if(lBlockSize > ELMO_MAXPACKETSIZE)								//画像サイズが読込MAXより大きい場合
		{
			lreqLength	= ELMO_MAXPACKETSIZE;
		}
		else
		{
			lreqLength	= lBlockSize;
		}
		lActual_count = 0;
		//受信
		do{
			lRes = libusb_bulk_transfer(m_pHandle,
										pInEndp->bEndpointAddress,
										(uchar *)pcBuffer,
										lreqLength,
										&lActual_length,
										m_iTimeout);
			if(lRes < 0){
				DBGFL("got error from libusb_bulk_transfer : %d",lRes);
				return lRes;
			}
			if(lActual_count++ > 5){
				DBGFL("lActual_length size is zero : %d",lActual_length);
				return IM_ERROR_DEVICE_BUSY;
			}
		}while(lActual_length == 0);
		//返信用変数へ追記
		qJpegData.append(pcBuffer, lActual_length);
		//残りパケットサイズから今読んだサイズを引く
		lBlockSize -= lActual_length;
	}
	QVariantList qVarList;
	qVarList << qSize;
	qVarList << qJpegData;
	qParam.clear();
	qParam = qVarList;

	return lJpegLength;
}
//--------------------------------------------------------------------------------
/*!
	@fn			CommandPresenter
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::CommandPresenter(qint32 /*clCmd*/, QVariant& /*qParameter*/)
{
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			getPicture
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::getPicture(QVariant &/*qParam*/)		//画像取得関数
{
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			execCommand
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::execCommand(ClUsbCmd &clCmd)
{
	QDataStream qCmdStream(this);						//create data stream for getPicture

	ClUsbCmd clRecive;									//this variable is for reception

	qCmdStream << clCmd;								//send command to device

	qCmdStream >> clRecive;								//receeive command from device

	if(qCmdStream.status() != QDataStream::Ok){			//if confirm whether QDataStream status is OK
		return IM_ERROR_COMMAND_FAILURE;
	}

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			getModelName
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::getModelName(QVariant &/*qParam*/)
{
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			getVersion
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::getVersion(QVariant &/*qParam*/)
{
	return IM_ERROR_SUCCESS;								//output version to QVariant
}
//--------------------------------------------------------------------------------
/*!
	@fn			execCaptureInit
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::execCaptureInit()						//I clear the capture flag
{
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			getCaptureStatus
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClDeviceInfo::getCaptureStatus(QVariant &/*qParam*/)		//acquire the capture status
{
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
