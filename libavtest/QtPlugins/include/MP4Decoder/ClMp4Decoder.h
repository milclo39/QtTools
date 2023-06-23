//--------------------------------------------------------------------------------//
/*!
	@file	ClMp4Decoder.h
	@brief	MP4デコーダヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLMP4DECODER_H
#define CLMP4DECODER_H

#include <QWidget>
#include <QWaitCondition>
#include <QMutex>
#include <QAudioOutput>
#include "ClVideoDecoder.h"
#include "ClAudioDecoder.h"
#include "ClVideoThread.h"
#include "ClAudioThread.h"

//--------------------------------------------------------------------------------//
/*!
	@defgroup	groupMp4Decoder
	@brief		MP4デコーダ
*/
//--------------------------------------------------------------------------------//

namespace UMC
{
	class MP4Splitter;
	class DataReader;
};
using namespace UMC;	//IPPで使っているnamespace

//--------------------------------------------------------------------------------//
/*!
	@ingroup	groupMp4Decoder
	@brief		MP4デコーダ
*/
//--------------------------------------------------------------------------------//
class ClMp4Decoder : public QObject
{
	Q_OBJECT

public:
	ClMp4Decoder();						//コンストラクタ
	~ClMp4Decoder();					//デストラクタ
	bool setFile(QString strFileName);	//ファイル設定

	QImage getImage();			//現在の画像取得
	qint64 getDuration();		//トータル時間取得
	qint64 getNowTime();		//現在の時間取得
	void waitEndThread();		//スレッド終了待ち
	/*!
		@brief	音声出力デバイス取得
		@retval	音声出力デバイス
	*/
	QAudioOutput *getAudioDevice(){return m_pclAudioDevice;}

protected:
	bool init();				//初期化
	bool initSplitter();		//スプリッタ初期化
	bool initVideoDecoder();	//画像デコーダ初期化
	bool initAudioDecoder();	//音声デコーダ初期化
	bool initAudioDevice();		//音声デバイス初期化
	bool reset();				//再初期化

public slots:
	void slotPlay();				//現在位置から再生
	void slotPause();				//一時停止
	void slotSeek(qint64 llTime);	//シーク
	void slotVolume(qreal dVolume);	//音量設定
	void slotChangeMute();			//ミュート

protected slots:
	void slotFinishedVideo();		//画像終了
	void slotFinishedAudio();		//音声終了
	void slotSkipFrame(qint64 llSkipTime);	//画像スキップ

signals:
	/*!
		@brief		画像取得返信
		@param[in]	clImage 取得した画像
		@param[in]	llTime 画像の時刻
	*/
	void sigNewImage(QImage clImage, qint64 llTime);
	/*!
		@brief		画像取得返信
		@param[in]	clImage 取得した画像
	*/
	void sigNewImage(QImage clImage);
	/*!
		@brief		再生時間通知
		@param[in]	llTime 現在の時間
	*/
	void sigNewTime(qint64 llTime);
	/*!
		@brief	再生終了通知
	*/
	void sigFinished();
	/*!
		@brief		エラー通知
		@param[in]	エラー文字列
	*/
	void sigError(QString strError);

protected:
	MP4Splitter *m_pumcSplitter;		//!< MP4スプリッタ
	DataReader *m_pumcDataReader;		//!< データリーダ
	ClVideoDecoder *m_pclVideoDecoder;	//!< 画像デコーダ
	ClAudioDecoder *m_pclAudioDecoder;	//!< 音声デコーダ
	ClVideoThread m_clVideoThread;		//!< 画像制御スレッド
	ClAudioThread m_clAudioThread;		//!< 音声制御スレッド
	QAudioOutput *m_pclAudioDevice;		//!< 音声出力デバイス
	bool m_bIsInit;						//!< 初期化フラグ
	bool m_bIsEndVideoStream;			//!< 画像再生終了フラグ
	bool m_bIsEndAudioStream;			//!< 音声再生終了フラグ
	qint64 m_llSeekTime;				//!< シーク時間
	QWaitCondition m_clWaitVideo;		//!< 画像デコーダ用待ち合わせ
	QWaitCondition m_clWaitAudio;		//!< 音声デコーダ用待ち合わせ
	qreal m_dBeforeSkipTime;			//!< 前回のスキップ時間
	qint64 m_llSkipTimeLimit;			//!< スキップできる限界時間
};
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@ingroup	groupMp4Decoder
	@brief		デコーダを持つウィジェット
*/
//--------------------------------------------------------------------------------//
class ClMp4DecoderWidget : public QWidget
{
	Q_OBJECT

public:
	ClMp4DecoderWidget(QWidget *pclParent = NULL);	//コンストラクタ
	/*!
		@brief	デコーダ取得
		@retval	デコーダ
	*/
	ClMp4Decoder *getDecoder(){return &m_clDecoder;}

private:
	ClMp4Decoder m_clDecoder;	//!< デコーダ
};
//--------------------------------------------------------------------------------//
#endif
