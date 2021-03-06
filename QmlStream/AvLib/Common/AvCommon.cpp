//--------------------------------------------------------------------------------//
/*!
	@file	AvCommon.cpp
	@brief	libAvライブラリ共通クラス
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#include "../AvCommon.h"

#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
extern "C"{
#include <libavcodec/avcodec.h>
}

//--------------------------------------------------------------------------------//
/*!
	@brief		コンストラクタ
*/
//--------------------------------------------------------------------------------//
ClAvImage::ClAvImage()
{
	m_dDuration = 0;
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		コンストラクタ
	@param[in]	pFrame : フレームデータ
	@param[in]	dDuration : 画像表示時間(msec)
*/
//--------------------------------------------------------------------------------//
ClAvImage::ClAvImage(AVFrame *pFrame, qreal dDuration)
{
	m_img = QImage(pFrame->data[0], pFrame->width, pFrame->height, QImage::Format_RGB888).copy();
	m_dDuration = dDuration;
}
//--------------------------------------------------------------------------------//
