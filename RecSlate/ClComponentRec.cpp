/*--------------------------------------------------------------------------------*/
/*!
	@file	ClComponentRec.cpp
	@brief	録画機能
*/
/*--------------------------------------------------------------------------------*/
#include "ClComponentRec.h"

#define REC_DEFAULT_NORMAL_FRAMERATE	(15)						//!< デフォルトの通常録画のフレームレート
#define REC_DEFAULT_INTERVAL_FRAMERATE	(5)							//!< デフォルトの間欠録画のフレームレート
#define REC_JPEG_QUALITY				(80)						//!< Qtに設定するJPEG品質
#define REC_CAPTURE_TIMEOUT_MS			(1000)						//!< 録画コンポーネントからのキャプチャ要求をタイムアウトさせる時間。単位はms。
#define DEFAULT_REC_FORMAT		(1)			//!< フォーマット。H264Encoder.hのフォーマット定義enumの値。H.264は再生できないOSがあるのでデフォルトはAvi
#define DEFAULT_REC_SIZE		QSize(1280, 720)	//!< 初期は720pとする
#define DEFAULT_REC_INTERVAL	(60)		//!< 間欠時間。これはそのまんま秒単位。
#define DEFAULT_REC_TYPE		(0)			//!< 録画形式。AbstEncoderThreadのeENCODER_REC_TYPE。値としては連続録画


/*================================================================================*/
/*--------------------------------------------------------------------------------*/
/*!
	@brief	JPEGエンコードクラスのコンストラクタ
*/
/*--------------------------------------------------------------------------------*/
ClEncodeJPEG::ClEncodeJPEG(QObject* in_pclParent) : QObject(in_pclParent)
{
}

/*--------------------------------------------------------------------------------*/
/*!
	@brief	JPEGエンコードクラスのデストラクタ
*/
/*--------------------------------------------------------------------------------*/
ClEncodeJPEG::~ClEncodeJPEG()
{
}

/*--------------------------------------------------------------------------------*/
/*!
	@brief	エンコード実行slot
	@note	JPEGエンコードを行い、結果をsignalで通知する
*/
/*--------------------------------------------------------------------------------*/
void ClEncodeJPEG::slotEncodeJPEG(QPixmap in_clPixmapSource)
{
	if(true == in_clPixmapSource.isNull())		//ソース画像が無効ではないことを確認
	{
		return;									//ソースが無効じゃどうにもならないので処理終了
	}

	QByteArray	l_clQBAJPEGData;										//JPEGデータ格納領域
	QBuffer		l_clJpgBuffer(&l_clQBAJPEGData);						//エンコード結果格納用QByteArrayのインターフェイス
	in_clPixmapSource.save(&l_clJpgBuffer, "jpg", REC_JPEG_QUALITY);	//JPEGエンコードを行う TODO:品質設定が動作せず、常にクオリティ100%で出力される。要調査

	emit this->sigEncodedJPEG(l_clQBAJPEGData);							//エンコードされた(かもしれない)ｓ
}
/*================================================================================*/

/*--------------------------------------------------------------------------------*/
/*!
	@brief	録画コンポーネントのコンストラクタ
*/
/*--------------------------------------------------------------------------------*/
ClComponentRec::ClComponentRec(QObject* in_pclParent) : QObject(in_pclParent)
{
	qRegisterMetaType<EL_CMD>("EL_CMD");

	this->m_pclEncoder			= new ClH264Encoder(NULL);									//録画プラグインを生成
	this->m_bGetNewImage		= false;													//最初は最新画像保持状態ではない
	this->m_eRecType			= static_cast<eENCODER_REC_TYPE>(DEFAULT_REC_TYPE);			//録画形式
	this->m_eRecFormat			= static_cast<eENCODER_CONTAINER_TYPE>(DEFAULT_REC_FORMAT);	//録画ファイル形式
	this->m_lFramerateNormal	= REC_DEFAULT_NORMAL_FRAMERATE;								//通常録画におけるフレームレート
	this->m_bIsWaitingForJPEG	= false;													//現在JPEGエンコード完了待ちかどうか
	this->m_clSize = DEFAULT_REC_SIZE;

	this->m_pclEncodeJPEG		= new ClEncodeJPEG(NULL);									//JPEGエンコードスレッドを生成
	this->m_pclThreadEncodeJPEG	= new QThread(NULL);										//↑を動作させるスレッドを生成
	this->m_pclEncodeJPEG->moveToThread(this->m_pclThreadEncodeJPEG);						//JPEGエンコードスレッドの動作スレッドをさっき生成したスレッドにする
	this->m_pclThreadEncodeJPEG->start(QThread::IdlePriority);								//早速スレッドとして動作させることで、イベントに対する処理を行わせる

	this->connection();
}

/*--------------------------------------------------------------------------------*/
/*!
	@brief	録画コンポーネントのデストラクタ
*/
/*--------------------------------------------------------------------------------*/
ClComponentRec::~ClComponentRec()
{
	this->m_bGetNewImage = true;						//念のため最新画像を取ることが出来たことにする

	if(NULL != this->m_pclEncoder)						//録画プラグインの存在を確認
	{
		if(true == this->m_pclEncoder->isEncoding())	//録画中っぽい場合
		{
			this->m_pclEncoder->slotEndEncode();		//録画を止める
		}
		delete this->m_pclEncoder;						//そして実体を削除する
		this->m_pclEncoder = NULL;						//格納先をNULLにしておく
	}
	if(NULL != this->m_pclEncodeJPEG)					//JPEGエンコードクラスの存在を確認
	{
		delete this->m_pclEncodeJPEG;					//実体を削除
		this->m_pclEncodeJPEG = NULL;					//格納先をNULLにしておく
	}
	if(NULL != this->m_pclThreadEncodeJPEG)				//JPEGエンコード用スレッドの存在を確認
	{
		this->m_pclThreadEncodeJPEG->quit();			//スレッドを停止させる
		this->m_pclThreadEncodeJPEG->wait();			//消す前にスレッド停止完了まで待つ
		delete this->m_pclThreadEncodeJPEG;				//実体を削除
		this->m_pclThreadEncodeJPEG = NULL;				//格納先をNULLにしておく
	}
}

/*--------------------------------------------------------------------------------*/
/*!
	@brief	接続処理
*/
/*--------------------------------------------------------------------------------*/
void ClComponentRec::connection()
{
	if(NULL != this->m_pclEncodeJPEG)																	//一応JPEGエンコードクラスが実体化していることを確認する
	{
		QObject::connect(this, SIGNAL(sigReqEncodeJPEG(QPixmap)),
						 this->m_pclEncodeJPEG, SLOT(slotEncodeJPEG(QPixmap)), Qt::QueuedConnection);	//JPEGエンコード要求を接続。一応明示的にQueued接続とする
		QObject::connect(this->m_pclEncodeJPEG, SIGNAL(sigEncodedJPEG(QByteArray)),
						 this, SLOT(slotEncodedJPEG(QByteArray)));										//JPEGエンコード完了通知を接続
	}
	if(NULL != this->m_pclEncoder)																	//一応録画プラグインが実体化していることを確認する
	{
		QObject::connect(this, SIGNAL(sigAddJpgData(QByteArray)),
						 this->m_pclEncoder, SLOT(slotAddVideoDataJpg(QByteArray)));				//録画プラグインにJPG画像を与える
		QObject::connect(this, SIGNAL(sigAddRawData(QImage)),
						 this->m_pclEncoder, SLOT(slotAddVideoDataRaw(QImage)));					//録画プラグインにRAW画像を与える
		QObject::connect(this->m_pclEncoder, SIGNAL(sigEncodeStopped(eENCODER_STOP_REASON)),
						 this, SLOT(slotEncodeStopped(eENCODER_STOP_REASON)));						//録画プラグインからの録画停止通知に対する処理
		QObject::connect(this->m_pclEncoder, SIGNAL(sigReqJpgCapture(QVariant&, qint32&)),
						 this, SLOT(slotReqJpgCapture(QVariant&, qint32&)), Qt::DirectConnection);	//録画プラグインからのJPG画像要求に対する処理
		QObject::connect(this->m_pclEncoder, SIGNAL(sigReqRawCapture(QVariant&, qint32&)),
						 this, SLOT(slotReqRawCapture(QVariant&, qint32&)), Qt::DirectConnection);	//録画プラグインからのRAW画像要求に対する処理
	}

	QObject::connect(&this->m_clTimer, SIGNAL(timeout()), this, SLOT(slotTryNewImage()));
}

/*--------------------------------------------------------------------------------*/
/*!
	@brief	録画開始処理

	@param[in]	in_qstrDestFilePath	保存先ファイルパス

	@retval	true	録画が開始された、または既に録画中
	@retval	false	録画の開始に失敗した
*/
/*--------------------------------------------------------------------------------*/
bool ClComponentRec::procStartRec(QString in_qstrDestFilePath)
{
	if(true == this->m_pclEncoder->isEncoding())	//現在録画中な場合
	{
		this->procStopRec();						//録画を停止させる
	}

	QString	l_qstrDestFilePath = this->setEncodeParam(in_qstrDestFilePath);			//引数で受け取ったファイルパスを与える。他はきっとレジストリから読む…のか？

	this->tryNewImage(REC_CAPTURE_TIMEOUT_MS);										//最新画像のキャプチャを行う
	if(true == this->m_bGetNewImage)												//最新画像の取得に成功した場合
	{
		if(ENCODER_CONTAINER_TYPE_MP4 == this->m_eRecFormat)						//録画フォーマットがMP4な場合。QImageが要求される。
		{
			this->m_pclEncoder->slotAddVideoDataRaw(this->m_clPixmapNew.toImage());	//QImageに変換した最新画像をエンコーダに渡す
		}
		else																		//録画フォーマットがMP4以外な場合。JPEGデータが要求される。
		{
			QByteArray	l_clJpgData;												//エンコード後の画像格納用
			QBuffer		l_clJpgBuffer(&l_clJpgData);								//↑のインターフェイス
			this->m_clPixmapNew.save(&l_clJpgBuffer, "jpg", REC_JPEG_QUALITY);		//要求があったのはJPEGなので、JPEGエンコードを行う TODO:品質設定が動作せず、常にクオリティ100%で出力される。要調査
			this->m_pclEncoder->slotAddVideoDataJpg(l_clJpgData);					//エンコードされたJPEG画像をエンコーダに渡す
		}
	}

	if(ENCODER_REC_TYPE_NORMAL == this->m_eRecType)												//録画形式が通常録画だった場合
	{
		this->m_clTimer.start(1000 / this->m_lFramerateNormal);									//フレームレートから逆算した時間間隔で最新画像を取得するタイマーを起動
	}

	this->m_pclEncoder->slotBeginEncode();														//録画開始を試みる
	if(true == this->m_pclEncoder->isEncoding())												//本当に録画が始まった場合
	{
		EL_CMD	l_clCmdNotifyRecStarted;														//録画開始通知送信用コマンド
		EL_SET_CMD_VAL(l_clCmdNotifyRecStarted, EL_CMD_KEY_CMD, IMA_CMD_NOTIFY_REC_STARTED);	//送信するコマンドに録画開始通知をセット
		EL_SET_CMD_VAL(l_clCmdNotifyRecStarted, EL_CMD_KEY_PARAM1, l_qstrDestFilePath);			//PARAM1に実際の保存先を格納
		emit this->sigSendCmd(l_clCmdNotifyRecStarted);											//コマンド発行
	}
	else																						//録画の開始に失敗した場合
	{
		this->procStopRec();																	//一応録画停止処理をしておく
		return false;																			//録画の開始に失敗したのでfalseを返す
	}

	return true;	//録画の開始に成功したっぽいのでtrueを返す
}

/*--------------------------------------------------------------------------------*/
/*!
	@brief	録画停止処理
	@note	録画プラグインに録画停止要求を行うだけ。
			以後の処理は実際に録画が止まってから行われる。
*/
/*--------------------------------------------------------------------------------*/
void ClComponentRec::procStopRec()
{
	this->m_clTimer.stop();							//通常録画用キャプチャタイマーを停止させる

	if(false == this->m_pclEncoder->isEncoding())	//録画プラグインに現在録画中かどうかを問い合わせ
	{
		return;										//録画中じゃないんなら何もしない
	}

	this->m_pclEncoder->slotEndEncode();
}

/*--------------------------------------------------------------------------------*/
/*!
	@brief	録画に関する設定を行う
	@note	レジストリから設定を読むことになるのか？ついでにメンバー変数にも格納しちゃう

	@param[in]	in_qstrDestFilePath	保存先ファイルパス、空っぽの場合は適当に名前を付けてRecフォルダーに格納する

	@retval		最終的な保存先ファイルパス
*/
/*--------------------------------------------------------------------------------*/
QString ClComponentRec::setEncodeParam(QString in_qstrDestFilePath)
{
	this->m_pclEncoder->setIsFragmentMP4(false);								//決め打ち：フラグメントMP4を作らない

	//this->m_clSize = m_clRecSize;										//レジストリから画像サイズを取得
	this->m_pclEncoder->setVideoWidth(this->m_clSize.width());					//動画の幅を設定
	this->m_pclEncoder->setVideoHeight(this->m_clSize.height());				//動画の高さを設定
	this->m_eRecType = this->getRecType();										//録画形式をメンバー変数に格納
	this->m_pclEncoder->setRecType(this->m_eRecType);							//レジストリから読み込んだ録画形式を設定
	if(ENCODER_REC_TYPE_NORMAL == this->m_eRecType)								//通常録画設定だった場合
	{
		this->m_pclEncoder->setVideoFrameRate(this->m_lFramerateNormal);		//決め打ち：通常録画用フレームレート
	}
	else																		//それ以外…多分間欠録画だろうけど
	{
		this->m_pclEncoder->setVideoFrameRate(REC_DEFAULT_INTERVAL_FRAMERATE);	//決め打ち：間欠録画用フレームレート
	}
	this->m_pclEncoder->setIntervalTime(this->getIntervalTime());				//レジストリから読み込んだ間欠時間を設定
	this->m_eRecFormat = this->getContainerType();								//ファイル形式をメンバー変数に格納
	this->m_pclEncoder->setContainerType(this->m_eRecFormat);					//レジストリから読み込んだファイル形式を設定

	/*----------------------------------------*/
	/*
		保存先ファイルパスを確定させて設定する
	*/
	/*----------------------------------------*/
	QString	l_qstrDestFilePath = in_qstrDestFilePath;										//まずは引数で与えられた文字列を愚直に受け取る
	if(true == l_qstrDestFilePath.isNull())													//そもそも中身が空っぽだった場合
	{
		l_qstrDestFilePath = this->createDefaultRecPath();									//デフォルトの保存先、デフォルトのファイル名を生成する
	}
	else																					//引数で与えられた文字列に何かしら文字列が入っていた場合
	{
		l_qstrDestFilePath = l_qstrDestFilePath.replace("\\", "/");							//面倒事を避けるため、セパレータは/統一
		QString	l_qstrDir = l_qstrDestFilePath.left(l_qstrDestFilePath.lastIndexOf("/"));	//最後のセパレータまでを切り取ることで、指定されたファイルまでのディレクトリとする
		QDir	l_clDir;																	//mkpathとかstaticにしてくれればいいのに
		l_clDir.mkpath(l_qstrDir);															//指定されたファイルパスまでのディレクトリ生成を試みる
	}
	l_qstrDestFilePath = this->adjustExtension(l_qstrDestFilePath);							//拡張子調整。実際のフォーマットに適合した拡張子を付与する
	this->m_pclEncoder->setDestFilePath(l_qstrDestFilePath);								//録画プラグインに保存先フルパスを設定

	return l_qstrDestFilePath;																//録画プラグインに設定した保存先フルパスを返す
}

/*--------------------------------------------------------------------------------*/
/*!
	@brief	新着画像の要求
	@note	新着画像の要求を発行し、最大でタイムアウト時間まで待つ
			取得結果の成否や実際の画像は、メンバー変数を参照すること。

	@param[in]	in_lTimeout_ms	タイムアウト時間。単位はms
*/
/*--------------------------------------------------------------------------------*/
void ClComponentRec::tryNewImage(qint32 in_lTimeout_ms)
{
	this->m_bGetNewImage = false;														//最新画像取得済みフラグをリセット

	EL_CMD	l_clCmdReqCapture;															//キャプチャ要求コマンド送信用
	EL_SET_CMD_VAL(l_clCmdReqCapture, EL_CMD_KEY_CMD, IMA_CMD_REQ_CAPTUREIMAGE);		//送信するコマンドはキャプチャ要求コマンド
	EL_SET_CMD_VAL(l_clCmdReqCapture, EL_CMD_KEY_PARAM1, dynamic_cast<QObject*>(this));	//PARAM1に要求元クラスのポインタとして自身のポインタを設定
	EL_SET_CMD_VAL(l_clCmdReqCapture, EL_CMD_KEY_PARAM2, this->m_clSize);				//PARAM2に欲しい画像のサイズを設定
	emit this->sigSendCmd(l_clCmdReqCapture);

	QMutex	l_clMutexSleep;						//Sleep代わりのMutex
	l_clMutexSleep.lock();						//まずはLock
	for(qint32 i = 0; i < in_lTimeout_ms; i++)	//待ち時間分繰り返す
	{
		l_clMutexSleep.tryLock(1);				//1msだけwait
		if(true == this->m_bGetNewImage)		//最新画像取得済みフラグ確認
		{
			break;								//最新画像が取得できていたなら、もう待つ必要は無い
		}
	}
}

eENCODER_CONTAINER_TYPE ClComponentRec::getContainerType()
{
	return static_cast<eENCODER_CONTAINER_TYPE>(DEFAULT_REC_FORMAT);
}

void ClComponentRec::setRecSize(QSize size)
{
	m_clSize = size;
}

QSize ClComponentRec::getRecSize()
{
	return m_clSize;
}

qint32 ClComponentRec::getIntervalTime()
{
	return DEFAULT_REC_INTERVAL;
}

eENCODER_REC_TYPE ClComponentRec::getRecType()
{
	return static_cast<eENCODER_REC_TYPE>(DEFAULT_REC_TYPE);
}

QString ClComponentRec::createDefaultRecPath()
{
	return QApplication::applicationDirPath() + "/" + QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
}


/*--------------------------------------------------------------------------------*/
/*!
	@brief	拡張子調整
	@note	録画プラグインに設定されたファイル形式に応じて、最適な拡張子を付与する

	@param[in]	in_qstrBaseFilePath	拡張子を付与したい文字列

	@retval	引数に最適な拡張子を付与または変更された文字列
*/
/*--------------------------------------------------------------------------------*/
QString ClComponentRec::adjustExtension(QString in_qstrBaseFilePath)
{
	QString	l_qstrExtension;
	eENCODER_CONTAINER_TYPE	l_eContainerType = this->m_pclEncoder->containerType();
	if(ENCODER_CONTAINER_TYPE_AVI == l_eContainerType)
	{
		l_qstrExtension = ".avi";
	}
	else if(ENCODER_CONTAINER_TYPE_MP4 == l_eContainerType)
	{
		l_qstrExtension = ".mp4";
	}

	if(in_qstrBaseFilePath.right(1) != ".")	//右端が.でない場合
	{
		return in_qstrBaseFilePath + l_qstrExtension;
	}
	else
	{
		QString l_qstrBaseExtension = in_qstrBaseFilePath.right(in_qstrBaseFilePath.length() - in_qstrBaseFilePath.lastIndexOf("."));	//拡張子を、"."を含めて抜き出し
		if(l_qstrExtension != l_qstrBaseExtension.toLower())	//一応小文字にしてから比較する
		{
			in_qstrBaseFilePath.chop(in_qstrBaseFilePath.length() - in_qstrBaseFilePath.lastIndexOf("."));	//"."を含めて、異なっている拡張子を一旦千切る
			return in_qstrBaseFilePath + l_qstrExtension;		//正しい拡張子を付与
		}
	}

	return in_qstrBaseFilePath;	//ここまで来たってことはどうやらいじる必要が無かったっぽいのでそのまんま引数を返す
}

/*--------------------------------------------------------------------------------*/
/*!
	@brief	コマンド受信処理
*/
/*--------------------------------------------------------------------------------*/
void ClComponentRec::slotRecvCmd(EL_CMD in_clCmd)
{
	QUuid l_clCommand = EL_GET_CMD_VAL2(in_clCmd, EL_CMD_KEY_CMD, QUuid);

	if(IMA_CMD_REQ_REC_START == l_clCommand)
	{	// 録画開始要求
		QString	l_qstrDestPath = EL_GET_CMD_VAL2(in_clCmd, EL_CMD_KEY_PARAM1, QString);	//PARAM1からファイルのフルパスを取得
		this->procStartRec(l_qstrDestPath);												//録画開始処理を実行
	}
	else if(IMA_CMD_REQ_REC_STOP == l_clCommand)
	{	// 録画停止要求
		this->procStopRec();	//録画停止処理を実行
	}
	else if(IMA_CMD_REQ_CAPTUREIMAGE == l_clCommand)
	{	// キャプチャ画像要求に対する応答
		QPixmap	l_clPixmap = EL_GET_CMD_VAL2(in_clCmd, EL_CMD_KEY_PARAM2, QPixmap);	//Param2から格納されているであろうキャプチャ画像を取得
		if(false == l_clPixmap.isNull())											//有効な画像が得られた場合
		{
			this->m_clPixmapNew = l_clPixmap;										//メンバー変数に保持
			this->m_bGetNewImage = true;											//新しい画像を取得したのでtrueをセット
		}
	}
}

/*--------------------------------------------------------------------------------*/
/*!
	@brief	JPEGエンコードクラスからのJPEG出来たよ通知

	@param[in]	in_clQBAJpegData	エンコードされたJPEGデータ
*/
/*--------------------------------------------------------------------------------*/
void ClComponentRec::slotEncodedJPEG(QByteArray in_clQBAJpegData)
{
	this->m_bIsWaitingForJPEG = false;			//JPEGエンコード完了待ちフラグを下げる
	emit this->sigAddJpgData(in_clQBAJpegData);	//エンコードスレッドへのJPEGデータ送信Signalを発行する
}

/*--------------------------------------------------------------------------------*/
/*!
	@brief	録画プラグインからの録画終了通知

	@param[in]	in_eStopReason	録画終了の理由。普通に終わったのか何かエラーが起こったのか。
*/
/*--------------------------------------------------------------------------------*/
void ClComponentRec::slotEncodeStopped(eENCODER_STOP_REASON in_eStopReason)
{
	this->m_clPixmapNew = QPixmap();	//保持している最新画像を無効な画像にしておく

	EL_CMD	l_clCmdRecTerminate;
	EL_SET_CMD_VAL(l_clCmdRecTerminate, EL_CMD_KEY_CMD, IMA_CMD_NOTIFY_REC_STOPPED);
	EL_SET_CMD_VAL(l_clCmdRecTerminate, EL_CMD_KEY_PARAM1, static_cast<qint32>(in_eStopReason));
	emit this->sigSendCmd(l_clCmdRecTerminate);

	if(ENCODER_STOP_REASON_NOERROR != in_eStopReason)
	{
		qDebug() << "error";
	}
}

/*--------------------------------------------------------------------------------*/
/*!
	@brief	録画プラグインからのJPG画像要求
*/
/*--------------------------------------------------------------------------------*/
void ClComponentRec::slotReqJpgCapture(QVariant &out_clJpgData, qint32 &out_lType)
{
	this->tryNewImage(REC_CAPTURE_TIMEOUT_MS);								//タイムアウト時間を指定してキャプチャ要求
	if(false == this->m_bGetNewImage)										//最新画像を得ることが出来なかった場合
	{
		out_lType = 2;														//失敗的な結果を格納
		return;																//最新画像が無いのではどうしようもないので処理終了
	}

	if(false == this->m_clPixmapNew.isNull())								//得られたと思った画像がまともであることを確認
	{
		QByteArray	l_clJpgData;											//エンコード後の画像格納用
		QBuffer		l_clJpgBuffer(&l_clJpgData);							//↑のインターフェイス
		this->m_clPixmapNew.save(&l_clJpgBuffer, "jpg", REC_JPEG_QUALITY);	//要求があったのはJPEGなので、JPEGエンコードを行う
		//qDebug() << "JPEG Size :" << l_clJpgData.size() << "Byte";

		out_clJpgData = l_clJpgData;										//画像出力先にエンコードされた画像を格納
		out_lType = 0;														//成功的な結果を格納
	}
	else																	//得られたと思った画像がまともじゃ無い場合
	{
		out_lType = 2;														//失敗的な結果を格納するのみ。
	}
}

/*--------------------------------------------------------------------------------*/
/*!
	@brief	録画プラグインからのRAW画像要求
*/
/*--------------------------------------------------------------------------------*/
void ClComponentRec::slotReqRawCapture(QVariant &out_clRawData, qint32 &out_lType)
{
	/*----------------------------------------*/
	/*
		最新画像更新要求を発行
	*/
	/*----------------------------------------*/
	this->tryNewImage(REC_CAPTURE_TIMEOUT_MS);					//タイムアウト時間を指定してキャプチャ要求
	if(false == this->m_bGetNewImage)							//最新画像を得ることが出来なかった場合
	{
		out_lType = 2;											//失敗的な結果を格納
		return;													//最新画像が無いのではどうしようもないので処理終了
	}

	/*----------------------------------------*/
	/*
		保存先に格納
	*/
	/*----------------------------------------*/
	if(false == this->m_clPixmapNew.isNull())					//得られたと思った画像がまともであることを確認
	{
		QImage	l_clRawData = this->m_clPixmapNew.toImage();	//録画プラグインはQImageを欲しがるのでQImageに変換
		out_clRawData = l_clRawData;							//変換したQImageを出力先に格納
		out_lType = 0;											//成功的な結果を格納
	}
	else														//得られたと思った画像がまともじゃ無い場合
	{
		out_lType = 2;											//失敗的な結果を格納するのみ
	}
}

/*--------------------------------------------------------------------------------*/
/*!
	@brief	通常録画用の新着画像要求slot
*/
/*--------------------------------------------------------------------------------*/
void ClComponentRec::slotTryNewImage()
{
	if(true == this->m_pclEncoder->isEncoding())
	{
		this->tryNewImage(REC_CAPTURE_TIMEOUT_MS);										//最新画像の取得を試みる
		if(true == this->m_bGetNewImage && false == this->m_clPixmapNew.isNull())		//最新画像の取得に成功したっぽい場合
		{
			if(ENCODER_CONTAINER_TYPE_AVI ==  this->m_pclEncoder->containerType())		//録画フォーマットがAVIだった場合。JPEGが要る。
			{
				if(false == this->m_bIsWaitingForJPEG)									//多重Signal発行抑止フラグを確認
				{
					this->m_bIsWaitingForJPEG = true;									//これからJPEGエンコード要求を出すので、エンコード完了待ちフラグを立てる
					emit this->sigReqEncodeJPEG(this->m_clPixmapNew);					//取得した最新画像をコピーしたQPixmapに対してJPEGエンコードを要求
				}
			}
			else if(ENCODER_CONTAINER_TYPE_MP4 == this->m_pclEncoder->containerType())	//録画フォーマットがMP4だった場合。QImageが要る。
			{
				emit this->sigAddRawData(this->m_clPixmapNew.toImage());				//録画プラグインへのQImage追加Signalを発行
			}
		}
	}
}

/*--------------------------------------------------------------------------------*/
/*!
	@brief	OSがスリープに入ったときの処理
	@note	MACでは、スリープ中にデスクトップキャプチャすると死ぬので、
			スリープに入った時点で録画を止める
*/
/*--------------------------------------------------------------------------------*/
void ClComponentRec::slotSystemSleep()
{
	this->procStopRec();	//録画停止させる
}
/*================================================================================*/
