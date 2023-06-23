//--------------------------------------------------------------------------------//
/*!
	@file	AbstDrawObject.h
	@brief	オブジェクト抽象クラスヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef ABSTDRAWOBJECT_H
#define ABSTDRAWOBJECT_H
#include "ClObjectKnob.h"
#include <QString>
#include <QPainter>
#include <QPointF>
#include <QStyleOptionGraphicsItem>
#include <QMenu>

class ClObjectKnob;
class ClObjectFrame;

//--------------------------------------------------------------------------------//
/*!
	@ingroup	groupDrawObjectPlugin
	@brief		描画オブジェクトの規定抽象クラス
	すべての描画オブジェクトはこのクラスを継承する。<br>
	描画オブジェクトは、描画オブジェクト生成プラグインにより生成され、描画ウィジェットに追加されることにより、<br>
	描画が可能になる。
	@attention	パラメータにはdefineで定義されたものしか設定することはできない。
*/
//--------------------------------------------------------------------------------//
class AbstDrawObject
{
public:
	AbstDrawObject();	//コンストラクタ
	virtual ~AbstDrawObject();	//デストラクタ
	virtual void copyParams(AbstDrawObject *pObjTarget);	//パラメータコピー

	//オブジェクト名取得
	virtual QString getName() const = 0;													//!< オブジェクト名取得

	//描画
	virtual void paint(QPainter *pclPainter,
					   const QStyleOptionGraphicsItem *pclOption, QWidget *pclWidget) = 0;	//!< オブジェクト化後の描画(Qtのオブジェクト描画)

	//生成用メソッド
	virtual bool init(QPointF clStartPoint, QMap<qint32, QVariant> clParams) = 0;			//!< 初期化
	virtual void newPoint(QPointF clPoint) = 0;												//!< 新しい座標通知
	virtual void endCreate() = 0;															//!< オブジェクト生成終了
	virtual QMap<qint32, QVariant> getCreateParams(){return m_clListParams;}				//!< 生成情報取得
	virtual QVariant getParam(qint32 lParamType) = 0;										//!< パラメータ取得
	virtual bool setParam(qint32 lParamType, QVariant clParam) = 0;							//!< パラメータ設定
	virtual bool isHit(QPointF clPoint);													//!< クリックヒット判定
	virtual bool isHit(QPointF clPoint1, QPointF clPoint2);									//!< ドラッグヒット判定

	//生成後用メソッド
	/*!
		@brief		サイズ変更(自分でサイズ調整)
		@param[in]	clRect : シーン上の領域
		@note		枠用
	*/
	virtual void resize(QRectF clRect){Q_UNUSED(clRect);}
	virtual bool select();																	//選択
	virtual void releaseSelect();															//選択解除
	virtual QList<ClObjectKnob*> getKnobs();												//つまみ取得
	virtual ClObjectFrame *getFrame();														//枠取得
	virtual void mousePress(QPointF clPoint){Q_UNUSED(clPoint);}							//!< マウス押下
	virtual void mouseDrag(QPointF clPoint){Q_UNUSED(clPoint);}								//!< マウスドラッグ
	virtual void mouseRelease(){}															//!< マウス押下終了

	/*!
		@brief		消しゴムで消せるか取得
		@retval		消せるならtrue
	*/
	virtual bool canDeleteEraser(){return true;}

	virtual void setFrame(ClObjectFrame *pFrame);	//枠設定

	//コンテキストメニュー処理
	/*!
		@brief		コンテキストメニュー取得
		@retval		表示するものがなければNULL
	*/
	QMenu *getContextMenu(){return m_pContextMenu;}
	/*!
		@brief		コンテキストメニュー処理
		@param[in]	pAction : 実行されたAction
	*/
	virtual void procContextMenuAction(QAction *pAction){Q_UNUSED(pAction);}

	//Undo用
	/*!
		@brief		領域取得
		@retval		領域
	*/
	virtual QRectF getRect(){return m_clRect;}
	/*!
		@brief		領域設定(そのまま設定)
		@param[in]	clRect : シーン上の領域
		@note		Undo用
	*/
	virtual void setRect(QRectF clRect){Q_UNUSED(clRect);}

protected:
	virtual QRectF getHitRect(QPointF clPoint);												//あたり判定で使う領域取得
	virtual QRectF getHitRect(QPointF clPoint1, QPointF clPoint2, qreal *pdParam1, qreal *pdParam2);	//あたり判定で使う領域取得
	/*!
		@brief		あたり判定
		@param[in]	clPoint : 相対座標
		@retval		あたったらtrue
	*/
	virtual bool checkHit(QPoint clPoint) = 0;
	virtual ClObjectFrame *createFrame();	//枠生成
	virtual QWidget *getDialogParentWidget();	//ダイアログの親ウィジェット取得

protected:
	QRectF					m_clRect;			//!< 領域
	ClObjectFrame*			m_pclFrame;			//!< 枠
	QMenu					*m_pContextMenu;	//!< コンテキストメニュー
	QSize					m_clSizeMinimum;	//!< 最小サイズ
	QSize					m_clSizeMaximum;	//!< 最大サイズ
	QMap<qint32, QVariant>	m_clListParams;		//!< パラメータリスト
};
//--------------------------------------------------------------------------------//
Q_DECLARE_METATYPE(QMenu*)
#endif //ABSTDRAWOBJECT_H
