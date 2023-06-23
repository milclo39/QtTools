//--------------------------------------------------------------------------------//
/*!
	@file	ClRectObjectCreator.h
	@brief	四角オブジェクトプラグインヘッダ
	@author 大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLRECTOBJECTCREATOR_H
#define CLRECTOBJECTCREATOR_H
#include "IDrawObjectCreator.h"
#include "DrawObjectDefine.h"

//--------------------------------------------------------------------------------//
/*!
	@brief	四角オブジェクト生成プラグイン
*/
//--------------------------------------------------------------------------------//
class ClRectObjectCreator : public IDrawObjectCreator
{
public:
	/*!
		@brief	コンストラクタ
	*/
	ClRectObjectCreator(){}
	/*!
		@brief	オブジェクト名取得
		@retval RECT_OBJECT_NAME
	*/
	virtual QString getObjectName() const{return RECT_OBJECT_NAME;}
	/*!
		@brief	オブジェクトで設定可能なパラメータ取得
		@retval	RECT_PARAMS
	*/
	virtual qint32 getSupportParams() const{return RECT_PARAMS;}
	virtual AbstDrawObject *createObject(QPointF clStartPoint,
										 QMap<qint32, QVariant> clParams);	//オブジェクト生成
	virtual AbstDrawObject *copyObject(AbstDrawObject *pObjSource);	//オブジェクトコピー
};
//--------------------------------------------------------------------------------//
#endif //CLRECTOBJECTCREATOR_H
