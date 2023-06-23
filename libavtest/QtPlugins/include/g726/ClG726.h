//--------------------------------------------------------------------------------//
/*!
	@file	ClG726.h
	@brief	G.726エンコーダ・デコーダヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLG726_H
#define CLG726_H
#include <QByteArray>

//--------------------------------------------------------------------------------//
/*!
	@brief	G.726エンコーダ・デコーダ
*/
//--------------------------------------------------------------------------------//
class ClG726
{
public:
	ClG726();	//コンストラクタ
	~ClG726();	//デストラクタ

	bool initDecoder(qint8 cRate);	//デコーダ初期化
	bool initEncoder(qint8 cRate);	//エンコーダ初期化

	QByteArray decode(QByteArray clData);	//デコード
	QByteArray encode(QByteArray clData);	//エンコード

private:
	quint8 *m_pucBufferDecode;	//!< デコード用バッファ
	qint8 m_cRateDecode;	//!< デコード元バイト数
	quint8 *m_pucBufferEncode;	//!< エンコード用バッファ
	qint8 m_cRateEncode;	//!< エンコード元バイト数
};
//--------------------------------------------------------------------------------//
#endif //CLG726_H
