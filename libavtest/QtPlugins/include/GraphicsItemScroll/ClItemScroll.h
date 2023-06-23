//--------------------------------------------------------------------------------//
/*!
	@file	ClItemScroll.h
	@brief	アイテムスクローラヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLITEMSCROLL_H
#define CLITEMSCROLL_H
#include <QGraphicsObject>
#include <QSequentialAnimationGroup>
#include <QPropertyAnimation>
#include <QPauseAnimation>

//--------------------------------------------------------------------------------//
/*!
	@brief	アイテムスクローラ
*/
//--------------------------------------------------------------------------------//
class ClItemScroll : public QGraphicsObject
{
	Q_OBJECT

	Q_PROPERTY(double itemPos READ getItemPos WRITE setItemPos)

public:
	//スクロール開始位置
	typedef enum
	{
		ePOS_TOP_LEFT = 0,	//!< 見える範囲
		ePOS_BOTTOM_RIGHT,	//!< 見えないところから見えなくなるまで
		ePOS_TOP_LEFT_SCROLLOUT		//!< 見える範囲から見えなくなるまで
	}eScrollStartPos;

	//アニメーション動作
	typedef enum
	{
		eMODE_AUTO = 0,	//!< 自動
		eMODE_HOVER,	//!< マウスが上にいるとき
		eMODE_MANUAL	//!< 手動
	}eAnimationMode;

public:
	ClItemScroll(QSizeF clSize, QGraphicsItem *pclParent = NULL);	//コンストラクタ
	ClItemScroll(qreal dWidth, qreal dHeight, QGraphicsItem *pclParent = NULL);	//コンストラクタ
	ClItemScroll(QGraphicsItem *pclParent = NULL);	//コンストラクタ
	~ClItemScroll();	//デストラクタ

	/*!
		@brief	領域取得
		@retval	領域
	*/
	QRectF boundingRect() const{return QRectF(QPointF(0, 0), m_clSize);}
	/*!
		@brief	描画
	*/
	void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*){}

	void setSize(QSizeF clSize);	//サイズ設定

	void setScrollItem(QGraphicsItem *pclItem);	//スクロールするアイテム設定
	/*!
		@brief	スクロールするアイテム取得
		@retval	スクロールするアイテム
	*/
	QGraphicsItem *getScrollItem(){return m_pclScrollItem;}
	QGraphicsItem *takeScrollItem();	//スクロールするアイテムはずして取得

	/*!
		@brief		スクロール方向設定
		@param[in]	eOrientation :　スクロール方向
	*/
	void setScrollOrientation(Qt::Orientation eOrientation){m_eScrollOrientation = eOrientation;}
	/*!
		@brief		スクロール方向取得
		@retval		スクロール方向
	*/
	Qt::Orientation getScrollOrientation(){return m_eScrollOrientation;}

	void setItemPos(double dPos);	//スクロールするアイテム座標設定
	double getItemPos();			//スクロールするアイテム座標取得

	void setScrollStartPos(eScrollStartPos ePos);	//スクロール開始位置設定
	void setAnimationMode(eAnimationMode eMode);	//アニメーション動作設定

	/*!
		@brief		スクロールにかける時間設定
		@param[in]	iDuration : 時間
	*/
	void setScrollDuration(int iDuration){m_clScrollAnimation.setDuration(iDuration);}
	/*!
		@brief		スクロール後の待ち時間設定
		@param[in]	iDuration : 時間
	*/
	void setScrollPauseDuration(int iDuration){m_clWaitAnimation.setDuration(iDuration);}
	void setIsLoopInfinity(bool bIsLoop);	//無限ループ設定

protected:
	void initialize(QSizeF clSize);	//初期化

	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent*);	//マウスが領域内に入ったイベント
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent*);	//マウスが領域外に出たイベント

	void setAnimationValue();	//アニメーション設定

public slots:
	void slotStartAnimation();	//アニメーション開始

signals:
	void sigEndAnimation();	//アニメーション終了通知

protected:
	QSizeF m_clSize;	//!< サイズ
	QGraphicsItem *m_pclScrollItem;	//!< スクロールするアイテム
	Qt::Orientation m_eScrollOrientation;	//!< スクロールする向き

	QSequentialAnimationGroup m_clAnimation;	//!< アニメーション
	QPropertyAnimation m_clScrollAnimation;	//!< 動作アニメーション
	QPauseAnimation m_clWaitAnimation;	//!< 待ちアニメーション

	eScrollStartPos m_eStartPos;	//!< スクロール開始位置
	eAnimationMode m_eMode;	//!< アニメーション動作
};
//--------------------------------------------------------------------------------//
#endif
