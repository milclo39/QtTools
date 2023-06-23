//--------------------------------------------------------------------------------//
/*!
	@file	ClGraphicsSlider.h
	@brief	スライダヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLGRAPHICSSLIDER_H
#define CLGRAPHICSSLIDER_H
#include <QGraphicsRectItem>

class ClGraphicsSliderKnob;
//--------------------------------------------------------------------------------//
/*!
	@brief	スライダー
*/
//--------------------------------------------------------------------------------//
class ClGraphicsSlider : public QObject, public QGraphicsRectItem
{
	Q_OBJECT

	Q_PROPERTY(Qt::Orientation orientation READ getOrientation WRITE setOrientation)
	Q_PROPERTY(int minimum READ getMinimum WRITE setMinimum)
	Q_PROPERTY(int maximum READ getMaximum WRITE setMaximum)
	Q_PROPERTY(int value READ getValue WRITE setValue)
	Q_PROPERTY(int pagestep READ getPageStep WRITE setPageStep)
	Q_PROPERTY(bool tracking READ getTracking WRITE setTracking)
	Q_PROPERTY(int wheelstep READ getWheelStep WRITE setWheelStep)

public:
	ClGraphicsSlider(QGraphicsItem *pclParent = NULL, Qt::Orientation eOrientation = Qt::Vertical);	//コンストラクタ
	ClGraphicsSlider(Qt::Orientation eOrientation);	//コンストラクタ
	~ClGraphicsSlider();	//デストラクタ

	void setOrientation(Qt::Orientation eOrientation);	//向き設定
	/*!
		@brief		向き取得
		@retval		向き
	*/
	Qt::Orientation getOrientation(){return m_eOrientation;}


	void setMinimum(int iMinimum);	//最小値設定
	/*!
		@brief		最小値取得
		@retval		最小値
	*/
	int getMinimum(){return m_iMinimum;}

	void setMaximum(int iMaximum);	//最大値設定
	/*!
		@brief		最大値取得
		@retval		最大値
	*/
	int getMaximum(){return m_iMaximum;}

	void setValue(int iValue);	//現在値設定
	/*!
		@brief		現在値取得
		@param[in]	現在値
	*/
	int getValue(){return changeValue();}

	void setPageStep(int iPageStep);	//ページステップ設定
	/*!
		@brief		ページステップ取得
		@retval		ページステップ
	*/
	int getPageStep(){return m_iPageStep;}

	/*!
		@brief		値変更通知設定
		@param[in]	bIsTracking : trueならば変更されるたびに通知、falseならばドラッグ終了時に通知
	*/
	void setTracking(bool bIsTracking){m_bIsTracking = bIsTracking;}
	/*!
		@brief		値変更通知取得
		@retval		trueならば変更されるたびに通知、falseならばドラッグ終了時に通知
	*/
	bool getTracking(){return m_bIsTracking;}

	/*!
		@brief		ホイールステップ設定
		@param[in]	iStep : ホイールステップ
	*/
	void setWheelStep(int iWheelStep){m_iWheelStep = iWheelStep;}
	/*!
		@brief		ホイールステップ取得
		@retval		ホイールステップ
	*/
	int getWheelStep(){return m_iWheelStep;}

	void setKnob(ClGraphicsSliderKnob *pclKnob);	//つまみ設定
	void setKnobPen(const QPen &clPen);	//つまみのペン設定
	void setKnobBrush(const QBrush &clBrush);	//つまみのブラシ設定

	void setRect(const QRectF &rect);	//領域設定

	/*!
		@brief		サイズ設定
		@param[in]	clSize : サイズ
	*/
	void setSize(QSizeF clSize){setRect(QRectF(QPointF(0, 0), clSize));}
	/*!
		@brief		サイズ設定
		@param[in]	dWidth : 幅
		@param[in]	dHeight : 高さ
	*/
	void setSize(qreal dWidth, qreal dHeight){setSize(QSizeF(dWidth, dHeight));}

	virtual bool eventFilter(QObject *, QEvent *pclEvent);	//イベントフィルタ

protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *pclEvent);	//マウス押下イベント
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *pclEvent);	//マウス移動イベント
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *pclEvent);	//マウス押下終了イベント
	virtual void wheelEvent(QGraphicsSceneWheelEvent *pclEvent);	//ホイールイベント
	virtual void timerEvent(QTimerEvent *pclEvent);	//タイマイベント

	void adjustRect();	//領域調整
	int changeValue();	//つまみ座標から値に変換

protected slots:
	void slotValueChangedFromKnob();	//つまみからの値変更通知

signals:
	/*!
		@brief		値変更通知
		@param[in]	iValue : 現在値
	*/
	void sigValueChange(int iValue);

protected:
	ClGraphicsSliderKnob *m_pclKnob;	//!< つまみ
	Qt::Orientation m_eOrientation;	//!< 向き
	int m_iMinimum;	//!< 最小値
	int m_iMaximum;	//!< 最大値
	int m_iValue;	//!< 現在値
	int m_iPageStep;	//!< ページステップ
	bool m_bIsTracking;	//!< 値変更通知設定
	int m_iWheelStep;	//!< ホイールステップ

	int m_iTimer;	//!< タイマID
	bool m_bIsTimerFirst;	//!< 初回タイマフラグ
	QPointF m_clMosuePos;	//!< マウス押下座標
};
//--------------------------------------------------------------------------------//
#endif
