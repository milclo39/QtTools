//--------------------------------------------------------------------------------//
/*!
	@file	ClFreelineObjectCreator.h
	@brief	自由線オブジェクトプラグインヘッダ
	@author 大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLFREELINEOBJECTCREATOR_H
#define CLFREELINEOBJECTCREATOR_H
#include "IDrawObjectCreator.h"
#include "DrawObjectDefine.h"

//--------------------------------------------------------------------------------//
/*!
	@ingroup	groupDrawObjectPlugin
	@defgroup	groupFreelineObjectPlugin
	@brief		自由線オブジェクト
	このモジュールは、マウス入力に合わせて自由に線をかけるオブジェクト、そのオブジェクトを生成するプラグイン、<br>
	描画ウィジェットに自由線描画を設定できるボタンからできている。<br>
*/
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		自由線オブジェクト生成プラグイン
	自由線オブジェクトの生成と、オブジェクトのパラメータの状態に合わせたカーソルの生成を行う。
*/
//--------------------------------------------------------------------------------//
class ClFreelineObjectCreator : public IDrawObjectCreator
{
public:
	/*!
		@brief	コンストラクタ
	*/
	ClFreelineObjectCreator(){}
	/*!
		@brief	オブジェクト名取得
		@retval MY_OBJECT_NAME
	*/
	virtual QString getObjectName() const{return FREELINE_OBJECT_NAME;}
	/*!
		@brief	オブジェクトで設定可能なパラメータ取得
		@retval	FREELINE_PARAMS
	*/
	virtual qint32 getSupportParams() const{return FREELINE_PARAMS;}
	virtual AbstDrawObject *createObject(QPointF clStartPoint,
										 QMap<qint32, QVariant> clParams);	//オブジェクト生成
	virtual AbstDrawObject *copyObject(AbstDrawObject *pObjSource);	//オブジェクトコピー
};
//--------------------------------------------------------------------------------//
#endif //CLFREELINEOBJECTCREATOR_H
