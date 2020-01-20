//--------------------------------------------------------------------------------//
/*!
	@file	ClStreamDecoder.h
	@brief	ストリームデコーダヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef AV_CLSTREAMDECODER_H
#define AV_CLSTREAMDECODER_H
#include <QThread>
#include <QImage>
#include "../AvCommon.h"

class ClPlayAudioThread;

//--------------------------------------------------------------------------------//
/*!
	@brief	ストリームデコーダ
*/
//--------------------------------------------------------------------------------//
class ClStreamDecoder : public QThread
{
	Q_OBJECT

public:
	ClStreamDecoder(QObject *pclParent);	//コンストラクタ
	~ClStreamDecoder();	//デストラクタ

	static void avStreamInit();	//初期化
	static void avStreamDeinit();	//終了

	void setStream(QString strStream);	//ストリーム設定
	void stop();	//停止

protected:
	void run();	//メイン処理

signals:
	/*!
	 * \brief 画像通知
	 * \param img : 画像
	 */
	void sigImage(ClAvImage img);

protected:
	QString m_strStream;	//!< デコードストリーム
	bool m_bIsStop;	//!< 停止フラグ
	ClPlayAudioThread *m_pThreadAudio;	//!< 音声再生スレッド
};
//--------------------------------------------------------------------------------//
#endif
