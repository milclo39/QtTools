//--------------------------------------------------------------------------------//
/*!
	@file	ClUvcDriverQbicmini.h
	@brief	QBiCMini用UVCドライバヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLUVCDRIVERQBICMINI_H
#define CLUVCDRIVERQBICMINI_H
#include "UvcDriver/ClUvcDriver.h"

//--------------------------------------------------------------------------------//
/*!
	@brief	QBiCMini用UVCドライバヘッダ
*/
//--------------------------------------------------------------------------------//
class ClUvcDriverQbicmini : public ClUvcDriver
{
public:
	ClUvcDriverQbicmini(QObject *pParent = NULL);	//コンストラクタ
	virtual ~ClUvcDriverQbicmini();	//デストラクタ

protected:
	virtual bool initialize();	//初期化

	virtual GUID getExtensionNodeGuid(quint32);	//拡張ノードGUID取得

private:
	quint32 m_ulExtensionNodeNumber;	//!< 拡張ノード番号
};
//--------------------------------------------------------------------------------//
#endif //CLUVCDRIVERQBICMINI_H
