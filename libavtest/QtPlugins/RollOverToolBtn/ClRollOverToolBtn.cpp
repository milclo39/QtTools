//***************************************************************************//
//***************************************************************************//
/*!
	@file		ClRollOverToolBtn.cpp

	@brief		ClRollOverToolBtnの実装

	@author		株式会社エルモ社  映像技術第二部

	Confidential

	Copyright(c) Elmo Industrial Co.,Ltd.		http://www.elmo.co.jp/
*/
//***************************************************************************//
//***************************************************************************//
#include	"RollOverToolBtn/ClRollOverToolBtn.h"

#include	<QBitmap>
#include	<QPainter>
#include	<QPaintEvent>


//---------------------------------------------------------------------------//
/*!
	@fn			ClRollOverToolBtn( QWidget* )
	@brief		引数つきコンストラクタ

	@param		a_pclParent	:	親ウィジェット
*/
//---------------------------------------------------------------------------//
ClRollOverToolBtn::ClRollOverToolBtn( QWidget  *a_pclParent )
				:	QToolButton( a_pclParent ),
					m_clShowIcon()
{
	//-------------------------------------------
	//	マウスカーソルが乗ったり降りたりした時に描画イベントを発行
	//-------------------------------------------
	setAttribute( Qt::WA_Hover );

	this->m_eState = QIcon::On;	//デフォルトではON状態の絵を描画する
	this->m_dOpacity = 1.0;		//デフォルト透過度を非透過に設定

	return;
}

//---------------------------------------------------------------------------//
/*!
	@fn			~ClRollOverToolBtn( void )
	@brief		デストラクタ
*/
//---------------------------------------------------------------------------//
ClRollOverToolBtn::~ClRollOverToolBtn( void )
{
	return;
}

//---------------------------------------------------------------------------//
/*!
	@fn			paintEvent( QPaintEvent* )
	@brief		描画処理

	@param		a_pclEvent	:	イベントクラス
*/
//---------------------------------------------------------------------------//
void
ClRollOverToolBtn::paintEvent( QPaintEvent  *a_pclEvent )
{
	//-------------------------------------------
	//	描画する画像を選ぶ
	//-------------------------------------------
	QIcon::Mode		eMode = QIcon::Normal;
	QIcon::State	eState = this->state();

	//	使用可能状態
	if( isEnabled() )
	{
		//	押下状態 or チェック状態
		if( isDown()  ||  isChecked() )
		{
			eMode = QIcon::Selected;
		}
		//	ウィンドウ上にマウスがある or フォーカスを持っている
		else if( underMouse()  ||  hasFocus() )
		{
			eMode = QIcon::Active;
		}
	}
	//	使用不可能状態
	else
	{
		eMode = QIcon::Disabled;
	}

	//-------------------------------------------
	//	描画
	//-------------------------------------------
	QPixmap  clDrawPixmap = m_clShowIcon.pixmap( size(), eMode, eState );	//	描画用画像

	//	描画用画像がある
	if( !clDrawPixmap.isNull() )
	{
		/*----------------------------------------*/
		/*
			一応サイズチェック。違う場合が有り得る。
		*/
		/*----------------------------------------*/
		if(this->size() != clDrawPixmap.size())																	//QIconさんが指定の大きさにしてくれなかった場合
		{
			clDrawPixmap = clDrawPixmap.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);	//自分で大きくする。オプションで綺麗に拡大させる。
		}

		QPainter  clPainter( this );

		//	マスクを設定し、ボタンとしての領域を確定
		setMask( clDrawPixmap.mask() );

		//	クリッピング領域を指定
		clPainter.setClipping( true );
		clPainter.setClipRect( a_pclEvent->rect() );
		clPainter.setOpacity(m_dOpacity);

		//	描画
		clPainter.drawPixmap( 0, 0, clDrawPixmap );
	}
	//	描画用画像がない
	else
	{
		//	マスクをクリア
		clearMask();

		//	デフォルト処理
		this->QToolButton::paintEvent( a_pclEvent );
	}

	//-------------------------------------------
	//	イベント処理済み
	//-------------------------------------------
	a_pclEvent->setAccepted( true );

	return;
}

