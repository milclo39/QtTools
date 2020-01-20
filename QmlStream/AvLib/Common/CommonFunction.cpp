//--------------------------------------------------------------------------------//
/*!
	@file	CommonFunction.cpp
	@brief	共通使用関数
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
#include "CommonFunction.h"
extern "C"{
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

//--------------------------------------------------------------------------------//
/*!
	@brief		画像フォーマット変換
	@param[in]	pFrameSource : 変換元フレーム
	@param[inout]	ppFrameDest : 変換後フレーム格納先,スケーリングする場合widthとheightを設定
					変換に成功した場合,av_freeでdata[0]を解放する必要あり
	@param[in]	eFormat : 変換するフォーマット
*/
//--------------------------------------------------------------------------------//
bool convertPixelFormat(AVFrame *pFrameSource,
							AVFrame **ppFrameDest,
							AVPixelFormat eFormat)
{
	if(NULL == pFrameSource ||
			NULL == ppFrameDest)
	{
		return false;
	}

	//出力フレーム準備
	int iDestBufferSize = avpicture_get_size(eFormat, (*ppFrameDest)->width, (*ppFrameDest)->height);
	uint8_t *ucBufferDest = (uint8_t*)av_malloc(iDestBufferSize);
	if(0 >= avpicture_fill((AVPicture*)*ppFrameDest, ucBufferDest, eFormat,
				   (*ppFrameDest)->width, (*ppFrameDest)->height))
	{
		return false;
	}

	//変換準備
	SwsContext *pSws = sws_getContext(pFrameSource->width, pFrameSource->height,
							  (AVPixelFormat)pFrameSource->format, (*ppFrameDest)->width, (*ppFrameDest)->height,
							  eFormat, 0, NULL, NULL, NULL);
	if(NULL == pSws)
	{
		return false;
	}

	//フォーマット変換
	if(0 >= sws_scale(pSws, pFrameSource->data, pFrameSource->linesize, 0, pFrameSource->height,
			  (*ppFrameDest)->data, (*ppFrameDest)->linesize))
	{
		goto ERROR;
	}

	//解放
	sws_freeContext(pSws);

	return true;
ERROR:
	if(NULL != ucBufferDest)
	{
		av_free(ucBufferDest);
	}
	if(NULL != pSws)
	{
		sws_freeContext(pSws);
	}
	return false;
}
//--------------------------------------------------------------------------------//
