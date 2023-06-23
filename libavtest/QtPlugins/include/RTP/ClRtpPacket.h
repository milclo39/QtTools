//--------------------------------------------------------------------------------//
/*!
	@file	ClRTPPacket.h
	@brief	RTPパケットヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLRTPPACKET_H
#define CLRTPPACKET_H
#include <QByteArray>
#include <QVector>

//--------------------------------------------------------------------------------//
/*!
	@brief	RTPパケット
*/
//--------------------------------------------------------------------------------//
class ClRtpPacket
{
private:
	typedef union
	{
#ifdef Q_LITTLE_ENDIAN
		struct
		{
			quint8 ucCsrcCount:4;
			bool bExtension:1;
			bool bPadding:1;
			quint8 ucVersion:2;
		}bits;
#else
		struct
		{
			quint8 ucVersion:2;
			bool bPadding:1;
			bool bExtension:1;
			quint8 ucCsrcCount:4;
		}bits;
#endif
		quint8 data;
	}uniFirstByte;

	typedef union
	{
#ifdef Q_LITTLE_ENDIAN
		struct
		{
			quint8 ucPayload:7;
			bool bMarker:1;
		}bits;
#else
		struct
		{
			bool bMarker:1;
			quint8 ucPayload:7;
		}bits;
#endif
		quint8 data;
	}uniSecondByte;

public:
	ClRtpPacket();	//コンストラクタ

	bool parse(QByteArray clPacket);	//RTPヘッダ解析
	bool createPacket(QByteArray clPayloadData);	//RTPパケット生成

	QByteArray getPacketData(){return m_clPacketData;}
	QByteArray getRtpHeader(){return m_clPacketData.left(m_lHeaderLength);}
	qint32 getRtpHeaderSize(){return m_lHeaderLength;}

	quint8 getVersion(){return m_uniFirst.bits.ucVersion;}
	bool getPadding(){return m_uniFirst.bits.bPadding;}
	bool getExtension(){return m_uniFirst.bits.bExtension;}
	quint8 getCsrcCount(){return m_uniFirst.bits.ucCsrcCount;}
	bool getMarker(){return m_uniSecond.bits.bMarker;}
	quint8 getPayload(){return m_uniSecond.bits.ucPayload;}
	quint16 getSequenceNumber(){return m_usSequenceNumber;}
	quint32 getTimeStamp(){return m_ulTimeStamp;}
	quint32 getSsrc(){return m_ulSsrc;}
	QVector<quint32> getCsrc(){return m_clCsrclist;}
	quint16 getExDefinedProfile(){return m_usExDefinedProfile;}
	quint16 getExLength(){return m_usExLength;}
	QByteArray getExHeaderData(){return m_clExHeaderData;}

	void setVersion(quint8 ucVersion){m_uniFirst.bits.ucVersion = ucVersion;}
	void setPadding(bool bPadding){m_uniFirst.bits.bPadding = bPadding;}
	void setExtension(bool bExtension){m_uniFirst.bits.bExtension = bExtension;}
	void setCsrcCount(quint8 ucCsrcCount){m_uniFirst.bits.ucCsrcCount = ucCsrcCount;}
	void setMarker(bool bMarker){m_uniSecond.bits.bMarker = bMarker;}
	void setPayload(quint8 ucPayload){m_uniSecond.bits.ucPayload = ucPayload;}
	void setSequenceNumber(quint16 usSequenceNumber){m_usSequenceNumber = usSequenceNumber;}
	void setTimeStamp(quint32 ulTimeStamp){m_ulTimeStamp = ulTimeStamp;}
	void setSsrc(quint32 ulSsrc){m_ulSsrc = ulSsrc;}
	void setCsrc(QVector<quint32> clCsrclist){m_clCsrclist = clCsrclist;}
	void setExDefinedProfile(quint16 usExDefinedProfile){m_usExDefinedProfile = usExDefinedProfile;}
	void setExLength(quint16 usExLength){m_usExLength = usExLength;}
	void setExHeaderData(QByteArray clExHeaderData){m_clExHeaderData = clExHeaderData;}

protected:
	void addData(QByteArray *pclData, quint32 ulValue, qint16 sSize);	//データ追加

protected:
	QByteArray m_clPacketData;		//!< パケットデータ
	qint32 m_lHeaderLength;			//!< RTPヘッダ長

	uniFirstByte m_uniFirst;		//!< 1バイト目データ
	uniSecondByte m_uniSecond;		//!< 2バイト目データ
	quint16 m_usSequenceNumber;		//!< シーケンス番号
	quint32 m_ulTimeStamp;			//!< タイムスタンプ
	quint32 m_ulSsrc;				//!< SSRC
	QVector<quint32> m_clCsrclist;	//!< CSRCリスト
	quint16 m_usExDefinedProfile;	//!< 拡張ヘッダ情報
	quint16 m_usExLength;			//!< 拡張ヘッダ長
	QByteArray m_clExHeaderData;	//!< 拡張ヘッダデータ
};
//--------------------------------------------------------------------------------//
#endif //CLRTPPACKET_H
