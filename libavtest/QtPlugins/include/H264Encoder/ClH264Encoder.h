/*--------------------------------------------------------------------------------*/
/*!
	@file	ClH264Encoder.h
	@brief	エンコーダプラグインの本体
			音声取得だけはこっちで行う
			他所から貰った映像データと自分で取得した音声データを
			エンコードスレッドクラスに引き渡す
*/
/*--------------------------------------------------------------------------------*/
#ifndef CLH264ENCODER_H
#define CLH264ENCODER_H

#include <QWidget>			//継承元のため必要
#include <QtMultimedia>		//QAudioInputのため必要

#include "H264Encoder/AbstEncoderThread.h"


/*----------------------------------------*/
/*!
	@enum	eENCODER_CONTAINER_TYPE
	@brief	対応するコンテナの種類
*/
/*----------------------------------------*/
enum eENCODER_CONTAINER_TYPE
{
	ENCODER_CONTAINER_TYPE_MP4 = 0,		//MP4コンテナ、H.264+AAC
	ENCODER_CONTAINER_TYPE_AVI,			//AVIコンテナ、MJPG+PCM
	ENCODER_CONTAINER_TYPE_JPGTHROUGH,	//JPGスルー、音声は無視

	ENCODER_CONTAINER_TYPE_TERMINATE
};




/*================================================================================*/
/*--------------------------------------------------------------------------------*/
/*!
	@class	ClH264Encoder
	@brief	各種エンコーダの親クラス
	@note	実際は設定されたコンテナタイプに応じてエンコードスレッドを生成し
			そいつに処理を一任する
*/
/*--------------------------------------------------------------------------------*/
class ClH264Encoder : public QWidget
{
	Q_OBJECT


	/*----------------------------------------*/
	/*
		プロパティ定義
	*/
	/*----------------------------------------*/
	//間欠録画における映像取得間隔
	Q_PROPERTY(quint32 m_ulIntervalTime READ intervalTime WRITE setIntervalTime)
private:
	quint32	m_ulIntervalTime;
public:
	quint32	intervalTime(){ return m_ulIntervalTime; }
	bool			setIntervalTime(quint32 in_ulIntervalTime);

	//受け付けるコンテナタイプ
	Q_PROPERTY(QStringList m_clQStrListAvailableContainerType READ availableContainerType)
private:
	QStringList	m_clQStrListAvailableContainerType;
public:
	QStringList availableContainerType(){ return m_clQStrListAvailableContainerType; }

	//受け付ける録画方式
	Q_PROPERTY(eENCODER_REC_TYPE m_eRecType READ recType WRITE setRecType)
private:
	eENCODER_REC_TYPE	m_eRecType;
public:
	eENCODER_REC_TYPE	recType(){ return m_eRecType; }
	bool				setRecType(eENCODER_REC_TYPE in_eRecType);

	//生成する動画ファイルのコンテナ
	Q_PROPERTY(eENCODER_CONTAINER_TYPE m_eContaierType READ containerType WRITE setContainerType)
private:
	eENCODER_CONTAINER_TYPE	m_eContaierType;
public:
	eENCODER_CONTAINER_TYPE	containerType(){ return m_eContaierType; }
	bool					setContainerType(eENCODER_CONTAINER_TYPE in_eContainerType);

	//現在エンコード中かどうか
	Q_PROPERTY(bool m_bIsEncoding READ isEncoding)
private:
	bool	m_bIsEncoding;
public:
	bool	isEncoding(){ return m_bIsEncoding; }

	//出力ファイルパス
	Q_PROPERTY(QString m_qstrDestFilePath READ destFilePath WRITE setDestFilePath)
private:
	QString		m_qstrDestFilePath;
public:
	QString		destFilePath(){ return m_qstrDestFilePath; }
	bool		setDestFilePath(QString in_qstrDestFilePath);

	//動画に音声を含めるかどうか
	Q_PROPERTY(bool m_bIsAudio READ isAudio WRITE setIsAudio)
private:
	bool	m_bIsAudio;
public:
	bool	isAudio(){ return m_bIsAudio; }
	bool	setIsAudio(bool in_bIsAudio);

	//MP4録画でフラグメントMP4を作るかどうか
	Q_PROPERTY(bool m_bIsFragmentMP4 READ isFragmentMP4 WRITE setIsFragmentMP4)
private:
	bool	m_bIsFragmentMP4;
public:
	bool	isFragmentMP4(){ return m_bIsFragmentMP4; }
	bool	setIsFragmentMP4(bool in_bIsFragmentMP4);


	//動画の目標ビットレート
	Q_PROPERTY(quint16 m_ulVideoBitRate READ videoBitRate WRITE setVideoBitRate)
private:
	quint32	m_ulVideoBitRate;
public:
	quint32	videoBitRate(){ return m_ulVideoBitRate; }
	bool			setVideoBitRate(quint32 in_ulVideoBitRate);
	
	//動画の目標フレームレート
	Q_PROPERTY(quint16 m_usVideoFrameRate READ videoFrameRate WRITE setVideoFrameRate)
private:
	quint16	m_usVideoFrameRate;
public:
	quint16	videoFrameRate(){ return m_usVideoFrameRate; }
	bool			setVideoFrameRate(quint16 in_usVideoFrameRate);

	//画像の幅
	Q_PROPERTY(quint32 m_ulVideoWidth READ videoWidth WRITE setVideoWidth)
private:
	quint32	m_ulVideoWidth;
public:
	quint32	videoWidth(){ return m_ulVideoWidth; }
	bool			setVideoWidth(quint32 in_ulVideoWidth);

	//画像の高さ
	Q_PROPERTY(quint32 m_ulVideoHeight READ videoHeight WRITE setVideoHeight)
private:
	quint32	m_ulVideoHeight;
public:
	quint32	videoHeight(){ return m_ulVideoHeight; }
	bool			setVideoHeight(quint32 in_ulVideoHeight);

	
	//入力する音声データのサンプリング周波数
	Q_PROPERTY(quint32 m_ulAudioInFrequency READ audioInFrequency WRITE setAudioInFrequency)
private:
	quint32	m_ulAudioInFrequency;
public:
	quint32	audioInFrequency(){ return m_ulAudioInFrequency; }
	bool			setAudioInFrequency(quint32 in_ulAudioInFrequency);

	//音声の1サンプルのビット数
	Q_PROPERTY(quint16 m_usAudioBitPerSample READ audioBitPerSample WRITE setAudioBitPerSample)
private:
	quint16	m_usAudioBitPerSample;
public:
	quint16	audioBitPerSample(){ return m_usAudioBitPerSample; }
	bool			setAudioBitPerSample(quint16 in_usAudioBitPerSample);

	//音声のチャンネル数、要するにステレオなら2だしモノラルなら1
	Q_PROPERTY(quint16 m_usAudioChannels READ audioChannels WRITE setAudioChannels)
private:
	quint16	m_usAudioChannels;
public:
	quint16	audioChannels(){ return m_usAudioChannels; }
	bool			setAudioChannels(quint16 in_usAudioChannels);

	//音声の目標ビットレート
	Q_PROPERTY(quint32 m_ulAudioOutBitrate READ audioOutBitrate WRITE setAudioOutBitrate)
private:
	quint32	m_ulAudioOutBitrate;
public:
	quint32	audioOutBitrate(){ return m_ulAudioOutBitrate; }
	bool			setAudioOutBitrate(quint32 in_ulAudioOutBitrate);


	/*----------------------------------------*/
	/*
		メンバー変数定義
	*/
	/*----------------------------------------*/
private:
	AbstEncoderThread*  m_pclEncodeThread;		//実際にエンコード処理を行うスレッドクラス

	QAudioInput*		m_pclAudioInput;		//音声取得クラス
	QIODevice*			m_pclAudioReceive;		//音声データを受けとるクラス

	QImage				m_clQImgNewVideoData;	//最新のRawデータ
	QByteArray			m_clQBaNewVideoData;	//最新のJPGデータ
	QByteArray			m_clQBaAudioBuffer;		//音声データ取得用バッファ

	QMutex				m_clQMtxEncoding;		//エンコード中はロックされるMuxex

	/*----------------------------------------*/
	/*
		メンバー関数定義
	*/
	/*----------------------------------------*/
public:
	ClH264Encoder(QWidget* parent = 0);	//コンストラクタ
	virtual ~ClH264Encoder();			//デストラクタ

private:
	bool	beginAudioGet();			//音声取得を開始する
	void	endAudioGet();				//音声取得を終了する

	/*----------------------------------------*/
	/*
		slot
	*/
	/*----------------------------------------*/
public slots:
	void	slotAddVideoDataRaw(QImage in_clNewVideoData);		//上位からのrawRGB版新しい映像データ追加スロット
	void	slotAddVideoDataJpg(QByteArray in_clNewVideoData);	//上位からのjpg版新しい映像データ追加スロット
	void	slotBeginEncode();									//上位からのエンコード開始要求
	void	slotEndEncode();									//上位からのエンコード終了要求

	void	slotEncodeStopped(eENCODER_STOP_REASON in_eStopReason);	//スレッドからのエンコード終了通知
	void	slotThreadFinished();								//スレッド終了通知を受け取る

private slots:
	void	slotAddAudioData();	//音声データ追加

	/*----------------------------------------*/
	/*
		signal
	*/
	/*----------------------------------------*/
signals:
	void	sigReqJpgCapture(QVariant& clQVarData, qint32& iType);	//Jpgデータを1枚貰う
	void	sigReqRawCapture(QVariant& clQVarData, qint32& iType);	//Rawデータを1枚貰う

	void	sigEncodeStopped(eENCODER_STOP_REASON eStopReason);		//エンコード停止を理由つきで通知する
	void	sigSoundDeviceNotFound();								//音声を使おうとして音声デバイスが見つからなかった？使えなかった？場合に発行される

	void	sigAddVideoDataRaw(QImage in_clNewVideoData);			//rawRGBの新着画像の受信を通知する
	void	sigAddVideoDataJpg(QByteArray in_clNewVideoData);		//JPGの新着画像の受信を通知する
	void	sigAddAudioData(QByteArray clNewAudioData);				//新しい音声データの受信を通知する
	void	sigManualRecTrigger();									//コマ撮り録画の画像追加トリガー
};
/*================================================================================*/

#endif	 //CLH264ENCODER_H
