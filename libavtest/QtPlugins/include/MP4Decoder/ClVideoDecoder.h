//--------------------------------------------------------------------------------//
/*!
	@file	ClVideoDecoder.h
	@brief	画像デコーダヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLVIDEODECODER_H
#define CLVIDEODECODER_H

#include <QObject>
#include <QImage>
#include <QThread>
#include <QWaitCondition>

namespace UMC
{
	class VideoDecoder;
	class VideoDecoderParams;
	class MP4Splitter;
};
using namespace UMC;	//IPPで使っているnamespace
class ClVideoThread;

//--------------------------------------------------------------------------------//
/*!
	@ingroup	groupMp4Decoder
	@brief		画像デコーダ
*/
//--------------------------------------------------------------------------------//
class ClVideoDecoder : public QThread
{
	Q_OBJECT

public:
	ClVideoDecoder(quint32 ulTrackNum, QWaitCondition *pclWait);	//コンストラクタ
	~ClVideoDecoder();	//デストラクタ

	bool init(MP4Splitter *pumcSplitter);	//初期化
	void reset();						//再初期化
	void stop();						//スレッド停止
	void pause();						//一時停止
	void seek(qreal dTime);				//シーク
	QImage getImage();					//現在の画像取得
	qreal getFrameRate();				//フレームレート取得
	/*!
		@brief		制御スレッド設定
		@param[in]	画像制御スレッド
	*/
	void setThread(ClVideoThread *pclThread){m_pclThread = pclThread;}

public slots:
	void slotReqDecode();			//画像デコード要求

protected:
	void run();		//!< メイン処理

signals:
	/*!
		@brief		デコード画像通知
		@param[in]	clImage 画像
		@param[in]	llTime　タイムスタンプ
	*/
	void sigImage(QImage clImage, qint64 llTime);
	/*!
		@brief		ストリーム終了通知
	*/
	void sigEndStream();

protected:
	VideoDecoder *m_pumcDecoder;	//!< 画像デコーダ
	VideoDecoderParams *m_pumcDecParams;	//!< デコーダパラメータ
	MP4Splitter *m_pumcSplitter;	//!< MP4スプリッタ
	quint32 m_ulTrackNum;			//!< トラック番号
	bool m_bIsStop;					//!< スレッド停止フラグ
	QWaitCondition *m_pclWait;		//!< 待ち合わせ用クラス
	qint32 m_lImageWidth;			//!< 画像幅
	qint32 m_lImageHeight;			//!< 画像高
	bool m_bIsEndStream;			//!< デコード終了フラグ
	qreal m_dLastTime;				//!< 最後にデコードされたタイムスタンプ
	qreal m_dFrameRate;				//!< フレームレート
	ClVideoThread *m_pclThread;		//!< 画像制御スレッド
};
//--------------------------------------------------------------------------------//
#endif
