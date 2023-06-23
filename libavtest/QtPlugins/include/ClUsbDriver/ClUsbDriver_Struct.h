#ifndef CAMERAFUNCSTRUCT_H
#define CAMERAFUNCSTRUCT_H

#if 0
#include <QMetatype>
#include <QWidget>

/*! --------------------------------------------- //
//
//		@brief	カメラの機能
//
// ---------------------------------------------- */
enum eCMR_FUNC_CODE
{
	eFUNC_NONE,

	// ---------------------------- Camera control
	eFUNC_CTRL_PAN,
	eFUNC_CTRL_TILT,
	eFUNC_CTRL_ROLL,
	eFUNC_CTRL_ZOOM,
	eFUNC_CTRL_EXPOSE,
	eFUNC_CTRL_IRIS,
	eFUNC_CTRL_FOCUS,
	eFUNC_CTRL_PRIVACY,
	eFUNC_CTRL_AUTO_EXPOSE_PRIORITY,

	// ---------------------------- Video amp
	eFUNC_AMP_BRIGHTNESS,
	eFUNC_AMP_CONTRAST,
	eFUNC_AMP_HUE,
	eFUNC_AMP_SATUATION,
	eFUNC_AMP_SHARPNESS,
	eFUNC_AMP_GAMMA,
	eFUNC_AMP_WB_TEMP,
	eFUNC_AMP_BACKLIGHT_COMPENSATION,
	eFUNC_AMP_GAIN,
	eFUNC_AMP_POWER_LINE_FREAQUENCY,

	// ---------------------------- Extension unit
	eFUNC_EXTENSION

};

/*! --------------------------- //
//	@brief フォーカスオプション
//  --------------------------- */
enum eCMR_FOCUS_OPTION_CODE
{
	eCMR_FOCUS_NEAR,		//!< ニア
	eCMR_FOCUS_FAR			//!< ファー
};

/*! --------------------------- //
//	@brief ズームオプション
//  --------------------------- */
enum eCMR_ZOOM_OPTION_CODE
{
	eCMR_ZOOM_TELE,			//!< テレ
	eCMR_ZOOM_WIDE			//!< ワイド
};

/*! --------------------------- //
//	@brief ブライトネスオプション
//  --------------------------- */
enum eCMR_BRIGHTNESS_OPTION_CODE
{
	eCMR_BRIGHTNESS_OPEN,	//!< オープン
	eCMR_BRIGHTNESS_CLOSE	//!< クローズ
};

/*! --------------------------------------------- //
//
//		@brief	生成するウィジェットコード
//
// ---------------------------------------------- */
enum eCMR_WIDGET_CODE
{
	eFUNC_WIDGET_NONE,			//!< None
	eFUNC_WIDGET_QPUSHBUTTON,	//!< QPushButton
	eFUNC_WIDGET_QSLIDER		//!< QSlider
};

/*! --------------------------------------------- //
//
//		@brief	機能要素
//
// ---------------------------------------------- */
struct stCMR_FUNC_WIDGET
{
	stCMR_FUNC_WIDGET(	eCMR_WIDGET_CODE	eWidgetCode = eFUNC_WIDGET_NONE,
						eCMR_FUNC_CODE		eFuncCode	= eFUNC_NONE,
						qint32				qiOption	= 0,
						QWidget*			pclWidget	= 0)
		: eWidgetCode	( eWidgetCode )
		, eFuncCode		( eFuncCode )
		, qiOption		( qiOption )
		, pclWidget		( pclWidget )
	{}

	eCMR_WIDGET_CODE	eWidgetCode;	//!< ウィジェットコード
	eCMR_FUNC_CODE		eFuncCode;		//!< 機能コード
	qint32				qiOption;		//!< オプションコード

	QWidget*			pclWidget;		//!< ウィジェット
};
Q_DECLARE_METATYPE(stCMR_FUNC_WIDGET)

#endif

#endif // CAMERAFUNCSTRUCT_H
