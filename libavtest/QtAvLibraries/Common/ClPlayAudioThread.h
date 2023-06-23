//--------------------------------------------------------------------------------//
/*!
	@file	ClPlayAudioThread.h
	@brief	音声再生スレッドヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef AV_CLPLAYAUDIOTHREAD_H
#define AV_CLPLAYAUDIOTHREAD_H
#include <QThread>
#include <QQueue>
#include <QAudioOutput>
#include <QMutex>

struct AVCodecContext;
struct AVFrame;
struct AVAudioResampleContext;

//--------------------------------------------------------------------------------//
/*!
	@brief	音声再生スレッド
*/
//--------------------------------------------------------------------------------//
class ClPlayAudioThread : public QThread
{
public:
	ClPlayAudioThread();	//コンストラクタ
	~ClPlayAudioThread();	//デストラクタ

	bool init(AVCodecContext *pContext);	//初期化
	void addAudio(AVFrame *pFrame);	//データ追加

protected:
	virtual void run();	//メイン処理

private:
	bool m_bIsStop;	//!< 停止フラグ
	QAudioOutput *m_pAudio;	//!< 音声制御
	QQueue<QByteArray> m_listAudioData;	//!< デコード済みデータリスト
	AVAudioResampleContext *m_pResample;	//!< 音声変換
	QMutex mutex;
};
//--------------------------------------------------------------------------------//
#endif
