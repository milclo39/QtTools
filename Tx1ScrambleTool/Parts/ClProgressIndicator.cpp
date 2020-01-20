//--------------------------------------------------------------------------------//
/*!
	@file	ClProgressIndicator.cpp
	@brief	ClProgressIndicator
	@author	星島
*/
//--------------------------------------------------------------------------------//
#include "ClProgressIndicator.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>

#define PATH_ANIM_IMG	( ":/indicator/indicator" )

// -------------------------------------------------------------------------------- //
/*!
	@brief		更新
	@param[in]	clValue	: 更新値
*/
// -------------------------------------------------------------------------------- //
void
ClIndicatorAnimation::updateCurrentValue( const QVariant& clValue )
{
	emit sigUpdate( m_clIndicatePix.transformed( QTransform().rotate( qvariant_cast< qint32 >( clValue ) )));
}
//--------------------------------------------------------------------------------//



//--------------------------------------------------------------------------------//
/*!
	@brief		コンストラクタ
	@param[in]	pclParent : 親
*/
//--------------------------------------------------------------------------------//
ClProgressIndicator::ClProgressIndicator( QString strText, QWidget *parent)
  : QWidget(parent)
  , m_pclLblAnim( new QLabel() )
{

	QHBoxLayout* pclMainLayout = new QHBoxLayout(this);

	QLabel* pclLblText = new QLabel(strText, this);

	m_pclLblAnim->setPixmap(QPixmap(PATH_ANIM_IMG));

	pclMainLayout->addWidget( pclLblText );
	pclMainLayout->addWidget(m_pclLblAnim);

	m_pclLblAnim->setAlignment( Qt::AlignCenter );



	// animation設定

	m_pclLogAnimation = new ClIndicatorAnimation( QPixmap(PATH_ANIM_IMG),this );

	m_pclLogAnimation->setDuration( 1000 );					// アニメーション全体の時間

	m_pclLogAnimation->setStartValue(0);						// 0度からスタート
	m_pclLogAnimation->setEndValue(359);						// 359度までのアニメーション

	connect(m_pclLogAnimation, SIGNAL(sigUpdate(QPixmap)),
			this, SLOT(slotAnimUpdate(QPixmap)));			// 画像更新シグナル接続

}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		画像更新
	@param[in]	clNewPixmap : 新画像
*/
//--------------------------------------------------------------------------------//
void
ClProgressIndicator::slotAnimUpdate( QPixmap clNewPixmap )
{
	m_pclLblAnim->setPixmap(clNewPixmap);	// 画像更新
}
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		表示非表示
	@param[in]	bIsHide : true(非表示),false(表示)
*/
//--------------------------------------------------------------------------------//
void ClProgressIndicator::setHide(bool bIsHide)
{
	if(true == bIsHide)
	{
		m_pclLogAnimation->stop();								//停止
		this->setHidden(bIsHide);
	}
	else
	{
		m_pclLogAnimation->setLoopCount(-1);					// 無限ループ
		m_pclLogAnimation->start();
		this->setHidden(bIsHide);
	}
}
//--------------------------------------------------------------------------------//

//--------------------------------------------------------------------------------//
/*!
	@brief		タイマー式表示非表示
	@param[in]	ulInterval : msec
*/
//--------------------------------------------------------------------------------//
void ClProgressIndicator::setInterval(int ulInterval)
{
	setHide(false);
	QTimer::singleShot(ulInterval, m_pclLogAnimation, SLOT(stop()));
	QTimer::singleShot(ulInterval, this, SLOT(hide()));
}
//--------------------------------------------------------------------------------//
