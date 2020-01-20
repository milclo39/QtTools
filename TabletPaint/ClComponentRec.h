/*--------------------------------------------------------------------------------*/
/*!
	@file	ClComponentRec.h
	@brief	録画機能を司るコンポーネント
*/
/*--------------------------------------------------------------------------------*/
#ifndef CLCOMPONENTREC_H
#define CLCOMPONENTREC_H

#include <QApplication>
#include <QObject>										//継承元
#include "CmnDefine.h"									//EL_CMDとかのため
#include "H264Encoder/ClH264Encoder.h"					//録画プラグイン
#include "IMACommand.h"

/*================================================================================*/
/*--------------------------------------------------------------------------------*/
/*!
	@class	ClEncodeJPEG
	@brief	JPEGエンコードクラス
	@note	ただ単にJPEGエンコードするだけのクラス。
			moveToThreadで別スレッドとして処理させることで価値が出る。
			別スレッドにしてやると、connectされたslotの処理内容は別スレッドで実行される。
			但し、slotを直接叩くと呼び出し元のスレッドで実行される。
*/
/*--------------------------------------------------------------------------------*/
class ClEncodeJPEG : public QObject
{
	Q_OBJECT

public:
	ClEncodeJPEG(QObject* in_pclParent);				//コンストラクタ
	virtual ~ClEncodeJPEG();							//デストラクタ

public slots:
	void	slotEncodeJPEG(QPixmap in_clPixmapSource);	//エンコード実行slot

signals:
	void	sigEncodedJPEG(QByteArray clQBAJpegData);	//!< エンコード完了通知。引数にJPEGデータが格納される
};
/*================================================================================*/
/*--------------------------------------------------------------------------------*/
/*!
	@class	ClComponentRec
	@brief	録画機能を司るコンポーネント
*/
/*--------------------------------------------------------------------------------*/
class ClComponentRec : public QObject
{
	Q_OBJECT

private:
	ClH264Encoder*			m_pclEncoder;			//!< エンコーダクラス
	QPixmap					m_clPixmapNew;			//!< 最新画像を保持する。どこかからキャプチャ要求応答がきたらとりあえずこの変数に格納される
	bool					m_bGetNewImage;			//!< キャプチャ完了フラグ、falseをセットして待ち始め、キャプチャが完了したらtrueになる
	QTimer					m_clTimer;				//!< 通常録画のためのキャプチャを要求させるタイマー

	QSize					m_clSize;				//!< 動画のサイズ
	eENCODER_REC_TYPE		m_eRecType;				//!< 録画形式
	eENCODER_CONTAINER_TYPE	m_eRecFormat;			//!< 録画ファイル形式
	qint32					m_lFramerateNormal;		//!< 連続録画におけるフレームレート

	ClEncodeJPEG*			m_pclEncodeJPEG;		//!< JPEG一発エンコードクラス
	QThread*				m_pclThreadEncodeJPEG;	//!< JPEGエンコードクラス用スレッド
	bool					m_bIsWaitingForJPEG;	//!< 現在JPEGエンコード完了待ちかどうか。Signalを溜め込ませないためにこんなフラグが必要。

public:
	ClComponentRec(QObject* in_pclParent = 0);				//コンストラクタ
	virtual ~ClComponentRec();								//デストラクタ

private:
	void	connection();									//接続処理

	bool	procStartRec(QString in_qstrDestFilePath);		//録画開始処理
	void	procStopRec();									//録画終了処理
	QString	setEncodeParam(QString in_qstrDestFilePath);	//録画設定を行う

	void	tryNewImage(qint32 in_lTimeout_ms);				//新しい画像の取得を試みる。実際に画像が取れたかどうかや取得した画像はメンバー変数にて格納される

	eENCODER_CONTAINER_TYPE	getContainerType();				//レジストリからファイル形式を読み込む
	QSize					getRecSize();					//レジストリからサイズを読み込む
	qint32					getIntervalTime();				//レジストリから間欠時間を読み込む
	eENCODER_REC_TYPE		getRecType();					//レジストリから録画形式を読み込む

	QString	createDefaultRecPath();							//デフォルトの保存先フルパスを生成する
	QString	adjustExtension(QString in_qstrBaseFilePath);	//拡張子補正

public:
	void setRecSize(QSize size);

public slots:
	void	slotRecvCmd(EL_CMD in_clCmd);									//いつものコマンド処理slot
	void	slotEncodedJPEG(QByteArray in_clQBAJpegData);					//JPEGエンコードクラスからのJPEGエンコード出来たよ通知
	void	slotEncodeStopped(eENCODER_STOP_REASON in_eStopReason);			//録画プラグインからの録画停止通知受信処理
	void	slotReqJpgCapture(QVariant& out_clJpgData, qint32& out_lType);	//録画プラグインからのJPG画像要求受信slot
	void	slotReqRawCapture(QVariant& out_clRawData, qint32& out_lType);	//録画プラグインからのRAW画像要求受信slot
	void	slotTryNewImage();												//通常録画用の新着画像要求slot

signals:
	void	sigSendCmd(EL_CMD clCmd);					//いつものコマンド送信signal
	void	sigReqEncodeJPEG(QPixmap clPixmapSource);	//JPEGエンコードスレッドへのエンコード要求
	void	sigAddJpgData(QByteArray clJpgData);		//録画プラグインに新しいJPG画像を与える。これによって与えられる画像は通常録画で使用される。
	void	sigAddRawData(QImage clRawData);			//録画プラグインに新しいRAW画像を与える。これによって与えられる画像は通常録画で使用される。
};
/*================================================================================*/

#endif // CLCOMPONENTREC_H
