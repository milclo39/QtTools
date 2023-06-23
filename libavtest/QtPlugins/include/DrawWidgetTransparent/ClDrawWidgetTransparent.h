//--------------------------------------------------------------------------------//
/*!
	@file	ClDrawWidgetTransparent.h
	@brief	透明描画ウィジェットヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLDRAWWIDGETTRANSPARENT_H
#define CLDRAWWIDGETTRANSPARENT_H
#include "DrawObject/IDrawObjectCreator.h"
#include "ClDrawUndo.h"
#include "ElmoEvent/ClElmoEvent.h"
#include "DrawObject/ClObjectKnob.h"
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMap>
#include <QVariant>

#include <QLabel>

#if 1
//線描画時にカメラ映像を毎回1fpsにする場合
#define	ALWAYS_CHANGE_FPS
#else
//描画時のfps切替をしばらく描画がなかったときだけ行う場合
#include <QTimer>
#endif

//--------------------------------------------------------------------------------//
/*!
	@brief		透明描画ウィジェット
*/
//--------------------------------------------------------------------------------//
class ClDrawWidgetTransparent : public QGraphicsView
{
	Q_OBJECT

public:
	typedef enum
	{
		eDRAW_OBJECT = 0,
		eDRAW_SELECT,
		eDRAW_CAPTURE
	}eDrawMode;

public:
	ClDrawWidgetTransparent(qint32 lLayerCount, QWidget *pclParent = NULL);	//コンストラクタ
	ClDrawWidgetTransparent(QWidget *pclParent = NULL);	//コンストラクタ
	~ClDrawWidgetTransparent();	//デストラクタ

	void setScene(QGraphicsScene *pclScene);	//シーン設定
	void setUseUndo(bool bIsUse);	//Undo使用設定

	/*!
		@brief		削除Undoに追加しないアイテムの親アイテム設定
		@param[in]	clExcldeList : このリストの子アイテムは削除Undoに追加されない
	*/
	void setExcludeRemoveParentItem(QList<QGraphicsItem*> clExcludeList){m_clExcludeRemoveList = clExcludeList;}

	virtual bool event(QEvent *pclEvent);			//イベント

	//オブジェクト生成
	virtual bool startCreateObject(QPointF clPos, QMap<qint32, QVariant> clParams);	//オブジェクト生成 開始処理
	virtual bool endCreateObject();													//オブジェクト生成 終了処理

	//選択
	bool isHitRect(QGraphicsItem* clItem);									//範囲選択用簡易あたり判定

protected:
	virtual void initialize(qint32 lLayerCount);	//初期化
	virtual void paintEvent(QPaintEvent *pclEvent);	//描画イベント
	virtual void resizeEvent(QResizeEvent *pclEvent);	//サイズ変更イベント
	virtual void tabletEvent(QTabletEvent *pclEvent);	//タブレットイベント
#ifdef Q_OS_MAC
	virtual void showEvent(QShowEvent*);	//表示イベント
	virtual void timerEvent(QTimerEvent *pclEvent);	//タイマイベント
#endif //Q_OS_MAC
	virtual void execLeftPress(QPointF clPoint);	//左ボタン押下処理
	virtual void execLeftRelease(QPointF clPoint);	//左ボタン押下終了処理
	virtual void execButtonPress(QPointF clPoint);	//左以外ボタン押下処理
	virtual void execButtonRelease(QPointF clPoint);	//左以外ボタン押下終了処理
	virtual void execMoveInput(QPointF clPoint);	//入力移動処理

	virtual void eraseObject(QPointF clPoint, QGraphicsItem *pclParent = NULL);		//オブジェクト削除
	virtual void setDrawLayer(int iLayerNumber);	//レイヤ設定
	virtual void clearSelect();	//選択解除
	virtual void setObjectZValue(QGraphicsItem *pItem);	//オブジェクトのZオーダ設定
	virtual int getPenPressure(void);				//筆圧取得

public slots:
	void slotSetObjectCreators(QList<IDrawObjectCreator*> clList);	//オブジェクトファクトリ設定
	void slotAddObjectCreator(IDrawObjectCreator *pclCreator);		//オブジェクトファクトリ追加
	void slotRemoveObjectCreator(QString strObject);				//オブジェクトファクトリ削除
	void slotClearObjectCreator();									//オブジェクトファクトリクリア

	void slotSetCanTouch(bool bCanTouch);	//触れる設定
	void slotSetEraseObject(bool bIsEraseObject);	//オブジェクト消し設定
	void slotSetMode(eDrawMode eMode);		//描画モード設定
	void slotSetObject(QString strObject);	//オブジェクト設定
	void slotSetObjectParam(qint32 lType, QVariant clParam);	//オブジェクトパラメータ設定
	void slotClearObject();	//オブジェクトクリア

	void slotUndo();	//Undo
	void slotRedo();	//Redo

	//オブジェクト操作
	void slotObjectSelected(bool bIsSelected);	//!< オブジェクトの選択状態通知(false:非選択,true:選択)
	void slotObjectCopy();						//!< オブジェクトの複製要求
	void slotObjectMoveFront();					//!< オブジェクトの前面移動
	void slotObjectMoveBack();					//!< オブジェクトの背面移動

	//状態通知
	void slotSelectedObject(bool bIsSelected){Q_UNUSED(bIsSelected)}	//!< オブジェクト選択通知

signals:
	/*!
		@brief		Undo可能状態変更通知
		@param[in]	bCanUndo : trueならばUndo可能になった
	*/
	void sigCanUndoChanged(bool bCanUndo);
	/*!
		@brief		Redo可能状態変更通知
		@param[in]	bCanRedo : trueならばRedo可能になった
	*/
	void sigCanRedoChanged(bool bCanRedo);

	void sigDrawing(bool bIsDrawing);			//!< 描画中かどうかを通知。trueで描画中、falseで描画中じゃなくなった

	//オブジェクト操作
	void sigObjectSelected(bool bIsSelected);	//!< オブジェクトの選択状態通知(false:非選択,true:選択)
	void sigObjectCopy();						//!< オブジェクトの複製要求
	void sigObjectMoveFront();					//!< オブジェクトの前面移動
	void sigObjectMoveBack();					//!< オブジェクトの背面移動

	//状態通知
	void sigSelectedObject(bool bIsSelected);	//!< オブジェクト選択通知

protected:
	bool m_bCanTouch;	//!< 触れる設定
	eDrawMode m_eMode;	//!< 描画モード
	QList<IDrawObjectCreator*> m_clCreatorList;	//!< オブジェクトファクトリリスト
	IDrawObjectCreator *m_pICurrentCreator;		//!< 現在のオブジェクトファクトリ
	AbstDrawObject *m_pclCreatingObject;		//!< 生成中オブジェクト
	QList<AbstDrawObject*> m_clSelectingObject;	//!< 選択中オブジェクト
	ClObjectKnob *m_pclSelectingKnob;			//!< 選択中つまみ
	QMap<qint32, QVariant> m_clObjectParam;	//!< オブジェクトパラメータ
	QPointF m_clBeforeMousePos;	//!< 前回のマウス入力座標
	QPointF m_clPreDrawPos;	//!< 前回の描画点
	bool m_bIsEraseObject;	//!< オブジェクト消しフラグ
	bool m_bIsErasing;	//!< 消し中フラグ
	bool m_bIsLeftPress;	//!< 左ドラッグ中
	bool m_bIsRightPress;	//!< 右ドラッグ中
	bool m_bIsMidPress;		//!< 中ドラッグ中
	qreal m_dZvalue;	//!< 次のアイテムのZオーダ
	QRectF m_clSelectRect;	//!< 選択領域
	QPointF m_clSelectStartPoint;	//!< 選択開始座標
	bool m_bIsPressedWidget;	//!< ウィジェット上で押下されたフラグ
	//Undo関係
	bool m_bIsUseUndo;								//!< Undo使用フラグ
	ClDrawUndo *m_pclUndo;							//!< Undo管理クラス
	QList<QGraphicsItem*> m_clRemoveItemList;		//!< 削除アイテムリスト
	QList<QGraphicsItem*> m_clRemoveItemListOther;	//!< 削除アイテムリスト(削除Undoに追加しないアイテム)
	QList<QGraphicsItem*> m_clExcludeRemoveList;	//!< 削除Undoに追加しないアイテムの親アイテムリスト
	QUndoCommand *m_pclUndoCommand;	//!< 一時記憶用Undoコマンド
    int m_iPenPresser;                              //!< 筆圧
	int m_iPenPresserTE;                            //!< tabletEventで取得した筆圧
#ifdef Q_OS_MAC
	int m_iSlowShowTimer;	//!< 表示遅らせるタイマID
	QList<QGraphicsItem*> m_clSlowShowItemList;	//!< 表示遅らせるアイテムリスト

	bool m_bIsMouseButtonPressLeft;			//!< QMouseEventで左クリックされたフラグ
	bool m_bIsMouseButtonPressRight;		//!< QMouseEventで右クリックされたフラグ
	bool m_bIsMouseButtonPressMid;			//!< QMouseEventで中クリックされたフラグ
#endif //Q_OS_MAC

#ifndef ALWAYS_CHANGE_FPS
	//=======================================================================//
	//	FPS制御
	//=======================================================================//
	bool	m_bIsSendChangeFPS;		//!< FPSを変更したかを記録するためのフラグ
	QTimer	m_clTimerChangeFPS;		//!< FPSを変更している場合にしばらく描画されないかを待つためのタイマー
private slots:
	void	slotTimeoutChangeFPS();	//!< FPS制御用タイマーがタイムアウトした時の処理
#endif

	//=======================================================================//
	//	スタンプ/画像挿入
	//=======================================================================//
public:
	//スタンプ
	bool addItemStamp(QString clStrPath, QRect clRect);

	//画像挿入
	bool addItemPicture(QString clStrPath, QRect clRect);
	void endCreateItemPicture();
	void changeRectPicture(QRectF clRect);

	//=======================================================================//
	//	範囲指定キャプチャ
	//=======================================================================//
protected:
	QRectF m_clCaptureRect;	//!< キャプチャ領域
	QPointF m_clCaptureStartPoint;	//!< 選択開始座標

public:
	virtual QRectF getCaptureRect(){return m_clCaptureRect;}

	//=======================================================================//
	//	教育支援モード
	//=======================================================================//
protected:
	bool m_bIsCreateFrame;			//!< オブジェクト生成時のFrame生成
	bool m_bIsVisibleSelectRect;	//!< 複数選択有効/無効フラグ

public:
	void setCreateFrameSetting(bool bFlg){m_bIsCreateFrame = bFlg;}			//true:オブジェクト生成時にFrame作らない,false:Frame作る
	void setVisibleSelectRect(bool bFlg)
	{
		clearSelect();														//選択解除
		m_bIsVisibleSelectRect = bFlg;										//true:複数選択有効,false:無効
	}
};
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@brief		ビューポート
*/
//--------------------------------------------------------------------------------//
class ClDrawViewPort : public QWidget
{
public:
	ClDrawViewPort(ClDrawWidgetTransparent *pclDraw);	//コンストラクタ

	virtual bool event(QEvent *pclEvent);	//イベント

protected:
	ClDrawWidgetTransparent *m_pclDraw;	//!< 描画ウィジェット
};
//--------------------------------------------------------------------------------//
#endif
