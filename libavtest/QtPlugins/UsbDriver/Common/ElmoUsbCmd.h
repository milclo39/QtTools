/**
 * @file ElmoUsbCmd.h
 * @brief command enum define for Image Mate and turning soft.
 * @author Yutaka Hibino
 *
 * Copyright (c) 2004-2010 ELMO CO., LTD.
 */

#ifndef __ELMOUSBCMD_H__
#define __ELMOUSBCMD_H__

/*----- enum define -----*/

/* tuning command code */
enum
{
	EL_GET_JPEG_DATA		= 0x808e, // get JPEG data
	EL_GET_JPEG_SXGA_DATA	= 0x8083, // get JPEG data
	EL_GET_JPEG_VGA_DATA    = 0x8084,

	// FPGA command
	EL_SET_FPGA_ADD_DATA 	= 0x0a00,
	EL_GET_FPGA_ADD_DATA 	= 0x8a00,
	// EEPROM command
	EL_SET_CACHE_ADD_DATA 	= 0x0a01,
	EL_GET_CACHE_ADD_DATA 	= 0x8a01,
	EL_SET_EEPROM_ADD_DATA	= 0x0a02,
	EL_GET_EEPROM_ADD_DATA 	= 0x8a02,
	EL_SET_EEPROM_DIRECT_ADD_DATA	= 0x0a06,
	EL_GET_EEPROM_DIRECT_ADD_DATA	= 0x8a06,
	
	// CMOS_TUN command
	EL_SET_CMOS_ADD_DATA	= 0x0a03,
	EL_GET_CMOS_ADD_DATA	= 0x8a03,
	
	//FOCUS IC cmmand
	EL_SET_VIDEO_IC_ADD_DATA = 0x0a04,
	
	//IChips cmmand
	EL_SET_ICHIPS_ADD_DATA	= 0x0a05,

	// HEAD FPGA command (since L-1z)
	EL_SET_HEAD_FPGA_ADD_DATA	= 0x0a07,
	EL_GET_HEAD_FPGA_ADD_DATA	= 0x8a07,

	//Temp Test Command
	EL_SET_USB_HOST_TEST	= 0x0fff,
	EL_SET_AUDIO_GAIN		= 0x0ffe,
	
	// LENS_CENTER_TUN command
//	EL_LENS_CENTER_TUN	= 0x0a13,
	
	// CMR_TUN command
//	EL_CMR_TUN			= 0x0a21,
	
	// IRIS_TUN command
	EL_IRIS_TUN	= 0x0a31,
	
	// FOCUS_TUN command
	EL_FOCUS_TUN	= 0x0a32,
	
	// ZOOM_TUN command
	EL_ZOOM_TUN		= 0x0a33,
	
	// WB_TUN command
	EL_WB_TUN		= 0x0a34,
	
	// JPEG_RATE_TUN command
	EL_JPEG_RATE_TUN	= 0x0a35,
	
	// GRAPHICS_TUN command
	EL_GRAPHICS_TUN		= 0x0a36,
	
	// VERSION_OSD_TUN command
	EL_VERSION_OSD_TUN	= 0x0a37,
	
	// REMOTE_CONTROL_TUN command
	EL_REMOTE_CONTROL_TUN	= 0x0a38,
	
	// SHUTTER_SPEED_TUN command
	EL_SHUTTER_SPEED_TUN	= 0x0a39,
	
	// POWER OFF TUN command
	EL_POWER_SUPPLY_TUN		= 0x0a40,
	
	// VERSION_DATA command
	EL_GET_VERSION_DATA		= 0x8a10,
	
	// STATE_DATA command
	EL_GET_STATE_DATA		= 0x8a11,
	
	EL_GET_UNIT_NAME	= 0x8b10,
	EL_GET_VERSION_INFO = 0x8b11,
	
	// SMART JPEG取得確認コマンド
	EL_GET_CAPTURE_STATUS	= 0x80f1,
	//Smart Image Rotation コマンド
	EL_SET_IMAGE_ROTATION	= 0x00eb,
	//Smart Posi Nega コマンド
	EL_SET_POSI_NEGA	= 0x00ec,
	//Smart Color B&W コマンド
	EL_SET_COLOR_BW		= 0x00ed,
	//Smart Slow Shutter コマンド
	EL_SET_SLOW_SHUTTER	= 0x00ee,
	//Smart Image Mode コマンド
	EL_SET_IMAGE_MODE	= 0x00ef,
	//Smart Microscope コマンド
	EL_SET_MICROSCOPE_MODE	= 0x00f0,

};

/* main command code */
enum
{
	EL_GET				= 0x8000,
	EL_SET				= 0x0000,

	// key command
	EL_SET_KEY			= 0x00fa,
	
	// lens command
	EL_SET_ZOOM			= 0x00e0,
	EL_SET_AF			= 0x00e1,
	EL_SET_IRIS			= 0x00e2,
	EL_SET_FOCUS		= 0x00ea,

	EL_SET_NEGAPOSI		= 0x0201,
	EL_SET_COLORBW		= 0x0202,
	EL_SET_ROTATE		= 0x0203,
	EL_SET_PAUSE		= 0x0204,
	EL_SET_CONTRAST		= 0x0205,
	EL_SET_LAMP			= 0x0206,
	EL_SET_INPUT		= 0x0207,
	EL_SET_DETAIL		= 0x0208,
	EL_SET_DZOOM		= 0x0209,
	EL_SET_SDMODE		= 0x020a,
	EL_SET_CAPTURE		= 0x020b,
	EL_SET_SLIDESHOW	= 0x020c,
	EL_SET_PRESET		= 0x020d,
	EL_SET_CALL			= 0x020e,

//	EL_GET_JPEG_DATA	= 0x808e, // get JPEG data
//	EL_GET_JPEG_SXGA_DATA	= 0x8083, // get JPEG data
//	EL_GET_JPEG_VGA_DATA    = 0x8084,

	// FPGA command
//	EL_SET_FPGA_ADD_DATA = 0x0a00,
	// EEPROM command
//	EL_SET_CACHE_ADD_DATA = 0x0a01,
//	EL_SET_EEPROM_ADD_DATA = 0x0a02,
	
	// card command
	EL_SET_JPG_FILE		= 0x0104,
	EL_SET_RMDIR		= 0x0105,
	EL_SET_MKDIR		= 0x0106,
	EL_SET_PLAYLIST		= 0x0107,
	EL_SET_REFRESH_TABLE	= 0x0108,

	EL_GET_MEDIA_STATUS = 0x8101, // get the state if a media is in or not.
	EL_GET_DIR_LIST		= 0x8102, // get the list of the directory.
	EL_GET_JPG_FILE		= 0x8104,
	EL_GET_JPG_LIST_INDIR	= 0x8105,
	EL_GET_PLAYLIST		= 0x8107,

	EL_GET_KEY_EVENT	= 0x8301, // get the event of button

	// FPGA command
//	EL_GET_FPGA_ADD_DATA = 0x8a00,
	// EEPROM command
//	EL_GET_CACHE_ADD_DATA = 0x8a01,
//	EL_GET_EEPROM_ADD_DATA = 0x8a02,
	
	EL_GET_DUMMYCMD		= 0x87ff, // for test
	EL_SET_DUMMYCMD		= 0x07ff, // for test

	EL_GET_DUMMYDAT		= 0x8fff, // for test
	EL_SET_DUMMYDAT		= 0x0fff, // for test	
};

/* responde code */
enum
{
	EL_RES_SUCCESS				= 0x0000,
	EL_RES_FAILURE				= 0x0001
};

/* menu parameter */
enum
{
    // select
    EL_SELECT               = 0x0D, // Enter
    EL_MENU                 = 'm',  // 0x6D
	// 'arrow keys'
    EL_UP                   = 'k',  // 0x6B
    EL_DOWN                 = 'j',  // 0x6A
    EL_LEFT                 = 'h',  // 0x68
    EL_RIGHT                = 'l'   // 0x6C	
};

/* lens parameter */
enum
{
	EL_TELE			= 0x01,
	EL_WIDE			= 0x02,
	EL_STOP			= 0x00,
	
	EL_NEAR			= 0x01,
	EL_FAR			= 0x02,
	
	EL_OPEN			= 0x02,
	EL_CLOSE		= 0x03,
	EL_ISTOP		= 0x04,

	EL_NORMAL		= 0x05,
};

/* event */
enum
{
	EL_EVENT_CAPTURE	= 0x00000001,
	EL_EVENT_REMOTE		= 0x00000002,
};

/**
 * @brief EL_GET_JPEG_DATAのParam1
 */
enum {
	EL_JPEG_SIZE_CURRENT = 0,
	EL_JPEG_SIZE_HIGH,
	EL_JPEG_SIZE_MID,
	EL_JPEG_SIZE_LOW,
};

/**
 * @brief EL_GET_VERSION_INFOのParam1
 */
enum
{
	EL_VER_HEAD = 0,
	EL_VER_MAIN,
	EL_VER_FPGA,
};

/* media status value */
enum
{
	EL_ST_NORMAL		= 0x00000000,
	EL_ST_NOMEDIA		= 0x00000001,
	EL_ST_NOFILE		= 0x00000002,
	EL_ST_NOREAD		= 0x00000004,
	EL_ST_LOCK			= 0x00000008,
	EL_ST_OVER			= 0x00000010,
	EL_ST_ALREADY		= 0x00000020,
	EL_ST_READONLY		= 0x00000040,
	EL_ST_WRITE			= 0x00000080,
	EL_ST_CANCEL		= 0x00000100,
	EL_ST_FULL			= 0x00000200,
	EL_ST_NODELETE		= 0x00000400,
	EL_ST_NODIR			= 0x00000800,
	EL_ST_OTHER			= 0x80000000,
};

/* IRIS_TUN */
enum
{
	EL_ST_IR_PARAM			= 0x0001,
	EL_ST_IR_OFFSET			= 0x0002,
	EL_ST_IR_GAIN			= 0x0003,
	EL_ST_IR_OPEN_END		= 0x0004,
	EL_ST_IR_OPEN_SHORT		= 0x0005,
	EL_ST_IR_CLOSE_END		= 0x0006,
	EL_ST_IR_CLOSE_SHORT	= 0x0007,
	EL_ST_IR_MANUAL			= 0x0008,
	EL_ST_IR_AUTO			= 0x0009,
	EL_ST_IR_AE_PARAM		= 0x000A,
	EL_ST_IR_PARAM_DIRECT	= 0x000B,
	EL_ST_IR_OPEN_SHORT_THROUGH		= 0x000C,
	EL_ST_IR_CLOSE_SHORT_THROUGH	= 0x000D,
	EL_ST_IR_MINUTENESS_PARAM	= 0x000E,
	EL_ST_IR_MINUTENESS_CLOCK	= 0x000F,
	EL_ST_IR_MINUTENESS_DIVISION	= 0x0011,
	EL_ST_IR_MINUTENESS_PERIOD		= 0x0012,
	EL_ST_IR_MINUTENESS_START		= 0x0013,
	EL_ST_IR_MINUTENESS_SAMPLING	= 0x0014,

	EL_GT_IR_TUN_PARAM		= 0x8002,
	EL_GT_IR_AE_PARAM		= 0x8003,
	EL_GT_IR_MINUTENESS_PARAM	= 0x800E,
	EL_GT_IR_MINUTENESS_INFO	= 0x8010,

};

/* FOCUS */
enum
{
//	EL_ST_FC_NEAR_TUN		= 0x0001,
//	EL_ST_FC_FAR_TUN		= 0x0002,
	EL_ST_FC_AF_TUN			= 0x0003,
	EL_ST_FC_PI_TUN			= 0x0004,
	EL_ST_FC_BACK_TUN		= 0x0005,
	EL_ST_FC_STEP_TUN		= 0x0006,
	EL_ST_FC_POSITION		= 0x0007,
	EL_ST_FC_INITIAL		= 0x0008,	

	EL_GT_FC_PARAM			= 0x8001,
	EL_GT_FC_IDEAL_PARAM	= 0x8002,
	EL_GT_FC_AF_INFO		= 0x8003,
	EL_GT_FC_PI_PARAM		= 0x8004,
	EL_GT_FC_PI_INFO		= 0x8005,
};

/* Focus Param2*/
enum
{
	MIDDLE		= 0x0000,
	TELE		= 0x0001,
	WIDE		= 0x0002,
};

/* ZOOM param1 */
enum
{
	EL_ST_ZM_PARAM			= 0x0001,
	EL_ST_ZM_DZOOM_STOP		= 0x0002,
	EL_ST_ZM_PARAM_FOCUS	= 0x0003,
	EL_ST_ZM_PI_RESEARCH	= 0x0004,
	EL_GT_ZM_PARAM			= 0x8001,
	EL_GT_ZM_PI_PARAM		= 0x8002,
	EL_GT_ZM_PI_INFO		= 0x8003,
	EL_GT_ZM_PI_RESEARCH	= 0x8004,
};

/* ZOOM param1 */
enum
{
	UNLOCK		= 0x00,
	LOCK		= 0x01,
};

/* WB param1*/
enum
{
	EL_ST_WB_MODE	= 0x0001,
	EL_GT_WB_MODE	= 0x8001,
};

/* WB param2*/
enum
{
	AUTO	= 0x01,
	MANUAL	= 0x02,
	ONEPUSH	= 0x03,
};

/* JPEG_RATE */
enum
{
	EL_ST_JPEG_RATE	= 0x0001,
	EL_GT_JPEG_RATE	= 0x8001,
};

/* REMOTE_CONTROL */
enum
{
	EL_ST_REMOTE_OFF	= 0x0000,
	EL_ST_REMOTE_ON		= 0x0001,
};

/* SHUTTER SPEED  param1*/
enum
{
	EL_ST_FPS		= 0x0001,
	EL_ST_SHS1		= 0x0002,
	EL_GT_FPS		= 0x8001,
	EL_GT_SHS1		= 0x8002,
};

/* SHUTTER SPEED  param2*/
enum
{
	SS_7_5FPS		= 0x01,
	SS_15FPS		= 0x02,
	SS_30FPS		= 0x03,
};

/* VERSION_DATA */
enum
{
	EL_GT_SH_VERSION	= 0x8005,
	EL_GT_LENS_VERSION	= 0x8006,
	EL_GT_FPGA_VERSION	= 0x8007,
};

/* POWER OFF */
enum
{
	EL_ST_POWER_OFF = 0x01,
};

/* STATE_DATA */
enum
{
	EL_GT_STATE0	= 0x8000,
	EL_GT_STATE2	= 0x8002,
	EL_GT_STATE3	= 0x8003,
};

/* SMART JPEG取得確認コマンド */
enum
{
	EL_CAPTURE_INIT	= 0x0001,
	EL_CAPTURE_INFO	= 0x0002,
};

/*!
@brief 調整ホール
*/
typedef enum
{
	EL_ADJ_SET_HALL_ON					= 0x0E00,		//!<	ホール調整モード　ON
	EL_ADJ_SET_HALL_OFF					= 0x0E01,		//!<	ホール調整モード　OFF
	EL_ADJ_SET_HALL_START				= 0x0E02,		//!<	ホール調整　開始
	EL_ADJ_INQ_HALL_STATE				= 0x0F00,		//!<	調整　ホール調整状態 問合せ
	EL_ADJ_INQ_HALL_ADJ_VALUE			= 0x0F01,		//!<	調整　ホール調整値 問合せ
}eEL_ADJ_HALL;

/*!
@brief 調整レンズ
*/
typedef enum
{
	EL_ADJ_SET_LENS_STMON				= 0x0E03,		//!<	レンズSTM特殊駆動　ON
	EL_ADJ_SET_LENS_STMOFF				= 0x0E04,		//!<	レンズSTM特殊駆動　OFF
	EL_ADJ_SET_LENS_2GSTEP				= 0x0E05,		//!<	2群レンズ ステップ数指定駆動
	EL_ADJ_SET_LENS_3GSTEP				= 0x0E06,		//!<	3群レンズ ステップ数指定駆動
	EL_ADJ_SET_LENS_4GSTEP				= 0x0E07,		//!<	4群レンズ ステップ数指定駆動
	EL_ADJ_SET_LENS_2GPARAM				= 0x0E08,		//!<	2群レンズ 駆動パラメータ設定
	EL_ADJ_SET_LENS_3GPARAM				= 0x0E09,		//!<	3群レンズ 駆動パラメータ設定
	EL_ADJ_SET_LENS_4GPARAM				= 0x0E0A,		//!<	4群レンズ 駆動パラメータ設定
	EL_ADJ_SET_LENS_ALLPARAM			= 0x0E0B,		//!<	全レンズ 駆動バラメータ初期化
	EL_ADJ_SET_LENS_2GDET				= 0x0E0C,		//!<	2群レンズ 原点検出駆動
	EL_ADJ_SET_LENS_3GDET				= 0x0E0D,		//!<	3群レンズ 原点検出駆動
	EL_ADJ_SET_LENS_4GDET				= 0x0E0E,		//!<	4群レンズ 原点検出駆動
	EL_ADJ_SET_LENS_MEMORY				= 0x0E0F,		//!<	メモリへ値を保存する
	EL_ADJ_INQ_LENS_MEMORY				= 0x0F08,		//!<	メモリのデータ 問合せ
	EL_ADJ_INQ_LENS_PIVAL				= 0x0F09,		//!<	位置検出PI出力AD値 問合せ
	EL_ADJ_INQ_LENS_POS					= 0x0F0A,		//!<	レンズ位置 問合せ
	EL_ADJ_SET_LENS_ORGVAL				= 0x0E10,		//!<	2群原点位置の補正値設定
}eEL_ADJ_LENS;

/*!
@brief 調整BF
*/
typedef enum
{
	EL_ADJ_SET_BF_VAL					= 0x0E11,		//!<	BF調整値を設定する
	EL_ADJ_SET_BF_ON					= 0x0E12,		//!<	BF調整モード　ON
	EL_ADJ_SET_BF_OFF					= 0x0E13,		//!<	BF調整モード　OFF
	EL_ADJ_SET_BF_COMPPLUS				= 0x0E14,		//!<	BF調整モード COMP++
	EL_ADJ_SET_BF_COMPMINUS				= 0x0E15,		//!<	BF調整モード COMP--
	EL_ADJ_SET_BF_VARIPLUS				= 0x0E16,		//!<	BF調整モード VARI++
	EL_ADJ_SET_BF_VARIMINUS				= 0x0E17,		//!<	BF調整モード VARI--
	EL_ADJ_SET_BF_STEP1					= 0x0E18,		//!<	BF調整モード STEP1へ
	EL_ADJ_SET_BF_STEP2					= 0x0E19,		//!<	BF調整モード STEP2へ
	EL_ADJ_SET_BF_STEP3					= 0x0E1A,		//!<	BF調整モード STEP3へ
	EL_ADJ_SET_BF_STEP4					= 0x0E1B,		//!<	BF調整モード STEP4へ
	EL_ADJ_SET_BF_STEP5					= 0x0E1C,		//!<	BF調整モード STEP5へ
	EL_ADJ_SET_BF_STEP6					= 0x0E1D,		//!<	BF調整モード STEP6へ
	EL_ADJ_SET_BF_STEP7					= 0x0E1E,		//!<	BF調整モード STEP7へ
}eEL_ADJ_BF;

/*!
@brief AE機能
*/
typedef enum
{
	EL_ADJ_OPE_AE_AUTO					= 0x0E50,		//!<	AEモードをオートへ設定(BRIGHTNESSをAUTO)
	EL_ADJ_OPE_AE_REMOTE				= 0x0E51,		//!<	AEモードをリモートへ設定(BRIGHTNESSをMANUAL)
	EL_ADJ_OPE_AE_COMPUP				= 0x0E52,		//!<	AEレベルをUP(BRIGHTNESSをUP)
	EL_ADJ_OPE_AE_COMPDOWN				= 0x0E53,		//!<	AEレベルをDOWN(BRIGHTNESSをDOWN)
	EL_ADJ_OPE_AE_COMPDIRECT			= 0x0E54,		//!<	AEレベルをDIRECT設定(BRIGHTNESSをリセット)
	EL_ADJ_OPE_AE_MANUAL				= 0x0E55,		//!<	AEをマニュアルモードへ移行
	EL_ADJ_OPE_AE_IRISUP				= 0x0E56,		//!<	MANUAL時のアイリスUP
	EL_ADJ_OPE_AE_IRISDOWN				= 0x0E57,		//!<	MANUAL時のアイリスDOWN
	EL_ADJ_OPE_AE_IRISDIRECT			= 0x0E58,		//!<	MANUAL時のアイリスDIRECT
}eEL_ADJ_OPE_AE;

/*!
@brief LENS機能 ZOOM
*/
typedef enum
{
	EL_ADJ_OPE_ZOOM_TELE				= 0x0E59,		//!<	ズームテレ(スタンダード)動作開始
	EL_ADJ_OPE_ZOOM_WIDE				= 0x0E5A,		//!<	ズームワイド(スタンダード)動作開始
	EL_ADJ_OPE_ZOOM_STOP				= 0x0E5B,		//!<	ズーム動作停止
	EL_ADJ_OPE_ZOOM_DIRECT				= 0x0E5C,		//!<	ズームDIRECT動作
	EL_ADJ_OPE_ZOOM_ON					= 0x0E5D,		//!<	デジタルズームON
	EL_ADJ_OPE_ZOOM_OFF					= 0x0E5E,		//!<	デジタルズームOFF
}eEL_ADJ_OPE_ZOOM;

/*!
@brief LENS機能 FOCUS
*/
typedef enum
{
	EL_ADJ_OPE_FOCUS_NEAR				= 0x0E5F,		//!<	フォーカスNEAR(スタンダード)動作開始
	EL_ADJ_OPE_FOCUS_FAR				= 0x0E60,		//!<	フォーカスFAR(スタンダード)動作開始
	EL_ADJ_OPE_FOCUS_STOP				= 0x0E61,		//!<	フォーカス動作停止
	EL_ADJ_OPE_FOCUS_DIRECT				= 0x0E62,		//!<	フォーカスDIRECT動作
	EL_ADJ_OPE_FOCUS_AF					= 0x0E63,		//!<	フォーカスモードをAUTOへ設定
	EL_ADJ_OPE_FOCUS_MF					= 0x0E64,		//!<	フォーカスモードをMANUALへ設定
	EL_ADJ_OPE_FOCUS_AFFULL				= 0x0E65,		//!<	AFモードをフルオートAFへ設定
	EL_ADJ_OPE_FOCUS_AFINTERVAL			= 0x0E66,		//!<	AFモードをインターバルAFへ設定
	EL_ADJ_OPE_FOCUS_AFTRIG				= 0x0E67,		//!<	AFモードをズームトリガーAFへ設定
	EL_ADJ_OPE_FOCUS_AFONEPUSH			= 0x0E68,		//!<	AFモードをワンプッシュAFへ設定
	EL_ADJ_OPE_FOCUS_AFONEPUSHTRIG		= 0x0E69,		//!<	ワンプッシュAF時のトリガ
}eEL_ADJ_OPE_FOCUS;

/*!
@brief WB機能
*/
typedef enum
{
	EL_ADJ_OPE_WB_AUTO					= 0x0E6A,		//!<	WBモードをオートへ設定
	EL_ADJ_OPE_WB_ONEPUSH				= 0x0E6B,		//!<	WBモードをワンプッシュへ設定
	EL_ADJ_OPE_WB_MANUAL				= 0x0E6C,		//!<	WBモードをマニュアルへ設定
	EL_ADJ_OPE_WB_RUP					= 0x0E6D,		//!<	MWBのRゲインをUP
	EL_ADJ_OPE_WB_RDOWN					= 0x0E6E,		//!<	MWBのRゲインをDOWN
	EL_ADJ_OPE_WB_RDIRECT				= 0x0E6F,		//!<	MWBのRゲインDIRECT設定
	EL_ADJ_OPE_WB_BUP					= 0x0E70,		//!<	MWBのBゲインをUP
	EL_ADJ_OPE_WB_BDOWN					= 0x0E71,		//!<	MWBのBゲインをDOWN
	EL_ADJ_OPE_WB_BDIRECT				= 0x0E72,		//!<	MWBのBゲインDIRECT設定
}eEL_ADJ_OPE_WB;

/*!
@brief CAMERA機能 IMAGE
*/
typedef enum
{
	EL_ADJ_OPE_IMAGE_TEXT1				= 0x0E73,		//!<	映像モードをTEXT1へ設定
	EL_ADJ_OPE_IMAGE_TEXT2				= 0x0E74,		//!<	映像モードをTEXT2へ設定
	EL_ADJ_OPE_IMAGE_TEXT3				= 0x0E75,		//!<	映像モードをTEXT3へ設定
	EL_ADJ_OPE_IMAGE_GRAPHICS1			= 0x0E76,		//!<	映像モードをGraphics1(DLP)へ設定
	EL_ADJ_OPE_IMAGE_GRAPHICS2			= 0x0E77,		//!<	映像モードをGraphics2へ設定
}eEL_ADJ_OPE_IMAGE;

/*!
@brief CAMERA機能 GAMMA
*/
typedef enum
{
	EL_ADJ_OPE_GAMMA_LOW				= 0x0E78,		//!<	Graphics2のガンマをLOWへ設定
	EL_ADJ_OPE_GAMMA_MID				= 0x0E79,		//!<	Graphics2のガンマをMIDDLEへ設定
	EL_ADJ_OPE_GAMMA_HI					= 0x0E7A,		//!<	Graphics2のガンマをHIGHへ設定
}eEL_ADJ_OPE_GAMMA;

/*!
@brief CAMERA機能 EDGE
*/
typedef enum
{
	EL_ADJ_OPE_EDGE_LOW					= 0x0E7B,		//!<	輪郭強調をLOWへ設定
	EL_ADJ_OPE_EDGE_MID					= 0x0E7C,		//!<	輪郭強調をMIDDLEへ設定
	EL_ADJ_OPE_EDGE_HI					= 0x0E7D,		//!<	輪郭強調をHIGHへ設定
}eEL_ADJ_OPE_EDGE;

/*!
@brief CAMERA機能 FLICKER
*/
typedef enum
{
	EL_ADJ_OPE_FLICKER_50HZ				= 0x0E7E,		//!<	60Hzフリッカーレス設定
	EL_ADJ_OPE_FLICKER_60HZ				= 0x0E7F,		//!<	50Hzフリッカーレス設定
}eEL_ADJ_OPE_FLICKER;

/*!
@brief 調整AF
*/
typedef enum
{
	EL_ADJ_SET_AF_AREA0					= 0x0E1F,		//!<	AREA0のサイズを設定
	EL_ADJ_SET_AF_AREA1					= 0x0E20,		//!<	AREA1のサイズを設定
	EL_ADJ_SET_AF_AREA2					= 0x0E21,		//!<	AREA2のサイズを設定
	EL_ADJ_SET_AF_MEASURE				= 0x0E22,		//!<	AF評価値測定開始
	EL_ADJ_INQ_AF_AREA0					= 0x0F0B,		//!<	現在の解像力とAREA0の設定サイズの問合せ
	EL_ADJ_INQ_AF_AREA1					= 0x0F0C,		//!<	現在の解像力とAREA1の設定サイズの問合せ
	EL_ADJ_INQ_AF_AREA2					= 0x0F0D,		//!<	現在の解像力とAREA2の設定サイズの問合せ
	EL_ADJ_INQ_AF_MEASURE				= 0x0F0E,		//!<	測定状態の問合せ
	EL_ADJ_INQ_AF_VALUE					= 0x0F0F,		//!<	測定結果の問合せ
}eEL_ADJ_INQ_AF;

#define		RETRY_MAX					(150)			//!<	本体と処理が被った場合のリトライ回数

#define		JPEG_QUARITY_DEF			(80)			//!<	JPEGデフォルト画像品質

/*!
@brief 画像取得エラー
*/
#define		CAMERA_MODE_SD				(0x0001)		//!<	SDモード
#define		CAMERA_MODE_REC				(0x0002)		//!<	本体録画中
#define		CAMERA_MODE_VIDEOREPLAY		(0x0003)		//!<	本体動画再生中
#define		CAMERA_MODE_BUSY			(0x0004)		//!<	本体と処理が被った
#define		CAMERA_MODE_FAIL			(0xffff)		//!<	エラー
#define		DEVICE_NOT_CONNECT			(0xffff)		//!<	デバイス非接続

#endif
