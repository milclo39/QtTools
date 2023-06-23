//--------------------------------------------------------------------------------//
/*!
	@file	CommonFunction.h
	@brief	共通使用関数ヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef AV_COMMONFUNCTION_H
#define AV_COMMONFUNCTION_H
extern "C"{
#include <libavcodec/avcodec.h>
}

bool convertPixelFormat(AVFrame *pFrameSource,
							AVFrame *pFrameDest,
							AVPixelFormat eFormat = AV_PIX_FMT_RGB24);	//画像変換
#endif
