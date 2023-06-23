//--------------------------------------------------------------------------------//
/*!
	@file	ClStraightlineObjectCreator.h
	@brief	直線オブジェクトプラグインヘッダ
	@author 大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLSTRAIGHTLINEOBJECTCREATOR_H
#define CLSTRAIGHTLINEOBJECTCREATOR_H
#include "IDrawObjectCreator.h"
#include "DrawObjectDefine.h"

//--------------------------------------------------------------------------------//
/*!
	@brief	直線オブジェクト生成プラグイン
*/
//--------------------------------------------------------------------------------//
class ClStraightlineObjectCreator : public IDrawObjectCreator
{
public:
	/*!
		@brief	コンストラクタ
	*/
	ClStraightlineObjectCreator(){}
	/*!
		@brief	オブジェクト名取得
		@retval STRAIGHTLINE_OBJECT_NAME
	*/
	virtual QString getObjectName() const{return STRAIGHTLINE_OBJECT_NAME;}
	/*!
		@brief	オブジェクトで設定可能なパラメータ取得
		@retval	STRAIGHTLINE_PARAMS
	*/
	virtual qint32 getSupportParams() const{return STRAIGHTLINE_PARAMS;}
	virtual AbstDrawObject *createObject(QPointF clStartPoint,
										 QMap<qint32, QVariant> clParams);	//オブジェクト生成
	virtual AbstDrawObject *copyObject(AbstDrawObject *pObjSource);	//オブジェクトコピー
};
//--------------------------------------------------------------------------------//
#endif //CLSTRAIGHTLINEOBJECTCREATOR_H
