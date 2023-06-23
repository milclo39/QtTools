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
	m_llPts = 0;
	m_llDts = 0;
	m_llDifferentialPts = 0;
	m_bKeyFrame = false;
	m_PicType = EL_FRAME_TYPE_UNKNOWN;
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
	m_llPts = pFrame->pts;
	m_llDts = pFrame->pkt_dts;
	m_bKeyFrame = pFrame->key_frame ? true:false;
	m_PicType = static_cast<EAVFRAMETYPE>(pFrame->pict_type);

	m_llDifferentialPts = *static_cast<int64_t*>(pFrame->opaque);
	delete pFrame->opaque;
}
//--------------------------------------------------------------------------------//
/*!
	@brief		コンストラクタ
	@param[in]	qImage : フレームデータ
	@param[in]	dDuration : 画像表示時間(msec)
*/
//--------------------------------------------------------------------------------//
ClAvImage::ClAvImage(QImage &qImage, qreal dDuration)
{
	m_img = qImage.copy();
	m_dDuration = dDuration;
	m_llPts = 0;
	m_llDts = 0;
	m_llDifferentialPts = 0;
	m_bKeyFrame = true;
	m_PicType = EL_FRAME_TYPE_I;
}
//--------------------------------------------------------------------------------//
//--------------------------------------------------------------------------------//
