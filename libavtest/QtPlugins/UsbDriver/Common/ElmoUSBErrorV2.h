/*
 * ElmoUsbErrorV2.h
 *
 * by (S技1)
 *
 * Copyright (c) 2011 ELMO CO., LTD.
 */

/*!
@file	ElmoUSBErrorV2.h
@brief	L-12 USBコマンド　応答定数用のヘッダ
*/

#ifndef ELMOUSBERRORV2_H
#define ELMOUSBERRORV2_H



// =============================================================================
//	 ResponseCod : 応答分類 Success or Error
// =============================================================================

/*!
@brief ResponseCod
*/
typedef enum
{
	EL_RES_SUCCESSV2						= 0x00000000,		//!<	通信成功
}eEL_RES_RESPONSE;


/*!
@brief 共通エラー
*/
typedef enum
{
	EL_ERR_CMD_TIMEOUT					= 0xFFFFFFFF,		//!<	共通エラー　タイムアウト
	EL_ERR_CMD_BUSY						= 0xFFFFFFFE,		//!<	共通エラー　BUSY
	EL_ERR_CMD_LEN						= 0xFFFFFFFD,		//!<	共通エラー　データ長不良
	EL_ERR_CMD_CODE						= 0xFFFFFFFC,		//!<	共通エラー　コマンド不良
	EL_ERR_CMD_SESSION					= 0xFFFFFFFB,		//!<	共通エラー　セッション不良
	EL_ERR_CMD_FAILURE					= 0xFFFFFFFA,		//!<	共通エラー　コマンド実行失敗
}eEL_ERR_CMD;

/*!
@brief ファイルアップロード　エラー
*/
typedef enum
{

	EL_ERR_WRITEIMAGE_TIMEOUT			= 0xFFFFFFF9,		//!<	画像アップロードエラー　タイムアウト
	EL_ERR_WRITEIMAGE_BUSY				= 0xFFFFFFF8,		//!<	画像アップロードエラー　BUSY
	EL_ERR_WRITEIMAGE_SIZEOVER			= 0xFFFFFFF7,		//!<	画像アップロードエラー　データ長不良
	EL_ERR_WRITEIMAGE_SIZEUNDER			= 0xFFFFFFF6,		//!<	画像アップロードエラー　全サイズに満たない
	EL_ERR_WRITEIMAGE_MEMORY_WRITE		= 0xFFFFFFF5,		//!<	画像アップロードエラー　キャッシュメモリーへの書き込みエラー（書き込み失敗）
	EL_ERR_WRITEIMAGE_MEMORY_OVER		= 0xFFFFFFF4,		//!<	画像アップロードエラー　キャッシュメモリーへの書き込みエラー（容量オーバー）
	EL_ERR_WRITEIMAGE_STORAGE_WRITE		= 0xFFFFFFF3,		//!<	画像アップロードエラー　記憶媒体への書き込みエラー（書き込み失敗）
	EL_ERR_WRITEIMAGE_STORAGE_OVER		= 0xFFFFFFF2,		//!<	画像アップロードエラー　記憶媒体への書き込みエラー（容量オーバー）
	EL_ERR_WRITEIMAGE_STORAGE_EXIST		= 0xFFFFFFF1,		//!<	画像アップロードエラー　記憶媒体への書き込みエラー（媒体な）
	EL_ERR_WRITEIMAGE_FILENAMEMAX		= 0xFFFFFFF0,		//!<	画像アップロードエラー　ファイル名の最大サイズ超過
	EL_ERR_WRITEIMAGE_SEQUENCE			= 0xFFFFFFEF,		//!<	画像アップロードエラー　シーケンスエラー
	EL_ERR_WRITEIMAGE_FAILURE			= 0xFFFFFFEE,		//!<	画像アップロードエラー　アップロード失敗
}eEL_ERR_WRITEIMAGE;

/*!
@brief ファイルダウンロード　エラー 
*/
typedef enum
{
	EL_ERR_READIMAGE_NOFILE				= 0xFFFFFFED,		//!<	画像ダウンロードエラー　指定データが存在しない
	EL_ERR_READIMAGE_NODIR				= 0xFFFFFFEC,		//!<	画像ダウンロードエラー　指定ディレクトリが存在しない
	EL_ERR_READIMAGE_FAILURE			= 0xFFFFFFEB,		//!<	画像ダウンロードエラー　読み出し失敗
}eEL_ERR_READIMAGE;

/*!
@brief 画像操作　エラー
*/
typedef enum
{
	EL_ERR_OPEIMAGE_DELETEINHIBIT		= 0xFFFFFFEA,		//!<	画像操作エラー　削除禁止ファイル
	EL_ERR_OPEIMAGE_FAILURE				= 0xFFFFFFE9,		//!<	画像操作エラー　コマンド実行失敗
}eEL_ERR_OPEIMAGE;

/*!
@brief ファイル操作エラー
*/
typedef enum
{
    EL_ERR_FILE_EXIST_DEVICE			= 0xFFFFFFE8,		//!<	ファイル操作エラー　媒体なし
    EL_ERR_FILE_EXIST_DIR				= 0xFFFFFFE7,		//!<	ファイル操作エラー　ディレクトリが存在しない
    EL_ERR_FILE_EXIST_FILE				= 0xFFFFFFE6,		//!<	ファイル操作エラー　データが存在しない
    EL_ERR_FILE_STATUS					= 0xFFFFFFE5,		//!<	ファイル操作エラー　ファイル状態不正
    EL_ERR_FILE_FORMAT					= 0xFFFFFFE4,		//!<	ファイル操作エラー　媒体フォーマット不良
    EL_ERR_FILE_READONLY				= 0xFFFFFFE3,		//!<	ファイル操作エラー　読込み専用（削除禁止）
}eEL_ERR_FILE;



#endif	//ELMOUSBERRORV2_H
