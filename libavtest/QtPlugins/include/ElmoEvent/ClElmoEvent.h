//--------------------------------------------------------------------------------//
/*!
	@file	ClElmoEvent.h
	@brief	イベントクラスヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLELMOEVENT_H
#define CLELMOEVENT_H
#include <QEvent>
#include <QPoint>

//--------------------------------------------------------------------------------//
/*!
	@brief	イベントタイプ
*/
//--------------------------------------------------------------------------------//
typedef enum
{
	eEVENT_INPUT = QEvent::User + 1,	//!< 入力イベント
	eEVENT_SHORTCUT						//!< ショートカット
}eElmoEventType;
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief	入力イベント
*/
//--------------------------------------------------------------------------------//
class ClInputEvent : public QEvent
{
public:
	typedef enum
	{
		eBTN_LEFT,		//左ボタン
		eBTN_RIGHT,		//右ボタン
		eBTN_MID		//中ボタン
	}eButtonType;

	ClInputEvent(QPoint clCursorPos);		//コンストラクタ
	virtual bool isBtnPress(eButtonType eButton) = 0;
	virtual bool isBtnRelease(eButtonType eButton) = 0;

	/*!
		@brief	座標取得
		@retval	イベントの持つ座標
	*/
	QPoint getPos(){return m_clCursorPos;}

    /*!
        @brief	筆圧取得
        @retval	ペンの筆圧
    */
    int getPresser(){return m_iPenPresser;}
    unsigned long getEventType(){return m_iEventType;}

protected:
	QPoint m_clCursorPos;	//!< 入力座標
	ushort m_usBtnState;	//!< ボタン状態
    int m_iPenPresser;      //!< ペン圧
    unsigned long m_iEventType;
};
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief	ショートカット
*/
//--------------------------------------------------------------------------------//
class ClShortcutEvent : public QEvent
{
public:
	typedef enum
	{
		eSHORTCUT_CAMERA = 0,
		eSHORTCUT_WHITEBOARD = 1,
		eSHORTCUT_CONTENTS_OPEN = 2,
		eSHORTCUT_CONTENTS_CREATE = 3,
		eSHORTCUT_SETTING = 4,
		eSHORTCUT_CLOSE = 5,
		eSHORTCUT_IWBCALIB_START = 16,	//IWB対応
		eSHORTCUT_IWBCALIB_END = 17	
	}eShortcut;

	ClShortcutEvent(eShortcut eValue);	//コンストラクタ

	/*!
		@brief	ショートカット設定取得
		@retval	ショートカット設定
	*/
	eShortcut getValue(){return m_eShortcut;}

protected:
	eShortcut m_eShortcut;	//!< ショートカット設定
};
//--------------------------------------------------------------------------------//
#endif
