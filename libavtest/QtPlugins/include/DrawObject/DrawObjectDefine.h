//--------------------------------------------------------------------------------/
/*!
	@file	DrawObjectDefine.h
	@brief	描画オブジェクト定義
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef DRAWOBJECTDEFINE_H
#define DRAWOBJECTDEFINE_H
#include "IDrawObjectCreator.h"

//オブジェクトパラメータ定義
#define DRAWOBJECT_PARAM_COLOR			(0x00000001)	//!< 色(QColor)
#define DRAWOBJECT_PARAM_PEN_WIDTH		(0x00000002)	//!< ペン幅(qreal)
#define DRAWOBJECT_PARAM_PEN_STYLE		(0x00000004)	//!< ペン種(Qt::PenStyle)
#define DRAWOBJECT_PARAM_ISFILL			(0x00000008)	//!< 塗りつぶし(bool)
#define DRAWOBJECT_PARAM_FILENAME		(0x00000010)	//!< ファイル名(QString)
#define DRAWOBJECT_PARAM_SCENESIZE		(0x00000020)	//!< sceneのサイズ(QSize)
#define DRAWOBJECT_PARAM_STAMPSIZE		(0x00000040)	//!< スタンプサイズ(QSize)
#define DRAWOBJECT_PARAM_PICTSIZE		(0x00000080)	//!< 画像サイズ(QSize)
#define DRAWOBJECT_PARAM_POINTS			(0x00000100)	//!< 座標リスト(QVector)
#define DRAWOBJECT_PARAM_POS_START		(0x00000200)	//!< 開始座標(QPointF)
#define DRAWOBJECT_PARAM_POS_END		(0x00000400)	//!< 終了座標(QPointF)
#define DRAWOBJECT_PARAM_CRE_FRAME		(0x00000800)	//!< Frame生成設定(bool)
#define DRAWOBJECT_PARAM_TEXT_COLOR		(0x00001000)	//!< 文字色(QColor)
#define DRAWOBJECT_PARAM_FONT			(0x00002000)	//!< フォント(QFont)
#define DRAWOBJECT_PARAM_CONTEXTMENU	(0x00004000)	//!< コンテキストメニュー設定(QMenu*)

//自由線
#define FREELINE_OBJECT_NAME	"Freeline"	//!< 自由線オブジェクト名
#define FREELINE_PARAMS (DRAWOBJECT_PARAM_COLOR | DRAWOBJECT_PARAM_PEN_WIDTH | DRAWOBJECT_PARAM_PEN_STYLE | DRAWOBJECT_PARAM_SCENESIZE)	//!< 自由線に設定できるパラメータ
//直線
#define STRAIGHTLINE_OBJECT_NAME	"Straightline"	//!< 直線オブジェクト名
#define STRAIGHTLINE_PARAMS (DRAWOBJECT_PARAM_COLOR | DRAWOBJECT_PARAM_PEN_WIDTH | DRAWOBJECT_PARAM_PEN_STYLE)	//!< 直線に設定できるパラメータ
//capa add start
//破線
#define DASHLINE_OBJECT_NAME	"Dashline"	//!< 破線オブジェクト名
#define DASHLINE_PARAMS (DRAWOBJECT_PARAM_COLOR | DRAWOBJECT_PARAM_PEN_WIDTH | DRAWOBJECT_PARAM_PEN_STYLE)	//!< 破線に設定できるパラメータ
//capa add end
//四角
#define RECT_OBJECT_NAME	"Rect"	//!< 四角オブジェクト名
#define RECT_PARAMS (DRAWOBJECT_PARAM_COLOR | DRAWOBJECT_PARAM_PEN_WIDTH | DRAWOBJECT_PARAM_PEN_STYLE | DRAWOBJECT_PARAM_ISFILL)	//!< 四角に設定できるパラメータ
//capa add start
//塗り四角
#define FILL_RECT_OBJECT_NAME	"Fill_Rect"	//!< 塗り四角オブジェクト名
#define FILL_RECT_PARAMS (DRAWOBJECT_PARAM_COLOR | DRAWOBJECT_PARAM_PEN_WIDTH | DRAWOBJECT_PARAM_PEN_STYLE | DRAWOBJECT_PARAM_ISFILL)	//!< 塗り四角に設定できるパラメータ
//capa add end
//楕円
#define ELLIPSE_OBJECT_NAME	"Ellipse"	//!< 楕円オブジェクト名
#define ELLIPSE_PARAMS (DRAWOBJECT_PARAM_COLOR | DRAWOBJECT_PARAM_PEN_WIDTH | DRAWOBJECT_PARAM_PEN_STYLE | DRAWOBJECT_PARAM_ISFILL)	//!< 楕円に設定できるパラメータ
//capa add start
//塗り楕円
#define FILL_ELLIPSE_OBJECT_NAME	"Fill_Ellipse"	//!< 塗り楕円オブジェクト名
#define FILL_ELLIPSE_PARAMS (DRAWOBJECT_PARAM_COLOR | DRAWOBJECT_PARAM_PEN_WIDTH | DRAWOBJECT_PARAM_PEN_STYLE | DRAWOBJECT_PARAM_ISFILL)	//!< 塗り楕円に設定できるパラメータ
//capa add end
//画像
#define PICTURE_OBJECT_NAME	"Picture"	//!< 画像オブジェクト名
#define PICTURE_PARAMS (DRAWOBJECT_PARAM_FILENAME)	//!< 画像で設定できるパラメータ
//スタンプ
#define STAMP_OBJECT_NAME	"Stamp"		//!< スタンプオブジェクト名
#define STAMP_PARAMS (PICTURE_PARAMS)	//!< スタンプで設定できるパラメータ
//フラッシュ
#define FLASH_OBJECT_NAME	"Flash"	//!< フラッシュオブジェクト名
#define FLASH_PARAMS (PARAM_COLOR | PARAM_PEN_WIDTH | PARAM_PEN_STYLE)	//!< フラッシュで設定できるパラメータ
//テキスト
#define TEXT_OBJECT_NAME	"Text"	//!< テキストオブジェクト名
#define TEXT_PARAMS	(DRAWOBJECT_PARAM_COLOR | DRAWOBJECT_PARAM_PEN_WIDTH | DRAWOBJECT_PARAM_TEXT_COLOR | DRAWOBJECT_PARAM_FONT)	//!< テキストで設定できるパラメータ

extern "C"{
	void setAutoAdjustPointMode(QString strMode);	//!< 自動座標補正設定(定義はAbstShapeObject.cpp)
	void createKnobImage();	//!< つまみ画像生成(定義はClObjectKnob.cpp)
	void deleteKnobImage();	//!< つまみ画像破棄(定義はClObjectKnob.cpp)
}

//オブジェクトデータ設定(コンテキストメニューで使用)
typedef QPair<qint32, QVariant> pairObjectData;	//データ種別とデータのペア
Q_DECLARE_METATYPE(pairObjectData)
#endif //DRAWOBJECTDEFINE_H
