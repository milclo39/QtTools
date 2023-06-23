//--------------------------------------------------------------------------------//
/*!
	@file	ClRtp.h
	@brief	RTP通信クラスヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLRTP_H
#define CLRTP_H
#include "ClPacketParseThread.h"
#include "ClRtpPacket.h"
#include <QTcpSocket>
#include <QTimer>
#include <QHostAddress>

//RTSPコマンド
#define COMMAND_OPTIONS		("OPTIONS")
#define COMMAND_DESCRIBE	("DESCRIBE")
#define COMMAND_SETUP		("SETUP")
#define COMMAND_PLAY		("PLAY")
#define COMMAND_PAUSE		("PAUSE")
#define COMMAND_TEARDOWN	("TEARDOWN")

//コマンドパラメータ
typedef QMap<QString, QString> rtspCommandParam;

//コマンドパラメータ型
#define COMMAND_PARAM_STREAMTYPE	("StreamType")
#define COMMAND_PARAM_PROTOCOL		("Protocol")
#define COMMAND_PARAM_SESSION		("Session")
#define COMMAND_PARAM_AUTH_ID		("Auth_ID")
#define COMMAND_PARAM_AUTH_PASSWORD	("Auth_Password")
#define COMMAND_PARAM_USE_AUDIO		("Use_Audio")		//音声使用しないならvalueに"OFF"を設定
#define COMMAND_PARAM_RTP_PORT		("RTP_Port")

//--------------------------------------------------------------------------------//
/*!
	@brief	RTP通信クラス
*/
//--------------------------------------------------------------------------------//
class ClRtp : public QThread
{
	Q_OBJECT

public:
	//メディア情報
	typedef struct{
		QString strMediaControl;	//RTSPアドレス
		QString strMedia;			//メディア（audio,video)
		int iPayloadType;			//ペイロードタイプ
	}stMediaInfo;

	//ストリーム情報
	typedef struct _stStreamInfo{
		_stStreamInfo()
		{
			pclSocket = NULL;
			usServerPort = 0;
		}
		QString strSession;				//セッション番号
		QString strMedia;				//メディア(audio,video)
		QString strMediaControl;		//RTSPアドレス
		QAbstractSocket *pclSocket;		//ソケット
		quint16 usServerPort;			//サーバポート
	}stStreamInfo;

	//定期通知情報
	typedef struct{
		QTimer *pclTimer;		//タイマ
		QString strCommand;		//送信コマンド
		rtspCommandParam clParam;	//コマンドパラメータ
	}stIntervalInfo;

public:
	ClRtp(QObject *pclParent = NULL);	//コンストラクタ
	virtual ~ClRtp();	//デストラクタ

	void initializeCommunication(quint16 usPortNumber = 554);	//通信初期化
	void setIntervalCommand(QString strCommand, qint32 lInterval, rtspCommandParam clParam);	//定期通知設定

	/*!
		@brief		通信先アドレス設定
		@param[in]	strAddress : アドレス
	*/
	void setAddress(QString strAddress){m_clAddress = strAddress;}

	/*!
		@brief		RTSPバージョン設定
		@param[in]	strVersion : バージョン
	*/
	void setRtspVersion(QString strVersion){m_strRtspVersion = strVersion;}

	/*!
		@brief		ストリームリスト取得
		@retval		ストリームリスト
	*/
	QList<ClRtp::stMediaInfo> getStreamList(){return m_clStreamMediaList;}

	/*!
		@brief	ストリーム情報リスト取得
		@retval	ストリーム情報リスト
	*/
	QList<ClRtp::stStreamInfo*> getStreamInfoList(){return m_clStreamInfoList;}

	/*!
		@brief		RTSPタイムアウト設定
		@param[in]	lTimeout : タイムアウト時間(ミリ秒),0を設定するとタイムアウトなし
	*/
	void setRtspTimeout(qint32 lTimeout){m_lRtspTimeout = lTimeout;}

	void disconnectRtsp();	//RTSP切断

protected:
	virtual void timerEvent(QTimerEvent *pclEvent);			//タイマイベント
	virtual void options(rtspCommandParam clParam);			//OPTIONS送信
	virtual void describe(rtspCommandParam clParam);		//DESCRIBE送信
	virtual void setup(rtspCommandParam clParam);			//SETUP送信
	virtual void play(rtspCommandParam clParam);			//PLAY送信
	virtual void pause( rtspCommandParam clParam);			//PAUSE送信
	virtual void teardown(rtspCommandParam clParam);		//TEARDOWN送信
	virtual void get_parameter(rtspCommandParam clParam);	//GET_PARAMETER送信
	virtual void set_parameter(rtspCommandParam clParam);	//SET_PARAMETER送信

	void addCSeq(QByteArray *pclPacket, qint32 lNumber);	//CSeq追加
	void addAccept(QByteArray *pclPacket);					//Accept追加
	void addTransport(QByteArray *pclPacket, QString strProtocol, QString strPort);	//Transport追加
	void addSession(QByteArray *pclPacket, QString strSession);	//Session追加
	void addAuthorization(QByteArray *pclPacket, QString strId, QString strPassword);	//Authorization追加

	virtual qint32 parseOptions(QByteArray clPacketAll);	//OPTIONS返信解析
	virtual qint32 parseDescribe(QByteArray clPacketAll);	//DESCRIBE返信解析
	virtual qint32 parseSetup(QByteArray clPacketAll);	//SEUPT返信解析
	virtual qint32 parsePlay(QByteArray clPacketAll);		//PLAY返信解析
	virtual qint32 parsePause(QByteArray clPacketAll);	//PAUSE返信解析
	virtual qint32 parseTeardown(QByteArray clPacketAll);	//TEARDOWN返信解析

	qint32 parseResponse(QByteArray clPacket);	//返信解析

public slots:
	void slotSetMute(bool bIsMute);	//ミュート設定
	void slotSetVolume(qreal dVolume);	//音量設定

protected slots:
	void slotRead();	//データ読み込み可能状態受信
	void slotSendRtspCommand(QString strCommand, rtspCommandParam clParam);	//コマンド送信
	void slotSocketStateChange(QAbstractSocket::SocketState eState);	//ソケット状態変更受信
	void slotIntervalFired();	//定期通信タイマ通知
	void slotSendStream(ClRtpPacket clPacket, QString strMediaControl);	//パケット送信

signals:
	/*!
		@brief		通信準備完了通知
		@param[in]	bIsReady : 準備完了したらtrue
	*/
	void sigReadyCommunication(bool bIsReady);
	/*!
		@brief		RTSP結果通知
		@param[in]	strCommand : コマンド
		@param[in]	lResult : 結果値(通信不可状態ならば-1)
	*/
	void sigResponseRtsp(QString strCommand, qint32 lResult);
	/*!
		@brief		画像通知
		@param[in]	clImage : 画像
		@param[in]	lSourceDataSize : 画像データサイズ
	*/
	void sigStreamImage(QImage clImage, qint32 lSourceDataSize);
	/*!
		@brief		JPEGデータ通知
		@param[in]	clData : JPEGデータ
	*/
	void sigStreamJpeg(QByteArray clData);
	/*!
		@brief		RTSP送信パケット通知
		@param[in]	clPacket : 送信パケット
	*/
	void sigSendRtspPacket(QByteArray clPacket);
	/*!
		@brief		RTSP受信パケット通知
		@param[in]	clPacket : 受信パケット
	*/
	void sigRecvRtspPacket(QByteArray clPacket);
	/*!
		@brief		RTPパケット通知
		@param[in]	strStream : ストリーム名
		@param[in]	clPacket : パケット情報
	*/
	void sigRtpPacket(QString strStream, ClRtpPacket clPacket);
	/*!
		@brief		RTP通信エラー通知
		@param[in]	strStream : ストリーム名
		@param[in]	ulTimeStamp : タイムスタンプ
		@param[in]	usSequenceNumberOld : 以前のパケットのシーケンス番号
		@param[in]	usSequenceNumberNew : エラーが発生したパケットのシーケンス番号
	*/
	void sigRtpError(QString strStream, quint32 ulTimeStamp, quint16 usSequenceNumberOld, quint16 usSequenceNumberNew);
	/*!
		@brief		画像生成失敗通知
		@param[in]	strStream : ストリーム名
		@param[in]	clImageData : 画像データ
		@param[in]	ulTimeStamp : タイムスタンプ
	*/
	void sigImageError(QString strStream, QByteArray clImageData, quint32 ulTimeStamp);
	/*!
		@brief		新しいパケット通知
		@param[in]	clPacket : パケット
		@param[in]	strStream : ストリーム名
	*/
	void sigNewPacket(QByteArray clPacket, QString strStream);

protected:
	QTcpSocket *m_pclSocketRtsp;	//!< RTSP通信ソケット
	QHostAddress m_clAddress;		//!< 通信先IPアドレス
	QString m_strRtspVersion;		//!< RTSPバージョン
	QString m_strWaitCommand;		//!< 返信待ちコマンド
	static qint32 gm_lCSeqNumber;	//!< CSeq番号
	static qint32 gm_lClientPort;	//!< 受信ポート番号
	QList<stMediaInfo> m_clStreamMediaList;	//!< ストリームメディアリスト
	QList<stStreamInfo*> m_clStreamInfoList;	//!< ストリーム情報リスト
	QByteArray m_clImageData;		//!< 画像データ
	QList<stIntervalInfo*> m_clIntervalInfoList;	//!< 定期通知情報リスト
	ClPacketParseThread *m_pclPacketThread;	//!< パケット解析スレッド
	QString m_strAuthorizationId;		//!< 認証ID
	QString m_strAuthorizationPassword;	//!< 認証パスワード
	qint32 m_lRtspTimeout;	//!< RTSPタイムアウト
	int m_iTimeoutTimerId;	//!< RTSPタイムアウトタイマID
};
//--------------------------------------------------------------------------------//
#endif //CLRTP_H
