#include "ClUsbDriver_global.h"
#include "CmnDefine.h"
#include "ClUsbCmdV2.h"

#include "ElmoStruct.h"

//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
ClUsbCmdV2::ClUsbCmdV2()
{
	m_ucHeadCmdVer = ELMO_HEAD_VER_V2<<4;						//matsubara changed <<4
	m_usCommand = 0;
	m_ucTransId = 0;
	m_ulDataSize = sizeof(ELMO_COMMAND_V2);						//
	m_qParam = NULL;											//matsubara add.
	m_qParam = new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

#if 0
	if(QMetaType::isRegistered(qMetaTypeId<ClUsbCmdV2>()) == false)
	{
		qRegisterMetaTypeStreamOperators< ClUsbCmdV2 >("ClUsbCmdV2");
	}
#else
	if(QMetaType::isRegistered(qRegisterMetaType<ClUsbCmdV2>()) == false)
	{
//		qRegisterMetaTypeStreamOperators< ClUsbCmdV2 >("ClUsbCmdV2");
	}
#endif
}
ClUsbCmdV2::ClUsbCmdV2(quint16 a_usCommand,
					   QByteArray &a_qArray,
					   quint32 a_ulDataSize,
					   quint8 a_ucTransId,
					   quint8 a_ucHeadCmdVer)// : a_qArray(QByteArray(sizeof(ELMO_COMMAND_V2),'0'))
{
	m_ucHeadCmdVer = a_ucHeadCmdVer;
	m_usCommand = a_usCommand;
	m_ucTransId = a_ucTransId;
	m_ulDataSize = a_ulDataSize;						//
	m_qParam = NULL;											//matsubara add.
	m_qParam = new QByteArray(a_qArray);
}
ClUsbCmdV2::ClUsbCmdV2(const char *pcCmd, qint32 lLen)
{
	Q_ASSERT(pcCmd);
	Q_ASSERT(lLen);
	QByteArray qCmdArray(pcCmd, lLen);
	QDataStream qWork(&qCmdArray,QIODevice::ReadOnly);
	qWork.setByteOrder(QDataStream::LittleEndian);		//
	qWork >> m_ucHeadCmdVer
			>> m_ucTransId
			>> m_usCommand
			>> m_ulDataSize;
	m_qParam = NULL;											//matsubara add.
	m_qParam = new QByteArray();
}

ClUsbCmdV2::ClUsbCmdV2(const ClUsbCmdV2 &srcCmd)
{
	//matsubara add.
	m_usCommand = srcCmd.m_usCommand;

	m_ucHeadCmdVer = srcCmd.m_ucHeadCmdVer;
	m_ucTransId = srcCmd.m_ucTransId;
	m_ulDataSize = srcCmd.m_ulDataSize;
	m_qParam = NULL;											//matsubara add.
	m_qParam = new QByteArray(*srcCmd.m_qParam);
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
ClUsbCmdV2 &ClUsbCmdV2::operator =(const ClUsbCmdV2 &srcCmd)
{
	//matsubara add.
	m_usCommand = srcCmd.m_usCommand;

	m_ucHeadCmdVer = srcCmd.m_ucHeadCmdVer;
	m_ucTransId = srcCmd.m_ucTransId;
	m_ulDataSize = srcCmd.m_ulDataSize;
	m_qParam = NULL;											//matsubara add.
	m_qParam = new QByteArray(*srcCmd.m_qParam);
	return *this;
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
ClUsbCmdV2::~ClUsbCmdV2()
{
	if(m_qParam != NULL){
		delete m_qParam;
		m_qParam = NULL;										//matsubara add.
	}
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
const char *ClUsbCmdV2::data() const
{
	QByteArray qCmdArray;
	QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
	qWork.setByteOrder(QDataStream::LittleEndian);	//
//	m_ulDataSize = m_qParam.length();
	qWork << m_ucHeadCmdVer
			<<	m_ucTransId
			<<	m_usCommand
			<<	m_ulDataSize;
	if(m_qParam != NULL){
		qWork << m_qParam;
	}
	return qCmdArray.data();
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
	QByteArray &ClUsbCmdV2::getParam()						//get from the param
	{
		return *m_qParam;
	}
	quint32 ClUsbCmdV2::getParam0() const					//get from the param 0
	{
		if(m_qParam != NULL){
			PELMO_COMMAND_V2 pstCmd;
			pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
			return pstCmd->Param0;
		}
		return NULL;
	}
	quint32 ClUsbCmdV2::getParam1() const					//get from the param 1
	{
		if(m_qParam != NULL){
			PELMO_COMMAND_V2 pstCmd;
			pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
			return pstCmd->Param1;
		}
		return NULL;
	}
	quint32 ClUsbCmdV2::getParam2() const					//get from the param 2
	{
		if(m_qParam != NULL){
			PELMO_COMMAND_V2 pstCmd;
			pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
			return pstCmd->Param2;
		}
		return NULL;
	}
	quint32 ClUsbCmdV2::getParam3() const					//get from the param 3
	{
		if(m_qParam != NULL){
			PELMO_COMMAND_V2 pstCmd;
			pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
			return pstCmd->Param3;
		}
		return NULL;
	}
	quint32 ClUsbCmdV2::getParam4() const					//get from the param 3
	{
		if(m_qParam != NULL){
			PELMO_COMMAND_V2 pstCmd;
			pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
			return pstCmd->Param4;
		}
		return NULL;
	}
	quint32 ClUsbCmdV2::getParam5() const					//get from the param 3
	{
		if(m_qParam != NULL){
			PELMO_COMMAND_V2 pstCmd;
			pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
			return pstCmd->Param5;
		}
		return NULL;
	}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
	void ClUsbCmdV2::setParam(QByteArray &qArray)
	{
		m_qParam = new QByteArray(qArray);
		m_ulDataSize = m_qParam->length();
	}
	void ClUsbCmdV2::setParam0(quint32 a_ulParam)
	{
		if(m_qParam != NULL){
			PELMO_COMMAND_V2 pstCmd;
			pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
			pstCmd->Param0 = a_ulParam;
		}
	}
	void ClUsbCmdV2::setParam1(quint32 a_ulParam)
	{
		if(m_qParam != NULL){
			PELMO_COMMAND_V2 pstCmd;
			pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
			pstCmd->Param1 = a_ulParam;
		}
	}
	void ClUsbCmdV2::setParam2(quint32 a_ulParam)
	{
		if(m_qParam != NULL){
			PELMO_COMMAND_V2 pstCmd;
			pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
			pstCmd->Param2 = a_ulParam;
		}
	}
	void ClUsbCmdV2::setParam3(quint32 a_ulParam)
	{
		if(m_qParam != NULL){
			PELMO_COMMAND_V2 pstCmd;
			pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
			pstCmd->Param3 = a_ulParam;
		}
	}
	void ClUsbCmdV2::setParam4(quint32 a_ulParam)
	{
		if(m_qParam != NULL){
			PELMO_COMMAND_V2 pstCmd;
			pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
			pstCmd->Param4 = a_ulParam;
		}
	}
	void ClUsbCmdV2::setParam5(quint32 a_ulParam)
	{
		if(m_qParam != NULL){
			PELMO_COMMAND_V2 pstCmd;
			pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
			pstCmd->Param5 = a_ulParam;
		}
	}

//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
quint32 ClUsbCmdV2::length() const
{
	if(m_qParam != NULL){
		return m_qParam->length() + sizeof(ELMO_PACKET_HEADER_V2);
	}
	return 0;
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
QDataStream &operator << (QDataStream &out, const ClUsbCmdV2 &clCmd)
{
	out.setByteOrder(QDataStream::LittleEndian);	//set LittleEndian to QDataStream

	ClUsbCmdV2 workCmd = clCmd;
	QByteArray qCmdArray;
	QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
	qWork.setByteOrder(QDataStream::LittleEndian);	//
	qWork
		<< workCmd.getHeaderVer()					//set PacketType
		<< workCmd.getTransId()						//set Trans ID
		<< workCmd.getCommand()						//set code
		<< workCmd.getParam();						//set Param
	out.writeRawData(qCmdArray.data(), qCmdArray.length());

	return out;
}
//--------------------------------------------------------------------------------
/*!
	@brief
 */
//--------------------------------------------------------------------------------
QDataStream &operator >> (QDataStream &in, ClUsbCmdV2 &clCmd)
{
	qint32 lLen = 0;
	stREADDATACMD stReadCmd;
	char cCmdData[ELMO_RES_PACKET_SIZE];	//
	memset(cCmdData,0,sizeof(cCmdData));	//

	QByteArray qCmdArray;
	QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
	qWork.setByteOrder(QDataStream::LittleEndian);	//
	qWork << clCmd.getHeaderVer()						//set HeaderVer
		<< clCmd.getTransId()							//set Trans ID
		<< clCmd.getCommand()							//set code
		<< clCmd.getDataSize()							//set DataSize
		<< clCmd.getParam0()							//set Param0
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
		PELMO_PACKET_V2 sReturn		= (PELMO_PACKET_V2)cCmdData;
		PELMO_COMMAND_V2 pRetPacket	= (PELMO_COMMAND_V2)sReturn->Data;
		clCmd.setPacketType(sReturn->Header.HeaderVer);
		clCmd.setTransId(sReturn->Header.TransID);
		clCmd.setCommand(sReturn->Header.OpCode);
		clCmd.setDataSize(sReturn->Header.DataSize);
		clCmd.setParam0(pRetPacket->Param0);
		clCmd.setParam1(pRetPacket->Param1);
		clCmd.setParam2(pRetPacket->Param2);
		clCmd.setParam3(pRetPacket->Param3);
		clCmd.setParam4(pRetPacket->Param4);
		clCmd.setParam5(pRetPacket->Param5);
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
QDataStream &operator >> (QDataStream &in, ClPictCmdV2 &clCmd)
{
	qint32 lLen = 0;
	stREADDATACMD stReadCmd;
	char cCmdData[ELMO_RES_PACKET_SIZE];				//
	memset(cCmdData,0,sizeof(cCmdData));	//

	QByteArray qCmdArray;
	QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
	qWork.setByteOrder(QDataStream::LittleEndian);		//
	qWork << clCmd.getHeaderVer()						//set HeaderVer
		<< clCmd.getTransId()							//set Trans ID
		<< clCmd.getCommand()							//set code
		<< clCmd.getDataSize()							//set DataSize
		<< clCmd.getParam0()							//set Param0
		<< clCmd.getParam1()							//set Param1
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
	if(lLen < 0){										//error
		DBGFL("got error for readRawData : %d",lLen);
		in.setStatus(QDataStream::ReadCorruptData);
	}else if(lLen == 0){								//guess, length is zero by SD mode
		clCmd.setDataSize(lLen);						//does Data-Size zero when SD mode
		PELMO_PACKET_V2 sReturn		= (PELMO_PACKET_V2)cCmdData;
		PELMO_COMMAND_V2 pRetPacket	= (PELMO_COMMAND_V2)sReturn->Data;
		clCmd.setCommand(sReturn->Header.OpCode);
		clCmd.setParam2(pRetPacket->Param2);
		in.setStatus(QDataStream::Ok);
	}else{												//success
		PELMO_PACKET_V2 sReturn		= (PELMO_PACKET_V2)cCmdData;
		PELMO_COMMAND_V2 pRetPacket	= (PELMO_COMMAND_V2)sReturn->Data;
		clCmd.setCommand(sReturn->Header.OpCode);
		clCmd.setParam2(pRetPacket->Param2);
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
QDataStream &operator >> (QDataStream &in, ClStartStreamCmdV2 &clCmd)
{
	qint32 lLen = 0;
	stREADDATACMD stReadCmd;
	char cCmdData[ELMO_RES_PACKET_SIZE];				//
	memset(cCmdData,0,sizeof(cCmdData));				//

	QByteArray qCmdArray;
	QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
	qWork.setByteOrder(QDataStream::LittleEndian);		//
	qWork << clCmd.getHeaderVer()						//set HeaderVer
		<< clCmd.getTransId()							//set Trans ID
		<< clCmd.getCommand()							//set code
		<< clCmd.getDataSize()							//set DataSize
		<< clCmd.getParam0()							//set Param0
		<< clCmd.getParam1()							//set Param1
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
	if(lLen < 0){										//error
		DBGFL("got error for readRawData : %d",lLen);
		in.setStatus(QDataStream::ReadCorruptData);
	}else if(lLen == 0){								//guess, length is zero by SD mode
		clCmd.setDataSize(lLen);						//does Data-Size zero when SD mode
		PELMO_PACKET_V2 sReturn		= (PELMO_PACKET_V2)cCmdData;
		PELMO_COMMAND_V2 pRetPacket	= (PELMO_COMMAND_V2)sReturn->Data;
		clCmd.setParam1(pRetPacket->Param1);			//stream type set
		in.setStatus(QDataStream::Ok);
	}else{												//success
		PELMO_PACKET_V2 sReturn		= (PELMO_PACKET_V2)cCmdData;
		PELMO_COMMAND_V2 pRetPacket	= (PELMO_COMMAND_V2)sReturn->Data;
		clCmd.setParam1(pRetPacket->Param1);			//stream type set
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
QDataStream &operator >> (QDataStream &in, ClStopStreamCmdV2 &clCmd)
{
	qint32 lLen = 0;
	stREADDATACMD stReadCmd;
	char cCmdData[ELMO_RES_PACKET_SIZE];				//
	memset(cCmdData,0,sizeof(cCmdData));				//

	QByteArray qCmdArray;
	QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
	qWork.setByteOrder(QDataStream::LittleEndian);		//
	qWork << clCmd.getHeaderVer()						//set HeaderVer
		<< clCmd.getTransId()							//set Trans ID
		<< clCmd.getCommand()							//set code
		<< clCmd.getDataSize()							//set DataSize
		<< clCmd.getParam0()							//set Param0
		<< clCmd.getParam1()							//set Param1
		<< clCmd.getParam2()							//set Param2
		<< clCmd.getParam3()							//set Param3
		<< clCmd.getParam4()							//set Param4
		<< clCmd.getParam5();                           //set Param5
	memcpy(cCmdData, qCmdArray.data(), qCmdArray.length());
	stReadCmd.pData = cCmdData;
	stReadCmd.llMaxSize = ELMO_RES_PACKET_SIZE;
	in.setByteOrder(QDataStream::LittleEndian);			//set LittleEndian to QDataStream
	lLen = in.readRawData(reinterpret_cast<char *>(&stReadCmd),sizeof(stREADDATACMD));
	if(lLen < 0){										//error
		DBGFL("got error for readRawData : %d",lLen);
		in.setStatus(QDataStream::ReadCorruptData);
	}else if(lLen == 0){								//guess, length is zero by SD mode
		clCmd.setDataSize(lLen);						//does Data-Size zero when SD mode
		in.setStatus(QDataStream::Ok);
	}else{												//success
		clCmd.setDataSize(lLen);						//does Data-Size zero when SD mode
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
QDataStream &operator >> (QDataStream &in, ClModelNameCmdV2 &clCmd)
{
	qint32 lLen = 0;
	stREADDATACMD stReadCmd;
	char cCmdData[ELMO_RES_PACKET_SIZE];	//
	memset(cCmdData,0,sizeof(cCmdData));	//

	QByteArray qCmdArray;
	QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
	qWork.setByteOrder(QDataStream::LittleEndian);		//
	qWork << clCmd.getHeaderVer()						//set HeaderVer
		<< clCmd.getTransId()							//set Trans ID
		<< clCmd.getCommand()							//set code
		<< clCmd.getDataSize()							//set DataSize
		<< clCmd.getParam0()							//set Param0
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
		PELMO_PACKET_V2 sReturn		= (PELMO_PACKET_V2)cCmdData;
		PELMO_COMMAND_V2 pRetPacket	= (PELMO_COMMAND_V2)sReturn->Data;
		QByteArray qCmdResArray;
		QDataStream qWorkRes(&qCmdResArray,QIODevice::ReadWrite);
		qWorkRes.setByteOrder(QDataStream::LittleEndian);		//
		qWorkRes 	<< (qint32)pRetPacket->Param0		//set Param0
					<< (qint32)pRetPacket->Param1		//set Param1
					<< (qint32)pRetPacket->Param2		//set Param2
					<< (qint32)pRetPacket->Param3		//set Param3
					<< (qint32)pRetPacket->Param4		//set Param4
					<< (qint32)pRetPacket->Param5;		//set Param5
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
QDataStream &operator >> (QDataStream &in, ClSHVerCmdV2 &clCmd)
{
	qint32 lLen = 0;
	stREADDATACMD stReadCmd;
	char cCmdData[ELMO_RES_PACKET_SIZE];	//
	memset(cCmdData,0,sizeof(cCmdData));	//

	QByteArray qCmdArray;
	QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
	qWork.setByteOrder(QDataStream::LittleEndian);		//
	qWork << clCmd.getHeaderVer()						//set HeaderVer
		<< clCmd.getTransId()							//set Trans ID
		<< clCmd.getCommand()							//set code
		<< clCmd.getDataSize()							//set DataSize
		<< clCmd.getParam0()							//set Param0
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
		PELMO_PACKET_V2 sReturn		= (PELMO_PACKET_V2)cCmdData;
		PELMO_COMMAND_V2 pRetPacket	= (PELMO_COMMAND_V2)sReturn->Data;
		QByteArray qCmdResArray0;
		QDataStream qWorkRes0(&qCmdResArray0,QIODevice::ReadWrite);
		qWorkRes0.setByteOrder(QDataStream::LittleEndian);		//
		qWorkRes0 	<< (qint32)((pRetPacket->Param0<<16)>>16);	//set Param0
		QByteArray qCmdResArray1;
		QDataStream qWorkRes1(&qCmdResArray1,QIODevice::ReadWrite);
		qWorkRes1.setByteOrder(QDataStream::LittleEndian);		//
		qWorkRes1 	<< (qint32)pRetPacket->Param1;				//set Param1
		QString qWkStr = QString("%1.%2.%3")
						 .arg(qCmdResArray0.constData())
						 .arg(qCmdResArray1.constData())
						 .arg(pRetPacket->Param2);
		clCmd.setVersion(qWkStr);
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
QDataStream &operator >> (QDataStream &in, ClLensVerCmdV2 &clCmd)
{
	qint32 lLen = 0;
	stREADDATACMD stReadCmd;
	char cCmdData[ELMO_RES_PACKET_SIZE];	//
	memset(cCmdData,0,sizeof(cCmdData));	//

	QByteArray qCmdArray;
	QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
	qWork.setByteOrder(QDataStream::LittleEndian);		//
	qWork << clCmd.getHeaderVer()						//set HeaderVer
		<< clCmd.getTransId()							//set Trans ID
		<< clCmd.getCommand()							//set code
		<< clCmd.getDataSize()							//set DataSize
		<< clCmd.getParam0()							//set Param0
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
		PELMO_PACKET_V2 sReturn		= (PELMO_PACKET_V2)cCmdData;
		PELMO_COMMAND_V2 pRetPacket	= (PELMO_COMMAND_V2)sReturn->Data;
		QByteArray qCmdResArray0;
		QDataStream qWorkRes0(&qCmdResArray0,QIODevice::ReadWrite);
		qWorkRes0.setByteOrder(QDataStream::LittleEndian);		//
		qWorkRes0 	<< (qint32)((pRetPacket->Param0<<16)>>16);	//set Param0
		QByteArray qCmdResArray1;
		QDataStream qWorkRes1(&qCmdResArray1,QIODevice::ReadWrite);
		qWorkRes1.setByteOrder(QDataStream::LittleEndian);		//
		qWorkRes1 	<< (qint32)pRetPacket->Param1;				//set Param1
		QString qWkStr = QString("%1.%2.%3")
						 .arg(qCmdResArray0.constData())
						 .arg(qCmdResArray1.constData())
						 .arg(pRetPacket->Param2);
		clCmd.setVersion(qWkStr);
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
QDataStream &operator >> (QDataStream &in, ClFPGAVerCmdV2 &clCmd)
{
	qint32 lLen = 0;
	stREADDATACMD stReadCmd;
	char cCmdData[ELMO_RES_PACKET_SIZE];	//
	memset(cCmdData,0,sizeof(cCmdData));	//

	QByteArray qCmdArray;
	QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
	qWork.setByteOrder(QDataStream::LittleEndian);		//
	qWork << clCmd.getHeaderVer()						//set HeaderVer
		<< clCmd.getTransId()							//set Trans ID
		<< clCmd.getCommand()							//set code
		<< clCmd.getDataSize()							//set DataSize
		<< clCmd.getParam0()							//set Param0
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
		PELMO_PACKET_V2 sReturn		= (PELMO_PACKET_V2)cCmdData;
		PELMO_COMMAND_V2 pRetPacket	= (PELMO_COMMAND_V2)sReturn->Data;
		QByteArray qCmdResArray0;
		QDataStream qWorkRes0(&qCmdResArray0,QIODevice::ReadWrite);
		qWorkRes0.setByteOrder(QDataStream::LittleEndian);		//
		qWorkRes0 	<< (qint32)((pRetPacket->Param0<<16)>>16);	//set Param0
		QByteArray qCmdResArray1;
		QDataStream qWorkRes1(&qCmdResArray1,QIODevice::ReadWrite);
		qWorkRes1.setByteOrder(QDataStream::LittleEndian);		//
		qWorkRes1 	<< (qint32)pRetPacket->Param1;				//set Param1
		QString qWkStr = QString("%1.%2.%3")
						 .arg(qCmdResArray0.constData())
						 .arg(qCmdResArray1.constData())
						 .arg(pRetPacket->Param2);
		clCmd.setVersion(qWkStr);
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
QDataStream &operator >> (QDataStream &in, ClProductCodeCmdV2 &clCmd)
{
	qint32 lLen = 0;
	stREADDATACMD stReadCmd;
	char cCmdData[ELMO_RES_PACKET_SIZE];	//
	memset(cCmdData,0,sizeof(cCmdData));	//

	QByteArray qCmdArray;
	QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
	qWork.setByteOrder(QDataStream::LittleEndian);		//
	qWork << clCmd.getHeaderVer()						//set HeaderVer
		<< clCmd.getTransId()							//set Trans ID
		<< clCmd.getCommand()							//set code
		<< clCmd.getDataSize()							//set DataSize
		<< clCmd.getParam0()							//set Param0
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
		PELMO_PACKET_V2 sReturn		= (PELMO_PACKET_V2)cCmdData;
		PELMO_COMMAND_V2 pRetPacket	= (PELMO_COMMAND_V2)sReturn->Data;
		QByteArray qCmdResArray;
		QDataStream qWorkRes(&qCmdResArray,QIODevice::ReadWrite);
		qWorkRes.setByteOrder(QDataStream::LittleEndian);		//
		qWorkRes 	<< (qint32)pRetPacket->Param0		//set Param0
					<< (qint32)pRetPacket->Param1		//set Param1
					<< (qint32)pRetPacket->Param2		//set Param2
					<< (qint32)pRetPacket->Param3		//set Param3
					<< (qint32)pRetPacket->Param4		//set Param4
					<< (qint32)pRetPacket->Param5;		//set Param5
		clCmd.setProductCode(qCmdResArray);
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
QDataStream &operator >> (QDataStream &in, ClCurrentModeCmdV2 &clCmd)
{
	qint32 lLen = 0;
	stREADDATACMD stReadCmd;
	char cCmdData[ELMO_RES_PACKET_SIZE];	//
	memset(cCmdData,0,sizeof(cCmdData));	//

	QByteArray qCmdArray;
	QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
	qWork.setByteOrder(QDataStream::LittleEndian);		//
	qWork << clCmd.getHeaderVer()						//set HeaderVer
		<< clCmd.getTransId()							//set Trans ID
		<< clCmd.getCommand()							//set code
		<< clCmd.getDataSize()							//set DataSize
		<< clCmd.getParam0()							//set Param0
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
		PELMO_PACKET_V2 sReturn		= (PELMO_PACKET_V2)cCmdData;
		PELMO_COMMAND_V2 pRetPacket	= (PELMO_COMMAND_V2)sReturn->Data;
		QStringList qStrListRes;
		qStrListRes.append(QString("%1").arg( (qint32)pRetPacket->Param0) );
		qStrListRes.append(QString("%1").arg( (qint32)pRetPacket->Param1) );
		qStrListRes.append(QString("%1").arg( (qint32)pRetPacket->Param2) );
		qStrListRes.append(QString("%1").arg( (qint32)pRetPacket->Param3) );
		qStrListRes.append(QString("%1").arg( (qint32)pRetPacket->Param4) );
		qStrListRes.append(QString("%1").arg( (qint32)pRetPacket->Param5) );
		clCmd.setCurrentMode(qStrListRes);
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
QDataStream &operator >> (QDataStream &in, ClUpFwFileCmdV2 &clCmd)
{
	qint32 lLen = 0;
	stREADDATACMD stReadCmd;
	char cCmdData[ELMO_RES_PACKET_SIZE];	//
	memset(cCmdData,0,sizeof(cCmdData));	//

	QByteArray qCmdArray;
	QDataStream qWork(&qCmdArray,QIODevice::ReadWrite);
	qWork.setByteOrder(QDataStream::LittleEndian);	//
	qWork << clCmd.getHeaderVer()						//set HeaderVer
		<< clCmd.getTransId()							//set Trans ID
		<< clCmd.getCommand()							//set code
		<< clCmd.getDataSize()							//set DataSize
		<< clCmd.getParam0()							//set Param0
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
		PELMO_PACKET_V2 sReturn		= (PELMO_PACKET_V2)cCmdData;
		PELMO_COMMAND_V2 pRetPacket	= (PELMO_COMMAND_V2)sReturn->Data;
		clCmd.setPacketType(sReturn->Header.HeaderVer);
		clCmd.setTransId(sReturn->Header.TransID);
		clCmd.setCommand(sReturn->Header.OpCode);
		clCmd.setDataSize(sReturn->Header.DataSize);
		clCmd.setParam0(pRetPacket->Param0);
		clCmd.setParam1(pRetPacket->Param1);
		clCmd.setParam2(pRetPacket->Param2);
		clCmd.setParam3(pRetPacket->Param3);
		clCmd.setParam4(pRetPacket->Param4);
		clCmd.setParam5(pRetPacket->Param5);
		clCmd.setTotalSize((qint16)(pRetPacket->Param1>>16));
		clCmd.setWroteSize((qint16)pRetPacket->Param1);
		clCmd.setStatus(pRetPacket->Param2);
		in.setStatus(QDataStream::Ok);
	}

	return in;
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClPictCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClPictCmdV2::ClPictCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_READIMAGE_CAMERA;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
ClPictCmdV2::ClPictCmdV2(qint32 lParam)
{
	Q_UNUSED(lParam)
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_READIMAGE_CAMERA;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

	if(m_qParam != NULL){
		PELMO_COMMAND_V2 pstCmd;
		pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
		pstCmd->Param0 = 100;														//圧縮率
		pstCmd->Param1 = 0;															//サムネイル有無
		pstCmd->Param2 = 0;
		pstCmd->Param3 = 0;
		pstCmd->Param4 = 0;
		pstCmd->Param5 = 0;
	}
}
ClPictCmdV2::ClPictCmdV2(const char *pcCmd, qint32 lLen) : ClUsbCmdV2(pcCmd,lLen)
{
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClZoomTeleCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClZoomTeleCmdV2::ClZoomTeleCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_ZOOM_TELE;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClZoomWideCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClZoomWideCmdV2::ClZoomWideCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_ZOOM_WIDE;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClZoomStopCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClZoomStopCmdV2::ClZoomStopCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_ZOOM_STOP;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClZoomVariantSetCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClZoomVariantSetCmdV2::ClZoomVariantSetCmdV2(qint32 lParam)
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_ZOOM_SET_VARIANT;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

	if(m_qParam != NULL){
		PELMO_COMMAND_V2 pstCmd;
		pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
		pstCmd->Param0 = lParam;													//ズーム位置
		pstCmd->Param1 = 0;
		pstCmd->Param2 = 0;
		pstCmd->Param3 = 0;
		pstCmd->Param4 = 0;
		pstCmd->Param5 = 0;
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClZoomStatusCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClZoomStatusCmdV2::ClZoomStatusCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_ZOOM_GET_STATUS;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClFocusNearCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClFocusNearCmdV2::ClFocusNearCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_FOCUS_NEAR;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClFocusFarCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClFocusFarCmdV2::ClFocusFarCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_FOCUS_FAR;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClAutoFocusCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClAutoFocusCmdV2::ClAutoFocusCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_FOCUS_AF;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClFocusStopCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClFocusStopCmdV2::ClFocusStopCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_FOCUS_STOP;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClFocusVariantSetCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClFocusVariantSetCmdV2::ClFocusVariantSetCmdV2(qint32 lParam)
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_FOCUS_SET_VARIANT;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

	if(m_qParam != NULL){
		PELMO_COMMAND_V2 pstCmd;
		pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
		pstCmd->Param0 = lParam;													//焦点位置
		pstCmd->Param1 = 0;
		pstCmd->Param2 = 0;
		pstCmd->Param3 = 0;
		pstCmd->Param4 = 0;
		pstCmd->Param5 = 0;
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClFocusStatusCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClFocusStatusCmdV2::ClFocusStatusCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_FOCUS_GET_STATUS;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClAFModeSetCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClAFModeSetCmdV2::ClAFModeSetCmdV2(qint32 lParam)
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_FOCUS_SET_AFMODE;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

	if(m_qParam != NULL){
		PELMO_COMMAND_V2 pstCmd;
		pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
		pstCmd->Param0 = lParam;													//ON/OFF
		pstCmd->Param1 = 0;
		pstCmd->Param2 = 0;
		pstCmd->Param3 = 0;
		pstCmd->Param4 = 0;
		pstCmd->Param5 = 0;
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClAFModeStatusCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClAFModeStatusCmdV2::ClAFModeStatusCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_FOCUS_GET_AFMODE;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClIrisOpenCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClIrisOpenCmdV2::ClIrisOpenCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_BRIGHT_BRIGHT;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClIrisCloseCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClIrisCloseCmdV2::ClIrisCloseCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_BRIGHT_DARK;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClIrisStopCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClIrisStopCmdV2::ClIrisStopCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_BRIGHT_STOP;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClIrisInitCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClIrisInitCmdV2::ClIrisInitCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_BRIGHT_INIT;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClIrisVariantSetCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClIrisVariantSetCmdV2::ClIrisVariantSetCmdV2(qint32 lParam)
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_BRIGHT_SET_VARIANT;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

	if(m_qParam != NULL){
		PELMO_COMMAND_V2 pstCmd;
		pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
		pstCmd->Param0 = lParam;													//明るさ
		pstCmd->Param1 = 0;
		pstCmd->Param2 = 0;
		pstCmd->Param3 = 0;
		pstCmd->Param4 = 0;
		pstCmd->Param5 = 0;
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClIrisStatusCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClIrisStatusCmdV2::ClIrisStatusCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_BRIGHT_GET_STATUS;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClBRTModeSetCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClBRTModeSetCmdV2::ClBRTModeSetCmdV2(qint32 lParam)
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_BRIGHT_SET_BRIGHTMODE;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

	if(m_qParam != NULL){
		PELMO_COMMAND_V2 pstCmd;
		pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
		pstCmd->Param0 = lParam;													//ON/OFF
		pstCmd->Param1 = 0;
		pstCmd->Param2 = 0;
		pstCmd->Param3 = 0;
		pstCmd->Param4 = 0;
		pstCmd->Param5 = 0;
	}
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClBRTModeStatusCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClBRTModeStatusCmdV2::ClBRTModeStatusCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_BRIGHT_GET_BRIGHTMODE;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClStartStreamCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClStartStreamCmdV2::ClStartStreamCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_READIMAGE_STREAM_START;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
ClStartStreamCmdV2::ClStartStreamCmdV2(qint32 lParam)
{
	Q_UNUSED(lParam)
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_READIMAGE_STREAM_START;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

	if(m_qParam != NULL){
		PELMO_COMMAND_V2 pstCmd;
		pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
		pstCmd->Param0 = 0x00;														//NONE/PCM/AAC
		pstCmd->Param1 = 0x00;														//JPG/H.264
		pstCmd->Param2 = 10;														//フレームレート 1/6/10/15/30
		pstCmd->Param3 = 0;
		pstCmd->Param4 = 0;
		pstCmd->Param5 = 0;
	}
}
ClStartStreamCmdV2::ClStartStreamCmdV2(const char *pcCmd, qint32 lLen) : ClUsbCmdV2(pcCmd,lLen)
{
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClStopStreamCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClStopStreamCmdV2::ClStopStreamCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_READIMAGE_STREAM_END;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
ClStopStreamCmdV2::ClStopStreamCmdV2(qint32 lParam)
{
	Q_UNUSED(lParam)
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_READIMAGE_STREAM_END;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

	if(m_qParam != NULL){
		PELMO_COMMAND_V2 pstCmd;
		pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
		pstCmd->Param0 = 0;															//SessionID
		pstCmd->Param1 = 0;
		pstCmd->Param2 = 0;
		pstCmd->Param3 = 0;
		pstCmd->Param4 = 0;
		pstCmd->Param5 = 0;
	}
}
ClStopStreamCmdV2::ClStopStreamCmdV2(const char *pcCmd, qint32 lLen) : ClUsbCmdV2(pcCmd,lLen)
{
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClUpImgFileCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClUpImgFileCmdV2::ClUpImgFileCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_WRITEIMAGE_FILE;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@fn			ClUpFwFileCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClUpFwFileCmdV2::ClUpFwFileCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_SYSTEM_WRITEFW;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClModelNameCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClModelNameCmdV2::ClModelNameCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_STATUS_CAMERANAME;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClSHVerCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClSHVerCmdV2::ClSHVerCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_GETVER_SH;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClLensVerCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClLensVerCmdV2::ClLensVerCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_GETVER_LENS;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClFPGAVerCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClFPGAVerCmdV2::ClFPGAVerCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_GETVER_FPGA;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClProductCodeCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClProductCodeCmdV2::ClProductCodeCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_STATUS_PRODUCTCODE;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClCurrentModeCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClCurrentModeCmdV2::ClCurrentModeCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_STATUS_GET_STATUS;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClResSwCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClResSwCmdV2::ClResSwCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_STATUS_RESOLUTION;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClImgOutputSwCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClImgOutSwCmdV2::ClImgOutSwCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_STATUS_OUTPUT_KIND;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClCapPushFlgCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClCapPushFlgSwCmdV2::ClCapPushFlgSwCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_STATUS_PUSHCAPTURE;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClSysTimeCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClSysTimeCmdV2::ClSysTimeCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_SYSTEM_DATETIME;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClModelNameSetCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClModelNameSetCmdV2::ClModelNameSetCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_SYSTEM_WRITECAMERANAME;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClProductCodeSetCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClProductCodeSetCmdV2::ClProductCodeSetCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_SYSTEM_WRITEPRODUCTCODE;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClWhiteSetCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClWhiteSetCmdV2::ClWhiteSetCmdV2(qint32 lParam)
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_WHITE_SET;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

	if(m_qParam != NULL){
		PELMO_COMMAND_V2 pstCmd;
		pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
		pstCmd->Param0 = lParam;													//0:Auto/1:Onepush/2:Manual
		pstCmd->Param1 = 0;
		pstCmd->Param2 = 0;
		pstCmd->Param3 = 0;
		pstCmd->Param4 = 0;
		pstCmd->Param5 = 0;
	}
}
//--------------------------------------------------------------------------------
/*!
	@class		ClWhiteStatusCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClWhiteStatusCmdV2::ClWhiteStatusCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_WHITE_GET_STATUS;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClFlickerSetCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClFlickerSetCmdV2::ClFlickerSetCmdV2(qint32 lParam)
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_FLICKER_SET;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

	if(m_qParam != NULL){
		PELMO_COMMAND_V2 pstCmd;
		pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
		pstCmd->Param0 = lParam;													//0:60/1:50
		pstCmd->Param1 = 0;
		pstCmd->Param2 = 0;
		pstCmd->Param3 = 0;
		pstCmd->Param4 = 0;
		pstCmd->Param5 = 0;
	}
}
//--------------------------------------------------------------------------------
/*!
	@class		ClFlickerStatusCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClFlickerStatusCmdV2::ClFlickerStatusCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_FLICKER_GET_STATUS;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClImageModeSetCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClImageModeSetCmdV2::ClImageModeSetCmdV2(qint32 lParam)
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_GRAPHIC_SET;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

	if(m_qParam != NULL){
		PELMO_COMMAND_V2 pstCmd;
		pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
		pstCmd->Param0 = lParam;													//0:Text1/1:Text2/2:Text3/100:Color1/101:Color2
		pstCmd->Param1 = 0;
		pstCmd->Param2 = 0;
		pstCmd->Param3 = 0;
		pstCmd->Param4 = 0;
		pstCmd->Param5 = 0;
	}
}
//--------------------------------------------------------------------------------
/*!
	@class		ClImageModeStatusCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClImageModeStatusCmdV2::ClImageModeStatusCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_GRAPHIC_GET_STATUS;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClEdgeSetCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClEdgeSetCmdV2::ClEdgeSetCmdV2(qint32 lParam)
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_EDGE_EFFECT;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

	if(m_qParam != NULL){
		PELMO_COMMAND_V2 pstCmd;
		pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
		pstCmd->Param0 = lParam;													//0:Low/1:Middle/2:High
		pstCmd->Param1 = 0;
		pstCmd->Param2 = 0;
		pstCmd->Param3 = 0;
		pstCmd->Param4 = 0;
		pstCmd->Param5 = 0;
	}
}
//--------------------------------------------------------------------------------
/*!
	@class		ClEdgeStatusCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClEdgeStatusCmdV2::ClEdgeStatusCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_EDGE_EFFECT_GET_STATUS;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClGammaSetCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClGammaSetCmdV2::ClGammaSetCmdV2(qint32 lParam)
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_GAMMA;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

	if(m_qParam != NULL){
		PELMO_COMMAND_V2 pstCmd;
		pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
		pstCmd->Param0 = lParam;													//0:Low/1:Middle/2:High
		pstCmd->Param1 = 0;
		pstCmd->Param2 = 0;
		pstCmd->Param3 = 0;
		pstCmd->Param4 = 0;
		pstCmd->Param5 = 0;
	}
}
//--------------------------------------------------------------------------------
/*!
	@class		ClGammaStatusCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClGammaStatusCmdV2::ClGammaStatusCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_GAMMA_GET_STATUS;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClFreezeSetCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClFreezeSetCmdV2::ClFreezeSetCmdV2(qint32 lParam)
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_FREEZE_SWITCH;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

	if(m_qParam != NULL){
		PELMO_COMMAND_V2 pstCmd;
		pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
		pstCmd->Param0 = lParam;													//0:Off/1:On
		pstCmd->Param1 = 0;
		pstCmd->Param2 = 0;
		pstCmd->Param3 = 0;
		pstCmd->Param4 = 0;
		pstCmd->Param5 = 0;
	}
}
//--------------------------------------------------------------------------------
/*!
	@class		ClFreezeStatusCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClFreezeStatusCmdV2::ClFreezeStatusCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_FREEZE_GET_STATUS;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClRotationSetCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClRotationSetCmdV2::ClRotationSetCmdV2(qint32 lParam)
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_ROTATION_SET;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

	if(m_qParam != NULL){
		PELMO_COMMAND_V2 pstCmd;
		pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
		pstCmd->Param0 = lParam;													//0:On/1:Off
		pstCmd->Param1 = 0;
		pstCmd->Param2 = 0;
		pstCmd->Param3 = 0;
		pstCmd->Param4 = 0;
		pstCmd->Param5 = 0;
	}
}
//--------------------------------------------------------------------------------
/*!
	@class		ClRotationStatusCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClRotationStatusCmdV2::ClRotationStatusCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_ROTATION_GET_STATUS;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClNegaPosiSetCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClNegaPosiSetCmdV2::ClNegaPosiSetCmdV2(qint32 lParam)
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_NEGAPOSI_SET;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

	if(m_qParam != NULL){
		PELMO_COMMAND_V2 pstCmd;
		pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
		pstCmd->Param0 = lParam;													//0:Posi/1:Nega
		pstCmd->Param1 = 0;
		pstCmd->Param2 = 0;
		pstCmd->Param3 = 0;
		pstCmd->Param4 = 0;
		pstCmd->Param5 = 0;
	}
}
//--------------------------------------------------------------------------------
/*!
	@class		ClNegaPosiStatusCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClNegaPosiStatusCmdV2::ClNegaPosiStatusCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_NEGAPOSI_GET_STATUS;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClColorMonoSetCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClColorMonoSetCmdV2::ClColorMonoSetCmdV2(qint32 lParam)
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_COLORMONO_SET;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

	if(m_qParam != NULL){
		PELMO_COMMAND_V2 pstCmd;
		pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
		pstCmd->Param0 = lParam;													//0:Color/1:Black and White
		pstCmd->Param1 = 0;
		pstCmd->Param2 = 0;
		pstCmd->Param3 = 0;
		pstCmd->Param4 = 0;
		pstCmd->Param5 = 0;
	}
}
//--------------------------------------------------------------------------------
/*!
	@class		ClColorMonoStatusCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClColorMonoStatusCmdV2::ClColorMonoStatusCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_COLORMONO_GET_STATUS;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClScopeSetCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClScopeSetCmdV2::ClScopeSetCmdV2(qint32 lParam)
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_SCOPE_SET;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

	if(m_qParam != NULL){
		PELMO_COMMAND_V2 pstCmd;
		pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
		pstCmd->Param0 = lParam;													//0:Off/1:On
		pstCmd->Param1 = 0;
		pstCmd->Param2 = 0;
		pstCmd->Param3 = 0;
		pstCmd->Param4 = 0;
		pstCmd->Param5 = 0;
	}
}
//--------------------------------------------------------------------------------
/*!
	@class		ClScopeStatusCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClScopeStatusCmdV2::ClScopeStatusCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_SCOPE_GET_STATUS;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClSpeakerSetCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClSpeakerSetCmdV2::ClSpeakerSetCmdV2(qint32 lParam)
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_SPEAKER_SET_VOLUME;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

	if(m_qParam != NULL){
		PELMO_COMMAND_V2 pstCmd;
		pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
		pstCmd->Param0 = lParam;													//音量
		pstCmd->Param1 = 0;
		pstCmd->Param2 = 0;
		pstCmd->Param3 = 0;
		pstCmd->Param4 = 0;
		pstCmd->Param5 = 0;
	}
}
//--------------------------------------------------------------------------------
/*!
	@class		ClSpeakerStatusCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClSpeakerStatusCmdV2::ClSpeakerStatusCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_SPEAKER_GET_STATUS;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClVideoSetCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClVideoSetCmdV2::ClVideoSetCmdV2(qint32 lParam)
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_VIDEO_SET;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

	if(m_qParam != NULL){
		PELMO_COMMAND_V2 pstCmd;
		pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
		pstCmd->Param0 = lParam;													//0:NTSC/1:PAL
		pstCmd->Param1 = 0;
		pstCmd->Param2 = 0;
		pstCmd->Param3 = 0;
		pstCmd->Param4 = 0;
		pstCmd->Param5 = 0;
	}
}
//--------------------------------------------------------------------------------
/*!
	@class		ClVideoStatusCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClVideoStatusCmdV2::ClVideoStatusCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_VIDEO_GET_STATUS;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClLanguageSetCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClLanguageSetCmdV2::ClLanguageSetCmdV2(qint32 lParam)
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_LANG_SET;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

	if(m_qParam != NULL){
		PELMO_COMMAND_V2 pstCmd;
		pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
		pstCmd->Param0 = lParam;													//0:JPN/1:ENG
		pstCmd->Param1 = 0;
		pstCmd->Param2 = 0;
		pstCmd->Param3 = 0;
		pstCmd->Param4 = 0;
		pstCmd->Param5 = 0;
	}
}
//--------------------------------------------------------------------------------
/*!
	@class		ClLanguageStatusCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClLanguageStatusCmdV2::ClLanguageStatusCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_LANG_GET_STATUS;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClGuideSetCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClGuideSetCmdV2::ClGuideSetCmdV2(qint32 lParam)
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_GUID_SET;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);

	if(m_qParam != NULL){
		PELMO_COMMAND_V2 pstCmd;
		pstCmd = reinterpret_cast<PELMO_COMMAND_V2>((char *)m_qParam->data());
		pstCmd->Param0 = lParam;													//0:Off/1:On
		pstCmd->Param1 = 0;
		pstCmd->Param2 = 0;
		pstCmd->Param3 = 0;
		pstCmd->Param4 = 0;
		pstCmd->Param5 = 0;
	}
}
//--------------------------------------------------------------------------------
/*!
	@class		ClGuideStatusCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClGuideStatusCmdV2::ClGuideStatusCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_GUID_GET_STATUS;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClSaveSettingCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClSaveSettingCmdV2::ClSaveSettingCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_SETTING_SAVE;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
/*!
	@class		ClLoadSettingCmdV2
	@brief
 */
//--------------------------------------------------------------------------------
ClLoadSettingCmdV2::ClLoadSettingCmdV2()
{
	m_ucHeadCmdVer	= ELMO_HEAD_VER_V2<<4;
	m_usCommand		= EL_CMD_SETTING_LOAD;
	m_ucTransId		= 0;
	m_ulDataSize	= sizeof(ELMO_COMMAND_V2);
	m_qParam		= NULL;
	m_qParam		= new QByteArray(sizeof(ELMO_COMMAND_V2),NULL);
}
//--------------------------------------------------------------------------------
