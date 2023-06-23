#ifndef CLUSBCMDV2_H
#define CLUSBCMDV2_H

#include "ClUsbDriver_global.h"
#include "ClUsbCmd.h"
#include "ElmoStructV2.h"
#include "ElmoUsbCmdV2.h"

//--------------------------------------------------------------------------------
class ClUsbCmdV2
{
protected:
    quint32	m_ulDataSize;			//the size of data that dose not include this header
	quint8	m_ucHeadCmdVer;			//ヘッダバージョン
	quint8	m_ucTransId;			//transaction ID
	quint16	m_usCommand;			//コマンド

	QByteArray	*m_qParam;			//コマンド部

public:
	ClUsbCmdV2();
	ClUsbCmdV2(quint16 a_usCommand,										//custom constractor
			   QByteArray &qArray,
               quint32 a_ulDataSize = sizeof(ELMO_COMMAND_V2),
			   quint8 a_ucTransId = 0,
			   quint8 a_ucHeadCmdVer = ELMO_HEAD_VER_V2<<4);
	ClUsbCmdV2(const ClUsbCmdV2 &srcCmd);							//copy constructor override
	ClUsbCmdV2(const char *pcCmd, qint32 lLen);
	virtual ~ClUsbCmdV2();

	ClUsbCmdV2 &operator =(const ClUsbCmdV2 &srcCmd);				//代入演算子のoverride

	quint8 getHeaderVer() const { return this->m_ucHeadCmdVer; }			//get from the Packet type
	quint8 getTransId() const { return this->m_ucTransId; }				//get from the Trans ID
	quint16 getCommand() const { return this->m_usCommand; }				//get from the code
    quint32 getDataSize() const { return this->m_ulDataSize; }			//get from the size of data

	QByteArray &getParam();							//get from the param
    quint32 getParam0() const;						//get from the param 0
    quint32 getParam1() const;					//get from the param 1
    quint32 getParam2() const;					//get from the param 2
    quint32 getParam3() const;					//get from the param 3
    quint32 getParam4() const;					//get from the param 3
    quint32 getParam5() const;					//get from the param 3

	void setPacketType(quint8 a_ucHeadCmdVer){ m_ucHeadCmdVer = a_ucHeadCmdVer; }
	void setTransId(quint8 a_ucTransId){ m_ucTransId = a_ucTransId; }
	void setCommand(quint16 a_usCommand){ m_usCommand = a_usCommand; }
    void setDataSize(quint32 a_ulDataSize){ m_ulDataSize = a_ulDataSize; }
	void setParam(QByteArray &qArray);
    void setParam0(quint32 a_ulParam);
    void setParam1(quint32 a_ulParam);
    void setParam2(quint32 a_ulParam);
    void setParam3(quint32 a_ulParam);
    void setParam4(quint32 a_ulParam);
    void setParam5(quint32 a_ulParam);

	void setSessionBit(){ ((PELMO_FUNC_BIT)(&m_ucHeadCmdVer))->BIT._SESSION = 1; }
	void clrSessionBit(){ ((PELMO_FUNC_BIT)(&m_ucHeadCmdVer))->BIT._SESSION = 0; }
	void setFinishBit(){ ((PELMO_FUNC_BIT)(&m_ucHeadCmdVer))->BIT._FINISH = 1; }
	void clrFinishBit(){ ((PELMO_FUNC_BIT)(&m_ucHeadCmdVer))->BIT._FINISH = 0; }
	quint8 getSessionBit() const { return ((PELMO_FUNC_BIT)(&m_ucHeadCmdVer))->BIT._SESSION; }
	quint8 getFinishBit() const { return ((PELMO_FUNC_BIT)(&m_ucHeadCmdVer))->BIT._FINISH; }

	const char *data() const;
    quint32 length() const;
};
//--------------------------------------------------------------------------------
class ClPictCmdV2 : public ClUsbCmdV2
{
public:
	ClPictCmdV2();
	ClPictCmdV2(qint32 lParam);
	ClPictCmdV2(const char *pcCmd, qint32 lLen);

	QByteArray *getJpegArray(){ return &m_qJpegArray; }
	void setJpegArray(QByteArray &qArray){ m_qJpegArray = qArray; }

private:
	QByteArray m_qJpegArray;			//for area of Jpeg data
};
//--------------------------------------------------------------------------------
class ClZoomTeleCmdV2 : public ClUsbCmdV2
{
public:
	ClZoomTeleCmdV2();
};
//--------------------------------------------------------------------------------
class ClZoomWideCmdV2 : public ClUsbCmdV2
{
public:
	ClZoomWideCmdV2();
};
//--------------------------------------------------------------------------------
class ClZoomStopCmdV2 : public ClUsbCmdV2
{
public:
	ClZoomStopCmdV2();
};
//--------------------------------------------------------------------------------
class ClZoomVariantSetCmdV2 : public ClUsbCmdV2
{
public:
	ClZoomVariantSetCmdV2(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClZoomStatusCmdV2 : public ClUsbCmdV2
{
public:
	ClZoomStatusCmdV2();
};
//--------------------------------------------------------------------------------
class ClFocusNearCmdV2 : public ClUsbCmdV2
{
public:
	ClFocusNearCmdV2();
};
//--------------------------------------------------------------------------------
class ClFocusFarCmdV2 : public ClUsbCmdV2
{
public:
	ClFocusFarCmdV2();
};
//--------------------------------------------------------------------------------
class ClAutoFocusCmdV2 : public ClUsbCmdV2
{
public:
	ClAutoFocusCmdV2();
};
//--------------------------------------------------------------------------------
class ClFocusStopCmdV2 : public ClUsbCmdV2
{
public:
	ClFocusStopCmdV2();
};
//--------------------------------------------------------------------------------
class ClFocusVariantSetCmdV2 : public ClUsbCmdV2
{
public:
	ClFocusVariantSetCmdV2(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClFocusStatusCmdV2 : public ClUsbCmdV2
{
public:
	ClFocusStatusCmdV2();
};
//--------------------------------------------------------------------------------
class ClAFModeSetCmdV2 : public ClUsbCmdV2
{
public:
	ClAFModeSetCmdV2(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClAFModeStatusCmdV2 : public ClUsbCmdV2
{
public:
	ClAFModeStatusCmdV2();
};
//--------------------------------------------------------------------------------
class ClIrisOpenCmdV2 : public ClUsbCmdV2
{
public:
	ClIrisOpenCmdV2();
};
//--------------------------------------------------------------------------------
class ClIrisCloseCmdV2 : public ClUsbCmdV2
{
public:
	ClIrisCloseCmdV2();
};
//--------------------------------------------------------------------------------
class ClIrisStopCmdV2 : public ClUsbCmdV2
{
public:
	ClIrisStopCmdV2();
};
//--------------------------------------------------------------------------------
class ClIrisInitCmdV2 : public ClUsbCmdV2
{
public:
	ClIrisInitCmdV2();
};
//--------------------------------------------------------------------------------
class ClIrisVariantSetCmdV2 : public ClUsbCmdV2
{
public:
	ClIrisVariantSetCmdV2(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClIrisStatusCmdV2 : public ClUsbCmdV2
{
public:
	ClIrisStatusCmdV2();
};
//--------------------------------------------------------------------------------
class ClBRTModeSetCmdV2 : public ClUsbCmdV2
{
public:
	ClBRTModeSetCmdV2(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClBRTModeStatusCmdV2 : public ClUsbCmdV2
{
public:
	ClBRTModeStatusCmdV2();
};
//--------------------------------------------------------------------------------
class ClStartStreamCmdV2 : public ClUsbCmdV2
{
public:
	ClStartStreamCmdV2();
	ClStartStreamCmdV2(qint32 lParam);
	ClStartStreamCmdV2(const char *pcCmd, qint32 lLen);

	QByteArray *getJpegArray(){ return &m_qJpegArray; }
	void setJpegArray(QByteArray &qArray){ m_qJpegArray = qArray; }

private:
	QByteArray m_qJpegArray;			//for area of Jpeg data
};
//--------------------------------------------------------------------------------
class ClStopStreamCmdV2 : public ClUsbCmdV2
{
public:
	ClStopStreamCmdV2();
	ClStopStreamCmdV2(qint32 lParam);
	ClStopStreamCmdV2(const char *pcCmd, qint32 lLen);

	QByteArray *getJpegArray(){ return &m_qJpegArray; }
	void setJpegArray(QByteArray &qArray){ m_qJpegArray = qArray; }

private:
	QByteArray m_qJpegArray;			//for area of Jpeg data
};
//--------------------------------------------------------------------------------
class ClUpImgFileCmdV2 : public ClUsbCmdV2
{
public:
	ClUpImgFileCmdV2();
};
//--------------------------------------------------------------------------------
class ClUpFwFileCmdV2 : public ClUsbCmdV2
{
public:
	ClUpFwFileCmdV2();

	qint16 getTotalSize(){ return m_lTotalSize; }
	void setTotalSize(qint16 lParam){ m_lTotalSize = lParam; }
	qint16 getWroteSize(){ return m_lWroteSize; }
	void setWroteSize(qint16 lParam){ m_lWroteSize = lParam; }
	qint32 getStatus(){ return m_lStatus; }
	void setStatus(qint32 lParam){ m_lStatus = lParam; }

private:
	qint16 m_lTotalSize;			//tortal file size
	qint16 m_lWroteSize;			//wrote file size
	qint32 m_lStatus;				//status
};
//--------------------------------------------------------------------------------
class ClModelNameCmdV2 : public ClUsbCmdV2
{
public:
	ClModelNameCmdV2();

	QString getModelName(){ return m_strModelName; }
	void setModelName(QString &qString){ m_strModelName = qString; }

private:
	QString m_strModelName;			//for area of model name
};
//--------------------------------------------------------------------------------
class ClSHVerCmdV2 : public ClUsbCmdV2
{
public:
	ClSHVerCmdV2();

	QString getVersion(){ return m_strVersion; }
	void setVersion(QString &qString){ m_strVersion = qString; }

private:
	QString m_strVersion;			//for area of version
};
//--------------------------------------------------------------------------------
class ClLensVerCmdV2 : public ClUsbCmdV2
{
public:
	ClLensVerCmdV2();

	QString getVersion(){ return m_strVersion; }
	void setVersion(QString &qString){ m_strVersion = qString; }

private:
	QString m_strVersion;			//for area of version
};
//--------------------------------------------------------------------------------
class ClFPGAVerCmdV2 : public ClUsbCmdV2
{
public:
	ClFPGAVerCmdV2();

	QString getVersion(){ return m_strVersion; }
	void setVersion(QString &qString){ m_strVersion = qString; }

private:
	QString m_strVersion;			//for area of version
};
//--------------------------------------------------------------------------------
class ClProductCodeCmdV2 : public ClUsbCmdV2
{
public:
	ClProductCodeCmdV2();

	QByteArray getProductCode(){ return m_clProductCode; }
	void setProductCode(QByteArray &clParam){ m_clProductCode = clParam; }

private:
	QByteArray m_clProductCode;
};
//--------------------------------------------------------------------------------
class ClCurrentModeCmdV2 : public ClUsbCmdV2
{
public:
	ClCurrentModeCmdV2();

	QStringList getCurrentMode(){ return m_clCurrentMode; }
	void setCurrentMode(QStringList &clParam){ m_clCurrentMode = clParam; }

private:
	QStringList m_clCurrentMode;
};
//--------------------------------------------------------------------------------
class ClResSwCmdV2 : public ClUsbCmdV2
{
public:
	ClResSwCmdV2();

	QByteArray getResSw(){ return m_clResSw; }
	void setResSw(QByteArray &clParam){ m_clResSw = clParam; }

private:
	QByteArray m_clResSw;
};
//--------------------------------------------------------------------------------
class ClImgOutSwCmdV2 : public ClUsbCmdV2
{
public:
	ClImgOutSwCmdV2();

	QByteArray getImgOutSw(){ return m_clImgOutSw; }
	void setImgOutSw(QByteArray &clParam){ m_clImgOutSw = clParam; }

private:
	QByteArray m_clImgOutSw;
};
//--------------------------------------------------------------------------------
class ClCapPushFlgSwCmdV2 : public ClUsbCmdV2
{
public:
	ClCapPushFlgSwCmdV2();

	QByteArray getCapPushFlgSw(){ return m_clCapPushFlgSw; }
	void setCapPushFlgSw(QByteArray &clParam){ m_clCapPushFlgSw = clParam; }

private:
	QByteArray m_clCapPushFlgSw;
};
//--------------------------------------------------------------------------------
class ClSysTimeCmdV2 : public ClUsbCmdV2
{
public:
	ClSysTimeCmdV2();
};
//--------------------------------------------------------------------------------
class ClModelNameSetCmdV2 : public ClUsbCmdV2
{
public:
	ClModelNameSetCmdV2();
};
//--------------------------------------------------------------------------------
class ClProductCodeSetCmdV2 : public ClUsbCmdV2
{
public:
	ClProductCodeSetCmdV2();
};
//--------------------------------------------------------------------------------
class ClWhiteSetCmdV2 : public ClUsbCmdV2
{
public:
	ClWhiteSetCmdV2(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClWhiteStatusCmdV2 : public ClUsbCmdV2
{
public:
	ClWhiteStatusCmdV2();
};
//--------------------------------------------------------------------------------
class ClFlickerSetCmdV2 : public ClUsbCmdV2
{
public:
	ClFlickerSetCmdV2(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClFlickerStatusCmdV2 : public ClUsbCmdV2
{
public:
	ClFlickerStatusCmdV2();
};
//--------------------------------------------------------------------------------
class ClImageModeSetCmdV2 : public ClUsbCmdV2
{
public:
	ClImageModeSetCmdV2(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClImageModeStatusCmdV2 : public ClUsbCmdV2
{
public:
	ClImageModeStatusCmdV2();
};
//--------------------------------------------------------------------------------
class ClEdgeSetCmdV2 : public ClUsbCmdV2
{
public:
	ClEdgeSetCmdV2(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClEdgeStatusCmdV2 : public ClUsbCmdV2
{
public:
	ClEdgeStatusCmdV2();
};
//--------------------------------------------------------------------------------
class ClGammaSetCmdV2 : public ClUsbCmdV2
{
public:
	ClGammaSetCmdV2(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClGammaStatusCmdV2 : public ClUsbCmdV2
{
public:
	ClGammaStatusCmdV2();
};
//--------------------------------------------------------------------------------
class ClFreezeSetCmdV2 : public ClUsbCmdV2
{
public:
	ClFreezeSetCmdV2(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClFreezeStatusCmdV2 : public ClUsbCmdV2
{
public:
	ClFreezeStatusCmdV2();
};
//--------------------------------------------------------------------------------
class ClRotationSetCmdV2 : public ClUsbCmdV2
{
public:
	ClRotationSetCmdV2(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClRotationStatusCmdV2 : public ClUsbCmdV2
{
public:
	ClRotationStatusCmdV2();
};
//--------------------------------------------------------------------------------
class ClNegaPosiSetCmdV2 : public ClUsbCmdV2
{
public:
	ClNegaPosiSetCmdV2(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClNegaPosiStatusCmdV2 : public ClUsbCmdV2
{
public:
	ClNegaPosiStatusCmdV2();
};
//--------------------------------------------------------------------------------
class ClColorMonoSetCmdV2 : public ClUsbCmdV2
{
public:
	ClColorMonoSetCmdV2(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClColorMonoStatusCmdV2 : public ClUsbCmdV2
{
public:
	ClColorMonoStatusCmdV2();
};
//--------------------------------------------------------------------------------
class ClScopeSetCmdV2 : public ClUsbCmdV2
{
public:
	ClScopeSetCmdV2(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClScopeStatusCmdV2 : public ClUsbCmdV2
{
public:
	ClScopeStatusCmdV2();
};
//--------------------------------------------------------------------------------
class ClSpeakerSetCmdV2 : public ClUsbCmdV2
{
public:
	ClSpeakerSetCmdV2(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClSpeakerStatusCmdV2 : public ClUsbCmdV2
{
public:
	ClSpeakerStatusCmdV2();
};
//--------------------------------------------------------------------------------
class ClVideoSetCmdV2 : public ClUsbCmdV2
{
public:
	ClVideoSetCmdV2(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClVideoStatusCmdV2 : public ClUsbCmdV2
{
public:
	ClVideoStatusCmdV2();
};
//--------------------------------------------------------------------------------
class ClLanguageSetCmdV2 : public ClUsbCmdV2
{
public:
	ClLanguageSetCmdV2(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClLanguageStatusCmdV2 : public ClUsbCmdV2
{
public:
	ClLanguageStatusCmdV2();
};
//--------------------------------------------------------------------------------
class ClGuideSetCmdV2 : public ClUsbCmdV2
{
public:
	ClGuideSetCmdV2(qint32 lParam);
};
//--------------------------------------------------------------------------------
class ClGuideStatusCmdV2 : public ClUsbCmdV2
{
public:
	ClGuideStatusCmdV2();
};
//--------------------------------------------------------------------------------
class ClSaveSettingCmdV2 : public ClUsbCmdV2
{
public:
	ClSaveSettingCmdV2();
};
//--------------------------------------------------------------------------------
class ClLoadSettingCmdV2 : public ClUsbCmdV2
{
public:
	ClLoadSettingCmdV2();
};
//--------------------------------------------------------------------------------
QDataStream &operator << (QDataStream &out, const ClUsbCmdV2 &clCmd);		//override << operator for ClUsbCmd to QDataStream
QDataStream &operator >> (QDataStream &in, ClUsbCmdV2 &clCmd);				//
QDataStream &operator >> (QDataStream &in, ClPictCmdV2 &clCmd);				//
QDataStream &operator >> (QDataStream &in, ClStartStreamCmdV2 &clCmd);		//
QDataStream &operator >> (QDataStream &in, ClStopStreamCmdV2 &clCmd);		//
QDataStream &operator >> (QDataStream &in, ClModelNameCmdV2 &clCmd);		//
QDataStream &operator >> (QDataStream &in, ClSHVerCmdV2 &clCmd);			//
QDataStream &operator >> (QDataStream &in, ClLensVerCmdV2 &clCmd);			//
QDataStream &operator >> (QDataStream &in, ClFPGAVerCmdV2 &clCmd);			//
QDataStream &operator >> (QDataStream &in, ClUpFwFileCmdV2 &clCmd);			//


Q_DECLARE_METATYPE(ClUsbCmdV2)
Q_DECLARE_METATYPE(ClPictCmdV2)

#endif // CLUSBCMDV2_H
