//--------------------------------------------------------------------------------//
/*!
    @file	ClFillEllipseObjectCreator.h
    @brief	塗り楕円オブジェクトプラグインヘッダ
    @author 塚本
*/
//--------------------------------------------------------------------------------//
#ifndef CLFILLELLIPSEOBJECTCREATOR_H
#define CLFILLELLIPSEOBJECTCREATOR_H
#include "IDrawObjectCreator.h"
#include "DrawObjectDefine.h"

//--------------------------------------------------------------------------------//
/*!
	@brief	楕円オブジェクト生成プラグイン
*/
//--------------------------------------------------------------------------------//
class ClFillEllipseObjectCreator : public IDrawObjectCreator
{
public:
	/*!
		@brief	コンストラクタ
	*/
    ClFillEllipseObjectCreator(){}
	/*!
		@brief	オブジェクト名取得
        @retval FILL_ELLIPSE_OBJECT_NAME
	*/
    virtual QString getObjectName() const{return FILL_ELLIPSE_OBJECT_NAME;}
	/*!
		@brief	オブジェクトで設定可能なパラメータ取得
        @retval	FILL_ELLIPSE_PARAMS
	*/
    virtual qint32 getSupportParams() const{return FILL_ELLIPSE_PARAMS;}
	virtual AbstDrawObject *createObject(QPointF clStartPoint,
										 QMap<qint32, QVariant> clParams);	//オブジェクト生成
	virtual AbstDrawObject *copyObject(AbstDrawObject *pObjSource);	//オブジェクトコピー
};
//--------------------------------------------------------------------------------//
#endif //CLFILLELLIPSEOBJECTCREATOR_H
