//***************************************************************************//
//***************************************************************************//
/*!
	@file		ClRollOverToolBtn.h

	@brief		マウスポインタが乗ると画像が切り替わるQToolButton

	@author		株式会社エルモ社  映像技術第二部

	Confidential

	Copyright(c) Elmo Industrial Co.,Ltd.		http://www.elmo.co.jp/
*/
//***************************************************************************//
//***************************************************************************//
#ifndef CLROLLOVERTOOLBTN_H
#define CLROLLOVERTOOLBTN_H


#include	<QToolButton>
//#include	<QtDesigner/QDesignerExportWidget>	//	QDESIGNER_WIDGET_EXPORT用


//***************************************************************************//
/*!
	@class	ClRollOverToolBtn
	@brief	マウスポインタが乗ると画像が切り替わるQToolButton
*/
//***************************************************************************//
//QDESIGNER_WIDGET_EXPORT
class ClRollOverToolBtn : public QToolButton
{
Q_OBJECT


	////////////////////////////////////////////////////////////////////
	//	プロパティ
	////////////////////////////////////////////////////////////////////
	//	表示用アイコン
	Q_PROPERTY( QIcon imgShow READ imgShow WRITE setImgShow )
public:
	virtual void	setImgShow( const QIcon  &a_clVal ){ m_clShowIcon = a_clVal; update(); }
	virtual QIcon	imgShow( void ) const { return m_clShowIcon; }

	//ONとOFFのどっちの絵を使うか
	//Q_PROPERTY(QIcon::State state READ state WRITE setState)
public:
	virtual void			setState(const QIcon::State in_eState){ m_eState = in_eState; update(); }
	virtual QIcon::State	state(){ return m_eState; }

	Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
public:
	virtual void	setOpacity(const qreal in_dOpacity){ m_dOpacity = in_dOpacity; update(); }
	virtual qreal	opacity(){ return m_dOpacity; }

	////////////////////////////////////////////////////////////////////
	//	メンバー変数
	////////////////////////////////////////////////////////////////////
protected:
	QIcon			m_clShowIcon;		//	表示する画像
	QIcon::State	m_eState;			//表示させる絵を決める。QIcon的な意味での状態
	qreal			m_dOpacity;			//透過度


	////////////////////////////////////////////////////////////////////
	//	生成・破棄
	////////////////////////////////////////////////////////////////////
public:
	ClRollOverToolBtn( QWidget  *a_pclParent = NULL );	//	引数つきコンストラクタ
	virtual ~ClRollOverToolBtn( void );					//	デストラクタ


	////////////////////////////////////////////////////////////////////
	//	オーバーライド
	////////////////////////////////////////////////////////////////////
protected:
	virtual void	paintEvent( QPaintEvent  *a_pclEvent );		//	描画処理
};

#endif // CLROLLOVERTOOLBTN_H
