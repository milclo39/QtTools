/*******************************************************************************
*                                                                              *
*	Win32 SXUPTP Library                                                       *
*                                                                              *
*   silex technology, Inc. CONFIDENTIAL                                        *
*                                                                              *
*----------+---------+---------------------------------------------------------*
*   日付   | Version |                         内  容                          *
*----------+---------+---------------------------------------------------------*
* 04-07-27 |  1.0.0  |                                                         *
*----------+---------+---------------------------------------------------------*
* 06-12-20 |  1.2.0  | 同期型ソケット版関数を追加                              *
*----------+---------+---------------------------------------------------------*
* 07-01-15 |  1.3.0  | JCP 検索ルール番号設定関数を追加                        *
*          |         | PnP 確認処理付き接続／切断関数を追加                    *
*----------+---------+---------------------------------------------------------*
* 08-12-14 |  1.4.0  | SX Keep Alive 機能の追加                                *
*----------+---------+---------------------------------------------------------*
* 09-04-27 |  1.5.0  | JCP 検索時に CPU 使用率を抑えるように対応               *
*----------+---------+---------------------------------------------------------*
* 09-09-02 |  1.6.0  | デバイスサーバ再起動 API を追加                         *
*----------+---------+---------------------------------------------------------*
* 13-06-13 |  1.8.0  | SXPSERVER 構造体にホスト名(szHostName)を追加            *
*          |         | システムステータス取得関数を追加                        *
*----------+---------+---------------------------------------------------------*
* 13-12-06 |  2.0.0  | 切断要求 API 追加                                       *
*----------+---------+---------------------------------------------------------*
* 14-03-03 |  2.1.0  | インスタンス ID 取得 API 追加                           *
*----------+---------+---------------------------------------------------------*
*******************************************************************************/
#ifndef _INC_SXUPTP_H
#define _INC_SXUPTP_H


/*----------------------------------------------------------------------------*/
/*		定数定義                                                              */
/*----------------------------------------------------------------------------*/
#define SXUPTP_MACHINE		16				// 機種名最大値
#define SXUPTP_PORT			32				// USB 論理ポート名最大値
#define SXUPTP_DEVICE		64				// デバイス名最大値
#define SXUPTP_LOCATION		16				// 接続位置情報
#define SXUPTP_HOSTNAME		16				// ホスト名最大値

#define SRU_NOTIFY			(WM_APP + 800)	// Request Use イベント発生
#define SRU_REQUEST			0x0001			// USB デバイス使用要求受信
#define SRU_CANCEL			0x0002			// 使用要求キャンセル
#define SRU_ACCEPT			0x0003			// 使用要求受付
#define SRU_REFUSE			0x0004			// 使用要求拒否
#define SRU_DISCONNECT		0x0005			// 切断完了
#define SRU_BUSY			0x0006			// 他者と会話中
#define SRU_REPLY			0x0007			// 使用要求の受信応答
#define SRU_NOTFOUND		0x0008			// USB デバイスが見つからない
#define SRU_REPLYEX			0x0009			// 受信応答(自動切断が有効)
#define SRU_DISCONTIME		0x000B			// 自動切断までの残時間応答
#define SRU_CONFIRM			0xFFFE			// USB デバイス接続状態確認
#define SRU_TIMEOUT			0xFFFF			// 通信タイムアウト


/*----------------------------------------------------------------------------*/
/*		構造体定義                                                            */
/*----------------------------------------------------------------------------*/
#pragma pack(1)
typedef struct _SXPSERVER {
	BYTE	bNodeaddr[6];					// Ethernet アドレス
	DWORD	dwIp;							// IP アドレス
	char	szMachineType[SXUPTP_MACHINE];	// デバイスサーバ機種名
	char	szHostName[SXUPTP_HOSTNAME];	// ホスト名
} SXPSERVER, *LPSXPSERVER;

typedef struct _SXUSBDEVICE {
	char	szPortName[SXUPTP_PORT];		// USB 論理ポート名
	char	szDeviceName[SXUPTP_DEVICE];	// デバイス名
	DWORD	dwIpPc;							// 接続先 PC の IP アドレス
	WORD	wStatus;						// ステータス情報
	WORD	wVid;							// デバイスの VID
	WORD	wPid;							// デバイスの PID
	char	szLocation[SXUPTP_LOCATION];	// デバイス接続位置情報
	WORD	wClass;							// USB Interface Class
} SXUSBDEVICE, *LPSXUSBDEVICE;

typedef struct _SRUINFO {
	DWORD	dwSize;							// SRUINFO 構造体サイズ
	WORD	wMessage;						// SVL Request Use Message
	char	szMyName[64];					// 送信元名(ユーザー名やコンピュータ名等)
	char	szDestName[64];					// 送信先名(ユーザー名やコンピュータ名等)
	DWORD	dwSrvIp;						// デバイスサーバの IP アドレス
	BYTE	bNodeaddr[6];					// デバイスサーバの Ethernet アドレス
	WORD	wVid;							// VID (企業 ID)
	WORD	wPid;							// PID (製品 ID)
	char	szDeviceName[SXUPTP_DEVICE];	// USB デバイス名
	char	szLocation[16];					// USB デバイス位置情報
	DWORD	dwRemainingTime;				// 自動切断実行までの残時間 (秒)
} SRUINFO, *LPSRUINFO;

typedef struct _SXDEVINST {
	LPSTR	lpszInstanceId;					// インスタンス ID
} SXDEVINST, *LPSXDEVINST;
#pragma pack()


/*----------------------------------------------------------------------------*/
/*		関数定義                                                              */
/*----------------------------------------------------------------------------*/
BOOL APIENTRY SxuptpEnumDeviceServers(LPDWORD, DWORD, LPVOID, DWORD, LPDWORD, LPDWORD);
BOOL APIENTRY SxuptpEnumDevices(DWORD, LPVOID, DWORD, LPDWORD, LPDWORD);
BOOL APIENTRY SxuptpDeviceConnect(DWORD, LPBYTE, LPSTR, BOOL);
BOOL APIENTRY SxuptpDeviceConnectEx(DWORD, LPBYTE, LPSXUSBDEVICE, BOOL);
BOOL APIENTRY SxuptpDeviceDisconnect(DWORD, LPBYTE, LPSTR);
BOOL APIENTRY SxuptpDeviceDisconnectEx(DWORD, LPBYTE, LPSXUSBDEVICE);
BOOL APIENTRY SxuptpGetDeviceServerInfo(DWORD, LPVOID, DWORD);
DWORD APIENTRY SxuptpGetDeviceStatus(DWORD, LPSTR);
BOOL APIENTRY SxuptpGetSystemStatus(DWORD, LPSTR, DWORD);
BOOL APIENTRY SxuptpSetSrchRuleCookie(WORD);
BOOL APIENTRY SxuptpRebootDeviceServer(DWORD, LPBYTE);

BOOL APIENTRY SruInitialize(HINSTANCE, HWND, LPCSTR);
BOOL APIENTRY SruUninitialize(HINSTANCE);
HANDLE APIENTRY SruSendRequest(HWND, DWORD, LPBYTE, LPSXUSBDEVICE, DWORD);
BOOL APIENTRY SruCancelRequest(HANDLE);
BOOL APIENTRY SruCloseHandle(HANDLE);
BOOL APIENTRY SruSendReply(WORD, DWORD);
BOOL APIENTRY SruAcceptRequest(VOID);
BOOL APIENTRY SruRefuseRequest(VOID);
BOOL APIENTRY SruDisconnectedDevice(VOID);
BOOL APIENTRY SruDeviceNotFound(VOID);

DWORD APIENTRY SxuptpGetInstanceId(DWORD, LPCSTR, LPSXDEVINST *);
BOOL APIENTRY SxuptpFreeInstanceId(LPSXDEVINST);

#endif /* _INC_SXUPTP_H */
