#define __USB_DEBUG

#include "ClUsbDriver/ClUsbDriver_global.h"
#include "Common/CmnDefine.h"
#include "ClUsbDriver/ClUsbDriver_linux.h"
#include "qtdxuttimer.h"
#include "linux/ClImDrvCtrl.h"
#include "linux/ClUevent.h"
#include "ClUsbDriver/Command_Presenter.h"
#include "ElmoUsbCmd.h"

//--------------------------------------------------------------------------------
/*!
	@fn			ClUsbDriver
	@brief		Constructor
 */
//--------------------------------------------------------------------------------
ClUsbDriver::ClUsbDriver(QWidget *parent = 0) : QWidget(parent)
{
	m_lPresenterId = PRODUCT_L1EX;				//Default productId
	m_bInitialized = false;						//set false for flagInitialize
	m_bZeroImage = false;
	m_pclUevent= NULL;
	m_lErrLevel = LIBUSB_ERR_LEVEL;				//set default for m_lErrLevel
	m_ctx = NULL;

	//register to ELMO production list
	m_clProductList << PRODUCT_P10S;
	m_clProductList << PRODUCT_L1N;
	m_clProductList << PRODUCT_FF1;
	m_clProductList << PRODUCT_P30S;
	m_clProductList << PRODUCT_P100;
	m_clProductList << PRODUCT_L1EX;
	m_clProductList << PRODUCT_P10;
	m_clProductList << PRODUCT_MC1EX;
	m_clProductList << PRODUCT_CO10;

	qRegisterMetaType<eKINDSTREAM>("eKINDSTREAM");
	qRegisterMetaType<eKINDRECTID>("eKINDRECTID");
}
//--------------------------------------------------------------------------------
/*!
	@fn			~ClUsbDriver
	@brief		Destructor
 */
//--------------------------------------------------------------------------------
ClUsbDriver::~ClUsbDriver()
{

	if(isInitialize() == true){
		closeDevice();										//release a device

		if(m_pclUevent != NULL){
			Q_ASSERT(m_pclUevent);
			bool bRes = false;
			emit sigUeventTerminate();
			bRes = m_pclUevent->wait(5000);						//wait end of thread at about 10 seconds
			if(bRes == false){
				DBGFL("quit Uevent thread failure");
			}
			delete m_pclUevent;									//release thread handle
			m_pclUevent = NULL;									//set NULL
		}
		libusb_exit(m_ctx);									//release a library
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn			initialize
	@brief
	@return
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::initialize()
{
	qint32 lRes = IM_ERROR_SUCCESS;				//set dafault value

	if(m_bInitialized == true){
		return IM_ERROR_SUCCESS;
	}
#ifdef Q_OS_LINUX
	m_pclUevent = new ClUevent(parent());			//create Uevent thread for pnp
	connect(m_pclUevent,SIGNAL(sigFatal()),this,SLOT(slotUeventFatal()));
	connect(m_pclUevent,SIGNAL(sigTerminate()),this,SLOT(slotUeventTerminated()));
	connect(m_pclUevent,SIGNAL(sigUevent(QString)),this,SLOT(slotUeventString(QString)));
	connect (this,SIGNAL(sigUeventTerminate()),m_pclUevent,SLOT(slotLoopTerminate()));
	m_pclUevent->start();
#endif

	lRes = libusb_init(&m_ctx);						//Initialize a library
	if(lRes != LIBUSB_SUCCESS){
		DBGFL("libusb_init error!");
		return lRes;
	}
	DBGFL( "m_lErrLevel : %d",m_lErrLevel);
	libusb_set_debug(m_ctx,m_lErrLevel);			//set for context and error level

	chkConnectedDeviceWithSysfs();					//is checking connect device
#ifdef Q_OS_LINUX
	chkConnectedDeviceWithSysfs4video();			//is checking connect video device
#endif
	m_bInitialized = true;							//succeeded by initialization
	return lRes;
}
//--------------------------------------------------------------------------------
/*!
	@fn			isInitialize
	@brief
	@return
 */
//--------------------------------------------------------------------------------
bool ClUsbDriver::isInitialize()
{
	return m_bInitialized;						//return flagInitialize
}
//--------------------------------------------------------------------------------
/*!
	@fn			openDevice
	@brief
	@return
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::openDevice(qint32 lProductId)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false){							//if does not initialization
		return IM_ERROR_NOT_INIT;							//return IM_ERROR_NOT_INIT
	}

	if(isOpened(lProductId) == true){						//if already opened
		return IM_ERROR_ALREADY_OPENED;						//return IM_ERROR_ALREADY_OPENED
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}

	if(pImCtrl->OpenDriver() == false){
		return IM_ERROR_NOT_OPEN;
	}

	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------------
/*!
	@fn			closeDevice
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::closeDevice(qint32 lProductId)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux

	if(isInitialize() == false || isOpened(lProductId) == false){		//if is not initialized or is not opened
		return;															//return
	}
	if(lProductId == 0){
		QMapIterator<qint32, QObject*> ite(m_ImDrvCtrlList);		//create iterator in look for ELMO productions
		while(ite.hasNext()){
			ite.next();
			pImCtrl = dynamic_cast<ClImDrvCtrl*>(ite.value());
			if(pImCtrl != NULL){
				stopStream(pImCtrl->getProductId(), JPEG_STREAM);		//stop stream
				stopStream(pImCtrl->getProductId(), RAW_STREAM);		//stop stream

				pImCtrl->CloseDriver();								//Call for CloseDriver
			}
		}
	}else{
		stopStream(lProductId, JPEG_STREAM);						//stop stream
		stopStream(lProductId, RAW_STREAM);							//stop stream

		pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
		if(pImCtrl == NULL){									//if pImCtrl is NULL
			return;												//return
		}

		pImCtrl->CloseDriver();									//Call for CloseDriver
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn			startStream
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::startStream(qint32 lProductId, eKINDSTREAM iKind, qint32 iFrate)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false){							// is not initialized
		return IM_ERROR_NOT_INIT;							// return IM_ERROR_NOT_INIT
	}
	if(isOpened(lProductId) == false){						// is not opened
		if(openDevice(lProductId) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	if(iFrate <= 0 || iFrate > 30){							//fool proof
		iFrate = 30;
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}
	return pImCtrl->startStream(this,iKind,iFrate);
}
//--------------------------------------------------------------------------------
/*!
	@fn			stopStream
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::stopStream(qint32 lProductId, eKINDSTREAM iKind)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false){							// is not initialized
		return IM_ERROR_NOT_INIT;							// return IM_ERROR_NOT_INIT
	}
	if(isOpened(lProductId) == false){						// is not opened
		return IM_ERROR_NOT_OPEN;							// return IM_ERROR_NOT_OPEN
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}

	return pImCtrl->stopStream(iKind);
}
//--------------------------------------------------------------------------------
/*!
	@fn			isOpened
	@brief
	@return		bool
 */
//--------------------------------------------------------------------------------
bool ClUsbDriver::isOpened(qint32 lProductId)
{
	bool bRes = false;
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false){							// is not initialized
		return false;										// return false
	}
	if(lProductId == 0){
		QMapIterator<qint32, QObject*> ite(m_ImDrvCtrlList);		//create iterator in look for ELMO productions
		while(ite.hasNext()){
			ite.next();
			pImCtrl = dynamic_cast<ClImDrvCtrl*>(ite.value());
			if(pImCtrl != NULL){
				bRes = pImCtrl->IsOpenDriver();						//Call for CloseDriver
			}
		}
	}else{
		pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
		if(pImCtrl == NULL){									//if pImCtrl is NULL
			return false;										//return false
		}
		bRes =  pImCtrl->IsOpenDriver();						//return whether a device opened
	}
	return bRes;
}
//--------------------------------------------------------------------------------
/*!
	@fn			isStreaming
	@brief
	@return		bool
 */
//--------------------------------------------------------------------------------
bool ClUsbDriver::isStreaming(qint32 lProductId, eKINDSTREAM iKind)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isInitialize() == false){							// is not initialized
		return false;										// return false
	}
	if(isOpened(lProductId) == false){						// is not opened
		if(openDevice(lProductId) != IM_ERROR_SUCCESS){
			return false;									// return IM_ERROR_NOT_OPEN
		}
	}
	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return false;										//return false
	}
	return pImCtrl->isStreaming(iKind);
}
//--------------------------------------------------------------------------------
/*!
	@fn			getPictureRect
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::getPictureRect(qint32 lProductId, QSize &qSize)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	qint32 lHeight = 0;
	qint32 lWidth = 0;

	if(isInitialize() == false)
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}
	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_HEIGHT_PICTURE, qParam);
	if(lRet == IM_ERROR_SUCCESS)
	{
		if(qParam.type() == QVariant::Int){
			lHeight = qParam.toInt();
		}else{
			return IM_ERROR_OTHER;
		}
	}
	lRet = pImCtrl->CommandPresenter(GET_WIDTH_PICTURE, qParam);
	if(lRet == IM_ERROR_SUCCESS)
	{
		if(qParam.type() == QVariant::Int){
			lWidth = qParam.toInt();
		}else{
			return IM_ERROR_OTHER;
		}
		qSize.setHeight(lHeight);
		qSize.setWidth(lWidth);
	}
	return lRet;
}
//--------------------------------------------------------------------------------
/*!
	@fn			getRectId
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::getRectId(qint32 lProductId, eKINDRECTID &lRectId)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;

	if(isInitialize() == false)
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}
	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_RECT_PICTURE, qParam);

	if(lRet == IM_ERROR_SUCCESS)
	{
		if(qParam.type() == QVariant::UserType){
			lRectId =  qParam.value<eKINDRECTID>();
		}else{
			lRet = IM_ERROR_OTHER;
		}
	}
	return lRet;
}
//--------------------------------------------------------------------------------
/*!
	@fn			getPictureSize
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::getPictureSize(qint32 lProductId, qint32 &lSizeId)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}
	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_SIZE_PICTURE, qParam);
	if(lRet == IM_ERROR_SUCCESS)
	{
		if(qParam.type() == QVariant::Int){
			lSizeId = qParam.toInt();
		}else{
			lRet = IM_ERROR_OTHER;
		}
	}

	return lRet;
}
//--------------------------------------------------------------------------------
/*!
	@fn			setRectId
	@brief		set sizeID of picture
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::setRectId(qint32 lProductId, eKINDRECTID lRectId)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}
	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}
	QVariant qParam;
	qParam.setValue<eKINDRECTID>(lRectId);
	lRet = pImCtrl->CommandPresenter(SET_RECT_PICTURE, qParam);

	return lRet;
}
//--------------------------------------------------------------------------------
/*!
	@fn			execCameraControl(qint32, qint32)
	@brief		exec camera control command
	@return		result of exec
	@param		lProductId	:	productID
				command		:	command
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::execCameraControl(qint32 lProductId, qint32 command)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)								// is not initialized
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}
	QVariant param;
	lRet = pImCtrl->CommandPresenter(command, param);
	return lRet;
}
//--------------------------------------------------------------------------------
/*!
	@fn			capture(qint32, eKINDSTREAM, QByteArray&)
	@brief		capture JPEG image
	@return		result
	@param		lProductId	:	productID
				data		:	reference of store capture data
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::captureJpeg(qint32 lProductId, QByteArray &qOutputJpgData, eKINDRECTID lRectId)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	qint32 lRes = IM_ERROR_SUCCESS;
	QVariant qvParam;

	if(isInitialize() == false)								// is not initialized
	{
		return IM_ERROR_NOT_INIT;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId) != IM_ERROR_SUCCESS){
			return IM_ERROR_NOT_OPEN;						// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){									//if pImCtrl is NULL
		return IM_ERROR_NOT_OPEN;							//return IM_ERROR_NOT_OPEN
	}
	if(lRectId == RECTID_PICTURE_DEFAULT){
		qint32 lRectRet = pImCtrl->CommandPresenter(GET_RECT_PICTURE, qvParam);
		if(lRectRet == IM_ERROR_SUCCESS){
			if(qvParam.type() != QVariant::UserType){
				qvParam.setValue<eKINDRECTID>(RECTID_PICTURE_DEFAULT);
			}
		}else{
			qvParam.setValue<eKINDRECTID>(RECTID_PICTURE_DEFAULT);
		}
	}else{
		qvParam = lRectId;
	}
	lRes = pImCtrl->CommandPresenter(GET_PICTURE,qvParam);

	if(lRes == IM_ERROR_SUCCESS)
	{
		if(qvParam.type() == QVariant::ByteArray){
			qOutputJpgData = qvParam.toByteArray();
		}
	}

	return lRes;
}
//--------------------------------------------------------------------------------
/*!
	@fn			capture(qint32, eKINDSTREAM, QImage&)
	@brief		capture Rawdata image
	@return		qint32
	@param		lProductId	:	productID
	@param		qOutputRawData		:	reference of store capture data
 */
//--------------------------------------------------------------------------------
qint32 ClUsbDriver::captureRaw(qint32 lProductId, QImage &qOutputRawData, eKINDRECTID lRectId)
{
	qint32 lRes = IM_ERROR_SUCCESS;

	QByteArray qJpgData;
	lRes = captureJpeg(lProductId, qJpgData, lRectId);
	if(lRes == IM_ERROR_SUCCESS)
	{
		if(qOutputRawData.loadFromData(qJpgData, "jpg") == false)
		{
			lRes = IM_ERROR_DECORD;
		}
	}
	return lRes;
}
//--------------------------------------------------------------------------------
/*!
	@fn			slotStartZeroImage
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotStartZeroImage(qint32 lProductId, eKINDSTREAM iKind)
{
	if(m_bZeroImage == true){
		return;
	}
	m_bZeroImage = true;
	emit sigStartZeroImage(lProductId, iKind);
}
//--------------------------------------------------------------------------------
/*!
	@fn			slotEndZeroImage
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotEndZeroImage(qint32 lProductId, eKINDSTREAM iKind)
{
	if(m_bZeroImage == false){
		return;
	}
	m_bZeroImage = false;
	emit sigEndZeroImage(lProductId, iKind);
}
//--------------------------------------------------------------------------------
/*!
	@fn			slotUeventFatal
	@brief		This slot is able to receive fatal error from uevent thread
	@note		it will be already dead when receive this event from uevent thread
	@return		None
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotUeventFatal()
{
	emit sigErrorFatal(IM_ERROR_FATAL);
	closeDevice();
}
//--------------------------------------------------------------------------------
/*!
	@fn			slotUeventTerminated
	@brief		This slot is able to receive terminate event from uevent trhead
	@return		None
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotUeventTerminated()
{
	closeDevice();
}
//--------------------------------------------------------------------------------
/*!
	@fn			slotUeventString
	@brief		This slot is able to receive kernel message from Uevent thread
	@return		None
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotUeventString(QString mesg)
{
#ifdef Q_OS_LINUX

	QStringList parseList = mesg.split("@");				//split token

	DBG(mesg);

	QStringList strDevPathList = parseList.at(1).split("/");
	bool bIsV4l = false;
	if(strDevPathList.at(1) == "devices"){						//look for string "devices"
		for(qint32 i = 0; i < strDevPathList.count(); i++ ){
			if(strDevPathList.at(i) == "video4linux"){			//look for Usb camera
				bIsV4l = true;
			}
		}
		if(bIsV4l == true){										//if it was Usb Camera
			if(parseList.at(0) == "add"){								//if it is "add"
				chkAddVideoDevice(SYSFS_VIDEO_PATH);					//check whether an ELMO device was connected
			}else if(parseList.at(0) == "remove"){						//if it is "remove"
				chkRemoveVideoDevice(SYSFS_VIDEO_PATH);					//check whether an ELMO device was disconnected
			}
		}
		else
		{
			if(parseList.at(0) == "add"){								//if it is "add"
				chkAddConnectedDevice("/sys" + parseList.at(1));		//check whether an ELMO device was connected
			}else if(parseList.at(0) == "remove"){						//if it is "remove"
				chkRemoveConnectedDevice("/sys" + parseList.at(1));		//check whether an ELMO device was disconnected
			}
		}
	}
#else
	mesg = mesg;
#endif // Q_OS_LINUX
}

//--------------------------------------------------------------------------------
/*!
	@fn			chkAddConnectedDevice
	@brief		if receive message is "add" from kernel, check the inside and tell of device addition
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::chkAddConnectedDevice(QString strMesg)
{
//	QString strDescVendorPath = strMesg + "/idVendor";		//path of Vendor ID
//	QString strDescProductPath = strMesg + "/idProduct";	//path of Product ID
	QString strDescVendorPath;					//path of Vendor ID
	QString strDescProductPath;					//path of Product ID

	QString strVendorId;									//temporary place with vendor ID
	QString strProductId;									//temporary place with product ID
	qint32 lVendorId = 0;
	qint32 lProductId = 0;

	QDir qDir(strMesg);

	for(;;){

		qDebug() << qDir.path();
		if(true == QFile.exists(qDir.path() + "/idVendor")){
			break;
		}else{
			if(false == qDir.cdUp()){
				DBGFL("not found idVendor / idProduct %s", qDir.path().data());
				return;
			}
		}
	}

	strDescVendorPath = qDir.path() + "/idVendor";
	strDescProductPath = qDir.path() + "/idProduct";

	QFile qVendorFile(strDescVendorPath);					//get ready to check vendor ID
	if(qVendorFile.open(QFile::ReadOnly) == true){			//open the vendor file for read-only
		QTextStream qStream(&qVendorFile);					//get ready to read text file with stream
		strVendorId = qStream.readAll();					//all read from vendor file

		lVendorId = strtol(strVendorId.toAscii().data(),NULL,16);	//convert to hex vendor ID
	}else{
		DBGFL("open1 error %s", strDescVendorPath.toAscii().data());
		return;
	}
	QFile qProductFile(strDescProductPath);					//get ready to check product ID
	if(qProductFile.open(QFile::ReadOnly) == true){			//open the product file for read-only
		QTextStream qStream(&qProductFile);					//get ready to read text file with stream
		strProductId = qStream.readAll();					//all read from product ID
		lProductId = strtol(strProductId.toAscii().data(),NULL,16);		//convert to hex product ID
	}else{
		DBGFL("open2 error %s", strDescProductPath.toAscii().data());
		return;
	}

	if(lVendorId == VENDOR_ELMO){													//if vendor is ELMO
		QList<qint32>::iterator qProdIdIte;											//get ready to reference ELMO product list
		for(qProdIdIte = m_clProductList.begin(); qProdIdIte != m_clProductList.end(); ++qProdIdIte){
			qint32 lProductIdWork = GETPRODUCTID(*qProdIdIte);
			if(lProductIdWork == lProductId){											//if same as product ID in product list

				if(setDevPermission(strMesg) == true){								//add WriteOther permission to device file
					ClImDrvCtrl *pclWkCtrl = new ClImDrvCtrl(*qProdIdIte, m_ctx,this);	//create ClImDrvCtrl
					pclWkCtrl->setDriverPath(strMesg);								//save as driver path for myself
					pclWkCtrl->initialize();
					m_ImDrvCtrlList[*qProdIdIte] = pclWkCtrl;						//create Prsenter controller

					emit sigOpenDevice(*qProdIdIte);
				}else{
					DBGFL("could not handle device file");
					emit sigErrorFatal(IM_ERROR_NOT_PERMISSION);					//couldn't handle device-file with setPermission
				}
				break;
			}
		}
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn			setDevPermission
	@brief
 */
//--------------------------------------------------------------------------------
bool ClUsbDriver::setDevPermission(QString strMesg)
{
	bool bRes = true;
#ifndef __DEBUG
	if(getuid() == 0){											//if I am root
#endif
		QString strDescBusnumPath = strMesg + "/busnum";		//path of bus number
		QString strDescDevnumPath = strMesg + "/devnum";		//path of device number

		qint32 lBusnum = 0;										//device of bus number
		qint32 lDevnum = 0;										//device of dev number

		QFile qBusnumFile(strDescBusnumPath);					//get ready to check busnum
		if(qBusnumFile.open(QFile::ReadOnly) == true){			//open the vendor file for read-only
			QTextStream qStream(&qBusnumFile);					//get ready to read text file with stream
			QString strBusnum = qStream.readAll();				//all read from busnum file

			lBusnum = strtol(strBusnum.toAscii().data(),NULL,10);	//convert to dec bus number
		}else{
			return false;
		}

		QFile qDevnumFile(strDescDevnumPath);					//get ready to check devnum
		if(qDevnumFile.open(QFile::ReadOnly) == true){			//open the vendor file for read-only
			QTextStream qStream(&qDevnumFile);					//get ready to read text file with stream
			QString strDevnum = qStream.readAll();				//all read from devnum file

			lDevnum = strtol(strDevnum.toAscii().data(),NULL,10);	//convert to dec dev number
		}else{
			return false;
		}
		QString strDevPath = DEV_USB_PATH;

		strDevPath += QString().sprintf("/%03d",lBusnum);		//add busnum to path
		strDevPath += QString().sprintf("/%03d",lDevnum);		//add devnum to path
		QFile qDevFile(strDevPath);								//prepare to read a device-file
		QFile::Permissions per = qDevFile.permissions();			//get device file permission now
		if((per & QFile::WriteOther) != QFile::WriteOther){				//confirm whether flag of WriteOther was set
			bool  bPerChanged = false;
			if( 0 != geteuid() )
			  {
				seteuid( 0 );
				bPerChanged = true;
			  }
			bRes = qDevFile.setPermissions(per | QFile::WriteOther);	//add WriteOther permission to device file (must be root)
			qDebug() << strDevPath << "setPermissions : " << bRes;
			if( false != bPerChanged )
			  {
				seteuid( getuid() );
			  }
		}
#ifndef __DEBUG
	}else{																//I amn't root
		QProcess qProcess;												//
		QStringList qArguments;											//for Arguments
		QString filePath;												//for device path

		qArguments << MAGIC_CORD;										//1st argument is MAGIC_CORD
		qArguments << strMesg;											//2nd argument is device path

		filePath = qApp->applicationDirPath() + "/SubDevCtrl";			//assign a place of the SubDevCtrl to filePath

		qProcess.start(filePath,qArguments);							//start process
		if(qProcess.waitForFinished(PROCESS_TIMEOUT) == false){			//wait process until finish
			bRes = false;												//it was time-out or process was failure
		}else{
			qint32 iExitCord = qProcess.exitCode();						//assign to iExitCord finished process value
			if(iExitCord != 1){											//it isn't 1
				bRes = false;											//it is failure
			}
		}
	}
#endif
	return bRes;
}

//--------------------------------------------------------------------------------
/*!
	@fn			chkRemoveConnectedDevice
	@brief		if receive message is "remove" from kernel, check the inside and tell of device addition
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::chkRemoveConnectedDevice(QString strMesg)
{
	QMapIterator<qint32, QObject*> ite(m_ImDrvCtrlList);			//create iterator in look for ELMO productions
	while(ite.hasNext()){
		ite.next();
		ClImDrvCtrl *pIm = dynamic_cast<ClImDrvCtrl*>(ite.value());
		if(pIm != NULL){
			if(pIm->getDriverPath() == strMesg){					//found presenter
				closeDevice(pIm->getProductId());
				m_ImDrvCtrlList.remove(pIm->getProductId());		//remove presenter in product list
				emit sigCloseDevice(pIm->getProductId());
//				delete pIm;											//release ClImDrvCtrl
				break;
			}
		}
	}

}
//--------------------------------------------------------------------------------
/*!
	@fn			chkConnectedDeviceWithSysfs
	@brief
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::chkConnectedDeviceWithSysfs()
{
	QDirIterator qDirIte(SYSFS_USB_PATH,QDirIterator::FollowSymlinks);
	while(qDirIte.hasNext()){
		qDirIte.next();
		if(qDirIte.fileInfo().isSymLink() == true){
			chkAddConnectedDevice(qDirIte.fileInfo().symLinkTarget());
		}
	}
}
#ifdef Q_OS_LINUX
//--------------------------------------------------------------------------------
/*!
	@fn			chkConnectedDeviceWithSysfs4video
	@brief
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::chkConnectedDeviceWithSysfs4video()
{
	QDirIterator qDirIte(SYSFS_VIDEO_PATH,QDirIterator::FollowSymlinks);
	while(qDirIte.hasNext()){
		qDirIte.next();
		if(qDirIte.fileInfo().isSymLink() == true){
			chkAddVideoDevice(qDirIte.fileInfo().symLinkTarget());
		}
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn			chkAddVideoDevice
	@brief		if receive message is "add" from kernel, check the inside and tell of video device addition
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::chkAddVideoDevice(QString strMesg)
{
	qint32 lVendorId = 0;
	qint32 lProductId = 0;
	QString strVideoUeventPath = strMesg + "/device/uevent";

	QFile qUeventFile(strVideoUeventPath);					//get ready to check busnum
	if(qUeventFile.open(QFile::ReadOnly) == true){			//open the vendor file for read-only
		QStringList qParamSplit,qDataSplit;
		QString strUeventLine;
		QTextStream qStream(&qUeventFile);					//get ready to read text file with stream
		do{
			strUeventLine = qStream.readLine();		//Line read from busnum file
			qParamSplit = strUeventLine.split("=");			//split sentence with "="
			QString strLeftPert = qParamSplit.at(0);
			if(strLeftPert == "PRODUCT"){					//if left pert is "PRODUCT"   ex.PRODUCT=xxxx/xxxx/xxxx
				QString strRightPert =  qParamSplit.at(1);
				qDataSplit = strRightPert.split("/");		//So split right pert
				lVendorId = strtol(qDataSplit.at(0).toAscii().data(),NULL,16);
				lProductId = strtol(qDataSplit.at(1).toAscii().data(),NULL,16);
				lProductId = ATTACH_V2P(lVendorId,lProductId);
				bool bFound = false;
				QMapIterator<qint32, QObject*> ite(m_ImDrvCtrlList);									//create iterator in look for ELMO productions
				while(ite.hasNext()){
					ite.next();
					ClImDrvCtrl *pIm = dynamic_cast<ClImDrvCtrl*>(ite.value());
					if(pIm != NULL){
						if(pIm->getProductId() == lProductId){		//Was video registered?
							bFound = true;
							break;									//video was already registered
						}
					}
				}
				if(bFound == false){
					ClWebCamCtrl *pclWkCtrl = new ClWebCamCtrl(lProductId, m_ctx);
					pclWkCtrl->setDriverPath(strMesg);								//save as driver path for myself
					pclWkCtrl->initialize();
					m_ImDrvCtrlList[lProductId] = dynamic_cast<ClImDrvCtrl*>(pclWkCtrl);		//create Prsenter controller

					emit sigOpenDevice(lProductId);
				}
			}
		}while(strUeventLine.isNull() == false);
	}
}
#endif
//--------------------------------------------------------------------------------
/*!
	@fn			chkRemoveVideoDevice
	@brief		if receive message is "remove" from kernel, check the inside and tell of device addition
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::chkRemoveVideoDevice(QString strMesg)
{
	QMapIterator<qint32, QObject*> ite(m_ImDrvCtrlList);			//create iterator in look for ELMO productions
	while(ite.hasNext()){
		ite.next();
		ClImDrvCtrl *pIm = dynamic_cast<ClImDrvCtrl*>(ite.value());
		if(pIm != NULL){
			if(pIm->getDriverPath() == strMesg){					//found presenter
				closeDevice(pIm->getProductId());
				m_ImDrvCtrlList.remove(pIm->getProductId());		//remove presenter in product list
				emit sigCloseDevice(pIm->getProductId());
				delete pIm;											//release ClImDrvCtrl
				break;
			}
		}
	}

}
//--------------------------------------------------------------------------------
/*!
		@fn			chkConnectedDevice
		@brief		perse kernel message from uevent thread
		@return		none
	 */
//--------------------------------------------------------------------------------
void ClUsbDriver::chkConnectedDevice()
{
	libusb_device **a_pList;
	ssize_t iCnt = libusb_get_device_list(NULL, &a_pList);
	ssize_t iIdx = 0;
	qint32 err = 0;

	libusb_device_descriptor *a_pDescriptor = new libusb_device_descriptor();

	for (iIdx = 0; iIdx < iCnt; iIdx++) {
		libusb_device *a_pDevice = a_pList[iIdx];
		err = libusb_get_device_descriptor(a_pDevice,a_pDescriptor);			//get descriptor for this device
		if(err != LIBUSB_SUCCESS){												//
			break;
		}
		if(a_pDescriptor->idVendor == VENDOR_ELMO){								//if device vendor is ELMO
			if(chkElmoProduction(a_pDescriptor->idProduct) == true){			//check whether the ELMO production
				bool bFound = false;
				QMapIterator<qint32, QObject*> ite(m_ImDrvCtrlList);			//create iterator in look for ELMO productions
				while(ite.hasNext()){
					ite.next();
					ClImDrvCtrl *pIm = dynamic_cast<ClImDrvCtrl*>(ite.value());
					if(pIm != NULL){
						if(pIm->getProductId() == a_pDescriptor->idProduct){	//found presenter
							bFound = true;
							break;
						}
					}
				}
				if(bFound == false){											//not found it
					ClImDrvCtrl *pclWkCtrl = new ClImDrvCtrl(a_pDescriptor->idProduct, m_ctx);
					pclWkCtrl->initialize();
					m_ImDrvCtrlList[a_pDescriptor->idProduct] = pclWkCtrl;		//create Prsenter controller

					emit sigOpenDevice(a_pDescriptor->idProduct);
				}
			}
		}
	}
	delete a_pDescriptor;												//release descriptor
	libusb_free_device_list(a_pList, 1);								//release list
}
//--------------------------------------------------------------------------------
/*!
	@fn			chkElmoProduction
	@brief		if Product ID is ELMO Production then return true
	@return		bool
 */
//--------------------------------------------------------------------------------
bool ClUsbDriver::chkElmoProduction(qint32 lProductId)
{
	bool bRes = false;

	switch(lProductId){
	case PRODUCT_L1N:						//L-1n
		bRes = true;
		break;
	case PRODUCT_L1EX:						//L-1ex
		bRes = true;
		break;
	case PRODUCT_P10S:						//P10s
		bRes = true;
		break;
	case PRODUCT_P30S:						//P30s
		bRes = true;
		break;
	case PRODUCT_P100:						//P100
		bRes = true;
		break;
	case PRODUCT_P10:						//P10
		bRes = true;
		break;
	case PRODUCT_FF1:						//FF-1
		bRes = true;
		break;
	case PRODUCT_MC1EX:						//MC-1ex
		bRes = true;
		break;
	default:
		break;
	}
	return bRes;							//return true or false
}
//--------------------------------------------------------------------------------
/*!
	@fn			chkAllDeviceInfo
	@brief
	@return		QString
 */
//--------------------------------------------------------------------------------
QString ClUsbDriver::chkAllDeviceInfo()
{
	ClImDrvCtrl *workCtrl = new ClImDrvCtrl();
	workCtrl->setContext(m_ctx);
	workCtrl->initialize();
	QString strDeviceData = workCtrl->chkAllDeviceInfo();
	delete workCtrl;

	return strDeviceData;
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotStartStream(qint32, eKINDSTREAM)
	@brief	start capture stream
	@param	lProductId	:	productID
			iKind		:	image type
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotStartStream(qint32 lProductId, eKINDSTREAM iKind, qint32 lFrate)
{

	qint32 lRes = startStream(lProductId, iKind, lFrate);
	if(lRes != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRes);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotStopStream(qint32, eKINDSTREAM)
	@brief	stop capture stream
	@param	lProductId	:	productID
			iKind		:	image type
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotStopStream(qint32 lProductId, eKINDSTREAM iKind)
{
	if(isStreaming(lProductId, iKind) == true){
		qint32 lRes = stopStream(lProductId, iKind);
		if(lRes != IM_ERROR_SUCCESS){
			emit sigErrorNormal(lRes);
		}
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetFRate(qint32, qint32, eKINDSTREAM)
	@brief	set frame lRate
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
			iKind		:	image type
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetFRate(qint32 lProductId, eKINDSTREAM iKind, qint32 &lFrate)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isStreaming(lProductId, iKind) == true)
	{
		pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
		if(pImCtrl == NULL){								//if pImCtrl is NULL
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;											//return IM_ERROR_NOT_OPEN
		}
		lFrate = pImCtrl->getFrameRate(iKind);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotSetFRate(qint32, eKINDSTREAM, qint32)
	@brief	set frame lRate
	@param	lProductId	:	productID
			iKind		:	image type
			lFrate		:	frame lRate
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotSetFRate(qint32 lProductId, eKINDSTREAM iKind, qint32 lFrate)
{
	ClImDrvCtrl	*pImCtrl = NULL;							//Image Mate driver control for Linux
	if(isStreaming(lProductId, iKind) == true)
	{
		pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
		if(pImCtrl == NULL){								//if pImCtrl is NULL
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;											//return IM_ERROR_NOT_OPEN
		}
		pImCtrl->setFrameRate(lFrate, iKind);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotCapture
	@brief	image capture
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
			iKind		:	image type
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotCapture(qint32 lProductId, eKINDSTREAM iKind, qint32 &lStatus, QVariant &qParam, eKINDRECTID lRectId = RECTID_PICTURE_DEFAULT)
{
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;										// return IM_ERROR_NOT_OPEN
		}
	}

	qint32 lRes = IM_ERROR_SUCCESS;
	switch(iKind)
	{
	case JPEG_STREAM:
		{
			QByteArray qArray;
			lRes = captureJpeg(lProductId, qArray, lRectId);
			if(lRes == IM_ERROR_SUCCESS){
				qParam = qArray;
			}else if(lRes == IM_ERROR_CANNOT_OUTPUT_IMAGE){
				emit sigZeroCapture(lProductId);
			}else{
				emit sigErrorNormal(lRes);
			}
			lStatus = lRes;											//set lStatus of lRes
			break;
		}
	case RAW_STREAM:
		{
			QImage qImg;
			lRes = captureRaw(lProductId, qImg, lRectId);
			if(lRes == IM_ERROR_SUCCESS){
				qParam = qImg;
			}else if(lRes == IM_ERROR_CANNOT_OUTPUT_IMAGE){
				emit sigZeroCapture(lProductId);
			}else{
				emit sigErrorNormal(lRes);
			}
			lStatus = lRes;
			break;
		}
	default:
		lStatus = IM_ERROR_FATAL;
		emit sigErrorFatal(IM_ERROR_FATAL);
		return;
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetPresenterName(qint32, qint32)
	@brief	get presenter name
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetPresenterName(qint32 lProductId, QString &qName)
{
	ClImDrvCtrl	*pImCtrl = NULL;								//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;												// return IM_ERROR_NOT_OPEN
		}
	}
	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){										//if pImCtrl is NULL
		return;													//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_MODEL_NAME, qParam);	//

	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::String){
		qName = qParam.toString();
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetPresenterVersion(qint32, qint32)
	@brief	get presenter version
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetPresenterVersion(qint32 lProductId, QStringList &qVerlist)
{
	ClImDrvCtrl	*pImCtrl = NULL;									//Image Mate driver control for Linux
	qint32 lRet = IM_ERROR_SUCCESS;
	if(isInitialize() == false)
	{
		return;
	}
	if(isOpened(lProductId) == false)
	{
		if(openDevice(lProductId) != IM_ERROR_SUCCESS){
			emit sigErrorNormal(IM_ERROR_NOT_OPEN);
			return;													// return IM_ERROR_NOT_OPEN
		}
	}

	pImCtrl = dynamic_cast<ClImDrvCtrl*>(m_ImDrvCtrlList[lProductId]);	//get equal lProduct ID of lProductId from open devices list
	if(pImCtrl == NULL){											//if pImCtrl is NULL
		return;														//return
	}
	QVariant qParam;
	lRet = pImCtrl->CommandPresenter(GET_MODEL_VERSION, qParam);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}else if(qParam.type() == QVariant::StringList){
		qVerlist = qParam.toStringList();
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetPresenterList(qint32)
	@brief	get connect presenter list
	@param	ID			:	ID that is sent signal
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetPresenterList(QVariantList &qSendList)
{
	//create list because can't remove list when disconnect presenter

	QMapIterator<qint32, QObject*> qIterate(m_ImDrvCtrlList);
	while(qIterate.hasNext()){
		qIterate.next();
		ClImDrvCtrl *pIm = dynamic_cast<ClImDrvCtrl*>(qIterate.value());
		if(pIm != NULL){
			qSendList << pIm->getProductId();
		}
	}
}

//--------------------------------------------------------------------------------
/*!
	@fn		slotGetPictureRect(qint32, eKINDSTREAM)
	@brief	get size of image
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetPictureRect(qint32 lProductId, QSize &qSize)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = getPictureRect(lProductId, qSize);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetRectId
	@brief	get size of image
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetRectId(qint32 lProductId, eKINDRECTID &lRectId)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = getRectId(lProductId, lRectId);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotGetPictureSize(qint32, qint32)
	@brief	get sizeID of image
	@param	ID			:	ID that is sent signal
			lProductId	:	productID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotGetPictureSize(qint32 lProductId, qint32 &lPictSize)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = getPictureSize(lProductId, lPictSize);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------6
/*!
	@fn		slotSetRectId(qint32, qint32)
	@brief	set sizeID of image
	@param	lProductId	:	productID
			sizeId		:	sizeID
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotSetRectId(qint32 lProductId, eKINDRECTID lRectId)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = setRectId(lProductId, lRectId);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotZoom(qint32, qint32)
	@brief	zoom
	@param	lProductId	:	productID
			command		:	command
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotZoom(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotIris(qint32, qint32)
	@brief	iris
	@param	lProductId	:	productID
			command		:	command
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotIris(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotFocus(qint32, qint32)
	@brief	focus
	@param	lProductId	:	productID
			command		:	command
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotFocus(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotRotate(qint32, qint32)
	@brief	image rotate
	@param	lProductId	:	productID
			command		:	command
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotRotate(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotPosiNega(qint32, qint32)
	@brief	posi,nega
	@param	lProductId	:	productID
			command		:	command
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotPosiNega(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotColorBw(qint32, qint32)
	@brief	color,blackwhite
	@param	lProductId	:	prodctID
			command		:	command
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotColorBw(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotSlowShutter(qint32, qint32)
	@brief	slowshutter
	@param	lProductId	:	productID
			command		:	command
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotSlowShutter(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotImageMode(qint32, qint32)
	@brief	imagemode
	@param	lProductId	:	productID
			command		:	command
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotImageMode(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotMicroscopeMode(qint32, qint32)
	@brief	microscope mode
	@param	lProductId	:	productID
			command		:	commnad
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotMicroscopeMode(qint32 lProductId, qint32 command)
{
	qint32 lRet = IM_ERROR_SUCCESS;
	lRet = execCameraControl(lProductId, command);
	if(lRet != IM_ERROR_SUCCESS){
		emit sigErrorNormal(lRet);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn		slotChkAllDevice()
	@brief	print all device for debug
 */
//--------------------------------------------------------------------------------
void ClUsbDriver::slotChkAllDevice()
{
	qDebug() << chkAllDeviceInfo();
}
//--------------------------------------------------------------------------------
