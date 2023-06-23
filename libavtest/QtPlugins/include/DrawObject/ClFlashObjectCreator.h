//--------------------------------------------------------------------------------//
/*!
	@file	ClFlashObjectCreator.h
	@brief	自由線オブジェクトプラグインヘッダ
	@author 大橋
*/
//--------------------------------------------------------------------------------//
#include "DrawWidgetTransparent/IDrawObjectCreator.h"
#include "DrawObjectDefine.h"

//--------------------------------------------------------------------------------//
/*!
	@ingroup	groupDrawObjectPlugin
	@defgroup	groupFlashObjectPlugin
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
class ClFlashObjectCreator : public IDrawObjectCreator
{
public:
	/*!
		@brief	コンストラクタ
	*/
	ClFlashObjectCreator(){};
	/*!
		@brief	オブジェクト名取得
		@retval MY_OBJECT_NAME
	*/
	virtual QString getObjectName() const{return FLASH_OBJECT_NAME;}
	/*!
		@brief	オブジェクトで設定可能なパラメータ取得
		@retval	Flash_PARAMS
	*/
	virtual qint32 getSupportParams() const{return FLASH_PARAMS;}
	virtual AbstDrawObject *createObject(QPointF clStartPoint,
										 QMap<qint32, QVariant> clParams);	//オブジェクト生成
};
//--------------------------------------------------------------------------------//
