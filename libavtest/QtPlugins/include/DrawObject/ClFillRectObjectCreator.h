//--------------------------------------------------------------------------------//
/*!
    @file	ClFillRectObjectCreator.h
    @brief	塗り四角オブジェクトプラグインヘッダ
    @author 塚本
*/
//--------------------------------------------------------------------------------//
#ifndef CLFILLRECTOBJECTCREATOR_H
#define CLFILLRECTOBJECTCREATOR_H
#include "IDrawObjectCreator.h"
#include "DrawObjectDefine.h"

//--------------------------------------------------------------------------------//
/*!
    @brief	塗り四角オブジェクト生成プラグイン
*/
//--------------------------------------------------------------------------------//
class ClFillRectObjectCreator : public IDrawObjectCreator
{
public:
	/*!
		@brief	コンストラクタ
	*/
    ClFillRectObjectCreator(){}
	/*!
		@brief	オブジェクト名取得
        @retval FILL_RECT_OBJECT_NAME
	*/
    virtual QString getObjectName() const{return FILL_RECT_OBJECT_NAME;}
	/*!
		@brief	オブジェクトで設定可能なパラメータ取得
        @retval	FILL_RECT_PARAMS
	*/
    virtual qint32 getSupportParams() const{return FILL_RECT_PARAMS;}
	virtual AbstDrawObject *createObject(QPointF clStartPoint,
										 QMap<qint32, QVariant> clParams);	//オブジェクト生成
	virtual AbstDrawObject *copyObject(AbstDrawObject *pObjSource);	//オブジェクトコピー
};
//--------------------------------------------------------------------------------//
#endif //CLFILLRECTOBJECTCREATOR_H
