//--------------------------------------------------------------------------------//
/*!
	@file	ClTextObjectCreator.h
	@brief	テキストオブジェクトプラグインヘッダ
	@author 大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLTEXTOBJECTCREATOR_H
#define CLTEXTOBJECTCREATOR_H
#include "IDrawObjectCreator.h"
#include "DrawObjectDefine.h"

//--------------------------------------------------------------------------------//
/*!
	@brief	楕円オブジェクト生成プラグイン
*/
//--------------------------------------------------------------------------------//
class ClTextObjectCreator : public IDrawObjectCreator
{
public:
	ClTextObjectCreator();	//コンストラクタ
	virtual ~ClTextObjectCreator();	//デストラクタ
	/*!
		@brief	オブジェクト名取得
		@retval TEXT_OBJECT_NAME
	*/
	virtual QString getObjectName() const{return TEXT_OBJECT_NAME;}
	/*!
		@brief	オブジェクトで設定可能なパラメータ取得
		@retval	TEXT_PARAMS
	*/
	virtual qint32 getSupportParams() const{return TEXT_PARAMS;}
	virtual AbstDrawObject *createObject(QPointF clStartPoint,
										 QMap<qint32, QVariant> clParams);	//オブジェクト生成
	virtual AbstDrawObject *copyObject(AbstDrawObject *pObjSource);	//オブジェクトコピー

	virtual void setContextMenuButtonSize(QSize sizeButton);	//コンテキストメニューボタンサイズ設定

protected:
	virtual QMenu *createContextMenu(){return NULL;}	//!< コンテキストメニュー生成

protected:
	QSize m_sizeContextMenuButtonSize;	//!< コンテキストメニュー生成前のボタンサイズ記憶用

private:
	QMenu *m_pContextMenu;	//!< コンテキストメニュー
};
//--------------------------------------------------------------------------------//
#endif //CLTEXTOBJECTCREATOR_H
