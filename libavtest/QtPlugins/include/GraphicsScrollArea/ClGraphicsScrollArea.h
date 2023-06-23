//--------------------------------------------------------------------------------//
/*!
	@file	ClGraphicsScrollArea.h
	@brief	スクロールエリアヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLGRAPHICSSCROLLAREA_H
#define CLGRAPHICSSCROLLAREA_H
#include "GraphicsSlider/ClGraphicsSlider.h"
#include <QGraphicsWidget>

//--------------------------------------------------------------------------------//
/*!
	@brief	スクロールエリア
*/
//--------------------------------------------------------------------------------//
class ClGraphicsScrollArea : public QGraphicsWidget
{
	Q_OBJECT
	Q_PROPERTY(QPointF pos READ pos WRITE setPos)

public:
	ClGraphicsScrollArea(QGraphicsItem *pclParent = NULL);	//コンストラクタ

	void setScrollItem(QGraphicsItem *pclItem);	//スクロールさせるアイテム設定
	/*!
		@brief	スクロールさせるアイテム取得
		@retval スクロールさせるアイテム、設定されていなければNULL
	*/
	QGraphicsItem *getScrollItem(){return m_pclScrollItem;}
	QGraphicsItem *takeScrollItem();	//スクロールさせるアイテムをはずして取得

	void setCornerItem(QGraphicsRectItem *pclItem);	//角アイテム設定

	void setShowScrollBar(bool bIsShowVertical, bool bIsShowHorizontal);	//スクロールバー表示設定

	void checkShowScrollBar();	//スクロールバー表示確認

	/*!
		@brief	縦スクロールバー取得
		@retval	縦スクロールバー
	*/
	ClGraphicsSlider *getVerticalScrollBar(){return m_pclVerticalScrollBar;}
	/*!
		@brief	横スクロールバー取得
		@retval	横スクロールバー
	*/
	ClGraphicsSlider *getHorizontalScrollBar(){return m_pclHorizontalScrollBar;}

protected:
	virtual void resizeEvent(QGraphicsSceneResizeEvent *pclEvent);	//サイズ変更イベント

protected slots:
	void slotBarValueChanged(int iValue);	//スクロールバーの値変更通知

protected:
	QGraphicsItem *m_pclScrollItem;	//!< スクロールさせるアイテム
	ClGraphicsSlider *m_pclVerticalScrollBar;	//!< 縦スクロールバー
	ClGraphicsSlider *m_pclHorizontalScrollBar;	//!< 横スクロールバー
	QGraphicsRectItem *m_pclCornerItem;	//!< 角アイテム
	bool m_bIsShowVerticalBar;	//!< 縦バー表示フラグ
	bool m_bIsShowHorizontalBar;	//!< 横バー表示フラグ
};
//--------------------------------------------------------------------------------//
#endif
