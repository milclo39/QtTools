//--------------------------------------------------------------------------------//
/*!
	@file	ClAudioThread.h
	@brief	音声制御スレッドヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLAUDIOTHREAD_H
#define CLAUDIOTHREAD_H

#include <QThread>
#include <QImage>
#include <QWaitCondition>
#include <QAudioOutput>
#include <QMutex>
#include "ClAudioDecoder.h"

//--------------------------------------------------------------------------------//
/*!
	@ingroup	groupMp4Decoder
	@brief		音声制御スレッド
*/
//--------------------------------------------------------------------------------//
class ClAudioThread : public QThread
{
	//!< 音声データ構造体
	typedef struct
	{
		QByteArray clAudio;	//!< 音声データ
		qint64 llTime;		//!< タイムスタンプ
	}stAudioData;

	Q_OBJECT

public:
	ClAudioThread();	//コンストラクタ
	~ClAudioThread();	//デストラクタ
	void setInfo(ClAudioDecoder *pclDecoder, QAudioOutput *pclDevice,	//デコードに必要な情報設定
				 QWaitCondition *pclWait);
	void setStartTime(qint64 llStartTime);		//再生開始時間設定
	void stop();	//スレッド停止

	void pause();					//一時停止
	void seek(qint64 llTime);		//シーク
	void setVolume(qreal dVolume);	//音量設定
	void changeMute();				//ミュート
	/*!
		@brief	ストリーム終了フラグ解除
	*/
	void releaseEndStreamFlag(){m_bIsEndAudioStream = false;}
	void setAudio(QByteArray clAudio, qint64 llTime);	//デコード音声受信
	void endStream();									//ストリーム終了通知
	/*!
		@brief		現在の時間取得
		@retval		現在の時間
	*/
	qint64 getNowTime(){return m_llNowTimeAudio;}

protected:
	void run();	//メイン処理
	void writeData(QByteArray clData);	//データ書き込み

signals:
	/*!
		@brief		再生時間通知
		@param[in]	llTime 現在の時間
	*/
	void sigNewTime(qint64 llTime);
	/*!
		@brief		音声終了通知
	*/
	void sigFinishedAudio();

protected:
	ClAudioDecoder *m_pclAudioDecoder;	//!< 音声デコーダ
	QList<stAudioData> m_clAudioList;	//!< 音声データリスト
	QAudioOutput *m_pclAudioDevice;		//!< 音声デバイス
	QIODevice *m_pclAudioStream;		//!< 音声再生デバイス
	QWaitCondition *m_pclWaitAudio;		//!< 音声デコーダ用待ち合わせ
	qint64 m_llStartTime;				//!< 再生開始時間
	qint64 m_llNowTimeAudio;			//!< 最後に書き込んだ音声のタイムスタンプ
	QMutex m_clAudioMutex;				//!< 音声データ操作ミューテックス
	bool m_bIsEndAudioStream;			//!< 音声デコード終了フラグ
	bool m_bIsStop;						//!< スレッド停止フラグ
	qreal m_dVolume;					//!< 音量
	bool m_bIsMute;						//!< ミュートフラグ
};
//--------------------------------------------------------------------------------//
#endif
