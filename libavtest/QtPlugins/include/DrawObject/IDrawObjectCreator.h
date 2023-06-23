//--------------------------------------------------------------------------------//
/*!
	@file	IDrawObjectCreator.h
	@brief	描画オブジェクト用プラグインヘッダ
	@author 大橋
*/
//--------------------------------------------------------------------------------//
#ifndef IDRAWOBJECTCREATOR_H
#define IDRAWOBJECTCREATOR_H

//--------------------------------------------------------------------------------//
/*!
	@defgroup	groupDrawObjectPlugin	描画オブジェクト
	@brief		描画ウィジェットで使用する動的プラグイン
	このモジュールには、描画オブジェクトと、描画オブジェクトを生成するためのプラグインが含まれる。<br>
	描画ウィジェットで描画オブジェクトを扱うには、AbstDrawObjectを継承した描画オブジェクトと、<br>
	そのオブジェクトを生成する、IDrawObjectCreatorを継承したプラグインが必要となる。
*/
//--------------------------------------------------------------------------------//
#include "AbstDrawObject.h"
#include <QGraphicsItem>

class AbstDrawObject;

//--------------------------------------------------------------------------------//
/*!
	@ingroup	groupDrawObjectPlugin
	@brief		描画オブジェクトの生成クラス
	このプラグインは、与えられた座標とパラメータをもとに、プラグインが持つ描画オブジェクトを生成する。
*/
//--------------------------------------------------------------------------------//
class IDrawObjectCreator
{
public:
	virtual QString getObjectName() const = 0;						//!< オブジェクト名取得
	virtual qint32 getSupportParams() const = 0;					//!< オブジェクトで設定可能なパラメータ取得
	virtual AbstDrawObject *createObject(QPointF clStartPoint,
										 QMap<qint32, QVariant> clParams) = 0;	//!< オブジェクト生成
	virtual AbstDrawObject *copyObject(AbstDrawObject *pObjSource) = 0;	//!< オブジェクトコピー
	virtual void setContextMenuButtonSize(QSize sizeButton){Q_UNUSED(sizeButton);}	//!< コンテキストメニューボタンサイズ設定
};
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@ingroup	groupDrawObjectPlugin
	@brief		描画オブジェクト生成クラス管理クラス
*/
//--------------------------------------------------------------------------------//
class IDrawObjectCreatorManager : public QObject
{
	Q_OBJECT

public:
	virtual bool init() = 0;	//!< 初期化

signals:
	/*!
		@brief		描画オブジェクト生成クラスリスト通知
		@param[in]	clCreatorList : オブジェクト生成クラスリスト
	*/
	void sigCreatorList(QList<IDrawObjectCreator*> clCreatorList);
};
//--------------------------------------------------------------------------------//
#endif //IDRAWOBJECTCREATOR_H
