//--------------------------------------------------------------------------------//
/*!
	@file	ClDrawUndo.h
	@brief	描画Undoヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLDRAWUNDO_H
#define CLDRAWUNDO_H
#include <QUndoCommand>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include "DrawObject/AbstDrawObject.h"

//--------------------------------------------------------------------------------//
/*!
	@brief	Undo管理
*/
//--------------------------------------------------------------------------------//
class ClDrawUndo : public QUndoStack
{
public:
	ClDrawUndo();	//コンストラクタ
	~ClDrawUndo();	//デストラクタ

	void push(QUndoCommand *pclCommand);				//コマンド追加
	void deleteItem(QGraphicsItem *pclItem);			//アイテム解放
	void deleteItems(QList<QGraphicsItem*> clItems);	//アイテム解放

protected:
	QList<QGraphicsItem*> m_clItemList;	//!< Undo登録されたアイテムリスト
};
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief	追加Undoコマンド
*/
//--------------------------------------------------------------------------------//
class ClAddUndoCommand : public QUndoCommand
{
public:
	ClAddUndoCommand(ClDrawUndo *pStack, QGraphicsScene *pScene, QList<QGraphicsItem*> listItems);		//コンストラクタ
	ClAddUndoCommand(ClDrawUndo *pclStack, QGraphicsScene *pclScene, QGraphicsItem *pclItem);		//コンストラクタ
	~ClAddUndoCommand();	//デストラクタ
	virtual void undo();	//Undo
	virtual void redo();	//Redo

	/*!
		@brief	アイテム取得
		@retval	アイテム
	*/
	QList<QGraphicsItem*> getUndoItem() const{return m_listItems;}

protected:
	ClDrawUndo *m_pclStack;	//!< Undo管理クラス
	QGraphicsScene *m_pclScene;	//!< 対象シーン
	QList<QGraphicsItem*> m_listItems;	//!< 対象アイテムリスト
	QList<QGraphicsItem*> m_listItemsParent;	//!< 対象アイテムの親リスト
};
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief	削除Undoコマンド
*/
//--------------------------------------------------------------------------------//
class ClRemoveUndoCommand : public QUndoCommand
{
public:
	ClRemoveUndoCommand(ClDrawUndo *pclStack, QGraphicsScene *pclScene, QList<QGraphicsItem*> clItemList);	//コンストラクタ
	~ClRemoveUndoCommand();	//デストラクタ
	virtual void undo();	//Undo
	virtual void redo();	//Redo

protected:
	ClDrawUndo *m_pclStack;	//!< Undo管理クラス
	QGraphicsScene *m_pclScene;	//!< 対象シーン
	QList<QGraphicsItem*> m_clItemList;	//!< 対象アイテム
	QList<QGraphicsItem*> m_clItemParentList;	//!< 対象アイテムの親
};
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief	領域Undoコマンド
*/
//--------------------------------------------------------------------------------//
class ClRectUndoCommand : public QUndoCommand
{
public:
	ClRectUndoCommand(ClDrawUndo *pclStack, QGraphicsScene *pclScene, QList<QGraphicsItem*> clItemList);		//コンストラクタ
	~ClRectUndoCommand();	//デストラクタ
	virtual void undo();	//Undo
	virtual void redo();	//Redo

protected:
	ClDrawUndo *m_pclStack;	//!< Undo管理クラス
	QGraphicsScene *m_pclScene;	//!< 対象シーン
	QList<QGraphicsItem*> m_clItemList;	//!< 対象アイテム
	QList<QRectF> m_clItemRectList;	//!< 対象アイテムの領域
	QList<QRectF> m_clItemRectListRedo;	//!< 対象アイテムの領域
};
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief	ZオーダーUndoコマンド
*/
//--------------------------------------------------------------------------------//
class ClZUndoCommand : public QUndoCommand
{
public:
	ClZUndoCommand(ClDrawUndo *pclStack, QGraphicsScene *pclScene, QMap<qreal, QGraphicsItem*> clItemList);		//コンストラクタ
	~ClZUndoCommand();		//デストラクタ
	virtual void undo();	//Undo
	virtual void redo();	//Redo

protected:
	ClDrawUndo *m_pclStack;	//!< Undo管理クラス
	QGraphicsScene *m_pclScene;	//!< 対象シーン

	QList<QGraphicsItem*> m_clItemList;	//!< 対象アイテム
	QList<qreal> m_clItemZList;	//!< 対象アイテムのZオーダー
	QList<qreal> m_clItemZListRedo;	//!< 対象アイテムのZオーダー
};
//--------------------------------------------------------------------------------//
#endif
