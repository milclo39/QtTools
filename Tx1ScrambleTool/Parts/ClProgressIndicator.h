//--------------------------------------------------------------------------------//
/*!
	@file	ClProgressIndicator.h
	@brief	ClProgressIndicator
	@author	星島
*/
//--------------------------------------------------------------------------------//
#ifndef CLPROGRESSINDICATOR_H
#define CLPROGRESSINDICATOR_H

#include <QWidget>
#include <QVariantAnimation>
#include <QLabel>

// -------------------------------------------------------------------------------- //
/*!
	@class	ClLogAnimation
	@brief	ログアニメーション
*/
// -------------------------------------------------------------------------------- //
class ClIndicatorAnimation : public QVariantAnimation
{
	Q_OBJECT

public:
	/*!
		@brief		コンストラクタ
		@param[in]	pclParent : 親
	*/
	explicit ClIndicatorAnimation( QPixmap clPix, QObject* pclParent = NULL ) : QVariantAnimation(pclParent), m_clIndicatePix( clPix ){}

signals:
	/*!
		@brief		更新通知
		@param[in]	clPix : 現在の画像
	*/
	void sigUpdate( QPixmap clPix );

protected:

	void updateCurrentValue( const QVariant& clValue );					// 更新

protected:

	QPixmap	m_clIndicatePix;	// インジケータ画像
};
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief	プログレスインジケータ
*/
//--------------------------------------------------------------------------------//
class ClProgressIndicator : public QWidget
{
    Q_OBJECT
public:
	explicit ClProgressIndicator( QString strText, QWidget *parent = 0);

	/*!
		@brief		表示非表示
		@param[in]	bIsHide : ture(非表示),false(表示)
	*/
	void setHide(bool bIsHide);

	/*!
		@brief		時間指定の表示非表示
		@param[in]	ulInterval : msec
	*/
	void setInterval(int ulInterval);

protected slots:

	void slotAnimUpdate( QPixmap clNewPixmap );

protected:

	QLabel* m_pclLblAnim;	//!< アニメーション用ラベル
	ClIndicatorAnimation *m_pclLogAnimation;
};

#endif // CLPROGRESSINDICATOR_H
