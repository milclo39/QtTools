//--------------------------------------------------------------------------------//
/*!
	@file	AvCommon.h
	@brief	libAvライブラリ共通クラスヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef AVCOMMON_H
#define AVCOMMON_H
#include <QImage>

struct AVFrame;

//--------------------------------------------------------------------------------//
/*!
	@brief	画像情報
*/
//--------------------------------------------------------------------------------//
class ClAvImage
{
public:
	ClAvImage();	//コンストラクタ
	ClAvImage(AVFrame *pFrame, qreal dDuration);	//コンストラクタ

	QImage getImage(){return m_img;}	//!< 画像取得
	qreal getDuration(){return m_dDuration;}	//!< 表示時間取得

private:
	QImage m_img;	//!< 画像
	qreal m_dDuration;	//!< 画像表示時間(msec)
};
//--------------------------------------------------------------------------------//
Q_DECLARE_METATYPE(ClAvImage)
#endif
