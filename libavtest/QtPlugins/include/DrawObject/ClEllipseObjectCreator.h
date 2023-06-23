//--------------------------------------------------------------------------------//
/*!
	@file	ClEllipseObjectCreator.h
	@brief	楕円オブジェクトプラグインヘッダ
	@author 大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLELLIPSEOBJECTCREATOR_H
#define CLELLIPSEOBJECTCREATOR_H
#include "IDrawObjectCreator.h"
#include "DrawObjectDefine.h"

//--------------------------------------------------------------------------------//
/*!
	@brief	楕円オブジェクト生成プラグイン
*/
//--------------------------------------------------------------------------------//
class ClEllipseObjectCreator : public IDrawObjectCreator
{
public:
	/*!
		@brief	コンストラクタ
	*/
	ClEllipseObjectCreator(){}
	/*!
		@brief	オブジェクト名取得
		@retval ELLIPSE_OBJECT_NAME
	*/
	virtual QString getObjectName() const{return ELLIPSE_OBJECT_NAME;}
	/*!
		@brief	オブジェクトで設定可能なパラメータ取得
		@retval	ELLIPSE_PARAMS
	*/
	virtual qint32 getSupportParams() const{return ELLIPSE_PARAMS;}
	virtual AbstDrawObject *createObject(QPointF clStartPoint,
										 QMap<qint32, QVariant> clParams);	//オブジェクト生成
	virtual AbstDrawObject *copyObject(AbstDrawObject *pObjSource);	//オブジェクトコピー
};
//--------------------------------------------------------------------------------//
#endif //CLELLIPSEOBJECTCREATOR_H
