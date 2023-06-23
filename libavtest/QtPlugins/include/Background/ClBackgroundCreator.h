/*--------------------------------------------------------------------------------*/
/*!
	@file	ClBackgroundCreator.h
	@brief	デフォルト背景を取りまとめる親玉クラス
*/
/*--------------------------------------------------------------------------------*/
#ifndef CLBACKGROUNDCREATOR_H
#define CLBACKGROUNDCREATOR_H

#include "IBackground.h"




/*================================================================================*/
/*--------------------------------------------------------------------------------*/
/*!
	@class	ClBackgroundCreator
	@brief	デフォルト背景を取りまとめる親玉クラス。Singleton。
*/
/*--------------------------------------------------------------------------------*/
class ClBackgroundCreator
{
	/*----------------------------------------*/
	/*
		メンバー変数
	*/
	/*----------------------------------------*/
private:
	bool				m_bIsInitialized;	//!< 初期化済みかどうか
	QList<IBackground*>	m_clListBackground;	//!< 背景描画クラスのリスト。初期化時に構築される
	QSize				m_clSizeScreen;		//!< 使用するモニタサイズ

	/*----------------------------------------*/
	/*
		メンバー関数
	*/
	/*----------------------------------------*/
private:
	ClBackgroundCreator();								//外部から呼ばれないコンストラクタ
public:
	virtual ~ClBackgroundCreator();						//デストラクタ
	static ClBackgroundCreator*	getInstance();			//実体を返す

	bool	isDefaultBackground(QString in_qstrName);	//与えられた名前がデフォルト背景であるかどうかを返す
	QImage	getDefaultBackground(QString in_qstrName, QSize in_clSize = QSize(), QList<QVariant> in_clOptions = QList<QVariant>(), bool in_bIsKeepParam = true);	//デフォルト背景を返す

	QPoint	getNearestCrossPoint(QString in_qstrName, QPoint in_clPoint);	//直近交点を返す
	void	setSizeScreen(QSize clSize);				//使用スクリーンのサイズ設定

private:
	bool	init();										//初期化処理。1回しか走らないといいなあ。
};
/*================================================================================*/

#endif // CLBACKGROUNDCREATOR_H
