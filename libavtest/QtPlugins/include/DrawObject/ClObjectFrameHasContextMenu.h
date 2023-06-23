//--------------------------------------------------------------------------------//
/*!
	@file	ClObjectFrameHasContextMenu.h
	@brief	コンテキストメニュー表示ボタンを持った枠ヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLOBJECTFRAMEHASCONTEXTMENU_H
#define CLOBJECTFRAMEHASCONTEXTMENU_H
#include "DrawObject/ClObjectFrame.h"
#include "ClButtonContextMenu.h"

//--------------------------------------------------------------------------------//
/*!
	@brief	コンテキストメニュー表示ボタンを持った枠
*/
//--------------------------------------------------------------------------------//
class ClObjectFrameHasContextMenu : public QObject, public ClObjectFrame
{
	Q_OBJECT

public:
	ClObjectFrameHasContextMenu(AbstDrawObject *pParent);	//コンストラクタ

protected:
	virtual void adjustKnobs();	//つまみ位置調整

protected slots:
	void slotShowContextMenu();	//コンテキストメニュー表示

protected:
	ClButtonContextMenu *m_pButtonShowContextMenu;	//!< コンテキストメニュー表示ボタン
};
//--------------------------------------------------------------------------------//
#endif //CLOBJECTFRAMEHASCONTEXTMENU_H
