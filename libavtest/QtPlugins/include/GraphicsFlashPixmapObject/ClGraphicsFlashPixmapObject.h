/*--------------------------------------------------------------------------------*/
/*!
	@file	ClGraphicsFlashPixmapObject.h
	@note	フェードエフェクトをかけながら点滅する。ことも出来る。
*/
/*--------------------------------------------------------------------------------*/
#ifndef CLGRAPHICSFLASHPIXMAPOBJECT_H
#define CLGRAPHICSFLASHPIXMAPOBJECT_H

#include <QtGui>
#include <QGraphicsPixmapItem>//20140723
#include <QGraphicsOpacityEffect>




/*================================================================================*/
/*--------------------------------------------------------------------------------*/
/*!
	@brief	点滅機能付きPixmapItem
	@note	フェードエフェクトをかけながら点滅する。ことも出来る。
*/
/*--------------------------------------------------------------------------------*/
class ClGraphicsFlashPixmapObject : public QObject, public QGraphicsPixmapItem
{
	Q_OBJECT


	/*----------------------------------------*/
	/*
		メンバー変数
	*/
	/*----------------------------------------*/
private:
	QGraphicsOpacityEffect*	m_pclEffect;			//!< このクラスを透過させるエフェクト
	QStateMachine*			m_pclStateMachine;		//!< ステートマシン。

	QPropertyAnimation*		m_pclAnimationFadeIn;	//!< フェードイン時のアニメーション。Duration設定のため保持する
	QPauseAnimation*		m_pclAnimationDisp;		//!< フェードインが完了して表示している間のアニメーション
	QPropertyAnimation*		m_pclAnimationFadeOut;	//!< フェードアウト時のアニメーション
	QPauseAnimation*		m_pclAnimationHide;		//!< フェードアウトが完了して消えている間のアニメーション


	/*----------------------------------------*/
	/*
		メンバー関数
	*/
	/*----------------------------------------*/
public:
	ClGraphicsFlashPixmapObject(QGraphicsItem* in_pclParent = NULL);	//コンストラクタ
	virtual ~ClGraphicsFlashPixmapObject();								//デストラクタ

	void	setFlash(bool in_bIsFlash);									//点滅状態の切り替え
	bool	isFlash();													//点滅してるかどうかを返す

	void	setDurationFadeIn(long in_lDuration);						//フェードインに要させる時間を設定する。単位は以後全部ms
	void	setDurationDisp(long in_lDuration);							//フェードインが完了して完全体を表示させる時間を設定する
	void	setDurationFadeOut(long in_lDuration);						//フェードアウトに要させる時間を設定する
	void	setDurationHide(long in_lDuration);							//フェードアウトが完了して完全非表示な時間を設定する

	/*----------------------------------------*/
	/*
		slot
	*/
	/*----------------------------------------*/
public slots:
	void	slotStateChange();	//StateMachineの状態が変わったら入ってくる。実際は各Stateのプロパティ変更完了通知だけど。
};
/*================================================================================*/

#endif // CLGRAPHICSFLASHPIXMAPOBJECT_H
