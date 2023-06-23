/*--------------------------------------------------------------------------------*/
/*!
	@file	CmnValue.h
	@brief	コンポーネント間で共通で使用する色々
*/
/*--------------------------------------------------------------------------------*/
#ifndef CMNVALUE_H
#define CMNVALUE_H

//**************************************************************************//
/*!
 *	@brief		メニュー位置
*/
//**************************************************************************//
typedef enum
{
    eMENU_LEFT_POS = 1,
    eMENU_RIGHT_POS = 2
} eMENU_POS;


//**************************************************************************//
/*!
 *	@brief		USBの画像解像度
*/
//**************************************************************************//
enum {
	USB_RES_DEFAULT = 0,
	USB_RES_LOW,
	USB_RES_MIDDLE,
	USB_RES_HIGH
};

//**************************************************************************//
/*!
 *	@brief		画質
*/
//**************************************************************************//
enum {
	eUSB_QUARITY_HIGH = 80,
	eUSB_QUARITY_MIDDLE = 60,
	eUSB_QUARITY_LOW = 40,
	eUSB_QUARITY_DEF = eUSB_QUARITY_MIDDLE
};

//**************************************************************************//
/*!
 *	@brief		画像種別
*/
//**************************************************************************//
enum
{
	IMG_UNKNOWN = -1,
	IMG_USB_NORMAL = 0,			//	USB経由で取得した画像
	IMG_USB_SD					//	書画カメラがSDモードの時の画像
};


//**************************************************************************//
/*!
 *	@brief		コンテンツ選択画面におけるフィルタの種類
*/
//**************************************************************************//
typedef enum
{
	eFILTER_TYPE_NONE = 0,	//!< フィルタ無し
	eFILTER_TYPE_PICTURE,	//!< 静止画
	eFILTER_TYPE_MOVIE,		//!< 動画

	eFILTER_TYPE_TERMINATE
} eFILTER_TYPE;

//**************************************************************************//
/*!
 *	@brief		消しゴムモード
*/
//**************************************************************************//
typedef enum
{
	eMODE_ERASER_ALL = 0,	//!< 全消し
	eMODE_ERASER_PART		//!< 部分消し
} eMODE_ERASER;
//**************************************************************************//
/*!
 *	@brief		リモコン用
*/
//**************************************************************************//
enum eZoomIndex{
	eACT_ZOOM_TELE = 0,
	eACT_ZOOM_WIDE,
	eACT_ZOOM_STOP,
};
enum eFocusIndex{
	eACT_FOCUS_NEAR = 0,
	eACT_FOCUS_FAR,
	eACT_FOCUS_AF,
	eACT_FOCUS_STOP,
};
enum eBrightnessIndex{
	eACT_BRIGHTNESS_OPEN = 0,
	eACT_BRIGHTNESS_CLOSE,
	eACT_BRIGHTNESS_NORMAL,
	eACT_BRIGHTNESS_STOP,
};
enum eResolutionIndex{
	ePARAM_VGA = 0,
	ePARAM_XGA,
	ePARAM_720p,
	ePARAM_1080i,
};
enum eFramerateIndex{
	ePARAM_30FPS = 0,
	ePARAM_15FPS,
};
enum eQualityIndex{
	ePARAM_LOW = 0,
	ePARAM_MIDDLE,
	ePARAM_HIGH,
};
//**************************************************************************//
//レジストリ
#define		qstrKey(x)						QString(x)

#define REG_ROOT_ORG						"ELMO"
#define REG_ROOT_APP						"ELMO Interactive Toolbox"
#define LOGFILENAME							"/eit_log%1.txt"

#define	REG_KEY_ENV							qstrKey("env/")

//LOG file
#define REG_KEY_LOGFILE_NUMBER				REG_KEY_ENV + qstrKey("lognum")

//MainView
#define REG_KEY_SETTING_OPENGL				REG_KEY_ENV + qstrKey("opengl")
#define REG_KEY_SETTING_ANTIALIASING		REG_KEY_ENV + qstrKey("antialiasing")
#define REG_KEY_SETTING_DISCONNECT_ICB		REG_KEY_ENV + qstrKey("disconnect_icb")

//UvcDriver
#define REG_KEY_SETTING_UVC_MEDIATYPE		REG_KEY_ENV + qstrKey("uvc_mediatype")
#define REG_KEY_SETTING_UVC_HEIGHT			REG_KEY_ENV + qstrKey("uvc_height")
#define REG_KEY_SETTING_UVC_WIDTH			REG_KEY_ENV + qstrKey("uvc_width")
#define REG_KEY_SETTING_UVC_FRAMERATE		REG_KEY_ENV + qstrKey("uvc_framerate")
#define REG_KEY_SETTING_UVC_STREAM			REG_KEY_ENV + qstrKey("uvc_stream")	//!< 正常にストリームが終了したかを判定するために使用

//Selector
#define REG_KEY_SETTING_CONNECT_CAM			REG_KEY_ENV + qstrKey("connect_cam")
#define REG_KEY_SETTING_CHECK_KEYSTATE		REG_KEY_ENV + qstrKey("check_keystate")

//Menu
#define	REG_KEY_SETTING_REMO_POSX			REG_KEY_ENV + qstrKey("remo_pos_x")
#define	REG_KEY_SETTING_REMO_POSY			REG_KEY_ENV + qstrKey("remo_pos_y")
#define REG_KEY_SETTING_UVC_CONF_SAVE		REG_KEY_ENV + qstrKey("uvc_conf_save")

//QSettings::SystemScope
#define	REG_PARAM_SRSDATASPATH				qstrKey("SRSDatasPath")							//!< String:共通データ格納領域

//LICENSING 2013.05.27追加　2013.06.04修正
#define REG_KEY_SETTING_LICENSING			qstrKey("Licensing/")				//!< 下記保存場所をユーザー領域からシステム領域へ引越すため場所変更2013.6.4
#define REG_PARAM_LICENSING_KEYCODE			qstrKey("LicenceKey")				//!< ライセンスキー
#define REG_PARAM_SERIAL_NO					qstrKey("SerialNo")					//!< シリアルNo.

//QSettings::UserScope
#define	REG_PARAM_CONTENTSPATH				qstrKey("RecPath")					//!< String:「ELMO Interactive Toolbox/Rec」とかそんな感じのフォルダーへのパス
#define	REG_PARAM_IS_OUTPUT_MOUSELOG		qstrKey("IsOutputMouseLog")			//!< String(bool):マウスクリックログ出力機能をONにするかどうか。当然ここがtrueってなってる場合のみ出力される
#define REG_PARAM_WRITE_TASTE				qstrKey("WriteAccelerator")			//!< String(bool):ペンの書き味を良いか悪いか、trueの時書き味が悪くなる。通常はfalseである

#define	REG_PARAM_DEFAULT_DBNAME			"EL_RESULT.db"


#define REG_PEN1_SETTING_KEY				"Component/Setting/SRSPen1/"
#define REG_PEN2_SETTING_KEY				"Component/Setting/SRSPen2/"
#define REG_PEN_SETTING_COLOR				"Color"
#define REG_PEN_SETTING_SNAPGRID			"SnapGrid"
#define REG_PEN_SETTING_SHAPE				"Shape"
#define REG_PEN_SETTING_THICK				"Thickness"
#define REG_PEN_SETTING_ISFREE				"IsFree"
#define REG_PEN_SETTING_FREE				"FreeColor"

#define REG_ERASER_SETTING_KEY				"Component/Setting/SRSEraser/"
#define REG_ERASER_SETTING_MODE				"Mode"

#define REG_UPD_SETTING_KEY					"Component/Setting/Srs/"
#define REG_UPD_SETTING_DBPATH				"DataBasePath"
#define REG_UPD_SETTING_DBNAME				"DataBaseName"

#if defined(TRIAL_COPY)
#define	REG_KEY_TRIAL_DATETIME				"DataBaseMode"						//!< トライアル版で起動時の確認に使うレジストリ ※トライアル版日付管理用ファイル(TrialCopyInfo.txt)の生成日時をMD5にしたもの
#endif //TRIAL_COPY

//USB
#define	REG_KEY_SETTING_USB					qstrKey("Component/Setting/USB/")	//!< USB関連設定へのレジストリパス
#define	REG_PARAM_USB_FR					qstrKey("Framerate")				//!< DWORD:通常状態における書画カメラのフレームレート
#define	REG_PARAM_USB_ISCHANGE_FR_DRAWING	qstrKey("IsChangeFramerateDrawing")	//!< String(bool):描画中にフレームレートを変化させるかどうか
#define	REG_PARAM_USB_FR_DRAWING			qstrKey("FramerateDrawing")			//!< DWORD:描画中のフレームレート。↑がtrueな場合のみ有効となる
#define	REG_PARAM_USB_RESOLUTION			qstrKey("Resolution")				//!< QString:解像度(解像度設定コマンド対応のカメラ接続時のみ使用)
#ifdef ENABLE_NDA_CTRL //Silex SDK 対応
#define REG_PARAM_USB_FR_UVC				qstrKey("FramerateUVC")				//!< UVCカメラのフレームレート設定(true=最大フレームレート/false=最小フレームレート)
#endif //ENABLE_NDA_CTRL

//REC
#define	REG_KEY_SETTING_REC					qstrKey("Component/Setting/Rec/")	//!< 録画関連設定へのレジストリパス
#define	REG_PARAM_REC_FORMAT				qstrKey("Format")					//!< DWORD:録画ファイルフォーマット
#define	REG_PARAM_REC_SIZE					qstrKey("Size")						//!< DWORD:録画画像の幅
#define	REG_PARAM_REC_INTERVAL				qstrKey("Interval")					//!< DWORD:間欠時間
#define	REG_PARAM_REC_TYPE					qstrKey("Type")						//!< DWORD:録画形式。中身はH264EncoreプラグインのeENCODER_REC_TYPE
#define	REG_PARAM_REC_LIMIT					qstrKey("RecLimit")					//!< DWORD:録画が許容される最大ドライブ使用率、0で制限を設けない

//マスク,ハイライト
#define REG_KEY_SETTING_DRAW				qstrKey("Component/Setting/Draw/")	//!< 描画設定レジストリパス
#define REG_PARAM_DRAW_MASK_COLOR			qstrKey("MaskColor")				//!< マスク色
#define REG_PARAM_DRAW_MASK_RECT			qstrKey("MaskRect")					//!< マスク領域
#define REG_PARAM_DRAW_HIGHLIGHT_COLOR		qstrKey("HighlightColor")			//!< ハイライト色
#define REG_PARAM_DRAW_HIGHLIGHT_RECT		qstrKey("HighlightRect")			//!< ハイライト領域

//MENU
#define REG_KEY_SETTING_MENU				qstrKey("Component/Setting/Menu/")  //!< メニュー設定レジストリパス
#define REG_PARAM_RIGHTMENU_POS				qstrKey("MenuPositon")				//!< サイドメニュ-の位置
#define REG_PARAM_MENU_INVISIBLE			qstrKey("MenuInvisible")			//!< メニュー隠し
#define REG_PARAM_PICTURE					qstrKey("Picture")					//!< 画像挿入
#define REG_PARAM_STAMP						qstrKey("Stamp")					//!< スタンプ

//消しゴム
#define REG_PARAM_MENU_ERASER               qstrKey("MenuEraser")               //!< 2層メニュー(消しゴム)のアクティブなボタン

#define REG_PARAM_MENU_SAVE                 qstrKey("MenuSave")                 //!< 2層メニュー(保存)のアクティブなボタン
#define REG_PARAM_MENU_CAMERA               qstrKey("MenuCamera")               //!< 2層メニュー(カメラ)のアクティブなボタン
#define REG_PARAM_MENU_TOOL                 qstrKey("MenuTool")                 //!< 2層メニュー(ツール)のアクティブなボタン

//コンテンツ選択画面
#define	REG_KEY_SETTING_CONTENTS			qstrKey("Component/Setting/Contents/")	//!< コンテンツ選択画面設定へのレジストリパス
#define	REG_PARAM_CONTENTS_DISP_FAVBGMANUAL	qstrKey("DispFavBGManual")				//!< String(bool):コンテンツ選択画面で、お気に入り背景ボタンを押したときにダイアログを出すかどうか

//スタンプ
#define	REG_KEY_SETTING_STAMP				qstrKey("Component/Setting/Stamp/")		//!< スタンプ関連設定へのレジストリパス
#define	REG_PARAM_STAMP_CATEGORY			qstrKey("Category")						//!< DWORD:スタンプのカテゴリ設定

//フォルダーやファイル名
#define	FOLDER_NAME_BASE					("ELMO Interactive Toolbox")		//!< フォルダーパス基準
#define	FOLDER_NAME_DEFAULT					("rec")								//!< 通常のキャプチャとかの保存先
#define	FOLDER_NAME_FAVORITE				("favorite")						//!< お気に入りフォルダー
#define	FOLDER_NAME_MYBACKGROUND			("MyBackground")					//!< カスタム背景フォルダー
#define	FOLDER_NAME_MYBACKGROUND_DEFAULT	("default")							//!< カスタム背景フォルダー内のデフォルト背景置き場
#define	FOLDER_NAME_MYSTAMP					("MyStamp")							//!< カスタムスタンプフォルダー
#define	FOLDER_NAME_FAVORITE_STAMP			("Favorites")						//!< スタンプお気に入りフォルダー

#define	FOLDER_NAME_STAMP_FAVORITE			QObject::tr("Favorites")			//!< スタンプお気に入りフォルダー表示文言
#define	FOLDER_NAME_STAMP_COINS				QObject::tr("Coins")				//!< スタンプコインフォルダー表示文言
#define	FOLDER_NAME_STAMP_FIGURES			QObject::tr("Figures")				//!< スタンプ図形フォルダー表示文言
#define	FOLDER_NAME_STAMP_FOODS				QObject::tr("Foods")				//!< スタンプ食べ物フォルダー表示文言
#define	FOLDER_NAME_STAMP_LIVINGTHINGS		QObject::tr("Living things")		//!< スタンプ生き物フォルダー表示文言
#define	FOLDER_NAME_STAMP_OTHERS			QObject::tr("Others")				//!< スタンプその他フォルダー表示文言
#define	FOLDER_NAME_STAMP_PLANTS			QObject::tr("Plants")				//!< スタンプ植物フォルダー表示文言
#define	FOLDER_NAME_STAMP_WEATHERCHART		QObject::tr("Weather charts")		//!< スタンプ天気フォルダー表示文言

#define	FOLDER_NAME_COMMON_BACKGROUND		("Background")						//!< 共通領域下のデフォルト背景フォルダー名
#define	FILE_NAME_FAVORITE_BG				("favoriteBG.fvl")					//!< お気に入り背景リスト

#define	FOLDER_NAME_COMMON_STAMP			("Stamp")							//!< 共通領域下のデフォルトスタンプフォルダー名

#define	FOLDER_NAME_DEFAULT_INS				("/ELMO/ELMOInteractiveToolbox")	//!< デフォルト背景のインストール先(コピー基フォルダー)

//使用モニター設定
#define	REG_KEY_USE_MONITOR					("env/UsePrimaryMonitor")			//!< 使用モニターー設定

/* 2014/12/22 - nahuy - Add Start */
#ifdef Q_OS_IOS
#ifdef WEB_VIEW
#define FOLDER_NAME_WEBVIEW                  ("WebView")                         //!< Web view
#endif	//WEB_VIEW
#ifdef CAM_VIEW
#define FOLDER_NAME_CAMVIEW                  ("CamView")                         //!< Cam view
#endif	//CAM_VIEW
#endif
/* 2014/12/22 - nahuy - Add End */

//カーソルホットスポット
#define CURSOR_ARROW_HOT_X		1	//矢印
#define CURSOR_ARROW_HOT_Y		1
#define CURSOR_PEN_HOT_X		2	//ペン、マーカー
#define CURSOR_PEN_HOT_Y		29
#define CURSOR_ERASER_HOT_X		5	//消しゴム
#define CURSOR_ERASER_HOT_Y		25
#define CURSOR_HAND_HOT_X		16	//手
#define CURSOR_HAND_HOT_Y		16
#define CURSOR_SIZE_HOT_X		16	//サイズ変更
#define CURSOR_SIZE_HOT_Y		16
#define CURSOR_PICT_HOT_X		1	//画像
#define CURSOR_PICT_HOT_Y		1

/*--------------------------------------------------------------------------------*/
/*
	デフォルト値
*/
/*--------------------------------------------------------------------------------*/
#define	DEFAULT_REC_FORMAT		(1)			//!< フォーマット。H264Encoder.hのフォーマット定義enumの値。H.264は再生できないOSがあるのでデフォルトはAvi
#if 0 //elmo del
#define	DEFAULT_REC_SIZE		QSize(0, 0)	//!< サイズ。0,0で、ディスプレイサイズとする
#else //elmo add
#define	DEFAULT_REC_SIZE		QSize(1280, 720)	//!< 初期は720pとする
#endif
#define	DEFAULT_REC_INTERVAL	(60)		//!< 間欠時間。これはそのまんま秒単位。
#define	DEFAULT_REC_TYPE		(0)			//!< 録画形式。AbstEncoderThreadのeENCODER_REC_TYPE。値としては連続録画
#define	DEFAULT_REC_LIMIT		(95)		//!< 録画を許容する最大ドライブ使用率。0なので無制限にしとく？

#define	DEFAULT_FAVORITEBG_MAX	(4)			//!< お気に入り背景の最大数

#define DEFAULT_PEN_COUNT		(2)			//!< ペン数

/*--------------------------------------------------------------------------------*/
/*
	D&D関連
*/
/*--------------------------------------------------------------------------------*/
#define	DANDD_MIMEDATATYPE_TYPE		("TYPE")			//!< MIMEデータタイプ名定義・誰からD&Dされたかを示す
#define	DANDD_MIMEDATATYPE_FILEPATH	("FILEPATH")		//!< MIMEデータタイプ名定義・サムネイルだった場合、そのサムネイルを示すファイルパスを示す
#define	DANDD_TYPE_THUMBNAIL		("FromThumbnail")	//!< サムネイルからD&Dされたことを示す
#define	DANDD_TYPE_FAVORITEBG		("FromFavoriteBG")	//!< お気に入り背景ボタンからD&Dされたことを示す

#endif // CMNVALUE_H
