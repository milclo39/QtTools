#include "ElConvClient.h"
#include <QDebug>

#define DEF_URL "https://w4a.elmocloud.com:443/elmo-cloud-rec1/xxx"
#define	TIME_CONNECT_TIMEOUT 30
#define TIME_SELECT_TIMEOUT 10
#define MAX_READBUF 0x4000

ElConvClient::ElConvClient()
{
	m_strUrl = DEF_URL;
	m_pcRecv = new char[MAX_READBUF]();
	m_isCurlConnected = STATE_DISCONNECTED;
	procInit();
	start();	// スレッド開始
}

ElConvClient::~ElConvClient()
{
	terminate();	// スレッド終了
	if(NULL != m_pclCurl){
		curl_easy_cleanup(m_pclCurl);
	}
	if(m_pcRecv){
		delete[] m_pcRecv;
	}
}

void ElConvClient::procInit()
{
	m_pclCurl = curl_easy_init();
	if(NULL == m_pclCurl){
		qDebug() << "failed to curl_easy_init:";
		return;
	}
	curl_easy_setopt(m_pclCurl, CURLOPT_URL, m_strUrl.toLocal8Bit().data());			//接続先URL指定
	curl_easy_setopt(m_pclCurl, CURLOPT_CONNECT_ONLY, true);	//HTTP系処理を行わせず、単純に接続のみを行わせる。

	//暗号化方式の設定。重たかったり脆弱なやつを使わないようにする。
	curl_easy_setopt(m_pclCurl, CURLOPT_SSL_CIPHER_LIST, "HIGH:MEDIUM:!AES256:!3DES:!RC4");
	curl_easy_setopt(m_pclCurl, CURLOPT_TIMEOUT, TIME_CONNECT_TIMEOUT);	//接続時のタイムアウト設定
#if 0
	curl_easy_setopt(m_pclCurl, CURLOPT_SSL_VERIFYPEER, true);	//証明書の検証を有効化
	curl_easy_setopt(m_pclCurl, CURLOPT_CAINFO, "cacert.pem");	//ルート証明書ストアの位置を指定
#else
	curl_easy_setopt(m_pclCurl, CURLOPT_SSL_VERIFYPEER, false);	// 証明書無効
	curl_easy_setopt(m_pclCurl, CURLOPT_SSL_VERIFYHOST, 0);
#endif
	curl_easy_setopt(m_pclCurl, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
}

void ElConvClient::setUrl(QString url, QString token)
{
	if(token.isEmpty()){
		curl_easy_setopt(m_pclCurl, CURLOPT_URL, url.toLocal8Bit().data());
		m_strUrl = url;
	}
	else{
		QString urlt = QString("%1?token=\"%2\"").arg(url).arg(token);
		curl_easy_setopt(m_pclCurl, CURLOPT_URL, urlt.toLocal8Bit().data());
		m_strUrl = urlt;
	}
	qDebug() << "seturl:" << m_strUrl;
}

void ElConvClient::slotConnect(bool val)
{
	if(val){	// 接続
		if(STATE_DISCONNECTED == m_isCurlConnected){
			m_isCurlConnected = STATE_CONNECTING;
		}
	}
	else{		// 切断
		if(STATE_CONNECTED == m_isCurlConnected){
			m_isCurlConnected = STATE_DISCONNECTING;
		}
	}
}

void ElConvClient::slotSend(QByteArray data)
{
	if(STATE_DISCONNECTING != m_isCurlConnected){
		QMutexLocker locker(&m_mutex);
		m_reqQueue.push_back(data);
	}
}

void ElConvClient::run()
{
	qDebug() << "thread start";
	while(true){
		switch(m_isCurlConnected){
		case STATE_CONNECTING:
			{
				CURLcode l_eCurlRet = curl_easy_perform(m_pclCurl);
				if(CURLE_OK != l_eCurlRet){
					m_reqQueue.clear();
					m_isCurlConnected = STATE_DISCONNECTING;
					qDebug() << "failed to curl_easy_perform:" << l_eCurlRet << curl_easy_strerror(l_eCurlRet);
					emit sigError();
					break;
				}
				m_isCurlConnected = STATE_CONNECTED;
				//emit sigMonitor();
				qDebug() << "client connected:";
			}
			break;
		case STATE_DISCONNECTING:
			if(m_reqQueue.empty()){
				curl_easy_cleanup(m_pclCurl);
				m_pclCurl = NULL;
				procInit();
				m_isCurlConnected = STATE_DISCONNECTED;
				//emit sigMonitor();
				qDebug() << "client disconnected:";
				break;
			}
			// データが残っていたら送信継続
		case STATE_CONNECTED:
			if(procRecv() < 0){
				sleep(1);	// 受信エラーはとりあえず待ち
				m_reqQueue.clear();
				break;
			}
			if(!m_reqQueue.empty()){
				QByteArray data;
				{
					QMutexLocker locker(&m_mutex);
					data = m_reqQueue.front();
					m_reqQueue.pop_front();
				}
				procSend(data);
			}
			break;
		}
		usleep(10000);
	}
}

void ElConvClient::procSend(QByteArray data)
{
	CURLcode l_eCurlRet;
	size_t size = 0;
resock:
	// ソケット取得
	curl_socket_t l_sockCurl = -1;
	l_eCurlRet = curl_easy_getinfo(m_pclCurl, CURLINFO_LASTSOCKET, &l_sockCurl);
	if(CURLE_OK != l_eCurlRet)
	{
		qDebug() << "failed to curl_easy_getinfo:" << curl_easy_strerror(l_eCurlRet);
		m_isCurlConnected = STATE_DISCONNECTING;
		return;
	}
	else if(l_sockCurl == -1)
	{
		curl_easy_cleanup(m_pclCurl);
		procInit();
		goto resock;
	}
resend:
	// 送信
	l_eCurlRet = curl_easy_send(m_pclCurl, data.data(), data.size(), &size);
	if(CURLE_AGAIN == l_eCurlRet)
	{
		// ソケットが使えるようになるまで待つ
		fd_set l_fdsetWrite, l_fdsetError;
		timeval l_stTimeout;
		FD_ZERO(&l_fdsetWrite);
		FD_SET(l_sockCurl, &l_fdsetWrite);
		FD_ZERO(&l_fdsetError);
		FD_SET(l_sockCurl, &l_fdsetError);
		l_stTimeout.tv_sec	= TIME_SELECT_TIMEOUT;		//タイムアウト時間設定
		l_stTimeout.tv_usec	= 0;

		select(l_sockCurl + 1, NULL, &l_fdsetWrite, &l_fdsetError, &l_stTimeout);
		qDebug() << "Wait till the socket is ready to use.";
		goto resend;
	}
	else if(CURLE_OK != l_eCurlRet)
	{
		qDebug() << "failed to curl_easy_send:" << curl_easy_strerror(l_eCurlRet);
		emit sigError();
		return;
	}
	qDebug() << "procSend:" << data.size();
}

int ElConvClient::procRecv()
{
	CURLcode l_eCurlRet;
	size_t size = 0;

	// 受信待ち
	l_eCurlRet = curl_easy_recv(m_pclCurl, m_pcRecv, MAX_READBUF, &size);
	if(CURLE_AGAIN == l_eCurlRet){
		return 0;
	}
	if(CURLE_UNSUPPORTED_PROTOCOL == l_eCurlRet){
		return 0;
	}
	else if(CURLE_OK != l_eCurlRet){
		qDebug() << "failed to curl_easy_recv:" << l_eCurlRet << curl_easy_strerror(l_eCurlRet);
		return -1;
	}
	sigRecv(QByteArray(m_pcRecv, size));
	return 1;
}

int ElConvClient::getStatus()
{
	return m_isCurlConnected;
}
