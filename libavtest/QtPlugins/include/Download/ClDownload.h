//--------------------------------------------------------------------------------//
/*!
	@file	ClDownload.h
	@brief	メインクラスヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLDOWNLOAD_H
#define CLDOWNLOAD_H
#include <QObject>
#include <QUrl>
#include <QPixmap>
#ifndef QT_NO_OPENSSL
#include <QSslError>
#endif

class ClCommunication;
class ClParseXmlNetwork;
class ClParseXmlUpdate;
class ClProgressDialog;

namespace Update
{
/*!
	@brief	読み込んだデータ情報
*/
typedef struct
{
	QString strFileName;	//!< ファイル名
	QByteArray clData;		//!< データ
}stDataInfo;
}
using namespace Update;
//--------------------------------------------------------------------------------//
/*!
	@brief	XML解析、通信制御クラス
*/
//--------------------------------------------------------------------------------//
class ClDownload : public QObject
{
	Q_OBJECT

protected:
	/*!
		@brief	アップデートファイル情報
	*/
	typedef struct
	{
		QString strDir;			//!< ディレクトリ名
		QString strFileName;	//!< ファイル名
		QString strHash;		//!< ハッシュ値
		bool bIsNotNeedCopy;		//!< コピーが必要ないファイル
	}stFileInfo;

public:
	ClDownload(bool bIsLibrary = true);	//コンストラクタ
	virtual ~ClDownload();	//デストラクタ
	bool exec(int argc = 0, char **argv = NULL);	//処理開始
	bool exec(QStringList clArguments);				//処理開始
	/*!
		@brief	終了したか取得
		@retval	終了していたらtrue
	*/
	bool getIsExit(){return m_bIsExit;}
	void setDispProgress();	//プログレスダイアログ表示設定

protected:
	virtual void clearSetting();		//設定クリア
	virtual void loadFile(QString strFileName);	//ファイルダウンロード
	virtual void saveTemp(QString strFileName, QByteArray clData);	//途中保存
	virtual bool saveFiles(qint64 updateFiles);					//正保存
	virtual void removeTempFiles();				//一時保存ファイル削除
	virtual bool waitEndProcess(QStringList clProcessList);	//プロセス終了待ち
	virtual void execRemove();	//削除実行
	virtual void removeDirectory(QString strDir);	//フォルダ削除
	virtual bool checkVersion();	//バージョン確認
	virtual bool compareVersion(QString strLoadVersion, QString strCurrentVersion);	//バージョン比較
	virtual void execExit();	//最終処理
	virtual bool copyLibraries();	//自分アップデート用ライブラリコピー
	virtual void setDefaultProxy();	// デフォルトプロキシセット
	virtual void setProgressLogo(QPixmap clImage); // プログレスダイアログロゴセット
#ifndef QT_NO_OPENSSL
	virtual void setIgnoreSslErrs( QList<QSslError::SslError> clLstErrs );	// 無視するSSLエラーをセット
#endif

protected slots:
	virtual void slotEndCommunication();		//通信終了
	virtual void slotError(QString strError);	//通信エラー通知
	virtual void slotCancel();					//キャンセル

signals:
	/*!
		@brief		成功通知
		@param[in]	clDataList : 読み込んだデータリスト
	*/
	void sigSuccess(QList<stDataInfo> clDataList);
	/*!
		@brief		エラー通知
		@param[in]	strError : エラー表示文字列
	*/
	void sigError(QString strError);
	/*!
		@brief		総ファイルダウンロード状態通知
		@param[in]	lReceived : ダウンロード済みファイル数
		@param[in]	lTotal : 総ファイル数
	*/
	void sigTotalProgress(qint32 lReceived, qint32 lTotal);
	/*!
		@brief		1ファイルダウンロード状態通知
		@param[in]	llReceived : 受信バイト数
		@param[in]	llTotal : 総バイト数
	*/
	void sigPartProgress(qint64 llReceived, qint64 llTotal);

protected:
	bool m_bIsExit;						//!< 終了フラグ
	bool m_bIsAutoUpdate;				//!< 自動アップデートフラグ
	bool m_bIsDispProgress;				//!< プログレスダイアログ表示フラグ
	bool m_bIsUseAsLibrary;				//!< ライブラリとして使用フラグ
	qint32 m_lListNumber;				//!< 現在のダウンロードリスト番号
	ClCommunication *m_pclCommunication;	//!< 通信クラス
	ClParseXmlNetwork *m_pclNetworkParser;	//!< ネットワーク設定XML解析クラス
	ClParseXmlUpdate *m_pclCurrentParser;	//!< カレントのXML解析クラス
	ClParseXmlUpdate *m_pclLoadParser;	//!< 読み込んだXML解析クラス
	ClProgressDialog *m_pclProgressDialog;	//!< プログレスダイアログ
	QString m_strUrlDirectory;			//!< 通信先URLディレクトリ
	QString m_strFileName;				//!< ダウンロード中ファイル名
	QString m_strXmlFileName;			//!< XMLファイル名
	QByteArray m_clLoadXmlData;			//!< 落としてきたXMLデータ
	QList<stFileInfo> m_clUpFileList;	//!< バージョンアップファイルリスト
	QList<stDataInfo> m_clDataList;		//!< ダウンロードデータリスト
	QList<QString> m_clRmFileList;		//!< ファイル削除リスト
	QList<QString> m_clRmDirList;		//!< ディレクトリ削除リスト
	QString m_strUpapp;					//!< 自分のバージョンアップアプリ名
	qint32 m_lRetryCount;				//!< リトライ回数
	bool m_bIsEndDownload;				//!< ダウンロード終了フラグ
	QString m_strUrl;					//!< url設定
	QString m_strNetworkFileName;		//!< ネットワーク設定ファイル名
	QString m_strMainApplication;		//!< メインのアップデート対象アプリ名
};
//--------------------------------------------------------------------------------//
#endif
