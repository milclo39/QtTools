//--------------------------------------------------------------------------------//
/*!
	@file	ClButtonContextMenu.h
	@brief	コンテキストメニュー表示ボタンヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLBUTTONCONTEXTMENU_H
#define CLBUTTONCONTEXTMENU_H
#include <QGraphicsItem>
#include "GraphicsButtonBase/ClGraphicsButtonBase.h"

//--------------------------------------------------------------------------------//
/*!
	@brief	コンテキストメニュー表示ボタン
*/
//--------------------------------------------------------------------------------//
class ClButtonContextMenu : public ClGraphicsButtonBase
{
	Q_OBJECT

public:
	ClButtonContextMenu(QGraphicsItem *pParent);	//コンストラクタ

	virtual bool initialize();	//初期化
	virtual void setIcon(QIcon icon, QSize sizeIcon);	//アイコン設定

protected:
	QPixmap createPixmap(QColor colorBase);	//画像生成
	QPainterPath createTrianglePath(qreal dR);	//下向き正三角形パス生成
};
//--------------------------------------------------------------------------------//
#endif //CLBUTTONCONTEXTMENU_H
