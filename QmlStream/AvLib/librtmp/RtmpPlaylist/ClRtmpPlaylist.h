//--------------------------------------------------------------------------------//
/*!
	@file	ClRtmpPlaylist.h
	@brief	crtmpserver/flvplayback用RTMPプレイリスト取得クラスヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#include <QThread>
#include <QStringList>
#include <QLibrary>

struct RTMP;

//--------------------------------------------------------------------------------//
/*!
	crtmpserver/flvplayback用RTMPプレイリスト取得クラス
*/
//--------------------------------------------------------------------------------//
class ClRtmpPlaylist : public QThread
{
	Q_OBJECT

public:
	ClRtmpPlaylist(QObject *pParent = NULL);	//コンストラクタ
	~ClRtmpPlaylist();	//デストラクタ
	void getList(QString strUrl);	//リスト取得

protected:
	virtual void run();	//メイン処理
	void endProc();	//終了処理

signals:
	void sigPlaylist(QStringList listStream);	//!< プレイスリト通知

private:
	RTMP *m_pRtmp;	//!< RTMP操作用変数
	QString m_strUrl;	//!< サーバURL
#ifdef Q_OS_WIN
	QLibrary m_libRtmp;	//!< librtmp
#endif
};
//--------------------------------------------------------------------------------//
