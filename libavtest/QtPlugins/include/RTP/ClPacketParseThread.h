//--------------------------------------------------------------------------------//
/*!
	@file	ClPacketParseThread.h
	@brief	パケット解析スレッドヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLPACKETPARSETHREAD_H
#define CLPACKETPARSETHREAD_H
#ifdef USE_G726
#include "g726/ClG726.h"
#endif
#include "ClRtpPacket.h"
#include <QThread>
#include <QImage>
#include <QMap>
#include <QAudioOutput>
#include <QMutex>
#include <QFile>

class ClH264Decoder;

//--------------------------------------------------------------------------------//
/*!
	@brief	パケット解析スレッド
*/
//--------------------------------------------------------------------------------//
class ClPacketParseThread : public QThread
{
	Q_OBJECT

private:
	typedef struct
	{
		QByteArray clPacket;	//パケット
		QString strStream;		//URI
	}stPacketInfo;

public:
	ClPacketParseThread();	//コンストラクタ
	~ClPacketParseThread();	//デストラクタ

	void stop();	//スレッド停止
	void setMute(bool bIsMute);	//ミュート設定

	/*!
		@brief		音声出力使用設定
		@param[in]	bIsUse : 出力するならtrue
	*/
	void setUseAudio(bool bIsUse){m_bIsUseAudio = bIsUse;}
	/*!
		@brief		音量設定
		@param[in]	dVolume : 元データからの割合
	*/
	void setVolume(qreal dVolume){m_dVolume = dVolume;}

protected:
	void run();		//メイン処理
	virtual void parseReceivePacket(QString strStream, ClRtpPacket clPacketInfo);	//受信パケット解析
	virtual void playAudio(QByteArray clData);	//音声再生

public slots:
	void slotSaveTest(bool bIsSave);	//保存テスト設定

protected slots:
	void slotAddPacket(QByteArray clPacket, QString strStream);	//パケットデータ追加
	void slotH264Image(QImage clImage);	//h264デコード画像受信

signals:
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
		@brief		画像生成失敗通知
		@param[in]	strStream : ストリーム名
		@param[in]	clImageData : 画像データ
		@param[in]	ulTimeStamp : タイムスタンプ
	*/
	void sigImageError(QString strStream, QByteArray clImageData, quint32 ulTimeStamp);
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

protected:
	bool m_bIsStop;	//!< 停止フラグ
	QMap<QString, QMap<quint32, QMap<quint16, QByteArray> > > m_clAccumulatePacketList;	//!< パケットデータリスト
	QList<stPacketInfo> m_clWaitPacketList;	//!< 処理待ちパケットリスト
	QAudioOutput *m_pclAudioOutput;	//!< 音声出力クラス
	QMutex m_clPacketControlMutex;	//!< パケットリスト操作ミューテックス
	QIODevice *m_pclAudioBuffer;	//!< 音声書き込み先
	bool m_bIsMute;					//!< ミュートフラグ
	qint16 m_sG726Reset;			//!< G726デコーダリセットフラグ
	bool m_bIsUseAudio;				//!< 音声出力使用設定
	qreal m_dVolume;				//!< 音量割合
#ifdef USE_G726
	ClG726 m_dec726;	//!< G726デコーダ
#endif
#ifdef USE_H264
	ClH264Decoder *m_pclDecoder;	//!< h264デコーダ
	bool m_bIsSaveTest;		//!< 保存テスト設定
	QFile *m_pFileSaveTest;	//!< ファイル保存
	int m_iTestFileNumber;	//!< 保存ファイル番号
#endif
};
//--------------------------------------------------------------------------------//
#endif //CLPACKETPARSETHREAD_H
