//--------------------------------------------------------------------------------//
/*!
    @file	ClDashlineObjectCreator.h
    @brief	破線オブジェクトプラグインヘッダ
    @author 塚本
*/
//--------------------------------------------------------------------------------//
#ifndef CLDASHLINEOBJECTCREATOR_H
#define CLDASHLINEOBJECTCREATOR_H
#include "IDrawObjectCreator.h"
#include "DrawObjectDefine.h"

//--------------------------------------------------------------------------------//
/*!
    @brief	破線オブジェクト生成プラグイン
*/
//--------------------------------------------------------------------------------//
class ClDashlineObjectCreator : public IDrawObjectCreator
{
public:
    /*!
        @brief	コンストラクタ
    */
    ClDashlineObjectCreator(){}
    /*!
        @brief	オブジェクト名取得
        @retval DASHLINE_OBJECT_NAME
    */
    virtual QString getObjectName() const{return DASHLINE_OBJECT_NAME;}
    /*!
        @brief	オブジェクトで設定可能なパラメータ取得
        @retval	DASHLINE_PARAMS
    */
    virtual qint32 getSupportParams() const{return DASHLINE_PARAMS;}
    virtual AbstDrawObject *createObject(QPointF clStartPoint,
                                         QMap<qint32, QVariant> clParams);	//オブジェクト生成
    virtual AbstDrawObject *copyObject(AbstDrawObject *pObjSource);	//オブジェクトコピー
};
//--------------------------------------------------------------------------------//
#endif //CLDASHLINEOBJECTCREATOR_H
