//--------------------------------------------------------------------------------//
/*!
	@file	ClAudioDecoder.h
	@brief	音声デコーダヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLAUDIODECODER_H
#define CLAUDIODECODER_H

#include <QObject>
#include <QThread>
#include <QWaitCondition>
#include <QVector>
#include <QVariant>
#include <QAudioFormat>

namespace UMC
{
	class AudioCodec;
	class MP4Splitter;
	typedef struct sAudioStreamInfo AudioStreamInfo;
};
using namespace UMC;	//IPPで使っているnamespace
class ClAudioThread;

/*!
	@addtogroup	groupMp4Decoder
	@{
*/

#define AUDIO_INFO_FORMAT_QT	"audio/pcm"	//!< Qtでのフォーマット宣言
#define AUDIO_INFO_FORMAT_UMC	1			//!< UMCでのフォーマット宣言(PCM)

//--------------------------------------------------------------------------------//
/*!
	@ingroup	groupMp4Decoder
	@brief		音声デコーダ
*/
//--------------------------------------------------------------------------------//
class ClAudioDecoder : public QThread
{
	Q_OBJECT

public:
	ClAudioDecoder(quint32 ulTrackNum, QWaitCondition *pclWait);	//コンストラクタ
	~ClAudioDecoder();	//デストラクタ
	QAudioFormat getInfo();	//音声情報取得
	bool init(MP4Splitter *pumcSplitter);	//初期化
	void stop();						//スレッド停止
	void pause();						//一時停止
	void seek(qreal dTime);				//シーク
	/*!
		@brief		制御スレッド設定
		@param[in]	画像制御スレッド
	*/
	void setThread(ClAudioThread *pclThread){m_pclThread = pclThread;}

protected:
	void run();			//!< メイン処理

signals:
	/*!
		@brief		デコード音声通知
		@param[in]	clAudio デコード音声
		@param[in]	llTime タイムスタンプ
	*/
	void sigAudio(QByteArray clAudio, qint64 llTime);
	/*!
		@brief		ストリーム終了通知
	*/
	void sigEndStream();

protected:
	AudioCodec *m_pumcDecoder;		//!< 音声デコーダ
	MP4Splitter *m_pumcSplitter;	//!< MP4スプリッタ
	AudioStreamInfo *m_pstStreamInfo;	//!< 音声ストリーム情報
	QAudioFormat m_formatAudio;		//!< 音声情報
	quint32 m_ulTrackNum;			//!< トラック番号
	bool m_bIsStop;					//!< スレッド停止フラグ
	QWaitCondition *m_pclWait;		//!< 待ち合わせ用クラス
	qint32 m_lBlockSize;			//!< デコードしたデータの1ブロックのサイズ
	bool m_bIsEndStream;			//!< デコード終了フラグ
	qreal m_dLastTime;				//!< 最後にデコードされたタイムスタンプ
	ClAudioThread *m_pclThread;		//!< 音声制御スレッド
};
//--------------------------------------------------------------------------------//
#endif
