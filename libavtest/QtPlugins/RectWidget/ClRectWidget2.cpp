//--------------------------------------------------------------------------------//
/*!
	@file	ClRectWidget2.cpp
	@brief	RectWidget初期化変更版
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#include "RectWidget/ClRectWidget.h"

//--------------------------------------------------------------------------------//
/*!
	@brief		コンストラクタ
	@param[in]	pParent :　親
*/
//--------------------------------------------------------------------------------//
ClRectWidget2::ClRectWidget2(QWidget *pParent) : ClRectWidget(pParent)
{
	setWindowFlags(static_cast<Qt::WindowFlags>(windowFlags() & (~Qt::WindowSystemMenuHint)));	//システムメニュー出さない
	setAttribute(Qt::WA_NoSystemBackground);	//背景描画しない
}
//--------------------------------------------------------------------------------//
