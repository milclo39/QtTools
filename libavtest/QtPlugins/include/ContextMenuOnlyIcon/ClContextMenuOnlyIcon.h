//--------------------------------------------------------------------------------//
/*!
	@file	ClContextMenuOnlyIcon.h
	@brief	アイコンのみコンテキストメニューヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLCONTEXTMENUONLYICON_H
#define CLCONTEXTMENUONLYICON_H
#include <QMenu>

//--------------------------------------------------------------------------------//
/*!
	@brief	アイコンのみコンテキストメニュー
*/
//--------------------------------------------------------------------------------//
class ClContextMenuOnlyIcon : public QMenu
{
public:
	ClContextMenuOnlyIcon(QSize sizeIcon);	//コンストラクタ
	ClContextMenuOnlyIcon(bool bIsChangeIcon, QSize sizeIcon);	//コンストラクタ
	virtual ~ClContextMenuOnlyIcon();	//デストラクタ

	void setIconSize(QSize sizeIcon);	//アイコンサイズ設定
	void setHMarginRate(qreal dMarginRate);	//アイコンの横余白のアイコン幅からの割合設定
	void setVMarginRate(qreal dMarginRate);	//アイコンの縦余白のアイコン高からの割合設定

	QAction *addActionIcon(const QIcon &icon);	//メニュー追加

	virtual QSize sizeHint() const;	//サイズヒント
};
//--------------------------------------------------------------------------------//
#endif //CLCONTEXTMENUONLYICON_H
