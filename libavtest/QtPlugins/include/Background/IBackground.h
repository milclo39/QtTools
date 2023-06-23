/*--------------------------------------------------------------------------------*/
/*!
	@file	IBackground.h
	@brief	デフォルト背景のインターフェイスクラス
*/
/*--------------------------------------------------------------------------------*/
#ifndef IBACKGROUND_H
#define IBACKGROUND_H

#include <QString>
#include <QImage>
#include <QList>
#include <QVariant>




/*================================================================================*/
/*--------------------------------------------------------------------------------*/
/*!
	@class	IBackground
	@brief	デフォルト背景のインターフェイスクラス
*/
/*--------------------------------------------------------------------------------*/
class IBackground
{
public:
	IBackground(){}					//コンストラクタ
	virtual ~IBackground(){}		//デストラクタ

	virtual QString	getName() = 0;	//識別子となる名前を返す
	virtual QImage	getImage(QSize in_clSize, QList<QVariant> in_clOptions, bool in_bIsKeepParam) = 0;	//背景画像を返す
};
/*================================================================================*/

#endif // IBACKGROUND_H
