//--------------------------------------------------------------------------------//
/*!
	@file	ClImageObjectCreator.h
	@brief	画像オブジェクトプラグインヘッダ
	@author 大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLIMAGEOBJECTCREATOR_H
#define CLIMAGEOBJECTCREATOR_H
#include "IDrawObjectCreator.h"
#include "DrawObjectDefine.h"

//--------------------------------------------------------------------------------//
/*!
	@brief	画像オブジェクト生成プラグイン
*/
//--------------------------------------------------------------------------------//
class ClImageObjectCreator : public IDrawObjectCreator
{
public:
	/*!
		@brief	コンストラクタ
	*/
	ClImageObjectCreator(){}
	/*!
		@brief	オブジェクト名取得
		@retval PICTURE_OBJECT_NAME
	*/
	virtual QString getObjectName() const{return PICTURE_OBJECT_NAME;}
	/*!
		@brief	オブジェクトで設定可能なパラメータ取得
		@retval	PICTURE_PARAMS
	*/
	virtual qint32 getSupportParams() const{return PICTURE_PARAMS;}
	virtual AbstDrawObject *createObject(QPointF clStartPoint,
										 QMap<qint32, QVariant> clParams);	//オブジェクト生成
	virtual AbstDrawObject *copyObject(AbstDrawObject *pObjSource);	//オブジェクトコピー
};
//--------------------------------------------------------------------------------//
#endif //CLRECTOBJECTCREATOR_H
