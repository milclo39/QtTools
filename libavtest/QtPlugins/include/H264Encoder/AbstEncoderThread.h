/*--------------------------------------------------------------------------------*/
/*!
	@file	AbstEncodeThread.h
	@brief	エンコードスレッドクラスの基本？クラス
*/
/*--------------------------------------------------------------------------------*/
#ifndef ABSTENCODERTHREAD_H
#define ABSTENCODERTHREAD_H

#include <QtCore>	//継承元のため必要
#include <QtGui>	//QImage使用のため


/*----------------------------------------*/
/*!
	@enum	eENCODER_STOP_REASON
	@brief	停止通知用enum
*/
/*----------------------------------------*/
enum eENCODER_STOP_REASON
{
	ENCODER_STOP_REASON_NOERROR = 0,		//正常終了
	ENCODER_STOP_REASON_INVALIDPARAM,		//パラメータ異常による設定失敗
	ENCODER_STOP_REASON_SIZECHANGED,		//設定したサイズと来た画像のサイズが異なっていた
	ENCODER_STOP_REASON_DISCONNECT,			//書画カメラが引っこ抜かれた
	ENCODER_STOP_REASON_SDMODE,				//書画カメラがSDモードになった
	ENCODER_STOP_REASON_FILESIZEOVER,		//コンテナの仕様上許可されたサイズを上回った

	ENCODER_STOP_REASON_UNKNOWN				//不明だけどなんか止まった
};


/*----------------------------------------*/
/*!
	@enum	eENCODER_REC_TYPE
	@brief	対応する録画方式
*/
/*----------------------------------------*/
enum eENCODER_REC_TYPE
{
	ENCODER_REC_TYPE_NORMAL = 0,	//通常録画
	ENCODER_REC_TYPE_INTERVAL,		//間欠録画
	ENCODER_REC_TYPE_MANUAL,		//コマ撮り録画

	ENCODER_REC_TYPE_TERMINATE
};




/*================================================================================*/
/*--------------------------------------------------------------------------------*/
/*!
	@class	AbstEncoderThread
	@brief	エンコードスレッドクラスの元クラス
	@note	最低限必要なSignal、Slot、プロパティ定義だけしてある
			その他実際にエンコード処理を行うrunの中身を実装すること。
*/
/*--------------------------------------------------------------------------------*/
class AbstEncoderThread : public QThread
{
	Q_OBJECT


	/*----------------------------------------*/
	/*
		プロパティ定義
	*/
	/*----------------------------------------*/
	//録画方式
private:
	eENCODER_REC_TYPE	m_eRecType;
public:
	eENCODER_REC_TYPE	recType(){ return m_eRecType; }
	void				setRecType(eENCODER_REC_TYPE in_eRecType){ m_eRecType = in_eRecType; }

	//間欠録画における間欠時間
private:
	quint32	m_ulIntervalTime;
public:
	quint32	intervalTime(){ return m_ulIntervalTime; }
	void			setIntervalTime(quint32 in_ulIntervalTime){ m_ulIntervalTime = in_ulIntervalTime; }

	//出力ファイルパス
private:
	QString		m_qstrDestFilePath;
public:
	QString		destFilePath(){ return m_qstrDestFilePath; }
	void		setDestFilePath(QString in_qstrDestFilePath){ m_qstrDestFilePath = in_qstrDestFilePath; }

	//動画に音声を含めるかどうか
private:
	bool	m_bIsAudio;
public:
	bool	isAudio(){ return m_bIsAudio; }
	void	setIsAudio(bool in_bIsAudio){ m_bIsAudio = in_bIsAudio; }

	//動画の目標ビットレート
private:
	quint32 m_ulVideoBitrate;
public:
	quint32	videoBitrate(){return m_ulVideoBitrate;}
	void			setVideoBitrate(quint32 ulBitrate){m_ulVideoBitrate = ulBitrate;}

	//容量オーバーによる停止時、自動的に新たなファイルを生成するかどうか
private:
	bool	m_bIsCreateOverSize;
public:
	bool	isCreateOverSize(){ return m_bIsCreateOverSize; }
	void	setIsCreateOverSize(bool in_bIsCreateOverSize){ m_bIsCreateOverSize = in_bIsCreateOverSize; }

	//MP4をフラグメントするかどうか。こんなのAbstに入れるのどーかと思うけど、設計的に敗北しているので仕方ないよね
private:
	bool	m_bIsFragmentMP4;
public:
	bool	isFragmentMP4(){ return m_bIsFragmentMP4; }
	void	setIsFragmentMP4(bool in_bIsFragmentMP4){ m_bIsFragmentMP4 = in_bIsFragmentMP4; }


	//動画の目標フレームレート
private:
	quint16	m_usVideoFrameRate;
public:
	quint16	videoFrameRate(){ return m_usVideoFrameRate; }
	void			setVideoFrameRate(quint16 in_usVideoFrameRate){ m_usVideoFrameRate = in_usVideoFrameRate; }

	//画像の幅
private:
	quint32	m_ulVideoWidth;
public:
	quint32	videoWidth(){ return m_ulVideoWidth; }
	void			setVideoWidth(quint32 in_ulVideoWidth){ m_ulVideoWidth = in_ulVideoWidth; }

	//画像の高さ
private:
	quint32	m_ulVideoHeight;
public:
	quint32	videoHeight(){ return m_ulVideoHeight; }
	void			setVideoHeight(quint32 in_ulVideoHeight){ m_ulVideoHeight = in_ulVideoHeight; }


	//入力する音声データのサンプリング周波数
private:
	quint32	m_ulAudioInFrequency;
public:
	quint32	audioInFrequency(){ return m_ulAudioInFrequency; }
	void			setAudioInFrequency(quint32 in_ulAudioInFrequency){ m_ulAudioInFrequency = in_ulAudioInFrequency; }

	//音声の1サンプルのビット数
private:
	quint16	m_usAudioBitPerSample;
public:
	quint16	audioBitPerSample(){ return m_usAudioBitPerSample; }
	void			setAudioBitPerSample(quint16 in_usAudioBitPerSample){ m_usAudioBitPerSample = in_usAudioBitPerSample; }

	//音声のチャンネル数、要するにステレオなら2だしモノラルなら1
private:
	quint16	m_usAudioChannels;
public:
	quint16	audioChannels(){ return m_usAudioChannels; }
	void			setAudioChannels(quint16 in_usAudioChannels){ m_usAudioChannels = in_usAudioChannels; }

	//音声の目標ビットレート
private:
	quint32	m_ulAudioOutBitrate;
public:
	quint32	audioOutBitrate(){ return m_ulAudioOutBitrate; }
	void			setAudioOutBitrate(quint32 in_ulAudioOutBitrate){ m_ulAudioOutBitrate = in_ulAudioOutBitrate; }


	/*----------------------------------------*/
	/*
		メンバー関数
	*/
	/*----------------------------------------*/
public:
	AbstEncoderThread(){}													//コンストラクタ
	virtual ~AbstEncoderThread(){}											//デストラクタ

	virtual bool	isThreadRunning() = 0;									//現在スレッド起動中かどうかを返す。大体はQThreadのisRunningをそのまま返せばいいような気がする。

protected:
	virtual void	run() = 0;												//オーバーライド：スレッド本体


	/*----------------------------------------*/
	/*
		slot
	*/
	/*----------------------------------------*/
public slots:
	virtual void	slotAddVideoDataRaw(QImage in_clNewVideoData) = 0;		//画像更新スロット、rawRGB用
	virtual void	slotAddVideoDataJpg(QByteArray in_clNewVideoData) = 0;	//画像更新スロット、jpg用
	virtual void	slotAddAudioData(QByteArray in_clNewSoundData) = 0;		//音声データ追加スロット
	virtual void	slotManualRecTrigger() = 0;								//コマ撮り録画の画像追加トリガー
	virtual void	slotThreadEnd() = 0;									//エンコード終了スロット


	/*----------------------------------------*/
	/*
		signal
	*/
	/*----------------------------------------*/
signals:
	void	sigEncodeStopped(eENCODER_STOP_REASON eStopReason);				//エンコード停止通知、理由付き
	void	sigReqJpgCapture(QVariant& clQVarData, qint32& iType);			//Jpgデータを1枚貰う
	void	sigReqRawCapture(QVariant& clQVarData, qint32& iType);			//Rawデータを1枚貰う

};
/*================================================================================*/


#endif // ABSTENCODERTHREAD_H
