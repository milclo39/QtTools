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
#include <QMetaType>

struct AVFrame;

//--------------------------------------------------------------------------------//
/*!
	@brief	画像情報
*/
//--------------------------------------------------------------------------------//
class ClAvImage
{
public:
	enum EAVFRAMETYPE {
		EL_FRAME_TYPE_UNKNOWN = 0,	///< unknown
		EL_FRAME_TYPE_I = 1,		///< Intra
		EL_FRAME_TYPE_P,			///< Predicted
		EL_FRAME_TYPE_B,			///< Bi-dir predicted
		EL_FRAME_TYPE_S,			///< S(GMC)-VOP MPEG4
		EL_FRAME_TYPE_SI,			///< Switching Intra
		EL_FRAME_TYPE_SP,			///< Switching Predicted
		EL_FRAME_TYPE_BI,			///< BI type
	};

	ClAvImage();	//コンストラクタ
	ClAvImage(AVFrame *pFrame, qreal dDuration);	//コンストラクタ
	ClAvImage(QImage &qImage, qreal dDuration);		//コンストラクタ

	QImage getImage(){return m_img;}			//!< 画像取得
	qreal getDuration(){return m_dDuration;}	//!< 表示時間取得
	qint64 getPts(){ return m_llPts; }
	qint64 getDts(){ return m_llDts; }
	qint64 getDiffPts(){ return m_llDifferentialPts; }
	bool getKeyFrame(){ return m_bKeyFrame; }

	void setPts(qint64 pts){ m_llPts = pts; }
	void setDts(qint64 dts){ m_llDts = dts; }
	void setDiffPts(qint64 diffpts){ m_llDifferentialPts = diffpts; }

private:
	QImage m_img;					//!< 画像
	qreal m_dDuration;				//!< 画像表示時間(msec)
	qint64 m_llPts;					//!< 表示用タイムスタンプ
	qint64 m_llDts;					//!< デコードタイムスタンプ
	qint64 m_llDifferentialPts;		//!< 次のPTSとの差
	bool m_bKeyFrame;				//!< キーフレーム
	EAVFRAMETYPE m_PicType;			//!< フレームタイプ

};
//--------------------------------------------------------------------------------//
Q_DECLARE_METATYPE(ClAvImage)
#endif
