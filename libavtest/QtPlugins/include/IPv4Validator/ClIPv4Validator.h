//--------------------------------------------------------------------------------//
/*!
	@file	ClIpv4Validator.h
	@brief	IPv4用バリデータヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLIPV4VALIDATOR_H
#define CLIPV4VALIDATOR_H
#include <QValidator>

//--------------------------------------------------------------------------------//
/*!
	@brief	IPv4用バリデータヘッダ
*/
//--------------------------------------------------------------------------------//
class ClIpv4Validator : public QValidator
{
public:
	ClIpv4Validator(QObject *pclParent = NULL);	//コンストラクタ

	virtual State validate(QString &strInput, int &iPos) const;	//有効確認

protected:
	mutable QString m_strBeforeInput;	//!< 以前の文字列
};
//--------------------------------------------------------------------------------//
#endif //CLIPV4VALIDATOR_H
