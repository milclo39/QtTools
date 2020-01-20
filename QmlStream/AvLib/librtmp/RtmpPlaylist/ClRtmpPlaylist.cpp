//--------------------------------------------------------------------------------//
/*!
	@file	ClRtmpPlaylist.cpp
	@brief	crtmpserver/flvplayback用RTMPプレイリスト取得クラス
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#include "ClRtmpPlaylist.h"
#include "rtmp.h"
#include "amf.h"
#ifdef Q_OS_WIN
#include <WinSock2.h>

extern "C"{
//RTMP関数
typedef RTMP *(__cdecl *LIB_RTMP_Alloc)();	//RTMP確保
typedef void (__cdecl *LIB_RTMP_ARC_RTMP)(RTMP*);	//引数がRTMP*だけの関数(RTMP_Init,RTMP_Close,RTMP_Free,RTMP_EnableWrite)
typedef int (__cdecl *LIB_RTMP_ARC_RTMP_INT)(RTMP*);	//引数がRTMP*だけで戻り値がint(RTMP_
typedef int (__cdecl *LIB_RTMP_SetupURL)(RTMP*, char*);	//URL設定
typedef int (__cdecl *LIB_RTMP_Connect)(RTMP*, RTMPPacket*);	//接続
typedef int (__cdecl *LIB_RTMP_SendPacket)(RTMP*, RTMPPacket*, int);	//パケット送信
typedef int (__cdecl *LIB_RTMP_ReadPacket)(RTMP*, RTMPPacket*);	//パケット受信

typedef int (__cdecl *LIB_RTMP_PACKET)(RTMPPacket*, int);

//AMF関数
typedef char *(__cdecl *LIB_AMF_EncodeString)(char*, char*, const AVal*);	//テキストAMFデータ生成
typedef char *(__cdecl *LIB_AMF_EncodeNumber)(char*, char*, const int);		//数値AMFデータ生成
typedef int (__cdecl *LIB_AMF_Decode)(AMFObject*, const char*, int, int);	//受信AMF解析
}
#elif defined(Q_OS_ANDROID)
#include <sys/socket.h>
#endif

#define SOCKET_CONNECT_TIMEOUT	1
#define SOCKET_BUFFERSIZE	1024
#include <QDebug>
//--------------------------------------------------------------------------------//
/*!
	@brief		リスト解析
	@param[in]	amfObj : AMF情報
	@param[out]	pListStream : リスト格納先
	@retval		リスト取得できていた場合true,それ以外はfalse
*/
//--------------------------------------------------------------------------------//
static bool parsePlaylist(AMFObject amfObj, QStringList *pListStream)
{
	bool bIsSetAvailableFlvs = false;	//リスト返信フラグ
	for(int i = 0; i < amfObj.o_num; i++)
	{
		AMFObjectProperty amfProperty = amfObj.o_props[i];
		switch(amfProperty.p_type & 0xff)	//Androidだとごみ?が入っているようなので1バイト目だけ確認する
		{
		case AMF_STRING:
		{
			if(0 == strcmp("SetAvailableFlvs", amfProperty.p_vu.p_aval.av_val))	//リスト返信だった
			{
				bIsSetAvailableFlvs = true;
			}
			break;
		}
		case AMF_OBJECT:	//旧
		case AMF_ECMA_ARRAY:	//新
		{
			AMFObject amfObjChild = amfProperty.p_vu.p_object;
			for(int j = 0; j < amfObjChild.o_num; j++)
			{
				AMFObjectProperty amfPropertyChild = amfObjChild.o_props[j];
				if((amfPropertyChild.p_type & 0xff) == AMF_STRING)	//ストリームリスト
				{
					pListStream->append(amfPropertyChild.p_vu.p_aval.av_val);
				}
			}
			break;
		}
		default:
		{
			break;
		}
		}
	}
	return bIsSetAvailableFlvs;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		コンストラクタ
	@param[in]	親
*/
//--------------------------------------------------------------------------------//
ClRtmpPlaylist::ClRtmpPlaylist(QObject *pParent) : QThread(pParent)
{
	m_pRtmp = NULL;
#ifdef Q_OS_WIN
	m_libRtmp.setFileName("librtmp-1.dll");
#endif
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		デストラクタ
*/
//--------------------------------------------------------------------------------//
ClRtmpPlaylist::~ClRtmpPlaylist()
{
	endProc();
	wait(1000);
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		リスト取得
	@param[in]	strUrl : サーバURL(アプリ名まで)
*/
//--------------------------------------------------------------------------------//
void ClRtmpPlaylist::getList(QString strUrl)
{
	if(false == isRunning())
	{
		m_strUrl = strUrl;
		start();
	}
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		メイン処理
*/
//--------------------------------------------------------------------------------//
void ClRtmpPlaylist::run()
{
	int iReadResult = 0;
#ifdef Q_OS_WIN
	if(false == m_libRtmp.load())
	{
		return;
	}
	LIB_RTMP_Alloc RTMP_Alloc = (LIB_RTMP_Alloc)m_libRtmp.resolve("RTMP_Alloc");
	LIB_RTMP_ARC_RTMP RTMP_Init = (LIB_RTMP_ARC_RTMP)m_libRtmp.resolve("RTMP_Init");
	LIB_RTMP_SetupURL RTMP_SetupURL = (LIB_RTMP_SetupURL)m_libRtmp.resolve("RTMP_SetupURL");
	LIB_RTMP_ARC_RTMP RTMP_EnableWrite = (LIB_RTMP_ARC_RTMP)m_libRtmp.resolve("RTMP_EnableWrite");
	LIB_RTMP_Connect RTMP_Connect = (LIB_RTMP_Connect)m_libRtmp.resolve("RTMP_Connect");
	LIB_RTMP_SendPacket RTMP_SendPacket = (LIB_RTMP_SendPacket)m_libRtmp.resolve("RTMP_SendPacket");
	LIB_RTMP_ReadPacket RTMP_ReadPacket = (LIB_RTMP_ReadPacket)m_libRtmp.resolve("RTMP_ReadPacket");
	LIB_AMF_EncodeString AMF_EncodeString = (LIB_AMF_EncodeString)m_libRtmp.resolve("AMF_EncodeString");
	LIB_AMF_EncodeNumber AMF_EncodeNumber = (LIB_AMF_EncodeNumber)m_libRtmp.resolve("AMF_EncodeNumber");
	LIB_AMF_Decode AMF_Decode = (LIB_AMF_Decode)m_libRtmp.resolve("AMF_Decode");
#endif
	//URLコピー
	char *pPath = (char*)malloc(m_strUrl.length() + 1);
	pPath[m_strUrl.length()] = 0;
	memcpy(pPath, m_strUrl.toLatin1().data(), m_strUrl.length());

	//RTMP取得
	m_pRtmp = RTMP_Alloc();
	if(NULL == m_pRtmp)
	{
		qDebug("alloc");
		return;
	}

	//RTMP初期化
	RTMP_Init(m_pRtmp);
	m_pRtmp->Link.timeout = SOCKET_CONNECT_TIMEOUT;	//接続,通信タイムアウト(秒)
	m_pRtmp->m_inChunkSize = SOCKET_BUFFERSIZE;		//受信チャンクサイズ
	m_pRtmp->m_outChunkSize = SOCKET_BUFFERSIZE;	//送信チャンクサイズ

	//URL設定
	if(0 == RTMP_SetupURL(m_pRtmp, pPath))
	{
		qDebug("setupurl");
		goto ENDPROC;
	}
	RTMP_EnableWrite(m_pRtmp);	//書き込み可能設定

	//接続(RTMP初期化までやってくれる)
	if(0 == RTMP_Connect(m_pRtmp, NULL))
	{
		qDebug("connect");
		goto ENDPROC;
	}

	//受信タイムアウト設定(設定しないと無駄に1秒ロスする)
	timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	setsockopt(m_pRtmp->m_sb.sb_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(timeval));

	//初期化の返信を読み飛ばす
	do
	{
		RTMPPacket packet;
		memset(&packet, 0, sizeof(RTMPPacket));
		iReadResult = RTMP_ReadPacket(m_pRtmp, &packet);
	}while(iReadResult);

	//リスト要求
	{
		RTMPPacket packet;
		char cBuffer[1024], *pcEnd = cBuffer + sizeof(cBuffer);

		packet.m_nChannel = 0x03;
		packet.m_headerType = RTMP_PACKET_SIZE_MEDIUM;
		packet.m_packetType = 0x14;
		packet.m_nInfoField2 = 0;
		packet.m_hasAbsTimestamp = 0;
		packet.m_body = cBuffer + RTMP_MAX_HEADER_SIZE;

		char *pcEnc = packet.m_body;
		char cmd[] = "getAvailableFlvs";
		AVal val = AVC(cmd);
		pcEnc = AMF_EncodeString(pcEnc, pcEnd, &val);
		pcEnc = AMF_EncodeNumber(pcEnc, pcEnd, 0);
		*pcEnc++ = AMF_NULL;

		packet.m_nBodySize = pcEnc - packet.m_body;
		if(0 == RTMP_SendPacket(m_pRtmp, &packet, true))
		{
			qDebug("sendpacket");
			goto ENDPROC;
		}
	}
	//リスト受信
	do
	{
		RTMPPacket packet;
		memset(&packet, 0, sizeof(RTMPPacket));
		iReadResult = RTMP_ReadPacket(m_pRtmp, &packet);
		if(iReadResult &&
				true == RTMPPacket_IsReady(&packet))
		{
			AMFObject amfObj;
			if(0 < AMF_Decode(&amfObj, packet.m_body, packet.m_nBodySize, 0))
			{
				QStringList listStream;
				if(true == parsePlaylist(amfObj, &listStream))	//受信パケット解析
				{
					emit sigPlaylist(listStream);	//プレイリスト通知
					break;	//リスト取得出来たので終了
				}
			}
		}
	}while(iReadResult);

ENDPROC:
#ifdef Q_OS_ANDROID
	if(NULL != pPath)
	{
		free(pPath);
	}
#endif
	endProc();
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		終了処理
*/
//--------------------------------------------------------------------------------//
void ClRtmpPlaylist::endProc()
{
#ifdef Q_OS_WIN
	if(true == m_libRtmp.isLoaded())
#endif
	{
		//RTMP終了処理
		if(NULL != m_pRtmp)
		{
#ifdef Q_OS_WIN
			LIB_RTMP_ARC_RTMP RTMP_Close = (LIB_RTMP_ARC_RTMP)m_libRtmp.resolve("RTMP_Close");
			LIB_RTMP_ARC_RTMP RTMP_Free = (LIB_RTMP_ARC_RTMP)m_libRtmp.resolve("RTMP_Free");
#endif
			if(m_pRtmp->m_sb.sb_socket != -1)
			{
				RTMP_Close(m_pRtmp);
			}
			RTMP_Free(m_pRtmp);
			m_pRtmp = NULL;
		}
#ifdef Q_OS_WIN
		//ライブラリ解放
		m_libRtmp.unload();
#endif
	}
}
//--------------------------------------------------------------------------------//
