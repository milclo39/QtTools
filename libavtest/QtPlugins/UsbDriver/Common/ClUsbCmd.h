#ifndef CLUSBCMD_H
#define CLUSBCMD_H

#include "ClUsbDriver_global.h"
#include "ElmoStruct.h"
#include "ElmoUsbCmd.h"

//--------------------------------------------------------------------------------
typedef struct _tagREADDATACMD
{
    _tagREADDATACMD(){
        pData = NULL;
        llMaxSize = 0;
        pByteArray = NULL;
    }
    char *pData;
    qint32 llMaxSize;
    QByteArray *pByteArray;
}stREADDATACMD;
//--------------------------------------------------------------------------------
class ClUsbCmd
{
public:
	ClUsbCmd();														//default constractor
	ClUsbCmd(quint16 a_usCode,										//custom constractor
             quint32 a_ulParam1 = 0,
             quint32 a_ulParam2 = 0,
             quint32 a_ulParam3 = 0,
             quint32 a_ulParam4 = 0,
             quint32 a_ulParam5 = 0,
			 quint16 a_usTransId = 0,
			 quint16 a_usPacketType = EL_SET,
             quint32 a_ulDataSize = sizeof(ELMO_COMMAND));
	ClUsbCmd(const char *cCmd,qint32 lLen = ELMO_CMD_PACKET_SIZE);
	virtual ~ClUsbCmd();

	quint16 getPagetType() const { return m_usPacketType; }			//get from the Packet type
	quint16 getTransId() const { return m_usTransId; }				//get from the Trans ID
    quint32 getDataSize() const { return m_ulDataSize; }				//get from the size of data
	quint16 getCode() const { return m_usCode; }						//get from the code
    quint32 getParam1() const { return m_ulParam1; }					//get from the param 1
    quint32 getParam2() const { return m_ulParam2; }					//get from the param 2
    quint32 getParam3() const { return m_ulParam3; }					//get from the param 3
    quint32 getParam4() const { return m_ulParam4; }					//get from the param 3
    quint32 getParam5() const { return m_ulParam5; }					//get from the param 3

	void setPacketType(quint16 a_usPacketType){ m_usPacketType = a_usPacketType; }
	void setTransId(quint16 a_usTransId){ m_usTransId = a_usTransId; }
    void setDataSize(quint32 a_ulDataSize){ m_ulDataSize = a_ulDataSize; }
	void setCode(quint16 a_usCode){ m_usCode = a_usCode; }
    void setParam1(quint32 a_ulParam1){ m_ulParam1 = a_ulParam1; }
    void setParam2(quint32 a_ulParam2){ m_ulParam2 = a_ulParam2; }
    void setParam3(quint32 a_ulParam3){ m_ulParam3 = a_ulParam3; }
    void setParam4(quint32 a_ulParam4){ m_ulParam4 = a_ulParam4; }
    void setParam5(quint32 a_ulParam5){ m_ulParam5 = a_ulParam5; }

protected:
	quint16 m_usPacketType;					//packet type (command, response, data)
	quint16 m_usTransId;						//transaction ID
    quint32	m_ulDataSize;						//the size of data that dose not include this header

	quint16 m_usCode;							//kind of code (operation code, response code)
    quint32	m_ulParam1;						//parameter 1
    quint32	m_ulParam2;						//parameter 2
    quint32	m_ulParam3;						//parameter 3
    quint32	m_ulParam4;						//parameter 4
    quint32	m_ulParam5;						//parameter 5
};
//--------------------------------------------------------------------------------
class ClPictCmd : public ClUsbCmd
{
public:
	ClPictCmd();
	ClPictCmd(qint32 lParam);
	ClPictCmd(const char *pcCmd, qint32 lLen);

	QByteArray *getJpegArray(){ return &m_qJpegArray; }
	void setJpegArray(QByteArray &qArray){ m_qJpegArray = qArray; }

private:
	QByteArray m_qJpegArray;			//for area of Jpeg data
};
//--------------------------------------------------------------------------------
class ClModelNameCmd : public ClUsbCmd
{
public:
	ClModelNameCmd();

	QString getModelName(){ return m_strModelName; }
	void setModelName(QString &qString){ m_strModelName = qString; }

private:
	QString m_strModelName;			//for area of model name
};
//--------------------------------------------------------------------------------
class ClModelVerCmd : public ClUsbCmd
{
public:
	ClModelVerCmd();
	ClModelVerCmd(qint32 lId);

	QString getVersion(){ return m_strVersion; }
	void setVersion(QString &qString){ m_strVersion = qString; }

private:
	QString m_strVersion;			//for area of version
};
//--------------------------------------------------------------------------------
QDataStream &operator << (QDataStream &out, const ClUsbCmd &clCmd);		//override << operator for ClUsbCmd to QDataStream
QDataStream &operator >> (QDataStream &in, ClUsbCmd &clCmd);			//
QDataStream &operator >> (QDataStream &in, ClPictCmd &clCmd);			//
QDataStream &operator >> (QDataStream &in, ClModelNameCmd &clCmd);		//
QDataStream &operator >> (QDataStream &in, ClModelVerCmd &clCmd);		//

Q_DECLARE_METATYPE(ClUsbCmd)
Q_DECLARE_METATYPE(ClPictCmd)
//--------------------------------------------------------------------------------
class ClZoomCmd : public ClUsbCmd
{
public:
	ClZoomCmd(){}
	ClZoomCmd(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClIrisCmd : public ClUsbCmd
{
public:
	ClIrisCmd(){}
	ClIrisCmd(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClFocusCmd : public ClUsbCmd
{
public:
	ClFocusCmd(){}
	ClFocusCmd(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClAutoFocusCmd : public ClUsbCmd
{
public:
	ClAutoFocusCmd();
};
//--------------------------------------------------------------------------------
class ClRotationCmd : public ClUsbCmd
{
public:
	ClRotationCmd(){}
	ClRotationCmd(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClPosiNegaCmd : public ClUsbCmd
{
public:
	ClPosiNegaCmd(){}
	ClPosiNegaCmd(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClColorBWCmd : public ClUsbCmd
{
public:
	ClColorBWCmd(){}
	ClColorBWCmd(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClSlowShutterCmd : public ClUsbCmd
{
public:
	ClSlowShutterCmd(){}
	ClSlowShutterCmd(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClImageModeCmd : public ClUsbCmd
{
public:
	ClImageModeCmd(){}
	ClImageModeCmd(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClMicroscopeModeCmd : public ClUsbCmd
{
public:
	ClMicroscopeModeCmd(){}
	ClMicroscopeModeCmd(qint32 lParam);
};
//--------------------------------------------------------------------------------

#endif // CLUSBCMD_H
