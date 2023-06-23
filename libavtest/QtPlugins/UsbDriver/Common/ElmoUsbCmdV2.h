/*
 * ElmoCommandV2.h
 *
 * by (S技1)
 *
 * Copyright (c) 2011 ELMO CO., LTD.
 */

/*!
@file	ElmoCommandV2.h
@brief	L-12 USBコマンド　送信定数用のヘッダ
*/


#ifndef ELMOCOMMANDV2_H
#define ELMOCOMMANDV2_H


// =============================================================================
//	 OpeCod : コマンド分類
// =============================================================================

/*!
@brief バージョン取得
*/
typedef enum
{
	EL_CMD_GETVER_SH					= 0x4310,		//!<	バージョン取得　Shマイコン
	EL_CMD_GETVER_LENS					= 0x4311,		//!<	バージョン取得　レンズマイコン
	EL_CMD_GETVER_FPGA					= 0x4312,		//!<	バージョン取得　FPGA
}eEL_CMD_GETVER;

/*!
@brief ステータス取得
*/
typedef enum
{
	EL_CMD_STATUS_CAMERANAME			= 0x4313,		//!<	ステータス取得　機種名
	EL_CMD_STATUS_PRODUCTCODE			= 0x4314,		//!<	ステータス取得　機種コード
	EL_CMD_STATUS_GET_STATUS			= 0x4315,		//!<	ステータス取得　状態取得
	EL_CMD_STATUS_RESOLUTION			= 0x4316,		//!<	ステータス取得　解像度切換スイッチ
	EL_CMD_STATUS_OUTPUT_KIND			= 0x4317,		//!<	ステータス取得　映像出力切換スイッチ
	EL_CMD_STATUS_PUSHCAPTURE			= 0x4318,		//!<	ステータス取得　キャプチャ押下flag
}eEL_CMD_STATUS;

/*!
@brief 操作停止
*/
typedef enum
{
    EL_CMD_STOP_OPESTOP					= 0x435D,		//!<	操作停止
}eEL_CMD_STOP;

/*!
@brief ズーム
*/
typedef enum
{
    EL_CMD_ZOOM_TELE					= 0x4319,		//!<	ズーム　Tele
    EL_CMD_ZOOM_WIDE					= 0x431A,		//!<	ズーム　Wide
    EL_CMD_ZOOM_STOP					= 0x431B,		//!<	ズーム　Stop
    EL_CMD_ZOOM_INIT					= 0x431C,		//!<	ズーム　初期値
    EL_CMD_ZOOM_SET_VARIANT				= 0x431D,		//!<	ズーム　変化量設定
    EL_CMD_ZOOM_GET_STATUS				= 0x431E,		//!<	ズーム　状態取得
}eEL_CMD_ZOOM;

/*!
@brief フォーカス
*/
typedef enum
{
    EL_CMD_FOCUS_NEAR					= 0x431F,		//!<	フォーカス　Near
    EL_CMD_FOCUS_FAR					= 0x4320,		//!<	フォーカス　Far
    EL_CMD_FOCUS_AF						= 0x4321,		//!<	フォーカス　AF
    EL_CMD_FOCUS_STOP					= 0x4322,		//!<	フォーカス　Stop
    EL_CMD_FOCUS_INIT					= 0x4323,		//!<	フォーカス　初期値
    EL_CMD_FOCUS_SET_VARIANT			= 0x4324,		//!<	フォーカス　変化量設定
    EL_CMD_FOCUS_GET_STATUS				= 0x4325,		//!<	フォーカス　状態取得
    EL_CMD_FOCUS_SET_AFMODE				= 0x4326,		//!<	フォーカス　AFモードON/OFF設定
    EL_CMD_FOCUS_GET_AFMODE				= 0x4327,		//!<	フォーカス　AFモード状態取得
}eEL_CMD_FOCUS;

/*!
@brief あかるさ
*/
typedef enum
{
    EL_CMD_BRIGHT_BRIGHT				= 0x4328,		//!<	あかるさ　明るく
    EL_CMD_BRIGHT_DARK					= 0x4329,		//!<	あかるさ　暗く
    EL_CMD_BRIGHT_STOP					= 0x432A,		//!<	あかるさ　Stop
    EL_CMD_BRIGHT_INIT					= 0x432B,		//!<	あかるさ　初期値
    EL_CMD_BRIGHT_SET_VARIANT			= 0x432C,		//!<	あかるさ　変化量設定
    EL_CMD_BRIGHT_GET_STATUS			= 0x432D,		//!<	あかるさ　状態取得
    EL_CMD_BRIGHT_SET_BRIGHTMODE		= 0x432E,		//!<	あかるさ　明るさモードON/OFF設定
    EL_CMD_BRIGHT_GET_BRIGHTMODE		= 0x432F,		//!<	あかるさ　明るさモード状態取得
}eEL_CMD_BRIGHT;

/*!
@brief 静止（フリーズ）
*/
typedef enum
{
	EL_CMD_FREEZE_SWITCH				= 0x4330,		//!<	静止（フリーズ）　切り替え
	EL_CMD_FREEZE_GET_STATUS			= 0x4331,		//!<	静止（フリーズ）　状態取得
}eEL_CMD_FREEZE;

/*!
@brief 画像保存（キャプチャ）
*/
typedef enum
{
	EL_CMD_CAPTURE_EXEC					= 0x4332,		//!<	画像保存（キャプチャ）　実行
	EL_CMD_CAPTURE_FLAGCLEAR			= 0x4333,		//!<	画像保存（キャプチャ）　キャプチャ押下flag Clear
}eEL_CMD_CAPTURE;

/*!
@brief 録画
*/
typedef enum
{
	EL_CMD_REC_START					= 0x4334,		//!<	録画　録画開始
	EL_CMD_REC_STOP						= 0x4335,		//!<	録画　録画終了
	EL_CMD_REC_GET_STATUS				= 0x4336,		//!<	録画　状態取得
}eEL_CMD_REC;

/*!
@brief 入力映像（モード）
*/
typedef enum
{
	EL_CMD_MODE_SWITCH					= 0x4337,		//!<	入力映像（モード）　切り替え
	EL_CMD_MODE_GET_STATUS				= 0x4338,		//!<	入力映像（モード）　状態取得
}eEL_CMD_MODE;

/*!
@brief 画像回転
*/
typedef enum
{
	EL_CMD_ROTATION_SET					= 0x4339,		//!<	画像回転　設定
	EL_CMD_ROTATION_GET_STATUS			= 0x433A,		//!<	画像回転　状態取得
}eEL_CMD_ROTATION;

/*!
@brief ポジ/ネガ
*/
typedef enum
{
	EL_CMD_NEGAPOSI_SET					= 0x433B,		//!<	ポジ/ネガ　設定
	EL_CMD_NEGAPOSI_GET_STATUS			= 0x433C,		//!<	ポジ/ネガ　状態取得
}eEL_CMD_NEGAPOSI;

/*!
@brief カラー/白黒
*/
typedef enum
{
	EL_CMD_COLORMONO_SET				= 0x433D,		//!<	カラー/白黒　設定
	EL_CMD_COLORMONO_GET_STATUS			= 0x433E,		//!<	カラー/白黒　状態取得
}eEL_CMD_COLORMONO;

/*!
@brief 白補正（ホワイトバランス）
*/
typedef enum
{
	EL_CMD_WHITE_SET					= 0x433F,		//!<	白補正（ホワイトバランス）　設定
	EL_CMD_WHITE_GET_STATUS				= 0x4340,		//!<	白補正（ホワイトバランス）　状態取得
}eEL_CMD_WHITE;

/*!
@brief フリッカー補正
*/
typedef enum
{
	EL_CMD_FLICKER_SET					= 0x4341,		//!<	フリッカー補正　設定
	EL_CMD_FLICKER_GET_STATUS			= 0x4342,		//!<	フリッカー補正　状態取得
}eEL_CMD_FLICKER;

/*!
@brief 顕微鏡
*/
typedef enum
{
	EL_CMD_SCOPE_SET					= 0x4343,		//!<	顕微鏡　設定
	EL_CMD_SCOPE_GET_STATUS				= 0x4344,		//!<	顕微鏡　状態取得
}eEL_CMD_SCOPE;

/*!
@brief スピーカ
*/
typedef enum
{
	EL_CMD_SPEAKER_SET_VOLUME			= 0x4345,		//!<	スピーカ　音量設定
	EL_CMD_SPEAKER_MUTE					= 0x4346,		//!<	スピーカ　MUTE
	EL_CMD_SPEAKER_GET_STATUS			= 0x4347,		//!<	スピーカ　状態取得
}eEL_CMD_SPEAKER;

/*!
@brief マイク
*/
typedef enum
{
	EL_CMD_MIKE_SET_VOLUME				= 0x4348,		//!<	マイク　音量設定
	EL_CMD_MIKE_MUTE					= 0x4349,		//!<	マイク　MUTE
	EL_CMD_MIKE_GET_STATUS				= 0x434A,		//!<	マイク　状態取得
}eEL_CMD_MIKE;



/*!
@brief ビデオ方式
*/
typedef enum
{
    EL_CMD_VIDEO_SET					= 0x434B,		//!<	ビデオ方式　設定
    EL_CMD_VIDEO_GET_STATUS				= 0x434C,		//!<	ビデオ方式　状態取得
}eEL_CMD_VIDEO;

/*!
@brief 言語
*/
typedef enum
{
    EL_CMD_LANG_SET						= 0x434D,		//!<	言語　設定
    EL_CMD_LANG_GET_STATUS				= 0x434E,		//!<	言語　状態取得
}eEL_CMD_LANG;

/*!
@brief 映像モード
*/
typedef enum
{
    EL_CMD_GRAPHIC_SET					= 0x434F,		//!<	映像モード　設定
    EL_CMD_GRAPHIC_GET_STATUS			= 0x4350,		//!<	映像モード　状態取得
}eEL_CMD_GRAPHIC;

/*!
@brief ガイド
*/
typedef enum
{
    EL_CMD_GUID_SET						= 0x4351,		//!<	ガイド　設定
    EL_CMD_GUID_GET_STATUS				= 0x4352,		//!<	ガイド　状態取得
}eEL_CMD_GUID;

/*!
@brief 設定
*/
typedef enum
{
    EL_CMD_SETTING_LOAD					= 0x4353,		//!<	設定　設定読み出し
    EL_CMD_SETTING_SAVE					= 0x4354,		//!<	設定　設定保存
}eEL_CMD_SETTING;

/*!
@brief エッジ
*/
typedef enum
{
	EL_CMD_EDGE_EFFECT					= 0x435E,		//!<	エッジ　輪郭強調
	EL_CMD_EDGE_EFFECT_GET_STATUS		= 0x435F,		//!<	エッジ　状態取得
}eEL_CMD_EDGE_EFFECT;

/*!
@brief ガンマ
*/
typedef enum
{
	EL_CMD_GAMMA						= 0x4360,		//!<	ガンマ　ガンマ補正
	EL_CMD_GAMMA_GET_STATUS				= 0x4361,		//!<	ガンマ　状態取得
}eEL_CMD_GAMMA;



// -----------------------------------------------------------------------------
// 画像送受信
// -----------------------------------------------------------------------------

/*!
@brief リスト取得
*/
typedef enum
{
	EL_CMD_GETIMAGEINFO_DEVICELIST		= 0x4A00,		//!<	リスト取得　デバイスリスト取得
	EL_CMD_GETIMAGEINFO_FILELIST		= 0x4A01,		//!<	リスト取得　ファイルリスト取得
}eEL_CMD_GETIMAGEINFO;

/*!
@brief 画像操作
*/
typedef enum
{
	EL_CMD_OPEIMAGE_DELETE_FILE			= 0x4A02,		//!<	画像操作　削除
	EL_CMD_OPEIMAGE_SET_FAVORITEON		= 0x4A03,		//!<	画像操作　お気に入りON
	EL_CMD_OPEIMAGE_SET_FAVORITEOFF		= 0x4A04,		//!<	画像操作　お気に入りOFF
}eEL_CMD_OPEIMAGE;

/*!
@brief 画像ダウンロード
*/
typedef enum
{
	EL_CMD_READIMAGE_FILEINFO			= 0x4A05,		//!<	画像ダウンロード　インフォメーション取得
	EL_CMD_READIMAGE_THUMBNAIL			= 0x4A06,		//!<	画像ダウンロード　サムネイル取得
	EL_CMD_READIMAGE_FILE				= 0x4A07,		//!<	画像ダウンロード　画像ファイル取得
	EL_CMD_READIMAGE_CAMERA				= 0x4A08,		//!<	画像ダウンロード　書画カメラ
	EL_CMD_READIMAGE_STREAM_START		= 0x4A0A,		//!<	画像ダウンロード　ストリーミング開始
	EL_CMD_READIMAGE_STREAM_END			= 0x4A0B,		//!<	画像ダウンロード　ストリーミング終了
}eEL_CMD_READIMAGE;

/*!
@brief 画像アップロード
*/
typedef enum
{
	EL_CMD_WRITEIMAGE_FILE				= 0x4A09,		//!<	画像アップロード　ファイル
}eEL_CMD_WRITEIMAGE;


/*--------------------------------------------------------------------------------*/
/*!
	@ingroup	USBF_Work
	@brief		テストコマンド
*/
/*--------------------------------------------------------------------------------*/
typedef enum
{
	EL_CMD_TEST_1	= 0xfa00,	//!< テストコマンドその1
	EL_CMD_TEST_2	= 0xfa01,	//!< テストコマンドその2
	EL_CMD_TEST_3	= 0xfa02,	//!< テストコマンドその3
	EL_CMD_TEST_4	= 0xfa03,	//!< テストコマンドその4
	EL_CMD_TEST_5	= 0xfa04,	//!< テストコマンドその5
	EL_CMD_TEST_6	= 0xfa05,	//!< テストコマンドその6
	EL_CMD_TEST_7	= 0xfa06,	//!< テストコマンドその7
	EL_CMD_TEST_8	= 0xfa07,	//!< テストコマンドその8
	EL_CMD_TEST_9	= 0xfa08,	//!< テストコマンドその9
	EL_CMD_TEST_A	= 0xfa09,	//!< テストコマンドその10
} eEL_CMD_TESTCMD;


// -----------------------------------------------------------------------------
// ディレクトリ操作
// -----------------------------------------------------------------------------

/*!
@brief デバイス操作
*/
typedef enum
{
    EL_CMD_DRV_UNMOUNT					= 0x4B01,		//!<	デバイス操作　アンマウント（取り出し)
    EL_CMD_DRV_FORMAT					= 0x4B02,		//!<	デバイス操作　フォーマット
    EL_CMD_DRV_GET_STATUS				= 0x4B03,		//!<	デバイス操作　ドライブ情報取得
}eEL_CMD_DRV;


// -----------------------------------------------------------------------------
// システム操作
// -----------------------------------------------------------------------------
/*!
@brief システム
*/
typedef enum
{
    EL_CMD_SYSTEM_DATETIME				= 0x4C00,		//!<	システム　時刻設定
    EL_CMD_SYSTEM_GET_LASTERROR			= 0x4C01,		//!<	システム　最後に起こったエラーを取得
    EL_CMD_SYSTEM_CLEAR_LASTERROR		= 0x4C02,		//!<	システム　最後に起こったエラーをクリア
	EL_CMD_SYSTEM_WRITECAMERANAME		= 0x4C03,		//!<	システム　機種名設定
	EL_CMD_SYSTEM_WRITEPRODUCTCODE		= 0x4C04,		//!<	システム　機種コード設定
	EL_CMD_SYSTEM_WRITEFW				= 0x4C05,		//!<	システム　ファームウェアアップロード
}eEL_CMD_SYSTEM;


// =============================================================================
//	 パラメータ
// =============================================================================

/*!
@brief ＯＦＦ／ＯＮ
*/
typedef enum
{
	EL_PRM_OFF							= 0x00,		//!<	OFF
	EL_PRM_ON							= 0x01,		//!<	ON
}eEL_PRM_ONOFF;

/*!
@brief LOW/MIDDLE/HIGH
*/
typedef enum
{
	EL_PRM_LOW							= 0x01,		//!<	LOW
	EL_PRM_MIDDLE						= 0x02,		//!<	MIDDLE
	EL_PRM_HIGH							= 0x03,		//!<	HIGH
}eEL_PRM_LMH;

/*!
@brief 記憶媒体
*/
typedef enum
{
    EL_PRM_SD							= 0x00,		//!<	SD
    EL_PRM_RAM							= 0x0F00,	//!<	RAM
	EL_PRM_USB01						= 0x0100,	//!<	USB01
}eEL_PRM_DEVICE;

/*!
@brief 入力映像（モード）
*/
typedef enum
{
	EL_PRM_CAMERA						= 0x00,		//!<	書画カメラ
	EL_PRM_MEMORY						= 0x01,		//!<	メモリ
	EL_PRM_PC							= 0x02,		//!<	パソコン
	EL_PRM_TABLET						= 0x03,		//!<	タブレット
}eEL_PRM_INPUT;

/*!
@brief ポジ/ネガ
*/
typedef enum
{
	EL_PRM_POSI							= 0x00,		//!<	ポジ
	EL_PRM_NEGA							= 0x01,		//!<	ネガ
}eEL_PRM_NEGAPOSI;

/*!
@brief ホワイトバランス
*/
typedef enum
{
	EL_PRM_AUTO							= 0x01,		//!<	オート
	EL_PRM_ONEPUSH						= 0x02,		//!<	ワンプッシュ
	EL_PRM_MANUAL						= 0x03		//!<	マニュアル
}eEL_PRM_WHITEBALANCE;

/*!
@brief カラー/白黒
*/
typedef enum
{
	EL_PRM_COLOR						= 0x00,		//!<	カラー
	EL_PRM_BLACKANDWHITE				= 0x01,		//!<	白黒
}eEL_PRM_BW;

/*!
@brief フリッカー補正
*/
typedef enum
{
	EL_PRM_FLICKER60					= 0x00,		//!<	60Hz
	EL_PRM_FLICKER50					= 0x01,		//!<	50Hz
}eEL_PRM_FLICKER;

/*!
@brief USBモード
*/
typedef enum
{
	EL_PRM_MASSSTORAGE					= 0x00,		//!<	マスストレージ
	EL_PRM_APPLICATION					= 0x01,		//!<	アプリケーション
}eEL_PRM_USB;

/*!
@brief ビデオ方式
*/
typedef enum
{
	EL_PRM_NTSC							= 0x00,		//!<	NTSC
	EL_PRM_PAL							= 0x01,		//!<	PAL
}eEL_PRM_VIDEO;

/*!
@brief 言語
*/
typedef enum
{
	EL_PRM_JAPANESE						= 0x00,		//!<	日本語
	EL_PRM_ENGLISH						= 0x01,		//!<	英語
}eEL_PRM_LANGAGE;

/*!
@brief 映像モード
*/
typedef enum
{
	EL_PRM_TEXT01						= 0x00,		//!<	テキスト01
	EL_PRM_TEXT02						= 0x01,		//!<	テキスト02
	EL_PRM_TEXT03						= 0x02,		//!<	テキスト03
	EL_PRM_COLOR01						= 0x0100,	//!<	カラー書類01
	EL_PRM_COLOR02						= 0x0101,	//!<	カラー書類02
}eEL_PRM_IMAGEMODE;

/*!
@brief ファイルリスト取得
*/
typedef enum
{
	EL_PRM_NOINFO						= 0x00,		//!<	インフォメーションなし
	EL_PRM_GETINFO						= 0x01,		//!<	インフォメーションあり
}eEL_PRM_FILELIST;

/*!
@brief 画像操作
*/
typedef enum
{
	EL_PRM_ALL							= 0x00,		//!<	全て
	EL_PRM_INDIVISUAL					= 0x01,		//!<	個別
}eEL_PRM_OPEIMAGE;

/*!
@brief L-12映像取得
*/
typedef enum
{
    EL_PRM_NOTHUMBNAIL					= 0x00,		//!<	サムネイルなし
    EL_PRM_GETTHUMBNAIL					= 0x01,		//!<	サムネイルあり
}eEL_PRM_READIMAGE;


#endif	//ELMOCOMMANDV2_H
