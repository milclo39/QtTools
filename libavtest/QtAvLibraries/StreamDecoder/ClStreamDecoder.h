//--------------------------------------------------------------------------------//
/*!
	@file	ClStreamDecoder.h
	@brief	ストリームデコーダヘッダ
	@author	大橋 須田
*/
//--------------------------------------------------------------------------------//
#ifndef AV_CLSTREAMDECODER_H
#define AV_CLSTREAMDECODER_H
#include <QThread>
#include <QQueue>
#include <QImage>
#include <QMutex>
#include "../AvCommon.h"

//MainConceptを使う場合
//#define USE_MAINCONCEPT		1

#ifdef USE_MAINCONCEPT
#include "inc/dec_avc.h"
#endif	//USE_MAINCONCEPT

class ClPlayAudioThread;
//--------------------------------------------------------------------------------//
/*!
	@brief	デコードスレッド
*/
//--------------------------------------------------------------------------------//
struct AVFormatContext;
struct AVCodecContext;
struct AVPacket;

class ClDecodeFrame : public QThread
{
	Q_OBJECT

public:
	ClDecodeFrame(QObject *pParent = NULL);	//コンストラクタ
	~ClDecodeFrame();

	bool init(AVCodecContext *pContextVideo, qreal dPts, qreal dDurationAvg);	//初期化
	void addPacket(AVPacket *pAvFrame);

#ifdef USE_MAINCONCEPT
	// MCDecoder
	void decode_init(void);
	qint32 decode_frame(AVPacket*);
	void decode_done(void);
#endif
protected:
	void run();	//メイン処理

signals:
	/*!
	 * \brief 画像通知
	 * \param img : 画像
	 */
	void sigImage(ClAvImage img);

	void sigChangeResolution(QSize size);

private:
	bool m_bIsStop;	//!< 停止フラグ
	QQueue<AVPacket*> m_listPacketData;	//!< パケットデータリスト
	QMutex mutex;
	qreal m_dPts;
	qreal m_dDurationAvg;
	AVCodecContext *m_pContextVideo;
	QSize m_nowResolution;				//!< 解像度切り替え対応

#ifdef USE_MAINCONCEPT
	bufstream_tt *videobs;
#endif
};

//--------------------------------------------------------------------------------//
/*!
	@brief	ストリームデコーダ
*/
//--------------------------------------------------------------------------------//
class ClStreamDecoder : public QThread
{
	Q_OBJECT

public:
	ClStreamDecoder(QObject *pParent);	//コンストラクタ
	~ClStreamDecoder();	//デストラクタ

	static void avStreamInit();	//初期化
	static void avStreamDeinit();	//終了

	void setStream(QString strStream);	//ストリーム設定
	void setUsesTcp(bool bUsesTcp);
	void stop();	//停止

protected:
	void run();	//メイン処理

signals:
	/*!
	 * \brief エラー通知
	 */
	void sigError();
	/*!
	 * \brief 画像通知
	 * \param img : 画像
	 */
	void sigImage(ClAvImage img);

	void sigChangeResolution(QSize size);

protected:
	QString m_strStream;	//!< デコードストリーム
	bool m_bIsStop;	//!< 停止フラグ
	ClDecodeFrame *m_pThreadDecode;		//!< 変換スレッド
	ClPlayAudioThread *m_pThreadAudio;	//!< 音声再生スレッド
	bool m_bUsesTcp;					//!< TCP
};
//--------------------------------------------------------------------------------//
#endif
