#define __USB_DEBUG

#include "ClUsbDriver/ClUsbDriver_global.h"
#include "Common/CmnDefine.h"
#include "ClUsbDriver/ClUsbDriver_linux.h"
#include "linux/ClDeviceInfo.h"
#include "ClUsbDriver/Command_Presenter.h"
#include "ClPresenterWebcam.h"


//--------------------------------------------------------------------------------
/*!
	@fn			ClPresenterWebcam
	@brief
 */
//--------------------------------------------------------------------------------
ClPresenterWebcam::ClPresenterWebcam(qint32 lProductId, QObject *parent) :
	ClDeviceInfo(lProductId, parent)
{
	m_pstCapBuf = NULL;
	m_lNumCapBufs = 0;
	m_lFd = 0;
}
//--------------------------------------------------------------------------------
/*!
	@fn			~ClPresenterWebcam
	@brief
 */
//--------------------------------------------------------------------------------
ClPresenterWebcam::~ClPresenterWebcam()
{
}
//--------------------------------------------------------------------------------
/*!
	@fn			open
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterWebcam::open()
{
	if(isOpen() == true){
		return LIBUSB_SUCCESS;
	}
	struct v4l2_requestbuffers  stReq;
	struct v4l2_cropcap         stCropCap;
	struct v4l2_crop            stCrop;
	struct v4l2_format          stFmt;
	struct v4l2_buffer          stBuf;
	enum v4l2_buf_type          lType;
	v4l2_std_id                 llStd;
	qint32						failCount = 0;
	qint32						lInput = 0;
	qint32						lRet = 0;


	m_lFd = ::open(V4L2_DEVICE, O_RDWR | O_NONBLOCK, 0);
	if(m_lFd == -1){
		return LIBUSB_ERROR_NO_DEVICE;
	}


	/* Query for capture device capabilities */
	if (xioctl(m_lFd, VIDIOC_QUERYCAP, &m_Capability) == -1) {
		if (errno == EINVAL) {
			DBGFL("%s is no V4L2 device", V4L2_DEVICE);
			::close(m_lFd);
			return LIBUSB_ERROR_NO_DEVICE;
		}
		DBGFL("Failed VIDIOC_QUERYCAP on %s (%s)", V4L2_DEVICE,
			strerror(errno));
		::close(m_lFd);
		return LIBUSB_ERROR_NO_DEVICE;
	}

	if (!(m_Capability.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		DBGFL("%s is no video capture device", V4L2_DEVICE);
		::close(m_lFd);
		return LIBUSB_ERROR_NO_DEVICE;
	}

	if (!(m_Capability.capabilities & V4L2_CAP_STREAMING)) {
		DBGFL("%s does not support streaming i/o", V4L2_DEVICE);
		::close(m_lFd);
		return LIBUSB_ERROR_NO_DEVICE;
	}

	stCropCap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (xioctl(m_lFd, VIDIOC_CROPCAP, &stCropCap) == -1) {
		DBGFL("VIDIOC_CROPCAP failed %d, %s", errno, strerror(errno));
		::close(m_lFd);
		return LIBUSB_ERROR_NO_DEVICE;
	}

	/* Set the video capture image format */
	CLEAR(stFmt);
	stFmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (xioctl(m_lFd, VIDIOC_G_FMT, &stFmt) == -1) {
		DBGFL("VIDIOC_S_FMT failed on %s (%s)", V4L2_DEVICE,
												strerror(errno));
		return LIBUSB_ERROR_NO_DEVICE;
	}
	stFmt.fmt.pix.width       = 640;
	stFmt.fmt.pix.height      = 480;
	/* Set the video capture format */
	if (xioctl(m_lFd, VIDIOC_S_FMT, &stFmt) == -1) {
		DBGFL("VIDIOC_S_FMT failed on %s (%s)", V4L2_DEVICE,
												strerror(errno));
		return LIBUSB_ERROR_NO_DEVICE;
	}
#if 0
	stCrop.type     = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	stCrop.c.left   = 0;
	stCrop.c.top    = 0;
	stCrop.c.width = 640;
	/*
	 * Note that we are optionally capturing 2 extra rows at the top and 4 at
	 * the bottom to have more valid image data to work with while removing
	 * interlacing artifacts.
	 */
//	stCrop.c.height = topOffset ? captureHeight + EXTRA_ROWS : captureHeight;
	stCrop.c.height = 480;
	DBGFL("Setting capture cropping (%dx%d) topOffset:%d", stCrop.c.width, stCrop.c.height,0);

	/* Crop the image depending on requested image size */
	if (xioctl(m_lFd, VIDIOC_S_CROP, &stCrop) == -1) {
		DBGFL("VIDIOC_S_CROP failed %d, %s", errno, strerror(errno));
		return LIBUSB_ERROR_NO_DEVICE;
	}

	DBGFL("Capturing %dx%d video (cropped to %dx%d)",
		   stFmt.fmt.pix.width, stFmt.fmt.pix.height, stCrop.c.width, stCrop.c.height);
#endif

	CLEAR(stReq);
	stReq.count  = NUM_CAPTURE_BUFS;
	stReq.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	stReq.memory = V4L2_MEMORY_MMAP;

	/* Allocate buffers in the capture device driver */
	if (xioctl(m_lFd, VIDIOC_REQBUFS, &stReq) == -1) {
		DBGFL("VIDIOC_REQBUFS failed on %s (%s)", V4L2_DEVICE,
												  strerror(errno));
		::close(m_lFd);
		return LIBUSB_ERROR_NO_DEVICE;
	}

	DBGFL("%d capture buffers were successfully allocated.", stReq.count);

	if (stReq.count < NUM_CAPTURE_BUFS) {
		DBGFL("Insufficient buffer memory on %s", V4L2_DEVICE);
		::close(m_lFd);
		return LIBUSB_ERROR_NO_DEVICE;
	}

	m_pstCapBuf = reinterpret_cast<stCAPTUREBUFFER*>(::calloc(stReq.count, sizeof(*m_pstCapBuf)));

	if (!m_pstCapBuf) {
		DBGFL("Failed to allocate memory for capture buffer structs.");
		::close(m_lFd);
		return LIBUSB_ERROR_NO_DEVICE;
	}

	/* Map the allocated buffers to user space */
	for (m_lNumCapBufs = 0; m_lNumCapBufs < stReq.count; m_lNumCapBufs++) {
		CLEAR(stBuf);
		stBuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		stBuf.memory = V4L2_MEMORY_MMAP;
		stBuf.index = m_lNumCapBufs;

		if (xioctl(m_lFd, VIDIOC_QUERYBUF, &stBuf) == -1) {
			DBGFL("Failed VIDIOC_QUERYBUF on %s (%s)", V4L2_DEVICE,
													   strerror(errno));
			::close(m_lFd);
			return LIBUSB_ERROR_NO_DEVICE;
		}

		m_pstCapBuf[m_lNumCapBufs].lLength = stBuf.length;
		m_pstCapBuf[m_lNumCapBufs].lOffset = stBuf.m.offset;
		m_pstCapBuf[m_lNumCapBufs].pStart  = ::mmap(NULL,
										  stBuf.length,
										  PROT_READ | PROT_WRITE,
										  MAP_SHARED,
										  m_lFd, stBuf.m.offset);

		if (m_pstCapBuf[m_lNumCapBufs].pStart == MAP_FAILED) {
			DBGFL("Failed to mmap buffer on %s (%s)", V4L2_DEVICE,
													  strerror(errno));
			::close(m_lFd);
			return LIBUSB_ERROR_NO_DEVICE;
		}

		DBGFL("Capture driver buffer %d at physical address %lu mapped to "
			"virtual address %#lx", m_lNumCapBufs, (unsigned long)
			m_pstCapBuf[m_lNumCapBufs].pStart, m_pstCapBuf[m_lNumCapBufs].lOffset);

		if (xioctl(m_lFd, VIDIOC_QBUF, &stBuf) == -1) {
			DBGFL("VIODIOC_QBUF failed on %s (%s)", V4L2_DEVICE,
													strerror(errno));
			::close(m_lFd);
			return LIBUSB_ERROR_NO_DEVICE;
		}
	}
	/* Start the video streaming */
	lType = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (xioctl(m_lFd, VIDIOC_STREAMON, &lType) == -1) {
		DBGFL("VIDIOC_STREAMON failed on %s (%s)", V4L2_DEVICE,
												   strerror(errno));
		::close(m_lFd);
		return LIBUSB_ERROR_NO_DEVICE;
	}
	if(QIODevice::open(ReadWrite | Unbuffered) == false){	//
		::close(m_lFd);
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
void ClPresenterWebcam::close()
{
	enum v4l2_buf_type lType;
	unsigned int       i;

	if(isOpen() == true){								//if available device

		/* Shut off the video capture */
		lType = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (xioctl(m_lFd, VIDIOC_STREAMOFF, &lType) == -1) {
			DBGFL("VIDIOC_STREAMOFF failed (%s)", strerror(errno));
		}

		if (::close(m_lFd) == -1) {
			DBGFL("Failed to close capture device (%s)", strerror(errno));
		}

		for (i = 0; i < m_lNumCapBufs; i++) {
			if (::munmap(m_pstCapBuf[i].pStart, m_pstCapBuf[i].lLength) == -1) {
				DBGFL("Failed to unmap capture buffer %d", i);
			}
		}

		delete(m_pstCapBuf);
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn			CommandPresenter
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterWebcam::CommandPresenter(qint32 CmdID, QVariant& qParameter)
{
	qint32 lRes = IM_ERROR_NOT_OPEN;						//未接続


	DBGFL("CommandPresenter linux");

	switch(CmdID){
	case GET_PICTURE:											//画像取得
		{
			lRes = getPicture(qParameter);
			break;
		}
	case RELEASE_PICTURE:								//取得した画像領域の解放
		{
			lRes = IM_ERROR_SUCCESS;
			break;
		}
	case GET_SIZE_PICTURE:								//直前に取得したサイズの返却
		{
			lRes = IM_ERROR_SUCCESS;					//取得成功
			qParameter = m_iPictureSize;				//
			break;
		}
	case GET_RECT_PICTURE:								//画像の大きさ取得
		{
			lRes = IM_ERROR_SUCCESS;					//取得成功
			qParameter.setValue<eKINDRECTID>(m_iPictureRect);	//
			break;
		}
	case SET_RECT_PICTURE:								//画像の大きさ設定
		{
			if(qParameter.type() == QVariant::UserType){
				if(chINRANGE(SIZE_PICTURE_HIGH,qParameter.value<eKINDRECTID>(),SIZE_PICTURE_LOW)){
					lRes = IM_ERROR_SUCCESS;					//取得成功
					m_iPictureRect = qParameter.value<eKINDRECTID>();
				}
			}
			break;
		}
	case GET_MODEL_NAME:								//機種名取得
		{
			lRes = getModelName(qParameter);
			break;
		}
	case GET_MODEL_VERSION:									//バージョン取得
		{
			lRes = getVersion(qParameter);
			break;
		}
	case GET_HEIGHT_PICTURE:								//現在の画像の縦幅
		{
			lRes = IM_ERROR_SUCCESS;					//取得成功
			qParameter = m_usHeight;					//
			break;
		}
	case GET_WIDTH_PICTURE:								//現在の画像の横幅
		{
			lRes = IM_ERROR_SUCCESS;					//取得成功
			qParameter = m_usWidth;						//
			break;
		}
	case ZOOM_WIDE:											//拡大
		{
			ClZoomCmd clCmd(EL_WIDE);
			lRes = execCommand(clCmd);
			break;
		}
	case ZOOM_TELE:											//縮小
		{
			ClZoomCmd clCmd(EL_TELE);
			lRes = execCommand(clCmd);
			break;
		}
	case ZOOM_STOP:											//ストップ
		{
			ClZoomCmd clCmd(EL_STOP);
			lRes = execCommand(clCmd);
			break;
		}
	case IRIS_OPEN:											//開く
		{
			ClIrisCmd clCmd(EL_OPEN);
			lRes = execCommand(clCmd);
			break;
		}
	case IRIS_CLOSE:										//閉じる
		{
			ClIrisCmd clCmd(EL_CLOSE);
			lRes = execCommand(clCmd);
			break;
		}
	case IRIS_STOP:											//ストップ
		{
			ClIrisCmd clCmd(EL_ISTOP);
			lRes = execCommand(clCmd);
			break;
		}
	case IRIS_INITIALIZE:									//初期位置
		{
			ClIrisCmd clCmd(EL_NORMAL);
			lRes = execCommand(clCmd);
			break;
		}
	case FOCUS_AUTO:										//自動
		{
			ClAutoFocusCmd clCmd;
			lRes = execCommand(clCmd);
			break;
		}
	case FOCUS_NEAR:										//近く
		{
			ClFocusCmd clCmd(EL_NEAR);
			lRes = execCommand(clCmd);
			break;
		}
	case FOCUS_FAR:											//遠く
		{
			ClFocusCmd clCmd(EL_FAR);
			lRes = execCommand(clCmd);
			break;
		}
	case FOCUS_STOP:										//ストップ
		{
			ClFocusCmd clCmd(EL_STOP);
			lRes = execCommand(clCmd);
			break;
		}
	default:
		lRes = IM_ERROR_INVALID_PARAMETER;					//対応したコマンドがない
		break;
	}

	return lRes;

}
//--------------------------------------------------------------------------------
/*!
	@fn			xioctl
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterWebcam::xioctl(qint32 ifd, qint32 iRequest, void *pvArg)
{
	qint32 iRes = -1;

	do{
		iRes = ::ioctl(ifd, iRequest, pvArg);
	}while(iRes == -1 && ((errno == EINTR) || (errno == EAGAIN)));

	if (iRes == -1) {
		qDebug("error %d, %s\n", errno, strerror(errno));
	}
	return iRes;
}
//--------------------------------------------------------------------------------
/*!
	@fn			getPicture
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterWebcam::getPicture(QVariant &qParam)		//画像取得関数
{
	m_iPictureRect = RECTID_UNSUPPORT;

	QDataStream qCmdStream(this);						//create data stream for getPicture

	ClPictCmd clRecive(RECTID_UNSUPPORT);					//this variable is for reception
	ClPictCmd clPict(RECTID_UNSUPPORT);					//this variable is for send

MEASU_BEGIN
	qCmdStream << clPict;								//send command to device

	qCmdStream >> clRecive;								//receive command from device
MEASU_END

	if(qCmdStream.status() != QDataStream::Ok){			//if confirm whether QDataStream status is OK
		qParam.clear();
		return IM_ERROR_CAPTURE_FAILURE;				//return IM_ERROR_COMMAND_FAILURE
	}
	if(clRecive.getDataSize() == 0){					//The data size is zero when may mode sd card
		qParam.clear();
		return IM_ERROR_CANNOT_OUTPUT_IMAGE;			//return IM_ERROR_CANNOT_OUTPUT_IMAGE
	}
	QSize qSize;
	qSize.setWidth(clRecive.getParam2());
	qSize.setHeight(clRecive.getParam3());
	m_usHeight = static_cast<quint16>(qSize.height());
	m_usWidth = static_cast<quint16>(qSize.width());

	m_iPictureSize = clRecive.getJpegArray()->length();
	qParam = *clRecive.getJpegArray();					//transfar jpeg data as output
	return IM_ERROR_SUCCESS;
}
//--------------------------------------------------------------------------------
/*!
	@fn			execCommand
	@brief
	@return		NONE
 */
//--------------------------------------------------------------------------------
qint32 ClPresenterWebcam::execCommand(ClUsbCmd &clCmd)
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
qint32 ClPresenterWebcam::getModelName(QVariant &/*qParam*/)
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
qint32 ClPresenterWebcam::getVersion(QVariant &/*qParam*/)
{
	return IM_ERROR_SUCCESS;								//output version to QVariant
}
//--------------------------------------------------------------------------------
/*!
	@fn			readData
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint64	ClPresenterWebcam::readData ( char * data, qint64 maxSize )
{
	qint32 lReqLength = 0;
	qint64 llRet = -1;
	struct v4l2_buffer	stV4l2buf;
	struct v4l2_format	stFmt;

	if(isOpen() == true){								//if available device
		if(maxSize > INT_MAX){
			DBG("got error from writeData");
			return -1;
		}
		lReqLength = static_cast<qint32>(maxSize);
		ClUsbCmd clUsb(data,lReqLength);							//

		if(clUsb.getCode() == EL_GET_JPEG_DATA){					//if code is Jpeg data

			CLEAR(stV4l2buf);
			stV4l2buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			stV4l2buf.memory = V4L2_MEMORY_MMAP;
//			stV4l2buf.index =

			DBGFL("VIDIOC_DQBUF");

			/* Get a frame buffer with captured data */
			if (xioctl(m_lFd, VIDIOC_DQBUF, &stV4l2buf) == -1) {
				if (errno == EAGAIN) {
					return -1;
				}
				DBGFL("VIDIOC_DQBUF failed (%s)", strerror(errno));
				return -1;
			}
			QByteArray *pqArray = reinterpret_cast<QByteArray*>(clUsb.getParam1());
			pqArray->append(reinterpret_cast<char*>(m_pstCapBuf[stV4l2buf.index].pStart),
							m_pstCapBuf[stV4l2buf.index].lLength);
			clUsb.setDataSize(m_pstCapBuf[stV4l2buf.index].lLength);
			llRet = m_pstCapBuf[stV4l2buf.index].lLength;
			CLEAR(stFmt);
			/* Get the video capture format */
			stFmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			if (xioctl(m_lFd, VIDIOC_G_FMT, &stFmt) != -1) {
				clUsb.setParam2(stFmt.fmt.pix.width);
				clUsb.setParam3(stFmt.fmt.pix.height);
			}
		}
	}
	return llRet;
}
//--------------------------------------------------------------------------------
/*!
	@fn			writeData
	@brief
	@return		qint32
 */
//--------------------------------------------------------------------------------
qint64	ClPresenterWebcam::writeData ( const char * data, qint64 maxSize )
{
	return -1;
}
