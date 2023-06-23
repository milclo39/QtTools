//***************************************************************************//
//***************************************************************************//
/*!
	@file		ClRectWidget.cpp

	@brief		ClRectWidgetの実装

	@author		株式会社エルモ社  映像技術第二部

	Confidential

	Copyright(c) Elmo Industrial Co.,Ltd.		http://www.elmo.co.jp/
*/
//***************************************************************************//
//***************************************************************************//
#include	"RectWidget/ClRectWidget.h"
#include	"Common/CmnDefine.h"

#include	<QMouseEvent>
#include	<QBitmap>
#include	<QPainter>
#include	<QLayout>
#include	<QApplication>


//	画像がない場合に設定する領域サイズ
#define		DEF_RGN_SIZE		(10)


//---------------------------------------------------------------------------//
/*!
	@fn			ClRectWidget( QWidget* )
	@brief		引数つきコンストラクタ

	@param		a_pclParent	:	親ウィジェット
*/
//---------------------------------------------------------------------------//
ClRectWidget::ClRectWidget( QWidget  *a_pclParent )
				:	QWidget( a_pclParent ),
					m_bEnableDragMove( false ),
					m_bEnableDragSize( false ),
					m_bIsMouseEventApp(false),
					m_eState( eMyDragStateNone ),
					m_clDragStartPos( 0, 0 ),
					m_clChangeSizeMargin( 0, 0 ),
					m_eUpdateRgnDrc( eMyDirectionNone ),
					m_lstBackInfo( eMyDirectionTerminate )
{
	//-------------------------------------------
	//	タイトルバー、ウィンドウ枠をなくす
	//-------------------------------------------
	setWindowFlags( windowFlags() & (~Qt::WindowTitleHint)
					| Qt::FramelessWindowHint );
	setAttribute(Qt::WA_TranslucentBackground);	//背景透過設定
	setAttribute(Qt::WA_NoSystemBackground, false);	//背景描画はするように設定

	//-------------------------------------------
	//	画像なしの状態で背景領域を初期化する
	//-------------------------------------------
	calcBackRgn( static_cast<eMyDirection>( eMyDirectionTopLeft +
											 eMyDirectionBottomRight +
											 eMyDirectionCenter )
				);

	//-------------------------------------------
	//	マスクを設定
	//-------------------------------------------
	setMyMask();

	//-------------------------------------------
	//	ボタンが何も押されていなくてもMouseMoveイベントが
	//	発生するようにする
	//-------------------------------------------
	setMouseTracking( true );

	return;
}

//---------------------------------------------------------------------------//
/*!
	@fn			~ClRectWidget( void )
	@brief		デストラクタ
*/
//---------------------------------------------------------------------------//
ClRectWidget::~ClRectWidget( void )
{
	return;
}

//---------------------------------------------------------------------------//
/*!
	@brief		イベントフィルタ
	@param[in]	pTarget : イベント対象オブジェクト
	@param[in]	pEvent : イベント
	@retval		ここで処理を止める場合true
*/
//---------------------------------------------------------------------------//
bool ClRectWidget::eventFilter(QObject *pTarget, QEvent *pEvent)
{
	Q_UNUSED(pTarget);
	if(true == m_bIsMouseEventApp)
	{
		QMouseEvent *pMouse = dynamic_cast<QMouseEvent*>(pEvent);
		if(NULL != pMouse)
		{
			QWidget *pWidgetTop = qApp->topLevelAt(pMouse->globalPos());	//マウス位置の最前面ウィジェット
			QPoint posRel = pMouse->globalPos() - pos();	//ウィジェットの相対位置に変換
			QMouseEvent eventMouseWiget(pEvent->type(), posRel, pMouse->globalPos(),
										pMouse->button(), pMouse->buttons(), pMouse->modifiers());	//ウィジェットの相対位置に変換したイベント
#if 0	//子ウィジェットのイベントよりも先に回ってくるのでいろいろ処理していいかわからないので移動のみ処理する
			if(QEvent::MouseButtonPress == pEvent->type() &&
					this == pWidgetTop)	//自分の上で押下された
			{
				procMousePress(&eventMouseWiget);	//押下処理
			}
			else if(QEvent::MouseButtonRelease == pEvent->type() &&
					eMyDragStateNone != m_eState)	//ドラッグ中
			{
				procMouseRelease(&eventMouseWiget);	//離した処理
			}
			else
#endif
				if(QEvent::MouseMove == pEvent->type() &&
					(this == pWidgetTop ||	//自分の上にいる
					 eMyDragStateNone != m_eState))	//ドラッグ中
			{
				procMouseMove(&eventMouseWiget);	//移動処理
			}
		}
	}
	return false;	//処理を止めない
}

//---------------------------------------------------------------------------//
/*!
	@fn			setDragMove( const bool& )
	@brief		マウスドラッグ移動ON/OFFを設定

	@param		a_clVal	:	マウスドラッグによる移動を許可するならtrue
*/
//---------------------------------------------------------------------------//
void
ClRectWidget::setDragMove( const bool  &a_bVal )
{
	//-------------------------------------------
	//	設定値とマウスドラッグ処理状態を更新
	//-------------------------------------------
	if( !a_bVal  &&  ( m_eState == eMyDragStateMove ) )
	{
		m_eState = eMyDragStateNone;
	}
	m_bEnableDragMove = a_bVal;

	return;
}

//---------------------------------------------------------------------------//
/*!
	@fn			setDragChangeSize( const bool& )
	@brief		マウスドラッグによるサイズ変更ON/OFFを設定

	@param		a_clVal	:	マウスドラッグによるサイズ変更を許可するならtrue
*/
//---------------------------------------------------------------------------//
void
ClRectWidget::setDragChangeSize( const bool  &a_bVal )
{
	//-------------------------------------------
	//	設定値とをマウスドラッグ処理状態を更新
	//-------------------------------------------
	if( !a_bVal  &&  ( m_eState & eMyDragStateSize ) )
	{
		m_eState = eMyDragStateNone;
	}
	if( a_bVal == false )
	{
		setCursor( QCursor( Qt::ArrowCursor ) );
	}
	m_bEnableDragSize = a_bVal;

	return;
}

//---------------------------------------------------------------------------//
/*!
	@brief		マウスイベント処理をQApplicationのEventFilterで行う設定
	@attention	QApplicationにEventFilter登録、解除をするので注意
*/
//---------------------------------------------------------------------------//
void ClRectWidget::setIsMouseEventApp(const bool &bValue)
{
	if(true == bValue)
	{
		qApp->installEventFilter(this);	//イベントフィルタ登録
	}
	else
	{
		qApp->removeEventFilter(this);	//イベントフィルタ解除
	}
	m_bIsMouseEventApp = bValue;
}

//---------------------------------------------------------------------------//
/*!
	@fn			sizeHint( void )
	@brief		このウィジェットの理想的なサイズを返す
*/
//---------------------------------------------------------------------------//
QSize
ClRectWidget::sizeHint( void ) const
{
	//-------------------------------------------
	//	設定されている画像の原寸大の大きさを足す
	//-------------------------------------------
	QSize  clSize( 0, 0 );		//	この関数の戻り値

	//	幅 = (TopLeft + Top + TopRight) or (BottomLeft + Bottom + BottomRight)
	{
		qint32  iWidthTop = 0;		//	上側の幅の合計

		iWidthTop = ( !m_lstBackInfo[eMyDirectionTopLeft].clSrcImg.isNull() ) ?
					m_lstBackInfo[eMyDirectionTopLeft].clSrcImg.width() :
					DEF_RGN_SIZE;
		iWidthTop += ( !m_lstBackInfo[eMyDirectionTop].clSrcImg.isNull() ) ?
					m_lstBackInfo[eMyDirectionTop].clSrcImg.width() :
					DEF_RGN_SIZE;
		iWidthTop += ( !m_lstBackInfo[eMyDirectionTopRight].clSrcImg.isNull() ) ?
					m_lstBackInfo[eMyDirectionTopRight].clSrcImg.width() :
					DEF_RGN_SIZE;

		qint32  iWidthBottom = 0;	//	下側の幅の合計

		iWidthBottom = ( !m_lstBackInfo[eMyDirectionBottomLeft].clSrcImg.isNull() ) ?
						m_lstBackInfo[eMyDirectionBottomLeft].clSrcImg.width() :
						DEF_RGN_SIZE;
		iWidthBottom += ( !m_lstBackInfo[eMyDirectionBottom].clSrcImg.isNull() ) ?
						m_lstBackInfo[eMyDirectionBottom].clSrcImg.width() :
						DEF_RGN_SIZE;
		iWidthBottom += ( !m_lstBackInfo[eMyDirectionBottomRight].clSrcImg.isNull() ) ?
						m_lstBackInfo[eMyDirectionBottomRight].clSrcImg.width() :
						DEF_RGN_SIZE;

		clSize.setWidth( ( iWidthBottom > iWidthTop ) ? iWidthBottom : iWidthTop );
	}

	//	高さ = (TopLeft + Left + BottomLeft) or (TopRight + Right + BottomRight)
	{
		qint32  iHeightLeft = 0;	//	左側の高さの合計

		iHeightLeft = ( !m_lstBackInfo[eMyDirectionTopLeft].clSrcImg.isNull() ) ?
						m_lstBackInfo[eMyDirectionTopLeft].clSrcImg.height() :
						DEF_RGN_SIZE;
		iHeightLeft += ( !m_lstBackInfo[eMyDirectionLeft].clSrcImg.isNull() ) ?
						m_lstBackInfo[eMyDirectionLeft].clSrcImg.height() :
						DEF_RGN_SIZE;
		iHeightLeft += ( !m_lstBackInfo[eMyDirectionBottomLeft].clSrcImg.isNull() ) ?
						m_lstBackInfo[eMyDirectionBottomLeft].clSrcImg.height() :
						DEF_RGN_SIZE;

		qint32  iHeightRight = 0;	//	右側の高さの合計

		iHeightRight = ( !m_lstBackInfo[eMyDirectionTopRight].clSrcImg.isNull() ) ?
						m_lstBackInfo[eMyDirectionTopRight].clSrcImg.height() :
						DEF_RGN_SIZE;
		iHeightRight += ( !m_lstBackInfo[eMyDirectionTop].clSrcImg.isNull() ) ?
						m_lstBackInfo[eMyDirectionTop].clSrcImg.height() :
						DEF_RGN_SIZE;
		iHeightRight += ( !m_lstBackInfo[eMyDirectionBottomRight].clSrcImg.isNull() ) ?
						m_lstBackInfo[eMyDirectionBottomRight].clSrcImg.height() :
						DEF_RGN_SIZE;

		clSize.setHeight( ( iHeightRight > iHeightLeft ) ? iHeightRight : iHeightLeft );
	}

	return clSize;
}

//---------------------------------------------------------------------------//
/*!
	@fn			minimumSizeHint( void )
	@brief		このウィジェットの理想的な最小サイズを返す
*/
//---------------------------------------------------------------------------//
QSize
ClRectWidget::minimumSizeHint( void ) const
{
	//-------------------------------------------
	//	minimumSizeと同値とする
	//-------------------------------------------
	return minimumSize();
}

//---------------------------------------------------------------------------//
/*!
	@fn			minimumWidth( void )
	@brief		このウィジェットの最小幅を返す
*/
//---------------------------------------------------------------------------//
int
ClRectWidget::minimumWidth( void ) const
{
	//-------------------------------------------
	//	四隅の画像の原寸大の大きさを足し、それに1加えた値を最小とする
	//-------------------------------------------
	qint32  iSize = 0;		//	この関数の戻り値

	//	幅 = (TopLeft + TopRight + 1) or (BottomLeft + BottomRight + 1)
	{
		qint32  iWidthTop = 0;		//	上側の幅の合計

		iWidthTop = ( !m_lstBackInfo[eMyDirectionTopLeft].clSrcImg.isNull() ) ?
					m_lstBackInfo[eMyDirectionTopLeft].clSrcImg.width() :
					DEF_RGN_SIZE;
		iWidthTop += ( !m_lstBackInfo[eMyDirectionTopRight].clSrcImg.isNull() ) ?
					m_lstBackInfo[eMyDirectionTopRight].clSrcImg.width() :
					DEF_RGN_SIZE;
		iWidthTop++;

		qint32  iWidthBottom = 0;	//	下側の幅の合計

		iWidthBottom = ( !m_lstBackInfo[eMyDirectionBottomLeft].clSrcImg.isNull() ) ?
						m_lstBackInfo[eMyDirectionBottomLeft].clSrcImg.width() :
						DEF_RGN_SIZE;
		iWidthBottom += ( !m_lstBackInfo[eMyDirectionBottomRight].clSrcImg.isNull() ) ?
						m_lstBackInfo[eMyDirectionBottomRight].clSrcImg.width() :
						DEF_RGN_SIZE;
		iWidthBottom++;

		iSize = ( iWidthBottom > iWidthTop ) ? iWidthBottom : iWidthTop;
	}

	//-------------------------------------------
	//	レイアウトが存在し、そちらの方が値が大きければ、そちらに合わせる
	//-------------------------------------------
	QLayout  *pclMyLayout = layout();

	if( NULL != pclMyLayout )
	{
		if( iSize < pclMyLayout->minimumSize().width() )
		{
			iSize = pclMyLayout->minimumSize().width();
		}
	}

	return iSize;
}

//---------------------------------------------------------------------------//
/*!
	@fn			minimumHeight( void )
	@brief		このウィジェットの最小高を返す
*/
//---------------------------------------------------------------------------//
int
ClRectWidget::minimumHeight( void ) const
{
	//-------------------------------------------
	//	四隅の画像の原寸大の大きさを足し、それに1加えた値を最小とする
	//-------------------------------------------
	qint32  iSize = 0;		//	この関数の戻り値

	//	高さ = (TopLeft + BottomLeft + 1) or (TopRight + BottomRight + 1)
	{
		qint32  iHeightLeft = 0;	//	左側の高さの合計

		iHeightLeft = ( !m_lstBackInfo[eMyDirectionTopLeft].clSrcImg.isNull() ) ?
						m_lstBackInfo[eMyDirectionTopLeft].clSrcImg.height() :
						DEF_RGN_SIZE;
		iHeightLeft += ( !m_lstBackInfo[eMyDirectionBottomLeft].clSrcImg.isNull() ) ?
						m_lstBackInfo[eMyDirectionBottomLeft].clSrcImg.height() :
						DEF_RGN_SIZE;
		iHeightLeft++;

		qint32  iHeightRight = 0;	//	右側の高さの合計

		iHeightRight = ( !m_lstBackInfo[eMyDirectionTopRight].clSrcImg.isNull() ) ?
						m_lstBackInfo[eMyDirectionTopRight].clSrcImg.height() :
						DEF_RGN_SIZE;
		iHeightRight += ( !m_lstBackInfo[eMyDirectionBottomRight].clSrcImg.isNull() ) ?
						m_lstBackInfo[eMyDirectionBottomRight].clSrcImg.height() :
						DEF_RGN_SIZE;
		iHeightRight++;

		iSize = ( iHeightRight > iHeightLeft ) ? iHeightRight : iHeightLeft;
	}

	//-------------------------------------------
	//	レイアウトが存在し、そちらの方が値が大きければ、そちらに合わせる
	//-------------------------------------------
	QLayout  *pclMyLayout = layout();

	if( NULL != pclMyLayout )
	{
		if( iSize < pclMyLayout->minimumSize().height() )
		{
			iSize = pclMyLayout->minimumSize().height();
		}
	}

	return iSize;
}

//---------------------------------------------------------------------------//
/*!
	@fn			minimumSize( void )
	@brief		このウィジェットの最小サイズを返す
*/
//---------------------------------------------------------------------------//
QSize
ClRectWidget::minimumSize( void ) const
{
	//-------------------------------------------
	//	minimumWidthとminimumHeightの組み合わせ
	//-------------------------------------------
	QSize  clSize( minimumWidth(), minimumHeight() );

	return clSize;
}

//---------------------------------------------------------------------------//
/*!
	@fn			mousePressEvent( QMouseEvent* )
	@brief		マウスボタン押下時の処理

	@param		a_pclEvent	:	イベントクラス
*/
//---------------------------------------------------------------------------//
void
ClRectWidget::mousePressEvent( QMouseEvent  *a_pclEvent )
{
	procMousePress(a_pclEvent);

	//-------------------------------------------
	//	イベント処理済み
	//-------------------------------------------
	a_pclEvent->setAccepted( true );
}

//---------------------------------------------------------------------------//
/*!
	@fn			mouseMoveEvent( QMouseEvent* )
	@brief		マウス移動時の処理

	@param		a_pclEvent	:	イベントクラス
*/
//---------------------------------------------------------------------------//
void
ClRectWidget::mouseMoveEvent( QMouseEvent  *a_pclEvent )
{
	if(false == m_bIsMouseEventApp)	//EventFilterで処理しない
	{
		procMouseMove(a_pclEvent);
	}

	//-------------------------------------------
	//	イベント処理済み
	//-------------------------------------------
	a_pclEvent->setAccepted( true );
}

//---------------------------------------------------------------------------//
/*!
	@fn			mouseReleaseEvent( QMouseEvent* )
	@brief		マウスボタンを離した時の処理

	@param		a_pclEvent	:	イベントクラス
*/
//---------------------------------------------------------------------------//
void
ClRectWidget::mouseReleaseEvent( QMouseEvent  *a_pclEvent )
{
	procMouseRelease(a_pclEvent);

	//-------------------------------------------
	//	イベント処理済み
	//-------------------------------------------
	a_pclEvent->setAccepted( true );

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
ClRectWidget::paintEvent( QPaintEvent  *a_pclEvent )
{
	//-------------------------------------------
	//	ウィジェット領域更新
	//-------------------------------------------
	if( m_eUpdateRgnDrc != eMyDirectionNone )
	{
		calcBackRgn( m_eUpdateRgnDrc );			//	領域を再計算
		setMyMask();							//	設定
		m_eUpdateRgnDrc = eMyDirectionNone;		//	領域更新方向をクリア
	}

	QPainter  clPaint( this );

	//-------------------------------------------
	//	描画しない領域はマスク
	//-------------------------------------------
	clPaint.setClipping( true );
	clPaint.setClipRegion( a_pclEvent->region() );

	//-------------------------------------------
	//	背景画像を描画
	//-------------------------------------------
	stBackInfo  *pstInfo = NULL;

	for( qint32 i = 0; i < m_lstBackInfo.size(); i++ )
	{
		pstInfo = &( m_lstBackInfo[i] );
		if( !pstInfo->clResizeImg.isNull() )
		{
			clPaint.drawPixmap( pstInfo->clRgn.boundingRect().topLeft(),
								pstInfo->clResizeImg
					);
		}
	}

	//-------------------------------------------
	//	イベント処理済み
	//-------------------------------------------
	a_pclEvent->setAccepted( true );

	return;
}

//---------------------------------------------------------------------------//
/*!
	@fn			resizeEvent( QResizeEvent* )
	@brief		サイズ変更時の処理

	@param		a_pclEvent	:	イベントクラス
*/
//---------------------------------------------------------------------------//
void
ClRectWidget::resizeEvent( QResizeEvent  *a_pclEvent )
{
#if QT_VERSION_5
	clearMask();	//マスク解除(paintEvent時に再設定)
#endif

	//-------------------------------------------
	//	デフォルト処理
	//-------------------------------------------
	this->QWidget::resizeEvent( a_pclEvent );

	//-------------------------------------------
	//	領域再計算フラグを更新
	//-------------------------------------------
	//	マウスドラッグによるリサイズであれば、ドラッグ方向を記憶
	if( m_eState & eMyDragStateSize )
	{
		m_eUpdateRgnDrc = static_cast<eMyDirection>( m_eUpdateRgnDrc |
													( m_eState - eMyDragStateSize )
													);
	}
	//	マウスドラッグ以外であれば、外部から強制的にサイズ変更された可能性が高いので、
	//	すべての領域を再計算
	else
	{
		m_eUpdateRgnDrc = static_cast<eMyDirection>( eMyDirectionTopLeft + eMyDirectionBottomRight + eMyDirectionCenter );
	}

	return;
}

//---------------------------------------------------------------------------//
/*!
	@brief		表示イベント
	@param[in]	pEvent : イベント
	@note		この処理がないと最小化から戻したときに再描画されない
*/
//---------------------------------------------------------------------------//
void ClRectWidget::showEvent(QShowEvent *pEvent)
{
	//マスク領域更新
	{
		calcBackRgn( m_eUpdateRgnDrc );			//	領域を再計算
		setMyMask();							//	設定
	}

#if QT_VERSION_4
	QEvent eventUpdate(QEvent::UpdateRequest);
	event(&eventUpdate);	//再描画イベント処理
#endif
	QWidget::showEvent(pEvent);
}

//---------------------------------------------------------------------------//
/*!
	@fn			setImgSub( const QPixmap&, qint32 )
	@brief		画像背景パスプロパティ設定の動作関数

	@param		a_clVal			:	ウィジェット背景画像
	@param		a_iDirection	:	参照する方向
*/
//---------------------------------------------------------------------------//
void
ClRectWidget::setImgSub( QPixmap  a_clVal, qint32  a_iDirection )
{
	//-------------------------------------------
	//	画像を記憶
	//-------------------------------------------
	m_lstBackInfo[a_iDirection].clSrcImg = a_clVal;

	//-------------------------------------------
	//	領域再計算フラグ更新
	//-------------------------------------------
	m_eUpdateRgnDrc = static_cast<eMyDirection>( m_eUpdateRgnDrc | a_iDirection );

	//-------------------------------------------
	//	描画を更新
	//-------------------------------------------
	update();

	return;
}

//---------------------------------------------------------------------------//
/*!
	@fn			setMyMask( void )
	@brief		ウィジェットにマスクを設定
*/
//---------------------------------------------------------------------------//
void
ClRectWidget::setMyMask( void )
{
	//-------------------------------------------
	//	背景の各領域を足し合わせる
	//-------------------------------------------
	QRegion		clMaskRegion;		//	マスクに使用する領域

	for( qint32 i = eMyDirectionNone; i < eMyDirectionTerminate; i++ )
	{
		clMaskRegion += m_lstBackInfo[i].clRgn;
	}

	//-------------------------------------------
	//	タイトルバーなどを消すため、自身の大きさでマスクを設定
	//-------------------------------------------
	setMask( clMaskRegion );

	return;
}

//---------------------------------------------------------------------------//
/*!
	@fn			checkWidgetEdge( QPoint* )
	@brief		座標がウィジェットのどの端の部分にあるのかを確認
	@attention	引数は座標であること

	@param		a_pclPtr	:	マウス座標

	@retval		eMyDirectionNone	:	端にはない
	@retval		eMyDirectionNone以外	:	eMyDirection
*/
//---------------------------------------------------------------------------//
ClRectWidget::eMyDirection
ClRectWidget::checkWidgetEdge( QPoint  *a_pclPtr )
{
	if( a_pclPtr == NULL )
	{
		return eMyDirectionNone;
	}

	//-------------------------------------------
	//	背景の領域設定と座標を比較
	//-------------------------------------------
	eMyDirection  eRet = eMyDirectionNone;

	//	左上
	if( m_lstBackInfo[eMyDirectionTopLeft].clRgn.contains( *a_pclPtr ) )
	{
		eRet = eMyDirectionTopLeft;
	}
	//	上
	else if( m_lstBackInfo[eMyDirectionTop].clRgn.contains( *a_pclPtr ) )
	{
		eRet = eMyDirectionTop;
	}
	//	右上
	else if( m_lstBackInfo[eMyDirectionTopRight].clRgn.contains( *a_pclPtr ) )
	{
		eRet = eMyDirectionTopRight;
	}
	//	左
	else if( m_lstBackInfo[eMyDirectionLeft].clRgn.contains( *a_pclPtr ) )
	{
		eRet = eMyDirectionLeft;
	}
	//	右
	else if( m_lstBackInfo[eMyDirectionRight].clRgn.contains( *a_pclPtr ) )
	{
		eRet = eMyDirectionRight;
	}
	//	左下
	else if( m_lstBackInfo[eMyDirectionBottomLeft].clRgn.contains( *a_pclPtr ) )
	{
		eRet = eMyDirectionBottomLeft;
	}
	//	下
	else if( m_lstBackInfo[eMyDirectionBottom].clRgn.contains( *a_pclPtr ) )
	{
		eRet = eMyDirectionBottom;
	}
	//	右下
	else if( m_lstBackInfo[eMyDirectionBottomRight].clRgn.contains( *a_pclPtr ) )
	{
		eRet = eMyDirectionBottomRight;
	}

	return eRet;
}

//---------------------------------------------------------------------------//
/*!
	@fn			calcBackRgn( eMyDirection )
	@brief		背景画像設定に従って、各方向の領域を計算
	@note		QPixmap::mask()は、全体が塗りつぶされた四角形の画像に対しては、
				空の領域が返ってくる

	@param		a_eDirection	:	計算し直す方向
*/
//---------------------------------------------------------------------------//
void
ClRectWidget::calcBackRgn( eMyDirection  a_eDirection )
{
	QSize			clWidgetSize = size();		//	このウィジェットの大きさ
	stBackInfo		*pstInfo = NULL;			//	(リスト参照に使用)

	if( a_eDirection == eMyDirectionNone )
	{
		return;
	}

	//-------------------------------------------
	//	中央を除いた、各方向の領域サイズを再計算
	//-------------------------------------------
	//	左上
	if( ( a_eDirection & eMyDirectionTopLeft ) == eMyDirectionTopLeft )
	{
		calcBackRgnSubCorner( eMyDirectionTopLeft );
	}

	//	左下
	if( ( a_eDirection & eMyDirectionBottomLeft ) == eMyDirectionBottomLeft )
	{
		calcBackRgnSubCorner( eMyDirectionBottomLeft );
	}

	//	右上
	if( ( a_eDirection & eMyDirectionTopRight ) == eMyDirectionTopRight )
	{
		calcBackRgnSubCorner( eMyDirectionTopRight );
	}

	//	右下
	if( ( a_eDirection & eMyDirectionBottomRight ) == eMyDirectionBottomRight )
	{
		calcBackRgnSubCorner( eMyDirectionBottomRight );
	}

	//	上
	//	※左上や右上の画像変更、左の大きさ変更、右の大きさ変更の場合にも再計算が必要
	if( ( a_eDirection & eMyDirectionTop )  ||  ( a_eDirection & eMyDirectionLeft )  ||
		( a_eDirection & eMyDirectionRight )
	)
	{
		calcBackRgnSub( eMyDirectionTop );
	}

	//	下
	//	※左下や右下の画像変更、左の大きさ変更、右の大きさ変更の場合にも再計算が必要
	if( ( a_eDirection & eMyDirectionBottom )  ||  ( a_eDirection & eMyDirectionLeft )  ||
		( a_eDirection & eMyDirectionRight )
	)
	{
		calcBackRgnSub( eMyDirectionBottom );
	}

	//	左
	//	※左下や左上の画像変更、上の大きさ変更、下の大きさ変更の場合にも再計算が必要
	if( ( a_eDirection & eMyDirectionLeft )  ||  ( a_eDirection & eMyDirectionTop )  ||
		( a_eDirection & eMyDirectionBottom )
	)
	{
		calcBackRgnSub( eMyDirectionLeft );
	}

	//	右
	//	※右下や右上の画像変更、上の大きさ変更、下の大きさ変更の場合にも再計算が必要
	if( ( a_eDirection & eMyDirectionRight )  ||  ( a_eDirection & eMyDirectionTop )  ||
		( a_eDirection & eMyDirectionBottom )
	)
	{
		calcBackRgnSub( eMyDirectionRight );
	}

	//-------------------------------------------
	//	中央を除いた、各方向の領域を配置
	//-------------------------------------------
	//	左
	pstInfo = &m_lstBackInfo[eMyDirectionLeft];
	pstInfo->clRgn.translate( pstInfo->clRgn.boundingRect().topLeft() * -1 );
	pstInfo->clRgn.translate( 0,
							m_lstBackInfo[eMyDirectionTopLeft].clRgn.boundingRect().height()
					);

	//	左下
	pstInfo = &m_lstBackInfo[eMyDirectionBottomLeft];
	pstInfo->clRgn.translate( pstInfo->clRgn.boundingRect().topLeft() * -1 );
	pstInfo->clRgn.translate( 0,
							clWidgetSize.height() - pstInfo->clRgn.boundingRect().height()
					);

	//	上
	pstInfo = &m_lstBackInfo[eMyDirectionTop];
	pstInfo->clRgn.translate( pstInfo->clRgn.boundingRect().topLeft() * -1 );
	pstInfo->clRgn.translate( m_lstBackInfo[eMyDirectionTopLeft].clRgn.boundingRect().width(),
							0
					);

	//	下
	pstInfo = &m_lstBackInfo[eMyDirectionBottom];
	pstInfo->clRgn.translate( pstInfo->clRgn.boundingRect().topLeft() * -1 );
	pstInfo->clRgn.translate( m_lstBackInfo[eMyDirectionBottomLeft].clRgn.boundingRect().width(),
							clWidgetSize.height() - pstInfo->clRgn.boundingRect().height()
					);
	//	右上
	pstInfo = &m_lstBackInfo[eMyDirectionTopRight];
	pstInfo->clRgn.translate( pstInfo->clRgn.boundingRect().topLeft() * -1 );
	pstInfo->clRgn.translate( clWidgetSize.width() - pstInfo->clRgn.boundingRect().width(),
							0
					);

	//	右
	pstInfo = &m_lstBackInfo[eMyDirectionRight];
	pstInfo->clRgn.translate( pstInfo->clRgn.boundingRect().topLeft() * -1 );
	pstInfo->clRgn.translate( clWidgetSize.width() - pstInfo->clRgn.boundingRect().width(),
							m_lstBackInfo[eMyDirectionTopRight].clRgn.boundingRect().height()
					);

	//	右下
	pstInfo = &m_lstBackInfo[eMyDirectionBottomRight];
	pstInfo->clRgn.translate( pstInfo->clRgn.boundingRect().topLeft() * -1 );
	pstInfo->clRgn.translate( clWidgetSize.width() - pstInfo->clRgn.boundingRect().width(),
							clWidgetSize.height() - pstInfo->clRgn.boundingRect().height()
					);

	//-------------------------------------------
	//	中央領域の大きさ、位置を再計算
	//-------------------------------------------
	{
		pstInfo = &m_lstBackInfo[eMyDirectionCenter];

		//	領域を空にする
		pstInfo->clRgn = QRegion();

		//	上下左右の領域に接する矩形を計算
		QRect  clCenterRect( QPoint(0,0), QPoint(0,0) );		//	領域を内包する矩形

		clCenterRect.setTop( m_lstBackInfo[eMyDirectionTop].clRgn.boundingRect().bottom() + 1 );
		clCenterRect.setBottom( m_lstBackInfo[eMyDirectionBottom].clRgn.boundingRect().top() - 1 );
		clCenterRect.setLeft( m_lstBackInfo[eMyDirectionLeft].clRgn.boundingRect().right() );
		clCenterRect.setRight( m_lstBackInfo[eMyDirectionRight].clRgn.boundingRect().left() - 1 );

		//	画像がある
		if( !pstInfo->clSrcImg.isNull() )
		{
			//	画像を読み込み、領域の大きさに引き伸ばす
			pstInfo->clResizeImg = pstInfo->clSrcImg.scaled( clCenterRect.size(),
															Qt::IgnoreAspectRatio,
															Qt::SmoothTransformation
													);

			//	画像の領域をそのままマスクとして使用
			pstInfo->clRgn = pstInfo->clResizeImg.mask();
			pstInfo->clRgn.translate( clCenterRect.left(), clCenterRect.top() );
		}

		//	領域が空なら、矩形をそのまま設定
		//	※画像パスがなかったり、画像が塗り潰しの四角形の場合に起き得る
		if( pstInfo->clRgn.isEmpty() )
		{
			pstInfo->clRgn = QRegion( clCenterRect );
		}
	}

	return;
}

//---------------------------------------------------------------------------//
/*!
	@fn			calcBackRgnSub( eMyDirection )
	@brief		calcBackRegion処理関数(上下左右)

	@param		a_eDirection	:	計算し直す方向
*/
//---------------------------------------------------------------------------//
void
ClRectWidget::calcBackRgnSub( eMyDirection  a_eDirection )
{
	QSize			clWidgetSize = size();		//	このウィジェットの大きさ
	stBackInfo		*pstInfo = NULL;			//	(リスト参照に使用)

	//-------------------------------------------
	//	領域を空にする
	//-------------------------------------------
	pstInfo = &m_lstBackInfo[a_eDirection];
	pstInfo->clRgn = QRegion();

	//-------------------------------------------
	//	領域の大きさを計算
	//-------------------------------------------
	QSize  clRgnSize = clWidgetSize / 3;				//	領域の大きさ

	switch( a_eDirection )
	{
		//	上
		case eMyDirectionTop:
		{
			clRgnSize.setWidth( clWidgetSize.width() -
								 m_lstBackInfo[eMyDirectionTopLeft].clRgn.boundingRect().width() -
								 m_lstBackInfo[eMyDirectionTopRight].clRgn.boundingRect().width()
						);
			break;
		}
		//	下
		case eMyDirectionBottom:
		{
			clRgnSize.setWidth( clWidgetSize.width() -
								 m_lstBackInfo[eMyDirectionBottomLeft].clRgn.boundingRect().width() -
								 m_lstBackInfo[eMyDirectionBottomRight].clRgn.boundingRect().width()
						);
			break;
		}
		//	左
		case eMyDirectionLeft:
		{
			clRgnSize.setHeight( clWidgetSize.height() -
								 m_lstBackInfo[eMyDirectionTopLeft].clRgn.boundingRect().height() -
								 m_lstBackInfo[eMyDirectionBottomLeft].clRgn.boundingRect().height()
						);
			break;
		}
		//	右
		case eMyDirectionRight:
		{
			clRgnSize.setHeight( clWidgetSize.height() -
								 m_lstBackInfo[eMyDirectionTopRight].clRgn.boundingRect().height() -
								 m_lstBackInfo[eMyDirectionBottomRight].clRgn.boundingRect().height()
						);
			break;
		}
		default:
		{
			return;
		}
	}

	//-------------------------------------------
	//	領域サイズを再計算
	//-------------------------------------------
	//	画像がある
	if( !pstInfo->clSrcImg.isNull() )
	{
		//	画像を読み込んで、領域の大きさを再設定
		switch( a_eDirection )
		{
			case eMyDirectionTop:	//	No break
			case eMyDirectionBottom:
			{
				clRgnSize.setHeight( pstInfo->clSrcImg.height() );
				break;
			}
			case eMyDirectionLeft:	//	No break
			case eMyDirectionRight:
			{
				clRgnSize.setWidth( pstInfo->clSrcImg.width() );
				break;
			}
			default:
			{
				return;
			}
		}

		//	領域の大きさに引き伸ばす
		pstInfo->clResizeImg = pstInfo->clSrcImg.scaled( clRgnSize,
														Qt::IgnoreAspectRatio,
														Qt::SmoothTransformation
												);

		//	画像の領域をそのままマスクとして使用
		pstInfo->clRgn = pstInfo->clResizeImg.mask();

		//	画像が塗りつぶしの四角形だった場合、領域が空になってしまうので、
		//	その場合は画像サイズの矩形領域を自分で作る
		if( pstInfo->clRgn.isEmpty() != false )
		{
			pstInfo->clRgn = QRegion( 0, 0, clRgnSize.width(), clRgnSize.height() );
		}
	}

	//	領域が空なら、矩形をそのまま設定
	//	※画像パスがなかった場合に起き得る
	if( pstInfo->clRgn.isEmpty() )
	{
		pstInfo->clRgn = QRegion( 0, 0, clRgnSize.width(), clRgnSize.height() );
	}

	return;
}

//---------------------------------------------------------------------------//
/*!
	@fn			calcBackRgnSubCorner( eMyDirection )
	@brief		calcBackRegion処理関数(角)

	@param		a_eDirection	:	計算し直す方向
*/
//---------------------------------------------------------------------------//
void
ClRectWidget::calcBackRgnSubCorner( eMyDirection  a_eDirection )
{
	stBackInfo  *pstInfo = NULL;			//	(リスト参照に使用)

	//-------------------------------------------
	//	領域を空にする
	//-------------------------------------------
	pstInfo = &m_lstBackInfo[a_eDirection];
	pstInfo->clRgn = QRegion();

	//-------------------------------------------
	//	領域サイズを再計算
	//-------------------------------------------
	//	画像がある
	if( !pstInfo->clSrcImg.isNull() )
	{
		//	描画用画像も、元画像そのままを使用
		pstInfo->clResizeImg = pstInfo->clSrcImg;

		//	画像の領域をそのままマスクとして使用
		pstInfo->clRgn = pstInfo->clResizeImg.mask();

		//	画像が塗りつぶしの四角形だった場合、領域が空になってしまうので、
		//	その場合は画像サイズの矩形領域を自分で作る
		if( pstInfo->clRgn.isEmpty() != false )
		{
			pstInfo->clRgn = QRegion( 0, 0, pstInfo->clResizeImg.width(), pstInfo->clResizeImg.height() );
		}
	}

	//	領域が空なら、適当な矩形を設定
	//	※画像パスがなかった場合に起き得る
	if( pstInfo->clRgn.isEmpty() )
	{
		QSize  clWidgetSize = size();		//	このウィジェットの大きさ

		pstInfo->clRgn = QRegion( 0, 0, clWidgetSize.width() / 3, clWidgetSize.height() / 3 );
	}

	return;
}

//---------------------------------------------------------------------------//
/*!
	@fn			setMouseCur( QPoint )
	@brief		マウス座標に合わせてマウスカーソルを設定

	@param		a_clPtr	:	マウス座標(ウィジェット座標系)
*/
//---------------------------------------------------------------------------//
void
ClRectWidget::setMouseCur( QPoint  a_clPtr )
{
	if(false == m_bEnableDragSize)
	{
		return;	//サイズ変更しないのでカーソル設定なし
	}

	//-------------------------------------------
	//	マウスポインタの位置に合わせてカーソルを選択
	//-------------------------------------------
	eMyDirection	eDir = checkWidgetEdge( &a_clPtr );		//	マウスポインタのある位置
	QCursor			clNewCursor;							//	新しく設定するカーソル

	switch( eDir )
	{
		case eMyDirectionTop:		//	No break
		case eMyDirectionBottom:
		{
			clNewCursor = QCursor( Qt::SizeVerCursor );
			break;
		}
		case eMyDirectionLeft:		//	No break
		case eMyDirectionRight:
		{
			clNewCursor = QCursor( Qt::SizeHorCursor );
			break;
		}
		case eMyDirectionTopRight:	//	No break
		case eMyDirectionBottomLeft:
		{
			clNewCursor = QCursor( Qt::SizeBDiagCursor );
			break;
		}
		case eMyDirectionTopLeft:	//	No break
		case eMyDirectionBottomRight:
		{
			clNewCursor = QCursor( Qt::SizeFDiagCursor );
			break;
		}
		default:
		{
			clNewCursor = QCursor( Qt::ArrowCursor );
			break;
		}
	}

	//-------------------------------------------
	//	設定する必要があれば設定
	//-------------------------------------------
	if( cursor().shape() != clNewCursor.shape() )
	{
		setCursor( clNewCursor );
	}

	return;
}

//---------------------------------------------------------------------------//
/*!
	@brief		マウス押下処理
	@param[in]	a_pclEvent : イベント
*/
//---------------------------------------------------------------------------//
void ClRectWidget::procMousePress(QMouseEvent *a_pclEvent)
{
	//-------------------------------------------
	//	左ボタンが押された
	//-------------------------------------------
	if( a_pclEvent->button() & Qt::LeftButton )
	{
		//	ドラッグ処理状態を初期化し、座標を記憶
		m_eState = eMyDragStateNone;
		m_clDragStartPos = a_pclEvent->pos();

		//	カーソルがウィジェットの端にあるかどうかを確認
		eMyDirection  eCurPos = checkWidgetEdge( &m_clDragStartPos );

		//	マウスドラッグによるサイズ変更OK、かつカーソルがウィジェットの端にある
		if( m_bEnableDragSize  &&  ( eCurPos != eMyDirectionNone ) )
		{
			//	ウィジェットサイズ変更状態に移行
			m_eState = static_cast<eMyDragState>( eCurPos + eMyDragStateSize );

			//	changeSizeByMargin用のマージンを計算
			m_clChangeSizeMargin = QSize( 0, 0 );
			if( eCurPos & eMyDirectionTop )			//	上を掴んでいる
			{
				m_clChangeSizeMargin.setHeight( a_pclEvent->pos().y() * -1 );
			}
			else if( eCurPos & eMyDirectionBottom )	//	下を掴んでいる
			{
				m_clChangeSizeMargin.setHeight( rect().height() - a_pclEvent->pos().y() );
			}
			if( eCurPos & eMyDirectionLeft )		//	左を掴んでいる
			{
				m_clChangeSizeMargin.setWidth( a_pclEvent->pos().x() * -1 );
			}
			else if( eCurPos & eMyDirectionRight )	//	右を掴んでいる
			{
				m_clChangeSizeMargin.setWidth( rect().width() - a_pclEvent->pos().x() );
			}
		}
		//	マウスドラッグによる移動OK
		else if( m_bEnableDragMove )
		{
			//	ウィジェット移動状態に移行
			m_eState = eMyDragStateMove;
		}
	}
}

//---------------------------------------------------------------------------//
/*!
	@brief		マウス離した処理
	@param[in]	a_pclEvent : イベント
*/
//---------------------------------------------------------------------------//
void ClRectWidget::procMouseRelease(QMouseEvent *a_pclEvent)
{
	//-------------------------------------------
	//	移動状態、サイズ変更状態終了
	//-------------------------------------------
	if( a_pclEvent->button() & Qt::LeftButton )
	{
		m_eState = eMyDragStateNone;
		setMouseCur(a_pclEvent->pos());	//カーソル更新
	}
}

#if QT_VERSION_5
//---------------------------------------------------------------------------//
/*!
	@brief		プラットフォームイベント
	@param[in]	pMessage : メッセージ
*/
//---------------------------------------------------------------------------//
bool ClRectWidget::nativeEvent(const QByteArray &, void *pMessage, long *)
{
#ifdef Q_OS_WIN
	MSG *pMsg = reinterpret_cast<MSG*>(pMessage);
	if(WM_SYSCOMMAND == pMsg->message &&
			SC_RESTORE == pMsg->wParam)	//ウィンドウ最小化解除
	{
		//最小化解除で再描画されなくなるのに対応
		hide();
		show();
	}
#endif
	return false;	//そのまま処理を流す
}
#endif

//---------------------------------------------------------------------------//
/*!
	@brief		マウス移動処理
	@param[in]	a_pclEvent : イベント
*/
//---------------------------------------------------------------------------//
void ClRectWidget::procMouseMove(QMouseEvent *a_pclEvent)
{
	//-------------------------------------------
	//	サイズ変更許可状態なら、マウスポインタ位置に合わせて
	//	カーソルを変更する
	//-------------------------------------------
	if( m_eState == eMyDragStateNone &&		//自分をドラッグ中ではない
			!(Qt::LeftButton & a_pclEvent->buttons())) 	//子ウィジェットをドラッグ中ではない
	{
		setMouseCur( a_pclEvent->pos() );
	}

	//-------------------------------------------
	//	移動処理中
	//-------------------------------------------
	if( m_eState == eMyDragStateMove )
	{
		//	移動
		setGeometry( geometry().translated( a_pclEvent->pos() - m_clDragStartPos ) );
	}
	//-------------------------------------------
	//	サイズ変更処理中
	//-------------------------------------------
	else if( m_eState & eMyDragStateSize )
	{
		//	変更に使う座標を求める
		//	※親ウィジェットがある場合は親ウィジェットの座標系を使用し、
		//	 ない場合はスクリーン座標系を使用しなければいけない
		QPoint	clCalPtr( 0, 0 );					//	サイズ計算に使用する座標

		if( parentWidget() == NULL )	//	親ウィジェットはない
		{
			clCalPtr = a_pclEvent->globalPos();
		}
		else							//	親ウィジェットがある
		{
			clCalPtr = a_pclEvent->pos() + geometry().topLeft();
		}

		//	changeSizeByDragに現在のマウスカーソル位置をそのまま渡すと、
		//	新しいウィジェットサイズは、現在のマウスカーソル位置がウィジェット端になるように計算される。
		//	これでは「ウィジェットの端より少し内側を掴んでいたのに、いつの間にか端ギリギリを
		//	掴んでいる」という現象が起きてしまうので、ドラッグ開始座標の位置の分だけずらす
		clCalPtr.rx()	+= m_clChangeSizeMargin.width();
		clCalPtr.ry()	+= m_clChangeSizeMargin.height();

		//	変更後のウィジェット位置を計算
		QRect	clGeometry = geometry();			//	ウィジェット位置
		QSize	clMinSize = minimumSizeHint();		//	最小サイズ

		changeSizeByDrag( &clGeometry, m_eState, &clCalPtr, &clMinSize );

		//	サイズ変更
		if( ( clGeometry.width() != geometry().width() )  ||
			( clGeometry.height() != geometry().height() )
		)
		{
			setGeometry( clGeometry );
		}
	}
}

//---------------------------------------------------------------------------//
/*!
	@fn			changeSizeByDrag( QRect*, eMyDragState, QPoint*, QSize* )
	@brief		マウスドラッグによるサイズ変更の大きさを計算

	@param		a_pclRect		:	変更対象のQRect
	@param		a_eState		:	マウスドラッグ領域
	@param		a_pclPtr		:	マウスドラッグ座標
	@param		a_pclMinSize	:	変更対象の最小サイズ
*/
//---------------------------------------------------------------------------//
void
ClRectWidget::changeSizeByDrag( QRect			*a_pclRect,
								eMyDragState	a_eState,
								QPoint			*a_pclPtr,
								QSize			*a_pclMinSize
				)
{
	if( ( a_pclRect == NULL)  ||  ( a_pclPtr == NULL )  ||
		( a_pclMinSize == NULL )
	)
	{
		return;
	}

	//-------------------------------------------
	//	最小サイズを下回らないように、変更後のサイズを計算
	//-------------------------------------------
	QRect  clAfterRect = *a_pclRect;

	//	上を掴んでいる
	if( a_eState & eMyDirectionTop )
	{
		if( clAfterRect.height() >= a_pclMinSize->height() )
		{
			clAfterRect.setTop( a_pclPtr->y() );
		}
		if( clAfterRect.height() < a_pclMinSize->height() )
		{
			clAfterRect.setTop( a_pclRect->bottom() - a_pclMinSize->height());		//下から計算する
		}
		if( clAfterRect.height() > maximumHeight() && clAfterRect.top() != a_pclRect->top() )
		{
			clAfterRect.setTop( a_pclRect->bottom() - ( maximumHeight() - 1 ));
		}
	}
	//	下を掴んでいる
	else if( a_eState & eMyDirectionBottom )
	{
		clAfterRect.setBottom( a_pclPtr->y() );
		if( clAfterRect.height() < a_pclMinSize->height() )
		{
			clAfterRect.setBottom( a_pclRect->bottom() );
			clAfterRect.setHeight( a_pclMinSize->height() );
		}
	}
	//	左を掴んでいる
	if( a_eState & eMyDirectionLeft )
	{
		if( clAfterRect.width() >= a_pclMinSize->width() )
		{
			clAfterRect.setLeft( a_pclPtr->x() );
		}
		if( clAfterRect.width() < a_pclMinSize->width() )
		{
			clAfterRect.setLeft( a_pclRect->right() - a_pclMinSize->width() );
		}
		if( clAfterRect.width() > maximumWidth() && clAfterRect.left() != a_pclRect->left() )
		{
			clAfterRect.setLeft( a_pclRect->right() - ( maximumWidth() - 1 ));
		}
	}
	//	右を掴んでいる
	else if( a_eState & eMyDirectionRight )
	{
		clAfterRect.setRight( a_pclPtr->x() );
		if( clAfterRect.width() < a_pclMinSize->width() )
		{
			clAfterRect.setRight( a_pclRect->right() );
			clAfterRect.setWidth( a_pclMinSize->width() );
		}
	}
	*a_pclRect = clAfterRect;

	return;
}

