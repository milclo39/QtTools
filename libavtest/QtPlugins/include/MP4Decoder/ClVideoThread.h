//--------------------------------------------------------------------------------//
/*!
	@file	ClVideoThread.h
	@brief	画像制御スレッドヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLVIDEOTHREAD_H
#define CLVIDEOTHREAD_H

#include <QThread>
#include <QImage>
#include <QWaitCondition>
#include <QMutex>
#include "ClVideoDecoder.h"

//--------------------------------------------------------------------------------//
/*!
	@ingroup	groupMp4Decoder
	@brief		画像制御スレッド
*/
//--------------------------------------------------------------------------------//
class ClVideoThread : public QThread
{
	Q_OBJECT

	//!< 画像データ構造体
	typedef struct
	{
		QImage clImage;	//!< 画像
		qint64 llTime;	//!< タイムスタンプ
	}stVideoData;

public:
	ClVideoThread();	//コンストラクタ
	~ClVideoThread();	//デストラクタ
	void setInfo(ClVideoDecoder *pclDecoder, QWaitCondition *pclWait);	//デコードに必要な情報設定
	void setStartTime(qint64 llStartTime);		//再生開始時間設定
	void stop();	//スレッド停止

	void pause();				//一時停止
	void seek(qint64 llTime);	//シーク
	/*!
		@brief	ストリーム終了フラグ解除
	*/
	void releaseEndStreamFlag(){m_bIsEndVideoStream = false;}
	void setImgae(QImage clImage, qint64 llTime);	//デコード画像受信
	void endStream();								//ストリーム終了通知
	/*!
		@brief		現在の時間取得
		@retval		現在の時間
	*/
	qint64 getNowTime(){return m_llNowTimeVideo;}

protected:
	void run();	//メイン処理

signals:
	/*!
		@brief		画像取得返信
		@param[in]	clImage 取得した画像
		@param[in]	llTime 画像の時刻ｓ
	*/
	void sigNewImage(QImage clImage, qint64 llTime);
	/*!
		@brief		再生時間通知
		@param[in]	llTime 現在の時間
	*/
	void sigNewTime(qint64 llTime);
	/*!
		@brief		画像終了通知
	*/
	void sigFinishedVideo();
	/*!
		@brief		画像スキップ
		@param[in]	llSkipTime 飛ばしたい時間
	*/
	void sigSkipFrame(qint64 llSkipTime);

protected:
	ClVideoDecoder *m_pclVideoDecoder;	//!< 画像デコーダ
	QList<stVideoData> m_clVideoList;	//!< 画像データリスト
	QWaitCondition *m_pclWaitVideo;		//!< 画像デコーダ用待ち合わせ
	qint64 m_llStartTime;				//!< 再生開始時間
	qint64 m_llNowTimeVideo;			//!< 表示画像のタイムスタンプ
	QMutex m_clVideoMutex;				//!< 画像データ操作ミューテックス
	bool m_bIsEndVideoStream;			//!< 画像デコード終了フラグ
	bool m_bIsStop;						//!< スレッド停止フラグ
	bool m_bIsSkip;						//!< スキップ実行フラグ
	qint32 m_lSkipTime;					//!< スキップ時間
	bool m_bIsWaitSkip;					//!< スキップ後の画像待ちフラグ
	qint32 m_lFrameCycle;				//!< 画像周期(msec)
};
//--------------------------------------------------------------------------------//
#endif
