//--------------------------------------------------------------------------------//
/*!
	@file	ClStampObjectCreator.h
	@brief	スタンプオブジェクトプラグインヘッダ
	@author 大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLSTAMPOBJECTCREATOR_H
#define CLSTAMPOBJECTCREATOR_H
#include "IDrawObjectCreator.h"
#include "DrawObjectDefine.h"

//--------------------------------------------------------------------------------//
/*!
	@brief	スタンプオブジェクト生成プラグイン
*/
//--------------------------------------------------------------------------------//
class ClStampObjectCreator : public IDrawObjectCreator
{
public:
	/*!
		@brief	コンストラクタ
	*/
	ClStampObjectCreator(){}
	/*!
		@brief	オブジェクト名取得
		@retval STAMP_OBJECT_NAME
	*/
	virtual QString getObjectName() const{return STAMP_OBJECT_NAME;}
	/*!
		@brief	オブジェクトで設定可能なパラメータ取得
		@retval	STAMP_PARAMS
	*/
	virtual qint32 getSupportParams() const{return STAMP_PARAMS;}
	virtual AbstDrawObject *createObject(QPointF clStartPoint,
										 QMap<qint32, QVariant> clParams);	//オブジェクト生成
	virtual AbstDrawObject *copyObject(AbstDrawObject *pObjSource);	//オブジェクトコピー
};
//--------------------------------------------------------------------------------//
#endif //CLSTAMPOBJECTCREATOR_H
