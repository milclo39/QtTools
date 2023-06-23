#include "ClUsbDriver_global.h"
#include "CmnDefine.h"
#include "ClUsbCmd.h"

//--------------------------------------------------------------------------------
/*!
	@fn			ClUsbCmd
	@brief
 */
//--------------------------------------------------------------------------------
ClUsbCmd::ClUsbCmd()
{
	m_usPacketType = EL_SET;
	m_usCode = 0;
	m_usTransId = 0;
	m_ulParam1 = 0;
	m_ulParam2 = 0;
	m_ulParam3 = 0;
	m_ulParam4 = 0;
	m_ulParam5 = 0;
	m_ulDataSize = sizeof(ELMO_COMMAND);						//

#if 0
	if(QMetaType::isRegistered(qMetaTypeId<ClUsbCmd>()) == false)
	{
		qRegisterMetaTypeStreamOperators< ClUsbCmd >("ClUsbCmd");
	}
#else
	if(QMetaType::isRegistered(qRegisterMetaType<ClUsbCmd>()) == false)
	{
//		qRegisterMetaTypeStreamOperators< ClUsbCmd >("ClUsbCmd");
	}
#endif
}

ClUsbCmd::ClUsbCmd(const char *pcCmd, qint32 lLen)
{
	Q_ASSERT(pcCmd);
	Q_ASSERT(lLen);
	quint16 usDummy;									//
	QByteArray qCmdArray(pcCmd, lLen);
	QDataStream qWork(&qCmdArray,QIODevice::ReadOnly);
	qWork.setByteOrder(QDataStream::LittleEndian);		//
	qWork >> m_usPacketType
			>> m_usTransId
			>> m_ulDataSize
			>> m_usCode
			>> usDummy
			>> m_ulParam1
			>> m_ulParam2
			>> m_ulParam3
			>> m_ulParam4
			>> m_ulParam5;
}

ClUsbCmd::ClUsbCmd(quint16 a_usCode,
				   quint32 a_ulParam1,
				   quint32 a_ulParam2,
				   quint32 a_ulParam3,
				   quint32 a_ulParam4,
				   quint32 a_ulParam5,
				   quint16 a_usTransId,
				   quint16 a_usPacketType,
				   quint32 a_ulDataSize)
{
	m_usPacketType = a_usPacketType;
	m_usCode = a_usCode;
	m_usTransId = a_usTransId;
	m_ulParam1 = a_ulParam1;
	m_ulParam2 = a_ulParam2;
	m_ulParam3 = a_ulParam3;
	m_ulParam4 = a_ulParam4;
	m_ulParam5 = a_ulParam5;
	m_ulDataSize = a_ulDataSize;						//
#if 0
	if(QMetaType::isRegistered(qMetaTypeId<ClUsbCmd>()) == false)
	{
		qRegisterMetaTypeStreamOperators< ClUsbCmd >("ClUsbCmd");
	}
#else
	if(QMetaType::isRegistered(qRegisterMetaType<ClUsbCmd>()) == false)
	{
//		qRegisterMetaTypeStreamOperators< ClUsbCmd >("ClUsbCmd");
	}
#endif
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClUsbCmd
	@brief
 */
//--------------------------------------------------------------------------------
ClUsbCmd::~ClUsbCmd()
{
}
//--------------------------------------------------------------------------------
/*!
	@fn			operator <<
	@brief
 */
//--------------------------------------------------------------------------------
QDataStream &operator << (QDataStream &out, const ClUsbCmd &clCmd)
{
	out.setByteOrder(QDataStream::LittleEndian);	//set LittleEndian to QDataStream

	QByteArray qCmdArray;
	QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
	qWork.setByteOrder(QDataStream::LittleEndian);	//
	qWork << clCmd.getPagetType()						//set PacketType
		<< clCmd.getTransId()							//set Trans ID
		<< clCmd.getDataSize()							//set DataSize
		<< clCmd.getCode()								//set code
		<< qint16(0x00)									//set dummy (Reserved)
		<< clCmd.getParam1()							//set Param1
		<< clCmd.getParam2()							//set Param2
		<< clCmd.getParam3()							//set Param3
		<< clCmd.getParam4()							//set Param4
		<< clCmd.getParam5();							//set Param5
	out.writeRawData(qCmdArray.data(), qCmdArray.length());

	return out;
}
//--------------------------------------------------------------------------------
/*!
	@fn			operator >>
	@brief
 */
//--------------------------------------------------------------------------------
QDataStream &operator >> (QDataStream &in, ClUsbCmd &clCmd)
{
	qint32 lLen = 0;
	stREADDATACMD stReadCmd;
	char cCmdData[ELMO_RES_PACKET_SIZE];	//
	memset(cCmdData,0,sizeof(cCmdData));	//

	QByteArray qCmdArray;
	QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
	qWork.setByteOrder(QDataStream::LittleEndian);	//
	qWork << clCmd.getPagetType()						//set PacketType
		<< clCmd.getTransId()							//set Trans ID
		<< clCmd.getDataSize()							//set DataSize
		<< clCmd.getCode()								//set code
		<< qint16(0x00)									//set dummy (Reserved)
		<< clCmd.getParam1()							//set Param1
		<< clCmd.getParam2()							//set Param2
		<< clCmd.getParam3()							//set Param3
		<< clCmd.getParam4()							//set Param4
		<< clCmd.getParam5();							//set Param5
	memcpy(cCmdData, qCmdArray.data(), qCmdArray.length());
	stReadCmd.pData = cCmdData;
	stReadCmd.llMaxSize = ELMO_RES_PACKET_SIZE;
	in.setByteOrder(QDataStream::LittleEndian);			//set LittleEndian to QDataStream
	lLen = in.readRawData(reinterpret_cast<char *>(&stReadCmd),sizeof(stREADDATACMD));
	if(lLen <= 0){
		DBGFL("got error for readRawData : %d",lLen);
		in.setStatus(QDataStream::ReadCorruptData);
	}else{
		clCmd = ClUsbCmd(stReadCmd.pData,stReadCmd.llMaxSize);
		in.setStatus(QDataStream::Ok);
	}
	return in;
}
//--------------------------------------------------------------------------------
/*!
	@fn			operator >>
	@brief
 */
//--------------------------------------------------------------------------------
QDataStream &operator >> (QDataStream &in, ClPictCmd &clCmd)
{
	qint32 lLen = 0;
	stREADDATACMD stReadCmd;
	char cCmdData[ELMO_RES_PACKET_SIZE];				//
	memset(cCmdData,0,sizeof(cCmdData));	//

	QByteArray qCmdArray;
	QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
	qWork.setByteOrder(QDataStream::LittleEndian);		//
	qWork << clCmd.getPagetType()						//set PacketType
		<< clCmd.getTransId()							//set Trans ID
		<< clCmd.getDataSize()							//set DataSize
		<< clCmd.getCode()								//set code
		<< qint16(0x00)									//set dummy (Reserved)
		<< clCmd.getParam1()                            //set Param1
		<< clCmd.getParam2()							//set Param2
		<< clCmd.getParam3()							//set Param3
		<< clCmd.getParam4()							//set Param4
		<< clCmd.getParam5();							//set Param5
	memcpy(cCmdData, qCmdArray.data(), qCmdArray.length());
	stReadCmd.pData = cCmdData;
	stReadCmd.llMaxSize = ELMO_RES_PACKET_SIZE;
	stReadCmd.pByteArray = clCmd.getJpegArray();
	in.setByteOrder(QDataStream::LittleEndian);			//set LittleEndian to QDataStream
	lLen = in.readRawData(reinterpret_cast<char *>(&stReadCmd),sizeof(stREADDATACMD));
	if(lLen < 0){
		DBGFL("got error for readRawData : %d",lLen);
		in.setStatus(QDataStream::ReadCorruptData);
	}else if(lLen == 0){								//guess, length is zero by SD mode
		ClPictCmd clUsb(stReadCmd.pData,stReadCmd.llMaxSize);			//
		clCmd.setCode(clUsb.getCode());
		clCmd.setParam1(clUsb.getParam1());
		clCmd.setParam2(clUsb.getParam2());
		clCmd.setParam3(clUsb.getParam3());
		clCmd.setParam4(clUsb.getParam4());
		clCmd.setParam5(clUsb.getParam5());
		clCmd.setDataSize(lLen);						//does Data-Size zero when SD mode
		in.setStatus(QDataStream::Ok);
	}else{
		ClPictCmd clUsb(stReadCmd.pData,stReadCmd.llMaxSize);	//success
		clCmd.setCode(clUsb.getCode());
		clCmd.setParam1(clUsb.getParam1());
		clCmd.setParam2(clUsb.getParam2());
		clCmd.setParam3(clUsb.getParam3());
		clCmd.setParam4(clUsb.getParam4());
		clCmd.setParam5(clUsb.getParam5());
		clCmd.setDataSize(lLen);
		in.setStatus(QDataStream::Ok);
	}
	return in;
}
//--------------------------------------------------------------------------------
/*!
	@fn			operator >>
	@brief
 */
//--------------------------------------------------------------------------------
QDataStream &operator >> (QDataStream &in, ClModelNameCmd &clCmd)
{
	qint32 lLen = 0;
	stREADDATACMD stReadCmd;
	char cCmdData[ELMO_RES_PACKET_SIZE];	//
	memset(cCmdData,0,sizeof(cCmdData));	//

	QByteArray qCmdArray;
	QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
	qWork.setByteOrder(QDataStream::LittleEndian);	//
	qWork << clCmd.getPagetType()						//set PacketType
		<< clCmd.getTransId()							//set Trans ID
		<< clCmd.getDataSize()							//set DataSize
		<< clCmd.getCode()								//set code
		<< qint16(0x00)									//set dummy (Reserved)
		<< clCmd.getParam1()							//set Param1
		<< clCmd.getParam2()							//set Param2
		<< clCmd.getParam3()							//set Param3
		<< clCmd.getParam4()							//set Param4
		<< clCmd.getParam5();							//set Param5
	memcpy(cCmdData, qCmdArray.data(), qCmdArray.length());
	stReadCmd.pData = cCmdData;
	stReadCmd.llMaxSize = ELMO_RES_PACKET_SIZE;
	in.setByteOrder(QDataStream::LittleEndian);			//set LittleEndian to QDataStream
	lLen = in.readRawData(reinterpret_cast<char *>(&stReadCmd),sizeof(stREADDATACMD));
	if(lLen <= 0){
		DBGFL("got error for readRawData : %d",lLen);
		in.setStatus(QDataStream::ReadCorruptData);
	}else{
		ClUsbCmd clWkCmd(stReadCmd.pData,stReadCmd.llMaxSize);
		QByteArray qCmdResArray;
		QDataStream qWorkRes(&qCmdResArray,QIODevice::ReadWrite);
		qWorkRes.setByteOrder(QDataStream::LittleEndian);		//
		qWorkRes 	<< clWkCmd.getParam1()						//set Param1
					<< clWkCmd.getParam2()						//set Param2
					<< clWkCmd.getParam3()						//set Param3
					<< clWkCmd.getParam4()						//set Param4
					<< clWkCmd.getParam5();						//set Param5
		QString qWkStr(qCmdResArray);
		clCmd.setModelName(qWkStr);
		in.setStatus(QDataStream::Ok);
	}
	return in;
}
//--------------------------------------------------------------------------------
/*!
	@fn			operator >>
	@brief
 */
//--------------------------------------------------------------------------------
QDataStream &operator >> (QDataStream &in, ClModelVerCmd &clCmd)
{
	qint32 lLen = 0;
	stREADDATACMD stReadCmd;
	char cCmdData[ELMO_RES_PACKET_SIZE];	//
	memset(cCmdData,0,sizeof(cCmdData));	//

	QByteArray qCmdArray;
	QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
	qWork.setByteOrder(QDataStream::LittleEndian);	//
	qWork << clCmd.getPagetType()						//set PacketType
		<< clCmd.getTransId()							//set Trans ID
		<< clCmd.getDataSize()							//set DataSize
		<< clCmd.getCode()								//set code
		<< qint16(0x00)									//set dummy (Reserved)
		<< clCmd.getParam1()							//set Param1
		<< clCmd.getParam2()							//set Param2
		<< clCmd.getParam3()							//set Param3
		<< clCmd.getParam4()							//set Param4
		<< clCmd.getParam5();							//set Param5
	memcpy(cCmdData, qCmdArray.data(), qCmdArray.length());
	stReadCmd.pData = cCmdData;
	stReadCmd.llMaxSize = ELMO_RES_PACKET_SIZE;
	in.setByteOrder(QDataStream::LittleEndian);			//set LittleEndian to QDataStream
	lLen = in.readRawData(reinterpret_cast<char *>(&stReadCmd),sizeof(stREADDATACMD));
	if(lLen <= 0){
		DBGFL("got error for readRawData : %d",lLen);
		in.setStatus(QDataStream::ReadCorruptData);
	}else{
		ClUsbCmd clWkCmd(stReadCmd.pData,stReadCmd.llMaxSize);
		QByteArray qCmdResArray;
		QDataStream qWorkRes(&qCmdResArray,QIODevice::ReadWrite);
		qWorkRes.setByteOrder(QDataStream::LittleEndian);		//
		qWorkRes << clWkCmd.getParam1()						//set Param1
				 << clWkCmd.getParam2()						//set Param2
				 << clWkCmd.getParam3()						//set Param3
				 << clWkCmd.getParam4()						//set Param4
				 << clWkCmd.getParam5();					//set Param5
		QString qWkStr(qCmdResArray);
		clCmd.setVersion(qWkStr);
		in.setStatus(QDataStream::Ok);
	}
	return in;
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClPictCmd
	@brief
 */
//--------------------------------------------------------------------------------
ClPictCmd::ClPictCmd()
{
	m_usPacketType = EL_SET;
	m_usCode = EL_GET_JPEG_DATA;
	m_usTransId = 0;
	m_ulParam1 = EL_JPEG_SIZE_CURRENT;
	m_ulParam2 = 0;
	m_ulParam3 = 0;
	m_ulParam4 = 0;
	m_ulParam5 = 0;
	m_ulDataSize = sizeof(ELMO_COMMAND);						//
}

ClPictCmd::ClPictCmd(qint32 lParam)
{
	m_usPacketType = EL_SET;
	m_usCode = EL_GET_JPEG_DATA;
	m_usTransId = 0;
	m_ulParam1 = lParam;
	m_ulParam2 = 0;
	m_ulParam3 = 0;
	m_ulParam4 = 0;
	m_ulParam5 = 0;
	m_ulDataSize = sizeof(ELMO_COMMAND);						//
}
ClPictCmd::ClPictCmd(const char *pcCmd, qint32 lLen) : ClUsbCmd(pcCmd,lLen)
{
}

//--------------------------------------------------------------------------------
/*!
	@fn			ClZoomCmd
	@brief
 */
//--------------------------------------------------------------------------------
ClZoomCmd::ClZoomCmd(qint32 lParam)
{
	m_usPacketType = EL_SET;
	m_usCode = EL_SET_ZOOM;
	m_usTransId = 0;
	m_ulParam1 = lParam;
	m_ulParam2 = 0;
	m_ulParam3 = 0;
	m_ulParam4 = 0;
	m_ulParam5 = 0;
	m_ulDataSize = sizeof(ELMO_COMMAND);						//
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClIrisCmd
	@brief
 */
//--------------------------------------------------------------------------------
ClIrisCmd::ClIrisCmd(qint32 lParam)
{
	m_usPacketType = EL_SET;
	m_usCode = EL_SET_IRIS;
	m_usTransId = 0;
	m_ulParam1 = lParam;
	m_ulParam2 = 0;
	m_ulParam3 = 0;
	m_ulParam4 = 0;
	m_ulParam5 = 0;
	m_ulDataSize = sizeof(ELMO_COMMAND);						//
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClFocusCmd
	@brief
 */
//--------------------------------------------------------------------------------
ClFocusCmd::ClFocusCmd(qint32 lParam)
{
	m_usPacketType = EL_SET;
	m_usCode = EL_SET_FOCUS;
	m_usTransId = 0;
	m_ulParam1 = lParam;
	m_ulParam2 = 0;
	m_ulParam3 = 0;
	m_ulParam4 = 0;
	m_ulParam5 = 0;
	m_ulDataSize = sizeof(ELMO_COMMAND);						//
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClAutoFocusCmd
	@brief
 */
//--------------------------------------------------------------------------------
ClAutoFocusCmd::ClAutoFocusCmd()
{
	m_usPacketType = EL_SET;
	m_usCode = EL_SET_AF;
	m_usTransId = 0;
	m_ulParam1 = 0;
	m_ulParam2 = 0;
	m_ulParam3 = 0;
	m_ulParam4 = 0;
	m_ulParam5 = 0;
	m_ulDataSize = sizeof(ELMO_COMMAND);						//
}
//--------------------------------------------------------------------------------
/*!
	@class		ClModelNameCmd
	@brief
 */
//--------------------------------------------------------------------------------
ClModelNameCmd::ClModelNameCmd()
{
	m_usPacketType = EL_SET;
	m_usCode = EL_GET_UNIT_NAME;
	m_usTransId = 0;
	m_ulParam1 = 0;
	m_ulParam2 = 0;
	m_ulParam3 = 0;
	m_ulParam4 = 0;
	m_ulParam5 = 0;
	m_ulDataSize = sizeof(ELMO_COMMAND);						//
}
//--------------------------------------------------------------------------------
/*!
	@class		ClModelVerCmd
	@brief
 */
//--------------------------------------------------------------------------------
ClModelVerCmd::ClModelVerCmd()
{
	m_usPacketType = EL_SET;
	m_usCode = EL_GET_VERSION_INFO;
	m_usTransId = 0;
	m_ulParam1 = EL_VER_HEAD;
	m_ulParam2 = 0;
	m_ulParam3 = 0;
	m_ulParam4 = 0;
	m_ulParam5 = 0;
	m_ulDataSize = sizeof(ELMO_COMMAND);						//
}

ClModelVerCmd::ClModelVerCmd(qint32 lId)
{
	m_usPacketType = EL_SET;
	m_usCode = EL_GET_VERSION_INFO;
	m_usTransId = 0;
	m_ulParam1 = lId;
	m_ulParam2 = 0;
	m_ulParam3 = 0;
	m_ulParam4 = 0;
	m_ulParam5 = 0;
	m_ulDataSize = sizeof(ELMO_COMMAND);						//
}
//--------------------------------------------------------------------------------
/*!
	@class		ClRotationCmd
	@brief
 */
//--------------------------------------------------------------------------------
ClRotationCmd::ClRotationCmd(qint32 lParam)
{
	m_usPacketType = EL_SET;
	m_usCode = EL_SET_IMAGE_ROTATION;
	m_usTransId = 0;
	m_ulParam1 = lParam;
	m_ulParam2 = 0;
	m_ulParam3 = 0;
	m_ulParam4 = 0;
	m_ulParam5 = 0;
	m_ulDataSize = sizeof(ELMO_COMMAND);						//
}
//--------------------------------------------------------------------------------
/*!
	@class		ClPosiNegaCmd
	@brief
 */
//--------------------------------------------------------------------------------
ClPosiNegaCmd::ClPosiNegaCmd(qint32 lParam)
{
	m_usPacketType = EL_SET;
	m_usCode = EL_SET_POSI_NEGA;
	m_usTransId = 0;
	m_ulParam1 = lParam;
	m_ulParam2 = 0;
	m_ulParam3 = 0;
	m_ulParam4 = 0;
	m_ulParam5 = 0;
	m_ulDataSize = sizeof(ELMO_COMMAND);						//
}
//--------------------------------------------------------------------------------
/*!
	@class		ClColorBWCmd
	@brief
 */
//--------------------------------------------------------------------------------
ClColorBWCmd::ClColorBWCmd(qint32 lParam)
{
	m_usPacketType = EL_SET;
	m_usCode = EL_SET_COLOR_BW;
	m_usTransId = 0;
	m_ulParam1 = lParam;
	m_ulParam2 = 0;
	m_ulParam3 = 0;
	m_ulParam4 = 0;
	m_ulParam5 = 0;
	m_ulDataSize = sizeof(ELMO_COMMAND);						//
}
//--------------------------------------------------------------------------------
/*!
	@class		ClSlowShutterCmd
	@brief
 */
//--------------------------------------------------------------------------------
ClSlowShutterCmd::ClSlowShutterCmd(qint32 lParam)
{
	m_usPacketType = EL_SET;
	m_usCode = EL_SET_SLOW_SHUTTER;
	m_usTransId = 0;
	m_ulParam1 = lParam;
	m_ulParam2 = 0;
	m_ulParam3 = 0;
	m_ulParam4 = 0;
	m_ulParam5 = 0;
	m_ulDataSize = sizeof(ELMO_COMMAND);						//
}
//--------------------------------------------------------------------------------
/*!
	@class		ClImageModeCmd
	@brief
 */
//--------------------------------------------------------------------------------
ClImageModeCmd::ClImageModeCmd(qint32 lParam)
{
	m_usPacketType = EL_SET;
	m_usCode = EL_SET_IMAGE_MODE;
	m_usTransId = 0;
	m_ulParam1 = lParam;
	m_ulParam2 = 0;
	m_ulParam3 = 0;
	m_ulParam4 = 0;
	m_ulParam5 = 0;
	m_ulDataSize = sizeof(ELMO_COMMAND);						//
}
//--------------------------------------------------------------------------------
/*!
	@class		ClMicroscopeModeCmd
	@brief
 */
//--------------------------------------------------------------------------------
ClMicroscopeModeCmd::ClMicroscopeModeCmd(qint32 lParam)
{
	m_usPacketType = EL_SET;
	m_usCode = EL_SET_MICROSCOPE_MODE;
	m_usTransId = 0;
	m_ulParam1 = lParam;
	m_ulParam2 = 0;
	m_ulParam3 = 0;
	m_ulParam4 = 0;
	m_ulParam5 = 0;
	m_ulDataSize = sizeof(ELMO_COMMAND);						//
}
