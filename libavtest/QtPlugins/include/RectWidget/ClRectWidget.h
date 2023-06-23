//***************************************************************************//
//***************************************************************************//
/*!
	@file		ClRectWidget.h

	@brief		グラフィカルなQWidget(四角形のみ)

	@author		株式会社エルモ社  映像技術第二部

	Confidential

	Copyright(c) Elmo Industrial Co.,Ltd.		http://www.elmo.co.jp/
*/
//***************************************************************************//
//***************************************************************************//
#ifndef CLRECTWIDGET_H
#define CLRECTWIDGET_H


#include	<QWidget>
//#include	<QtDesigner/QDesignerExportWidget>	//	QDESIGNER_WIDGET_EXPORT用


//***************************************************************************//
/*!
	@class	ClRectWidget
	@brief	グラフィカルなQWidget
	@note	四角形のみ
*/
//***************************************************************************//
//QDESIGNER_WIDGET_EXPORT
class ClRectWidget : public QWidget
{
Q_OBJECT


	////////////////////////////////////////////////////////////////////
	//	定義
	////////////////////////////////////////////////////////////////////
protected:
	//	方向を表す値
	typedef enum
	{
		eMyDirectionNone = 0,
		eMyDirectionTop = 0x001,					//	上
		eMyDirectionLeft = 0x002,					//	左
		eMyDirectionRight = 0x004,					//	右
		eMyDirectionBottom = 0x008,					//	下
		eMyDirectionTopLeft =
			eMyDirectionTop + eMyDirectionLeft,		//	左上
		eMyDirectionTopRight =
			eMyDirectionTop + eMyDirectionRight,	//	右上
		eMyDirectionBottomLeft =
			eMyDirectionBottom + eMyDirectionLeft,	//	左下
		eMyDirectionBottomRight =
			eMyDirectionBottom + eMyDirectionRight,	//	右下
		eMyDirectionCenter = 0x010,					//	中央
		eMyDirectionTerminate = 0x011
	} eMyDirection;

	//	マウスドラッグ処理状態を表す値
	typedef enum
	{
		eMyDragStateNone = 0,
		eMyDragStateSize = 0x010,		//	サイズ変更中(※ビット演算専用)
		eMyDragStateSizeTopLeft =
			eMyDragStateSize +
			eMyDirectionTop +
			eMyDirectionLeft,			//	サイズ変更中(左上ドラッグ)
		eMyDragStateSizeTop =
			eMyDragStateSize +
			eMyDirectionTop,			//	サイズ変更中(上ドラッグ)
		eMyDragStateSizeTopRight =
			eMyDragStateSize +
			eMyDirectionTop +
			eMyDirectionRight,			//	サイズ変更中(右上ドラッグ)
		eMyDragStateSizeLeft =
			eMyDragStateSize +
			eMyDirectionLeft,			//	サイズ変更中(左ドラッグ)
		eMyDragStateSizeRight =
			eMyDragStateSize +
			eMyDirectionRight,			//	サイズ変更中(右ドラッグ)
		eMyDragStateSizeBottomLeft =
			eMyDragStateSize +
			eMyDirectionBottom +
			eMyDirectionLeft,			//	サイズ変更中(左下ドラッグ)
		eMyDragStateSizeBottom =
			eMyDragStateSize +
			eMyDirectionBottom,			//	サイズ変更中(下ドラッグ)
		eMyDragStateSizeBottomRight =
			eMyDragStateSize +
			eMyDirectionBottom +
			eMyDirectionRight,			//	サイズ変更中(右下ドラッグ)
		eMyDragStateMove = 0x20			//	移動中
	} eMyDragState;

	//	背景設定の情報
	typedef struct _stBackInfo
	{
		QPixmap		clSrcImg;		//	元となる画像ファイル
		QPixmap		clResizeImg;	//	表示用に伸縮処理された画像ファイル
		QRegion		clRgn;			//	この画像の、ウィジェットにおける領域
		_stBackInfo()	:	clSrcImg(),
							clResizeImg(),
							clRgn()
		{
		}
	} stBackInfo;


	////////////////////////////////////////////////////////////////////
	//	プロパティ
	////////////////////////////////////////////////////////////////////
	//	マウスドラッグ移動ON/OFF
	Q_PROPERTY( bool dragMove READ dragMove WRITE setDragMove )
protected:
	bool		m_bEnableDragMove;		//	マウスドラッグによる移動を許可するならtrue
public:
	virtual void	setDragMove( const bool  &a_bVal );
	virtual bool	dragMove( void ) const { return m_bEnableDragMove; }

	//	マウスドラッグによるサイズ変更ON/OFF
	Q_PROPERTY( bool dragChangeSize READ dragChangeSize WRITE setDragChangeSize )
protected:
	bool		m_bEnableDragSize;		//	マウスドラッグによるサイズ変更を許可するならtrue
public:
	virtual void	setDragChangeSize( const bool  &a_bVal );
	virtual bool	dragChangeSize( void ) const { return m_bEnableDragSize; }

	//	マウスイベント処理をQApplicationからのEventFilterで行うフラグ
	Q_PROPERTY(bool isMouseEventApp READ getIsMouseEventApp WRITE setIsMouseEventApp)
protected:
	bool m_bIsMouseEventApp;	//マウスイベント処理をEventFilterで行うならばtrue
public:
	virtual void setIsMouseEventApp(const bool &bValue);
	virtual bool getIsMouseEventApp() const {return m_bIsMouseEventApp;}

	//	ウィジェット背景画像(左上)
	Q_PROPERTY( QPixmap imgTopLeft READ imgTopLeft WRITE setImgTopLeft )
public:
	virtual void	setImgTopLeft( const QPixmap  &a_clVal )
					{ setImgSub( a_clVal, eMyDirectionTop + eMyDirectionLeft ); }
	virtual QPixmap	imgTopLeft( void ) const
					{ return m_lstBackInfo[ eMyDirectionTop + eMyDirectionLeft ].clSrcImg; }

	//	ウィジェット背景画像(上)
	Q_PROPERTY( QPixmap imgTop READ imgTop WRITE setImgTop )
public:
	virtual void	setImgTop( const QPixmap  &a_clVal )
					{ setImgSub( a_clVal, eMyDirectionTop ); }
	virtual QPixmap	imgTop( void ) const
					{ return m_lstBackInfo[ eMyDirectionTop ].clSrcImg; }

	//	ウィジェット背景画像(右上)
	Q_PROPERTY( QPixmap imgTopRight READ imgTopRight WRITE setImgTopRight )
public:
	virtual void	setImgTopRight( const QPixmap  &a_clVal )
					{ setImgSub( a_clVal, eMyDirectionTop + eMyDirectionRight ); }
	virtual QPixmap	imgTopRight( void ) const
					{ return m_lstBackInfo[ eMyDirectionTop + eMyDirectionRight ].clSrcImg; }

	//	ウィジェット背景画像(左)
	Q_PROPERTY( QPixmap imgLeft READ imgLeft WRITE setImgLeft )
public:
	virtual void	setImgLeft( const QPixmap  &a_clVal )
					{ setImgSub( a_clVal, eMyDirectionLeft ); }
	virtual QPixmap	imgLeft( void ) const
					{ return m_lstBackInfo[ eMyDirectionLeft ].clSrcImg; }

	//	ウィジェット背景画像(中央)
	Q_PROPERTY( QPixmap imgCenter READ imgCenter WRITE setImgCenter )
public:
	virtual void	setImgCenter( const QPixmap  &a_clVal )
					{ setImgSub( a_clVal, eMyDirectionCenter ); }
	virtual QPixmap	imgCenter( void ) const
					{ return m_lstBackInfo[ eMyDirectionCenter ].clSrcImg; }

	//	ウィジェット背景画像(右)
	Q_PROPERTY( QPixmap imgRight READ imgRight WRITE setImgRight )
public:
	virtual void	setImgRight( const QPixmap  &a_clVal )
					{ setImgSub( a_clVal, eMyDirectionRight ); }
	virtual QPixmap	imgRight( void ) const
					{ return m_lstBackInfo[ eMyDirectionRight ].clSrcImg; }

	//	ウィジェット背景画像(左下)
	Q_PROPERTY( QPixmap imgBottomLeft READ imgBottomLeft WRITE setImgBottomLeft )
public:
	virtual void	setImgBottomLeft( const QPixmap  &a_clVal )
					{ setImgSub( a_clVal, eMyDirectionBottom + eMyDirectionLeft ); }
	virtual QPixmap	imgBottomLeft( void ) const
					{ return m_lstBackInfo[ eMyDirectionBottom + eMyDirectionLeft ].clSrcImg; }

	//	ウィジェット背景画像(下)
	Q_PROPERTY( QPixmap imgBottom READ imgBottom WRITE setImgBottom )
public:
	virtual void	setImgBottom( const QPixmap  &a_clVal )
					{ setImgSub( a_clVal, eMyDirectionBottom ); }
	virtual QPixmap	imgBottom( void ) const
					{ return m_lstBackInfo[ eMyDirectionBottom ].clSrcImg; }

	//	ウィジェット背景画像(右下)
	Q_PROPERTY( QPixmap imgBottomRight READ imgBottomRight WRITE setImgBottomRight )
public:
	virtual void	setImgBottomRight( const QPixmap  &a_clVal )
					{ setImgSub( a_clVal, eMyDirectionBottom + eMyDirectionRight ); }
	virtual QPixmap	imgBottomRight( void ) const
					{ return m_lstBackInfo[ eMyDirectionBottom + eMyDirectionRight ].clSrcImg; }


	////////////////////////////////////////////////////////////////////
	//	メンバー変数
	////////////////////////////////////////////////////////////////////
protected:
	eMyDragState			m_eState;				//	マウスドラッグ処理状態
	QPoint					m_clDragStartPos;		//	ドラッグ開始時のマウスカーソル座標(自身の座標系)
	QSize					m_clChangeSizeMargin;	//	ドラッグでのサイズ変更時に使用するマージン
													//	※changeSizeByDragと併用する
	eMyDirection			m_eUpdateRgnDrc;		//	ウィジェット背景領域を更新する方向（or演算）
	QVector<stBackInfo>		m_lstBackInfo;			//	ウィジェット背景情報のリスト


	////////////////////////////////////////////////////////////////////
	//	生成・破棄
	////////////////////////////////////////////////////////////////////
public:
	ClRectWidget( QWidget  *a_pclParent = NULL );	//	引数つきコンストラクタ
	virtual ~ClRectWidget( void );					//	デストラクタ

	virtual bool eventFilter(QObject *pTarget, QEvent *pEvent);	//イベントフィルタ

	////////////////////////////////////////////////////////////////////
	//	オーバーライド
	////////////////////////////////////////////////////////////////////
public:
	virtual QSize	sizeHint( void ) const;					//	このウィジェットの理想的なサイズを返す
	virtual QSize	minimumSizeHint( void ) const;			//	このウィジェットの理想的な最小サイズを返す
	virtual int		minimumWidth( void ) const;				//	このウィジェットの最小幅を返す
	virtual int		minimumHeight( void ) const;			//	このウィジェットの最小高を返す
	virtual QSize	minimumSize( void ) const;				//	このウィジェットの最小サイズを返す
protected:
	virtual void	mousePressEvent( QMouseEvent  *a_pclEvent );		//	マウスボタン押下時の処理
	virtual void	mouseMoveEvent( QMouseEvent  *a_pclEvent );			//	マウス移動時の処理
	virtual void	mouseReleaseEvent( QMouseEvent  *a_pclEvent );		//	マウスボタンを離した時の処理
	virtual void	paintEvent( QPaintEvent  *a_pclEvent );				//	描画処理
	virtual void	resizeEvent( QResizeEvent  *a_pclEvent );			//	サイズ変更時の処理
	virtual void	showEvent(QShowEvent *pEvent);	//表示イベント


	////////////////////////////////////////////////////////////////////
	//	内部用
	////////////////////////////////////////////////////////////////////
protected:
	virtual void			setImgSub( QPixmap  a_clVal, qint32  a_iDirection );	//	画像背景プロパティ設定の動作関数
	virtual void			setMyMask( void );										//	ウィジェットにマスクを設定
	virtual eMyDirection	checkWidgetEdge( QPoint  *a_pclPtr );					//	座標がウィジェットのどの端の部分にあるのかを確認
	virtual void			calcBackRgn( eMyDirection  a_eDirection );				//	背景画像設定に従って、各方向の領域を計算
	virtual void			calcBackRgnSub( eMyDirection  a_eDirection );			//	calcBackRegion処理関数(上下左右)
	virtual void			calcBackRgnSubCorner( eMyDirection  a_eDirection );		//	calcBackRegion処理関数(角)
	virtual void			setMouseCur( QPoint  a_clPtr );							//	マウス座標に合わせてマウスカーソルを設定
	virtual void procMousePress(QMouseEvent *a_pclEvent);	//マウス押下処理
	virtual void procMouseMove(QMouseEvent *a_pclEvent);	//マウス移動処理
	virtual void procMouseRelease(QMouseEvent *a_pclEvent);	//マウス離した処理

#if ((QT_VERSION & 0xff0000) == 0x050000)
	virtual bool nativeEvent(const QByteArray&, void *pMessage, long *);	//プラットフォームイベント
#endif


	////////////////////////////////////////////////////////////////////
	//	メンバー変数を参照しない、計算用の関数
	////////////////////////////////////////////////////////////////////
protected:
	virtual void	changeSizeByDrag( QRect		*a_pclRect,
									eMyDragState	a_eState,
									QPoint			*a_pclPtr,
									QSize			*a_pclMinSize );		//	マウスドラッグによるサイズ変更の大きさを計算
};

//--------------------------------------------------------------------------------//
/*!
	@brief	RectWidget初期化変更版
*/
//--------------------------------------------------------------------------------//
class ClRectWidget2 : public ClRectWidget
{
public:
	ClRectWidget2(QWidget *pParent = NULL);	//コンストラクタ
};
//--------------------------------------------------------------------------------//
#endif // CLRECTWIDGET_H
