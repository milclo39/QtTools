//--------------------------------------------------------------------------------//
/*!
	@file	QbicCommandDefine.h
	@brief	QBiCコマンド定義
	@author	大橋
	@author	星島
*/
//--------------------------------------------------------------------------------//
#ifndef QBICCOMMANDDEFINE_H
#define QBICCOMMANDDEFINE_H
#include <QDataStream>

//カメラタイプ
#define QBIC_VALUE_TYPE_UNKNOWN		-1	//不明
#define QBIC_VALUE_TYPE_NORMAL		0	//QBiC
#define QBIC_VALUE_TYPE_SECURITY	1	//QBiC S
#define QBIC_VALUE_TYPE_D			2	//D QBiC
#define QBIC_VALUE_TYPE_D2			3	//D QBiC 360

#define QBIC_TYPE_NORMAL			("QBiC")
#define QBIC_TYPE_SECURITY			("QBiC-S")
#define	QBIC_TYPE_D_OLD				("D-QBiC")
#define QBIC_TYPE_D_OLD2			("DQBiC135")
#define QBIC_TYPE_D					("RemoteCamera01")
#define QBIC_TYPE_D2				("RemoteCamera02")

#define QBIC_CMD_HEADER_SIZE		(8)		//コマンドヘッダサイズ
#define QBIC_CMD_DATA_SIZE_DEF		(24)	//基本コマンドデータサイズ
#define QBIC_CMD_PACKET_SIZE_DEF	(QBIC_CMD_HEADER_SIZE + QBIC_CMD_DATA_SIZE_DEF)	//基本1パケットサイズ

#define QBIC_USB_STREAM_ENDIAN	QDataStream::LittleEndian	//USB通信エンディアン

//パケットタイプ
#define QBIC_PKT_COMMAND	((quint16)0)	//コマンド
#define QBIC_PKT_RESPONSE	((quint16)1)	//返信
#define QBIC_PKT_DATA		((quint16)2)	//データ

//エラーコード
#define QBIC_CMD_ERROR				((quint16)0xff00)	//エラー識別値

#define QBIC_CMD_ERROR_SUCCESS		((quint16)0x0000)	//成功
#define QBIC_CMD_ERROR_UNKNOWN		((quint16)0xffff)	//不明なエラー
#define QBIC_CMD_ERROR_TIMEOUT		((quint16)0xfffe)	//タイムアウト
#define QBIC_CMD_ERROR_DATA_LENGTH	((quint16)0xfffd)	//データ長エラー
#define QBIC_CMD_ERROR_COMMAND		((quint16)0xfffc)	//未対応コマンド
#define QBIC_CMD_ERROR_PARAM		((quint16)0xfffb)	//パラメータエラー
#define QBIC_CMD_ERROR_EXEC			((quint16)0xfffa)	//コマンド実行失敗

//execコマンドで使われるはず
#define QBIC_CMD_ERROR_BUSY			((quint16)0xfff9)	//busy
#define QBIC_CMD_ERROR_INTERNAL		((quint16)0xfff8)	//内部エラー
#define QBIC_CMD_ERROR_NOT_SD_SPACE	((quint16)0xfff7)	//SD空き容量不足
#define QBIC_CMD_ERROR_NO_SD_ACCESS	((quint16)0xfff6)	//SDにアクセス不可
#define QBIC_CMD_ERROR_BATTERY_LOW	((quint16)0xfff5)	//Battry容量不足
#define QBIC_CMD_ERROR_FILESYSTEM	((quint16)0xfff4)	//無効なファイルシステム
#define QBIC_CMD_ERROR_LIVE_STREAM	((quint16)0xfff3)	//ライブ中のため失敗
#define QBIC_CMD_ERROR_RECORDING	((quint16)0xfff2)	//録画中のため失敗
#define QBIC_CMD_ERROR_MODE			((quint16)0xfff1)	//コマンド実行できないモード

//--------------------------------------------------------------------------------//
//コマンド
//--------------------------------------------------------------------------------//
//ネットワーク設定取得
#define QBIC_CMD_GET_WIFI_INFO		0x5001	//wi-fi情報取得
#define QBIC_CMD_GET_AP_LIST		0x5002	//アクセスポイントリスト取得
#define QBIC_CMD_GET_WIFI_MODE		0x5003	//起動時wi-fi接続設定取得
#define QBIC_CMD_GET_USE_AUTH		0x5004	//ストリームユーザ認証有無設定取得
#define QBIC_CMD_GET_AUTH_PASSWORD	0x5005	//ストリーム認証パスワード取得
#define QBIC_CMD_GET_ADHOC_ADDRESS	0x5006	//アドホック接続のIPアドレス取得
#define QBIC_CMD_GET_ADHOC_SUBNET	0x5007	//アドホック接続のサブネットマスク取得
#define QBIC_CMD_GET_INFRA_USE_DHCP	0x5008	//インフラストラクチャ接続のDHCPサーバ使用設定取得
#define QBIC_CMD_GET_INFRA_ADDRESS	0x5009	//インフラストラクチャ接続のIPアドレス取得
#define QBIC_CMD_GET_INFRA_SUBNET	0x500a	//インフラストラクチャ接続のサブネットマスク取得
#define QBIC_CMD_GET_INFRA_GATEWAY	0x500b	//インフラストラクチャ接続のデフォルトゲートウェイ取得
//ネットワーク設定
#define QBIC_CMD_SET_WIFI_MODE		0x5803	//起動時wi-fi接続設定変更
#define QBIC_CMD_SET_USE_AUTH		0x5804	//ストリームユーザ認証有無設定
#define QBIC_CMD_SET_AUTH_PASSWORD	0x5805	//ストリーム認証パスワード設定
#define QBIC_CMD_SET_ADHOC_ADDRESS	0x5806	//アドホック接続のIPアドレス設定
#define QBIC_CMD_SET_ADHOC_SUBNET	0x5807	//アドホック接続のサブネットマスク設定
#define QBIC_CMD_SET_INFRA_USE_DHCP	0x5808	//インフラストラクチャ接続のDHCPサーバ使用設定
#define QBIC_CMD_SET_INFRA_ADDRESS	0x5809	//インフラストラクチャ接続のIPアドレス設定
#define QBIC_CMD_SET_INFRA_SUBNET	0x580a	//インフラストラクチャ接続のサブネットマスク取得
#define QBIC_CMD_SET_INFRA_GATEWAY	0x580b	//インフラストラクチャ接続のデフォルトゲートウェイ取得
//録画・画像設定取得
#define QBIC_CMD_GET_REC_STATE		0x5010	//録画状態取得
#define QBIC_CMD_GET_INTERVAL_INFO	0x5011	//インターバル撮影設定取得
#define QBIC_CMD_GET_REC_IMAGESIZE	0x5012	//録画画像サイズ取得
#define QBIC_CMD_GET_REC_FRAMERATE	0x5013	//録画フレームレート取得
#define QBIC_CMD_GET_LIVE_IMAGESIZE	0x5014	//ストリーム画像サイズ取得
#define QBIC_CMD_GET_LIVE_FRAMERATE	0x5015	//ストリームフレームレート取得
//録画・画像設定
#define QBIC_CMD_SET_INTERVAL_INFO	0x5811	//インターバル撮影設定
#define QBIC_CMD_SET_REC_IMAGESIZE	0x5812	//録画画像サイズ設定
#define QBIC_CMD_SET_REC_FRAMERATE	0x5813	//録画フレームレート設定
#define QBIC_CMD_SET_LIVE_IMAGESIZE	0x5814	//ストリーム画像サイズ設定
#define QBIC_CMD_SET_LIVE_FRAMERATE	0x5815	//ストリームフレームレート設定
//GPS
#define QBIC_CMD_GET_GPS_INFO		0x5020	//GPS情報取得
#define QBIC_CMD_GET_GPS_SETTING	0x5021	//GPS設定取得
#define QBIC_CMD_SET_GPS_SETTING	0x5821	//GPS設定設定
//カメラ情報・状態取得
#define QBIC_CMD_GET_CAMERANAME		0x5030	//カメラ名取得
#define QBIC_CMD_GET_TIME			0x5031	//時刻取得
#define QBIC_CMD_GET_TIMEZONE		0x5032	//タイムゾーン取得
#define QBIC_CMD_GET_LED_ON_START	0x5033	//起動時LED点灯設定取得
#define QBIC_CMD_GET_LED_STATE		0x5034	//LED状態取得
#define QBIC_CMD_GET_OPTIMIZER		0x5035	//コントラストオプティマイザ使用設定取得
#define QBIC_CMD_GET_ELECTRIC_STATE	0x5036	//給電状態取得
#define QBIC_CMD_GET_BATTERY_STATE	0x5037	//電池残量取得
#define QBIC_CMD_GET_AUTO_POWEROFF	0x5038	//自動電源OFF設定取得
#define QBIC_CMD_GET_SD_INFO		0x5039	//SDカード情報取得
#define QBIC_CMD_GET_VERSION		0x503a	//バージョン情報取得
#define QBIC_CMD_GET_INFRA_SSID		0x503b	//インフラストラクチャ用SSID
#define QBIC_CMD_GET_INFRA_KEY		0x503c	//インフラストラクチャ用暗号キー
//カメラ情報・状態設定
#define QBIC_CMD_SET_CAMERANAME		0x5830	//カメラ名設定
#define QBIC_CMD_SET_TIME			0x5831	//時刻設定
#define QBIC_CMD_SET_TIMEZONE		0x5832	//タイムゾーン設定
#define QBIC_CMD_SET_LED_ON_START	0x5833	//起動時LED点灯設定
#define QBIC_CMD_SET_LED_STATE		0x5834	//LED状態設定
#define QBIC_CMD_SET_OPTIMIZER		0x5835	//コントラストオプティマイザ使用設定
#define QBIC_CMD_SET_AUTO_POWEROFF	0x5838	//自動電源OFF設定
#define QBIC_CMD_SET_INFRA_SSID		0x583b	//インフラストラクチャ用SSID
#define QBIC_CMD_SET_INFRA_KEY		0x583c	//インフラストラクチャ用暗号キー

//add
#define QBIC_CMD_GET_WIFI_COUNTRY	0x5041	//wifi対応国取得
#define QBIC_CMD_GET_ADHOC_SECURE	0x5042	//アドホックセキュリティ種別取得
#define QBIC_CMD_GET_RTSP_PORT		0x5043	//RTSPポート番号取得
#define QBIC_CMD_GET_STUN_SERVER	0x5044	//STUNサーバーアドレス取得
#define QBIC_CMD_GET_PRIMARY_DNS	0x5045	//プライマリDNSサーバーアドレス取得
#define QBIC_CMD_GET_SECONDARY_DNS	0x5046	//セカンダリDNSサーバーアドレス取得
#define QBIC_CMD_GET_WIFI_ONOFF		0x5047	//wifi OnOff取得
#define QBIC_CMD_GET_AUTO_WIFIOFF	0x5048	//自動wifiOff
#define QBIC_CMD_GET_IMAGE_QUARITY	0x5049	//画質設定取得
#define QBIC_CMD_GET_BATTERY_ALARM	0x504a	//電池残量アラーム設定取得

#define QBIC_CMD_SET_WIFI_COUNTRY	0x5841	//wifi対応国設定
#define QBIC_CMD_SET_ADHOC_SECURE	0x5842	//アドホックセキュリティ種別設定
#define QBIC_CMD_SET_RTSP_PORT		0x5843	//RTSPポート番号設定
#define QBIC_CMD_SET_STUN_SERVER	0x5844	//STUNサーバーアドレス設定
#define QBIC_CMD_SET_PRIMARY_DNS	0x5845	//プライマリDNSサーバーアドレス設定
#define QBIC_CMD_SET_SECONDARY_DNS	0x5846	//セカンダリDNSサーバーアドレス設定
#define QBIC_CMD_SET_WIFI_ONOFF		0x5847	//wifi OnOff設定
#define QBIC_CMD_SET_AUTO_WIFIOFF	0x5848	//自動wifiOff
#define QBIC_CMD_SET_IMAGE_QUARITY	0x5849	//画質設定設定
#define QBIC_CMD_SET_BATTERY_ALARM	0x584a	//電池残量アラーム設定

//Status (QBiC-S~)
#define QBIC_CMD_GET_REC_BITRATE	0x504b	//録画のビットレート取得
#define QBIC_CMD_GET_LIVE_BITRATE	0x504c	//ストリームビットレート取得
#define QBIC_CMD_GET_LED_ON_SECURE	0x504d	//警戒モード中LED設定取得
#define QBIC_CMD_GET_SCHE_ONOFF		0x5050	//スケジュールON/OFF設定取得
#define QBIC_CMD_GET_SCHE_SETTING	0x5051	//スケジュール設定取得
#define QBIC_CMD_GET_SEQ_MODE_START	0x5052	//起動時警戒モード設定取得
#define QBIC_CMD_GET_SECURE_KEYLOCK	0x5053	//警戒時キーロック設定取得
#define QBIC_CMD_GET_DDNS_ADDRESS	0x5054	//DDNSサーバアドレス取得
#define QBIC_CMD_GET_ROTATE180		0x5055	//映像180度回転設定取得
#define QBIC_CMD_GET_SECURE_CURR	0x5056	//現在の警戒モード取得
#define QBIC_CMD_GET_SCHE_CURRENT	0x5057	//現在のスケジュール状態取得
#define QBIC_CMD_GET_HTTP_PORT		0x5058	//httpポート番号取得

#define QBIC_CMD_SET_REC_BITRATE	0x584b	//録画のビットレート設定
#define QBIC_CMD_SET_LIVE_BITRATE	0x584c	//ストリームビットレート設定
#define QBIC_CMD_SET_LED_ON_SECURE	0x584d	//警戒モード中LED設定
#define QBIC_CMD_SET_SCHE_ONOFF		0x5850	//スケジュールON/OFF設定
#define QBIC_CMD_SET_SCHE_SETTING	0x5851	//スケジュール設定
#define QBIC_CMD_SET_SEQ_MODE_START	0x5852	//起動時警戒モード設定
#define QBIC_CMD_SET_SECURE_KEYLOCK	0x5853	//警戒時キーロック設定
#define QBIC_CMD_SET_DDNS_ADDRESS	0x5854	//DDNSサーバアドレス設定
#define QBIC_CMD_SET_ROTATE180		0x5855	//映像180度回転設定
#define QBIC_CMD_SET_HTTP_PORT		0x5858	//httpポート番号設定

//Status(QBiC-D)
#define QBIC_CMD_GET_CTRL_SERVER	0x5059	//CTRLサーバURI設定取得
#define QBIC_CMD_GET_QUALITY_PRESET 0x5060	//品質プリセット設定取得
#define QBIC_CMD_GET_AUDIO_UPPORT	0x5061	//音声上りポート設定取得
#define QBIC_CMD_GET_SCHE_SELECT	0x5062	//スケジュール種類設定取得
#define QBIC_CMD_GET_START_LOCK		0x5063	//起動時操作ロック設定取得
#define QBIC_CMD_GET_REMOTE_RECTIME	0x5064	//遠隔最大録画時間設定取得
#define QBIC_CMD_GET_REC_TIME		0x5065	//本体最大録画時間設定取得
#define QBIC_CMD_GET_PRE_ALARM		0x5066	//プレアラーム設定取得
#define QBIC_CMD_GET_MOTION_AREA	0x5067	//動体検知エリア設定取得
#define QBIC_CMD_GET_MOTION_SENSOR	0x5068	//動体検知感度設定取得
#define QBIC_CMD_GET_UNMOTION_SENS	0x5069	//不動体検知感度設定取得
#define QBIC_CMD_GET_VOLUME_LEVEL	0x506A	//音量設定取得
#define QBIC_CMD_GET_DIGCAM_SHUTTER	0x506B	//デジカメモードシャッター音設定取得
#define QBIC_CMD_GET_SECURE_SHUTTER	0x506C	//警戒モードシャッター音設定取得
#define QBIC_CMD_GET_WATCH_SHUTTER	0x506D	//見守りモードシャッター音設定取得
#define QBIC_CMD_GET_DOOR_SHUTTER	0x506E	//ドアホンモードシャッター音設定取得
#define QBIC_CMD_GET_CONT_SHUTTER	0x506F	//連続録画モードシャッター音設定取得
#define QBIC_CMD_GET_QR_NO_OPE		0x5070	//QRモード無操作終了時間設定取得
#define QBIC_CMD_GET_WATCH_INTERVAL	0x5071	//見守りモード間隔設定取得
#define QBIC_CMD_GET_DOOR_INTERVAL	0x5072	//ドアホンモード動体検知間隔設定取得
#define QBIC_CMD_GET_UPLOAD_ONOFF	0x5073	//アップロード可否設定取得
#define QBIC_CMD_GET_UPLOAD_DELETE	0x5074	//アップロード後削除設定取得
#define QBIC_CMD_GET_SCHE_CYCLE		0x5075	//スケジュール周期設定取得
#define QBIC_CMD_GET_SCHE_DAY_TIME	0x5076	//スケジュール曜日時間設定取得
#define QBIC_CMD_GET_SCHE_DAY_MODE	0x5077	//スケジュールモード等設定取得
#define QBIC_CMD_GET_REC_PARAMETER	0x5078	//録画パラメータ設定取得
#define QBIC_CMD_GET_LIVE_PARAMETER	0x5079	//ライブパラメータ設定取得

#define QBIC_CMD_GET_CURRENT_MODE	0x507D	//動作モード設定取得
#define QBIC_CMD_GET_LOCK_STATE		0x507F	//操作ロック状態設定取得
#define QBIC_CMD_GET_UPDATE_SETTING	0x5080	//設定更新日時取得
#define QBIC_CMD_GET_UPDATE_SCHE	0x5081	//スケジュール更新日時取得
#define QBIC_CMD_GET_ACTIVATION_FLAG 0x5082	//アクティベーションフラグ
#define QBIC_CMD_GET_WAKE_ON_LAN	0x5083	//Wake On Lan ONOFF
#define QBIC_CMD_GET_PORTFORWARDING	0x5084	//PortFowarding ON/OFF
#define QBIC_CMD_GET_FORWARDING_PORT 0x5085	//ポートフォワディング音声上りポート取得
#define QBIC_CMD_GET_FACTORY_FPS_BPS 0x5086	//各解像度のFps,Bpsの工場出荷値取得
#define QBIC_CMD_GET_QUANTIZATION	0x5087	//量子化量上下限取得
#define QBIC_CMD_GET_DUMMY_FRAME	0x5088	//ダミーフレームONOFF取得

//S
#define QBIC_CMD_GET_DISTRI_ENCODE	0x5089	//配信エンコードタイプ設定取得
#define QBIC_CMD_GET_CAPTURE_SIZE	0x508A	//静止画解像度設定取得
#define QBIC_CMD_GET_CAPTU_QUARITY	0x508B	//静止画品質設定取得
#define QBIC_CMD_GET_ALERT_MODE		0x508C	//警戒モード種別設定取得
#define QBIC_CMD_GET_POST_ALARM_REC	0x508D	//ポストアラーム録画設定取得
#define QBIC_CMD_GET_GRAYSCALE		0x508E	//低照度時グレースケール設定取得
#define QBIC_CMD_GET_DET_ALGORITHM	0x508F	//検知アルゴリズム設定取得
#define QBIC_CMD_GET_MOTION_UN		0x5090	//動体不動体設定取得
#define QBIC_CMD_GET_UN_DETECT_TIME	0x5091	//不動体検出時間設定取得
#define QBIC_CMD_GET_DETECTION_TIME	0x5092	//動体検出時間設定取得
#define QBIC_CMD_GET_MOTION_SIZE	0x5093	//動体判定サイズ設定取得
#define QBIC_CMD_GET_MOTION_FRAME	0x5094	//動体検出枠表示設定取得
#define QBIC_CMD_GET_FACE_SENS		0x5095	//顔検出感度設定取得
#define QBIC_CMD_GET_FACE_FRAME		0x5096	//顔検出枠表示設定取得

#define QBIC_CMD_GET_USE_IPADDR		0x5097	//現在使用中のIP取得
#define QBIC_CMD_GET_PRI_MASK_ONOFF	0x5098	//プライベートマスクONOFF取得
#define QBIC_CMD_GET_PRI_MASK_POS	0x5099	//プライベートマスク位置取得

#define QBIC_CMD_SET_CTRL_SERVER	0x5859	//CTRLサーバURI設定
#define QBIC_CMD_SET_QUALITY_PRESET 0x5860	//品質プリセット設定
#define QBIC_CMD_SET_AUDIO_UPPORT	0x5861	//音声上りポート設定
#define QBIC_CMD_SET_SCHE_SELECT	0x5862	//スケジュール種類設定
#define QBIC_CMD_SET_START_LOCK		0x5863	//起動時操作ロック設定
#define QBIC_CMD_SET_REMOTE_RECTIME	0x5864	//遠隔最大録画時間設定
#define QBIC_CMD_SET_REC_TIME		0x5865	//本体最大録画時間設定
#define QBIC_CMD_SET_PRE_ALARM		0x5866	//プレアラーム設定
#define QBIC_CMD_SET_MOTION_AREA	0x5867	//動体検知エリア設定
#define QBIC_CMD_SET_MOTION_SENSOR	0x5868	//動体検知感度設定
#define QBIC_CMD_SET_UNMOTION_SENS	0x5869	//不動体検知感度設定
#define QBIC_CMD_SET_VOLUME_LEVEL	0x586A	//音量設定
#define QBIC_CMD_SET_DIGCAM_SHUTTER	0x586B	//デジカメモードシャッター音設定
#define QBIC_CMD_SET_SECURE_SHUTTER	0x586C	//警戒モードシャッター音設定
#define QBIC_CMD_SET_WATCH_SHUTTER	0x586D	//見守りモードシャッター音設定
#define QBIC_CMD_SET_DOOR_SHUTTER	0x586E	//ドアホンモードシャッター音設定
#define QBIC_CMD_SET_CONT_SHUTTER	0x586F	//連続録画モードシャッター音設定
#define QBIC_CMD_SET_QR_NO_OPE		0x5870	//QRモード無操作終了時間設定
#define QBIC_CMD_SET_WATCH_INTERVAL	0x5871	//見守りモード間隔設定
#define QBIC_CMD_SET_DOOR_INTERVAL	0x5872	//ドアホンモード動体検知間隔設定
#define QBIC_CMD_SET_UPLOAD_ONOFF	0x5873	//アップロード可否設定
#define QBIC_CMD_SET_UPLOAD_DELETE	0x5874	//アップロード後削除設定
#define QBIC_CMD_SET_SCHE_CYCLE		0x5875	//スケジュール周期設定
#define QBIC_CMD_SET_SCHE_DAY_TIME	0x5876	//スケジュール曜日時間設定
#define QBIC_CMD_SET_SCHE_DAY_MODE	0x5877	//スケジュールモード等設定
#define QBIC_CMD_SET_REC_PARAMETER	0x5878	//録画パラメータ設定
#define QBIC_CMD_SET_LIVE_PARAMETER	0x5879	//ライブパラメータ設定

#define QBIC_CMD_SET_ACTIVATION_FLAG 0x5882	//アクティベーションフラグ

#define QBIC_CMD_SET_WAKE_ON_LAN	0x5883	//Wake On Lan ONOFF
#define QBIC_CMD_SET_PORTFORWARDING	0x5884	//PortFowarding ON/OFF
#define QBIC_CMD_SET_FORWARDING_PORT 0x5885	//ポートフォワディング音声上りポート設定
#define QBIC_CMD_SET_QUANTIZATION	0x5887	//量子化量上下限設定
#define QBIC_CMD_SET_DUMMY_FRAME	0x5888	//ダミーフレームONOFF設定

//S
#define QBIC_CMD_GET_TIMESTAMP_ON	0x507A	//タイムスタンプオンオフ設定取得
#define QBIC_CMD_GET_TIMESTAMP_POS	0x507B	//タイムスタンプオーバーレイ使用設定取得
#define QBIC_CMD_GET_TIMESTAMP_FORM	0x507C	//タイムスタンプフォーマット

#define QBIC_CMD_SET_TIMESTAMP_ON	0x587A	//タイムスタンプオンオフ設定
#define QBIC_CMD_SET_TIMESTAMP_POS	0x587B	//タイムスタンプオーバーレイ使用設定
#define QBIC_CMD_SET_TIMESTAMP_FORM	0x587C	//タイムスタンプフォーマット設定


#define QBIC_CMD_SET_DISTRI_ENCODE	0x5889	//配信エンコードタイプ設定
#define QBIC_CMD_SET_CAPTURE_SIZE	0x588A	//静止画解像度設定
#define QBIC_CMD_SET_CAPTU_QUARITY	0x588B	//静止画品質設定
#define QBIC_CMD_SET_ALERT_MODE		0x588C	//警戒モード種別設定
#define QBIC_CMD_SET_POST_ALARM_REC	0x588D	//ポストアラーム録画設定
#define QBIC_CMD_SET_GRAYSCALE		0x588E	//低照度時グレースケール設定
#define QBIC_CMD_SET_DET_ALGORITHM	0x588F	//検知アルゴリズム設定
#define QBIC_CMD_SET_MOTION_UN		0x5890	//動体不動体設定
#define QBIC_CMD_SET_UN_DETECT_TIME	0x5891	//不動体検出時間設定
#define QBIC_CMD_SET_DETECTION_TIME	0x5892	//動体検出時間設定
#define QBIC_CMD_SET_MOTION_SIZE	0x5893	//動体判定サイズ設定
#define QBIC_CMD_SET_MOTION_FRAME	0x5894	//動体検出枠表示設定
#define QBIC_CMD_SET_FACE_SENS		0x5895	//顔検出感度設定
#define QBIC_CMD_SET_FACE_FRAME		0x5896	//顔検出枠表示設定

#define QBIC_CMD_SET_PRI_MASK_ONOFF	0x5898	//プライベートマスクONOFF設定
#define QBIC_CMD_SET_PRI_MASK_POS	0x5899	//プライベートマスク位置設定
#define QBIC_CMD_GET_JPEG_LIVE_PARAM 0x509A	//jpegライブパラメータ設定取得
#define QBIC_CMD_SET_JPEG_LIVE_PARAM 0x589A	//jpegライブパラメータ設定

#define	QBIC_CMD_GET_POLL_SERVER	0x509B	//!< ポーリング接続先URI取得
#define	QBIC_CMD_SET_POLL_SERVER	0x589B	//!< ポーリング接続先URI設定
#define	QBIC_CMD_GET_POLL_PORT		0x509C	//!< ポーリング接続先ポート番号取得
#define	QBIC_CMD_SET_POLL_PORT		0x589C	//!< ポーリング接続先ポート番号設定
#define QBIC_CMD_GET_USE_SSL		0x509D	//!< HTTP or HTTPS
#define QBIC_CMD_SET_USE_SSL		0x589D	//!< HTTP or HTTPS
#define	QBIC_CMD_GET_PORTFWD_IP		0x509E	//!< ポートフォワーディング用グローバルIPアドレス取得
#define	QBIC_CMD_SET_PORTFWD_IP		0x589E	//!< ポートフォワーディング用グローバルIPアドレス設定
#define	QBIC_CMD_GET_PORTFWD_PORT_PIC_DOWN	0x509F	//!< ポートフォワーディング用映像下りポート番号取得
#define	QBIC_CMD_SET_PORTFWD_PORT_PIC_DOWN	0x589F	//!< ポートフォワーディング用映像下りポート番号設定
#define	QBIC_CMD_GET_PORTFWD_PORT_PIC_DOWN_CTRL	0x50A0	//!< ポートフォワーディング用映像下りRTCPポート番号取得
#define	QBIC_CMD_SET_PORTFWD_PORT_PIC_DOWN_CTRL	0x58A0	//!< ポートフォワーディング用映像下りRTCPポート番号設定
#define	QBIC_CMD_GET_PORTFWD_PORT_AUD_DOWN	0x50A1	//!< ポートフォワーディング用音声下りポート番号取得
#define	QBIC_CMD_SET_PORTFWD_PORT_AUD_DOWN	0x58A1	//!< ポートフォワーディング用音声下りポート番号設定
#define	QBIC_CMD_GET_CAMERA_RELEASE_VERSION	0x50A2	//!< カメラリリースバージョン取得

#define	QBIC_CMD_GET_PORTFWD_PORT_FOR_GIP	0x50A3	//!< グローバルIPアドレス解決に用いるポート番号取得
#define	QBIC_CMD_SET_PORTFWD_PORT_FOR_GIP	0x58A3	//!< グローバルIPアドレス解決に用いるポート番号設定

#define QBIC_CMD_GET_LED_AUTOOFF	0x50A4	//!< 自動LEDオフ取得
#define QBIC_CMD_SET_LED_AUTOOFF	0x58A4	//!< 自動LEDオフ設定
#define QBIC_CMD_GET_AUTOPOWERON	0x50A5	//!< 自動復旧取得
#define QBIC_CMD_SET_AUTOPOWERON	0x58A5	//!< 自動復旧設定
#define QBIC_CMD_GET_GLOBALIP_GETMODE	0x50A6	//!< グローバルIP取得方法取得
#define QBIC_CMD_SET_GLOBALIP_GETMODE	0x58A6	//!< グローバルIP取得方法取得

//カメラ操作
#define QBIC_CMD_POWEROFF			0x6801	//電源OFF
#define QBIC_CMD_RESTART			0x6802	//再起動
#define QBIC_CMD_SD_FORMAT			0x6803	//SDカードフォーマット
#define QBIC_CMD_INIT_SETTING		0x6804	//工場出荷設定に戻す
#define QBIC_CMD_CTRL_REC			0x6805	//録画開始・停止
#define QBIC_CMD_CAPTURE			0x6806	//キャプチャ
#define QBIC_CMD_CONNECT_WIFI		0x6807	//wi-fi接続
#define QBIC_CMD_WIFI_CHANGE		0x6808	//動的wifi接続変更
#define QBIC_CMD_WIFI_SWITCH		0x6809	//動的wifiOnOff
#define QBIC_CMD_RF_TESTMODE		0x680A	//RFテストモード
#define QBIC_CMD_RF_START			0x680B	//RFテストモード開始
#define QBIC_CMD_MFG_TX				0x680C	//PE14テスト用パケット送信
#define QBIC_CMD_MFG_ACC			0x680D	//PE14テスト用電波送信

//カメラ操作(QBiC-S~)
#define QBIC_CMD_CHG_SECURE_CURR	0x6810	//現在の警戒モード設定
#define QBIC_CMD_CHG_SCHE_SECURE	0x6811	//警戒時スケジュールON/OFF設定

//カメラ操作(DQBiC)
#define QBIC_CMD_CHG_MOTION_MODE	0x6812	//動作モード切替
#define QBIC_CMD_CHG_LOCK_MODE		0x6813	//操作ロック状態切替
#define QBIC_CMD_CHG_SCHE_MODE		0x6814	//スケジュール切替

#define QBIC_CMD_STREAM_START		0x6815	//USBストリーム開始
#define QBIC_CMD_STREAM_STEAL		0x6816	//USBストリームスティール
#define QBIC_CMD_STREAM_END			0x6817	//USBストリーム停止
#define QBIC_CMD_CHK_SOUND_EFFECT	0x6818	//音確認

//Profile取得
#define QBIC_CMD_GET_SERIALNO		0x4001	//シリアル番号設定
#define QBIC_CMD_GET_MACADDRESS		0x4002	//MACアドレス取得
#define QBIC_CMD_GET_MACHINENAME	0x4003	//機種名設定
#define QBIC_CMD_GET_SSID			0x4004	//SSID設定
#define QBIC_CMD_GET_ADHOCKEY		0x4005	//アドホック暗号キー設定
#define QBIC_CMD_GET_CREATEDAY		0x4006	//製造年月日設定
#define QBIC_CMD_GET_COUNTRY		0x4007	//仕向け設定
#define QBIC_CMD_GET_LENSINFO		0x4008	//レンズ情報設定
#define QBIC_CMD_GET_UUID			0x4009	//UUID

//Profile設定
#define QBIC_CMD_SET_SERIALNO		0x4801	//シリアル番号設定
#define QBIC_CMD_SET_MACHINENAME	0x4803	//機種名設定
#define QBIC_CMD_SET_SSID			0x4804	//SSID設定
#define QBIC_CMD_SET_ADHOCKEY		0x4805	//アドホック暗号キー設定
#define QBIC_CMD_SET_CREATEDAY		0x4806	//製造年月日設定
#define QBIC_CMD_SET_COUNTRY		0x4807	//仕向け設定
#define QBIC_CMD_SET_LENSINFO		0x4808	//レンズ情報設定
#define QBIC_CMD_SET_UUID			0x4809	//UUID
#define QBIC_CMD_SET_DQBIC_SERIAL	0x480A	//D専用シリアル番号設定

//調整
#define QBIC_CMD_EXEC_ALIGNMENT		0x0021	//軸ずれ検査実行
#define QBIC_CMD_EXEC_RESOLUTION	0x0022	//解像度検査実行
#define QBIC_CMD_EXEC_VIGNETTING	0x0023	//ケラレ検査実行
#define QBIC_CMD_EXEC_AWB			0x0024	//AWB検査実行
#define QBIC_CMD_EXEC_DUST			0x0025	//ごみ検査実行
#define QBIC_CMD_EXEC_DOTERROR		0x0026	//画素欠陥検査実行
#define QBIC_CMD_EXEC_RAM			0x0027	//RAM検査実行
#define QBIC_CMD_SAVE_RAWDATA		0x0031	//RAWデータ保存
#define QBIC_CMD_SAVE_YCDATA		0x0032	//YCデータ保存
#define QBIC_CMD_SAVE_ADJUSTDATA	0x0033	//調整データ保存

#define QBIC_CMD_SET_ADJUSTFLAG		0x0041	//
#define QBIC_CMD_GET_ADJUSTFLAG		0x0042	//

#define QBIC_CMD_EXEC_AEMODE		0x0051	//AE Mode
#define QBIC_CMD_EXEC_AELOCK		0x0052	//AA Lock

#define QBIC_CMD_EXEC_AGING			0x0061	//Aging

//データ通信
#define QBIC_CMD_REQ_JPEG			0x3001	//JPEG要求
#define QBIC_CMD_REQ_AUDIO			0x3002	//音声要求
#define QBIC_CMD_REQ_LOG			0x3003	//ログ要求
#define QBIC_CMD_EXEC_FWUPDATEDATA	0x2001	//ファームウェアアップデートデータ
#define QBIC_CMD_EXEC_FWUPDATE		0x2002	//ファームウェアアップデート

//--------------------------------------------------------------------------------//
//コマンドパラメータ、返信値
//--------------------------------------------------------------------------------//
//共通
#define QBIC_VALUE_COMMON_OFF			1	//無効・OFF
#define QBIC_VALUE_COMMON_ON			2	//有効・ON
//wifi
#define QBIC_VALUE_WIFI_LINK_OFF		1	//切断
#define QBIC_VALUE_WIFI_LINK_ON			2	//リンク中
#define QBIC_VALUE_WIFI_LINK_ADHOC		3	//アドホックリンク中
#define QBIC_VALUE_WIFI_LINK_WIFIOFF	4	//Wifiオフ
#define QBIC_VALUE_WIFI_LINK_ERROR		5	//リンク失敗
#define QBIC_VALUE_WIFI_CODE_NONE		0	//暗号化なし
#define QBIC_VALUE_WIFI_CODE_WEP		1	//WEP
#define QBIC_VALUE_WIFI_CODE_TKIP		2	//TKIP
#define QBIC_VALUE_WIFI_CODE_AES		4	//AES
#define QBIC_VALUE_WIFI_CODE_MIXED		6	//Mixed
#define QBIC_VALUE_WIFI_CONNECT_INFRA	1	//インフラストラクチャ接続
#define QBIC_VALUE_WIFI_CONNECT_ADHOC	2	//アドホック接続
#define QBIC_VALUE_WIFI_AUTH_NONE		0	//WPA使用しない
#define QBIC_VALUE_WIFI_AUTH_WPA_PSK	1	//WPA-PSK
#define QBIC_VALUE_WIFI_AUTH_WPA2_PSK	3	//WPA2-PSK
//ストリーム
#define QBIC_VALUE_LIVE_IMAGESIZE1		1	//800x480	800x480
#define QBIC_VALUE_LIVE_IMAGESIZE2		2	//320x240	432x240
#define QBIC_VALUE_LIVE_IMAGESIZE3		3	//			640x360
#define QBIC_VALUE_LIVE_FRAMERATE1		1	//1fps
#define QBIC_VALUE_LIVE_FRAMERATE2		6	//6fps
#define QBIC_VALUE_LIVE_FRAMERATE3		10	//10fps
#define QBIC_VALUE_LIVE_FRAMERATE4		15	//15fps
#define QBIC_VALUE_LIVE_FRAMERATE5		30	//30fps
//録画
#define QBIC_VALUE_REC_STATE_STOP		1	//録画中ではない
#define QBIC_VALUE_REC_STATE_RUNNING	2	//録画中
#define QBIC_VALUE_REC_STATE_BATTERY	3	//電池残量が足りない
#define QBIC_VALUE_REC_IMAGESIZE1		1	//1920x1080
#define QBIC_VALUE_REC_IMAGESIZE2		2	//1280x720
#define QBIC_VALUE_REC_IMAGESIZE3		3	//640x480
#define QBIC_VALUE_REC_IMAGESIZE4		4	//864x480
#define QBIC_VALUE_REC_FRAMERATE1		1	//1fps
#define QBIC_VALUE_REC_FRAMERATE2		6	//6fps
#define QBIC_VALUE_REC_FRAMERATE3		10	//10fps
#define QBIC_VALUE_REC_FRAMERATE4		15	//15fps
#define QBIC_VALUE_REC_FRAMERATE5		30	//30fps
//GPS
#define QBIC_VALUE_GPS_INFO_RMC			1	//RMC
#define QBIC_VALUE_GPS_INFO_GSV			2	//GSV
#define QBIC_VALUE_GPS_LOG_TYPE_RMC		1	//GCA+RMC
#define QBIC_VALUE_GPS_LOG_TYPE_ALL		2	//GCA+GLL+GSA+GSV+RMC+VTG+ZDA
//タイムゾーン
#define QBIC_VALUE_TIMEZONE_GMT0		0	//GMT+0
#define QBIC_VALUE_TIMEZONE_GMT1		1	//GMT+1
#define QBIC_VALUE_TIMEZONE_GMT2		2	//GMT+2
#define QBIC_VALUE_TIMEZONE_GMT3		3	//GMT+3
#define QBIC_VALUE_TIMEZONE_GMT4		4	//GMT+4
#define QBIC_VALUE_TIMEZONE_GMT5		5	//GMT+5
#define QBIC_VALUE_TIMEZONE_GMT6		6	//GMT+6
#define QBIC_VALUE_TIMEZONE_GMT7		7	//GMT+7
#define QBIC_VALUE_TIMEZONE_GMT8		8	//GMT+8
#define QBIC_VALUE_TIMEZONE_GMT9		9	//GMT+9
#define QBIC_VALUE_TIMEZONE_GMT10		10	//GMT+10
#define QBIC_VALUE_TIMEZONE_GMT11		11	//GMT+11
#define QBIC_VALUE_TIMEZONE_GMT12		12	//GMT+12
#define QBIC_VALUE_TIMEZONE_GMT_1		13	//GMT-1
#define QBIC_VALUE_TIMEZONE_GMT_2		14	//GMT-2
#define QBIC_VALUE_TIMEZONE_GMT_3		15	//GMT-3
#define QBIC_VALUE_TIMEZONE_GMT_4		16	//GMT-4
#define QBIC_VALUE_TIMEZONE_GMT_5		17	//GMT-5
#define QBIC_VALUE_TIMEZONE_GMT_6		18	//GMT-6
#define QBIC_VALUE_TIMEZONE_GMT_7		19	//GMT-7
#define QBIC_VALUE_TIMEZONE_GMT_8		20	//GMT-8
#define QBIC_VALUE_TIMEZONE_GMT_9		21	//GMT-9
#define QBIC_VALUE_TIMEZONE_GMT_10		22	//GMT-10
#define QBIC_VALUE_TIMEZONE_GMT_11		23	//GMT-11
#define QBIC_VALUE_TIMEZONE_GMT_12		24	//GMT-12
//電池残量
#define QBIC_VALUE_BATTERY_STATE_FEW	0	//1%以下
#define QBIC_VALUE_BATTERY_STATE_25		1	//~25%
#define QBIC_VALUE_BATTERY_STATE_50		2	//~50%
#define QBIC_VALUE_BATTERY_STATE_75		3	//~75%
#define QBIC_VALUE_BATTERY_STATE_100	4	//~100%
//SDカード
#define QBIC_VALUE_SD_FORMAT_FAT12		1	//FAT12
#define QBIC_VALUE_SD_FORMAT_FAT16		2	//FAT16
#define QBIC_VALUE_SD_FORMAT_FAT32		3	//FAT32

#define QBIC_VALUE_WIFI_COUNTRY1		1	//JP
#define QBIC_VALUE_WIFI_COUNTRY2		2	//US
#define QBIC_VALUE_WIFI_COUNTRY3		3	//EU
#define QBIC_VALUE_WIFI_COUNTRY4		4	//WW

#define QBIC_VALUE_ADHOC_SECURE1		1
#define QBIC_VALUE_ADHOC_SECURE2		2

//スケジュール
#define QBIC_VALUE_WEEKDAY_SUN			1	//日曜日
#define QBIC_VALUE_WEEKDAY_MON			2	//月曜日
#define QBIC_VALUE_WEEKDAY_TUE			3	//火曜日
#define QBIC_VALUE_WEEKDAY_WED			4	//水曜日
#define QBIC_VALUE_WEEKDAY_THU			5	//木曜日
#define QBIC_VALUE_WEEKDAY_FRI			6	//金曜日
#define QBIC_VALUE_WEEKDAY_SAT			7	//土曜日
#define QBIC_VALUE_WEEKDAY_EVR			8	//毎日
//スケジュール状態
#define QBIC_VALUE_SCHE_ON				1	//スケジュール実行
#define QBIC_VALUE_SCHE_OFF				2	//スケジュール非実行
//回転状態
#define QBIC_VALUE_ROTATE_0				1	//正立
#define QBIC_VALUE_ROTATE_180			2	//倒立
//起動時モード
#define QBIC_VALUE_STARTMODE_SECURE		1	//警戒モード
#define QBIC_VALUE_STARTMODE_MAINTE		2	//メンテナンスモード
//モード
#define QBIC_VALUE_MODE_SECURE_UNLOCK	1	//警戒モード(非ロック)
#define QBIC_VALUE_MODE_MAINTE			2	//メンテナンスモード
#define QBIC_VALUE_MODE_SECURE_LOCK		3	//警戒モード(ロック)

//オーバーレイ位置
#define QBIC_VALUE_TIMESTAMP_POS_LU		1	//左上
#define QBIC_VALUE_TIMESTAMP_POS_LD		2	//左下
#define QBIC_VALUE_TIMESTAMP_POS_RU		3	//右上
#define QBIC_VALUE_TIMESTAMP_POS_RD		4	//右下

//フォーマット
#define QBIC_VALUE_TIMESTAMP_FORM1		1	//YYYY/MM/DD
#define QBIC_VALUE_TIMESTAMP_FORM2		2	//MM/DD/YYYY
#define QBIC_VALUE_TIMESTAMP_FORM3		3	//DD/MM/YYYY

//品質
#define QBIC_VALUE_QUALITY_PRESET_HIGHT	1	//品質プリセット高品質
#define QBIC_VALUE_QUALITY_PRESET_LOW	2	//品質プリセット低品質

//スケジュールモード
#define QBIC_VALUE_SCHE_MODE_OFF		1	//スケジュールオフ
#define QBIC_VALUE_SCHE_MODE_WEEKDAY	2	//スケジュール日時
#define QBIC_VALUE_SCHE_MODE_CYCLE		3	//スケジュール周期

//音
#define QBIC_VALUE_SHUTTER_OFF			1	//OFF
#define QBIC_VALUE_SHUTTER_KASYA1		2	//カシャ音1
#define QBIC_VALUE_SHUTTER_KASYA2		3	//カシャ音2
#define QBIC_VALUE_SHUTTER_SIREN1		4	//サイレン1
#define QBIC_VALUE_SHUTTER_SIREN2		5	//サイレン2
#define QBIC_VALUE_SHUTTER_MELODY1		6	//メロディ1
#define QBIC_VALUE_SHUTTER_MELODY2		7	//メロディ2
#define QBIC_VALUE_SHUTTER_WARNING1		8	//警告音1
#define QBIC_VALUE_SHUTTER_WARNING2		9	//警告音2

//MODE
#define QBIC_VALUE_CYCLE_CAPTURE		1	//デジカメ静止画モード
#define QBIC_VALUE_CYCLE_CAM_REC		2	//デジカメ動画モード

//スケジュールモード
#define QBIC_VALUE_SCHE_MODE_INVALID	1	//無効
#define QBIC_VALUE_SCHE_MODE_CAPTURE	2	//デジカメ静止画
#define QBIC_VALUE_SCHE_MODE_REC		3	//デジカメ動画
#define QBIC_VALUE_SCHE_MODE_WARNING	4	//警戒
#define QBIC_VALUE_SCHE_MODE_WATCH		5	//見守り
#define QBIC_VALUE_SCHE_MODE_DOOR		6	//ドアホン
#define QBIC_VALUE_SCHE_MODE_POWER_OFF	7	//電源オフ

#define QBIC_VALUE_SCHE_TIME_CLEARSETTINGS	255	//設定削除スケジュール番号

//動作モード
#define QBIC_VALUE_DO_MODE_DIGI_CAM		1	//デジカメ
#define QBIC_VALUE_DO_MODE_WARNING		2	//警戒
#define QBIC_VALUE_DO_MODE_WATCH		3	//見守り
#define QBIC_VALUE_DO_MODE_DOOR			4	//ドアホン
#define QBIC_VALUE_DO_MODE_INTERVAL		5	//連続録画

#define QBIC_VALUE_KEY_LOCK				1	//操作ロック中
#define QBIC_VALUE_KEY_UNLOCK			2	//操作非ロック中

//グローバルIP取得モード
#define QBIC_VALUE_GLOBALIP_FROM_STUN	1	//STUN
#define QBIC_VALUE_GLOBALIP_FROM_MANUAL	2	//手動

//PE14テスト転送レート
#define QBIC_VALUE_MFG_RATE_1		2
#define QBIC_VALUE_MFG_RATE_2		4
#define QBIC_VALUE_MFG_RATE_5_5		11
#define QBIC_VALUE_MFG_RATE_6		12
#define QBIC_VALUE_MFG_RATE_9		18
#define QBIC_VALUE_MFG_RATE_11		22
#define QBIC_VALUE_MFG_RATE_12		24
#define QBIC_VALUE_MFG_RATE_18		36
#define QBIC_VALUE_MFG_RATE_24		48
#define QBIC_VALUE_MFG_RATE_36		72
#define QBIC_VALUE_MFG_RATE_48		96
#define QBIC_VALUE_MFG_RATE_54		108
#define QBIC_VALUE_MFG_RATE_MSC0	0x80
#define QBIC_VALUE_MFG_RATE_MSC1	0x81
#define QBIC_VALUE_MFG_RATE_MSC2	0x82
#define QBIC_VALUE_MFG_RATE_MSC3	0x83
#define QBIC_VALUE_MFG_RATE_MSC4	0x84
#define QBIC_VALUE_MFG_RATE_MSC5	0x85
#define QBIC_VALUE_MFG_RATE_MSC6	0x86
#define QBIC_VALUE_MFG_RATE_MSC7	0x87


#define QBIC_VALUE_ALERT_CONTINU_REC	1	//24h連続録画
#define QBIC_VALUE_ALERT_PURE_ALARM		2	//プレアラーム

#define QBIC_VALUE_DET_ALGORITHM_MOTION	1	//動体検知
#define QBIC_VALUE_DET_ALGORITHM_FACE	2	//顔検知
#define QBIC_VALUE_DET_ALGORITHM_FULL	3	//動体＋顔


//--------------------------------------------------------------------------------//
//検査、調整デフォルト値
#define QBIC_PARAM_DOTERROR_DEF		4		//欠陥画素検出
#define QBIC_PARAM_VIGNETTING_DEF	80120	//ケラレ検査
#define QBIC_PARAM_RESOLUTION_DEF	0		//解像度検査
#define QBIC_PARAM_ALIGNMENT_DEF	8		//軸ずれ検査
#define QBIC_PARAM_AWB_DEF			50		//WB調整
#define QBIC_PARAM_DUST_DEF			0		//ごみ検査
//軸ずれ検査エラー
#define QBIC_VALUE_ALIGNMENT_SUCCESS	0
#define QBIC_VALUE_ALIGNMENT_ERROR8		208	//左中央エッジ未検出
#define QBIC_VALUE_ALIGNMENT_ERROR9		209	//右中央エッジ未検出
#define QBIC_VALUE_ALIGNMENT_ERROR12	212	//左上エッジ未検出
#define QBIC_VALUE_ALIGNMENT_ERROR13	213	//右上エッジ未検出
#define QBIC_VALUE_ALIGNMENT_ERROR14	214	//左下エッジ未検出
#define QBIC_VALUE_ALIGNMENT_ERROR15	215	//右下エッジ未検出
#define QBIC_VALUE_ALIGNMENT_ERROR16	216	//上左右差分
#define QBIC_VALUE_ALIGNMENT_ERROR17	217	//下左右差分
#define QBIC_VALUE_ALIGNMENT_ERROR18	218	//パラメータエラー
//ケラレ検査エラー
#define QBIC_VALUE_VIGNETTING_SUCCESS	0
#define QBIC_VALUE_VIGNETTING_ERROR0	112	//水平方向ケラレNG
#define QBIC_VALUE_VIGNETTING_ERROR1	113	//垂直方向ケラレNG
#define QBIC_VALUE_VIGNETTING_ERROR2	114	//パラメータエラー
//AWB検査エラー
#define QBIC_VALUE_AWB_SUCCESS			0
#define QBIC_VALUE_AWB_ERROR0			400	//AWB調整値(R)範囲外
#define QBIC_VALUE_AWB_ERROR1			401	//AWB調整値(B)範囲外
#define QBIC_VALUE_AWB_ERROR2			402	//パラメータエラー
//ごみ検査エラー
#define QBIC_VALUE_DUST_SUCCESS			0
#define QBIC_VALUE_DUST_ERROR0			500	//ブロック判定NG
#define QBIC_VALUE_DUST_ERROR1			501	//左上エリアNG
#define QBIC_VALUE_DUST_ERROR2			502	//右上エリアNG
#define QBIC_VALUE_DUST_ERROR3			503	//左下エリアNG
#define QBIC_VALUE_DUST_ERROR4			504	//右下エリアNG
//画素欠陥検査エラー
#define QBIC_VALUE_DOTERROR_SUCCESS		0
#define QBIC_VALUE_DOTERROR_ERROR0		600	//欠陥個数オーバー
#define QBIC_VALUE_DOTERROR_ERROR1		602	//パラメータエラー
//RAM検査エラー
#define QBIC_VALUE_RAM_SUCCESS			0

#endif //QBICCOMMANDDEFINE_H
