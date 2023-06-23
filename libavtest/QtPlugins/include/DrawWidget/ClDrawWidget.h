//--------------------------------------------------------------------------------//
/*!
	@file	ClDrawWidget.h
	@brief	描画ウィジェット

	@author	株式会社エルモ社　映像技術第二部
*/
//--------------------------------------------------------------------------------//
#ifndef CLDRAWWIDGET_H
#define CLDRAWWIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#ifdef USE_QPIXMAP
#include <QPixmap>
#else
#include <QImage>
#endif
#include <QPainter>
#include <QScrollBar>
#include "AbstObject.h"

#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QUndoCommand>
#include <QUndoStack>

class ClGraphicUndo;

//--------------------------------------------------------------------------------//
//動作モード
typedef enum
{
	ePAINT_DIRECT = 0,	//直接描画
	ePAINT_OBJECT		//オブジェクト
}ePaintMode;

//描画モード
typedef enum
{
	eDRAW_FREELINE = 0,	//自由線
	eDRAW_LINE,			//直線
	eDRAW_RECT,			//四角
	eDRAW_CIRCLE,		//円
	eDRAW_ERASER,		//消しゴム
	eDRAW_SCROLL,		//スクロール
	eDRAW_ZOOMIN,		//拡大
	eDRAW_ZOOMOUT,		//縮小
	eDRAW_SELECT,		//選択

	eDRAW_TERM
}eDrawMode;

//回転
typedef enum
{
	eROTATE_RIGHT = 90,		//右回転
	eROTATE_LEFT = -90,		//左回転
	eROTATE_UPDOWN = 180	//上下反転(点対称回転)
}eRotateValue;

//カーソルの種類
typedef enum
{
	eCURSOR_FREELINE = 0,	//自由線
	eCURSOR_LINE,			//直線
	eCURSOR_RECT,			//四角
	eCURSOR_CIRCLE,			//円
	eCURSOR_ERASER,			//消しゴム
	eCURSOR_HAND_OPEN,		//スクロール(ドラッグ中)
	eCURSOR_HAND_CLOSE,		//スクロール(非ドラッグ中)
	eCURSOR_ZOOMIN,			//拡大
	eCURSOR_ZOOMOUT,		//縮小
	eCURSOR_SELECT,			//選択

	eCURSOR_TERM
}eCursorType;

//--------------------------------------------------------------------------------//
/*!
	@class	ClUndoStack
	@brief	アンドゥ管理クラス
*/
//--------------------------------------------------------------------------------//
class ClUndoStack : public QUndoStack
{
	Q_OBJECT

public:
	ClUndoStack(QObject *pclParent = NULL);		//コンストラクタ
	virtual void push(QUndoCommand *pclCmd);	//コマンド追加
	virtual void clear();	//コマンドクリア

public slots:
	virtual void undo();	//アンドゥ実行
	virtual void redo();	//リドゥ実行

signals:
	void sigCanUndo();	//アンドゥ可能シグナル
	void sigCantUndo();	//アンドゥ不可シグナル
	void sigCanRedo();	//リドゥ可能シグナル
	void sigCantRedo();	//リドゥ不可シグナル
	void sigSetUndoGraphic(qint32, qint32, qint32);	//アンドゥで画像調整した

protected:
	bool m_bCanUndo;	//アンドゥ可能フラグ
	bool m_bCanRedo;	//リドゥ可能フラグ
};
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@class	ClDrawArea
	@brief	描画制御クラス
*/
//--------------------------------------------------------------------------------//
class ClDrawArea : public QGraphicsScene
{
	Q_OBJECT

	//カーソル画像情報
	typedef struct
	{
		QPixmap clCursorMap;	//画像
		QPoint	clHotPoint;		//座標位置
	}stCursorMapInfo;

public:
	ClDrawArea();		//コンストラクタ
	~ClDrawArea();		//デストラクタ
	void setDrawMode(eDrawMode eMode);	//描画モード設定
	void setSize(QSize clSize);		//サイズ設定
	void areaClear();				//全消去

	//ペン、ブラシ設定
	void setColor(QColor clColor){											//色
		m_clPen.setColor(clColor);
		m_clBrush.setColor(clColor);
	}
	void setPenWidth(qreal dWidth){m_clPen.setWidth(dWidth);}				//ペン幅
	void setPenStyle(Qt::PenStyle eStyle){m_clPen.setStyle(eStyle);}		//ペン種
	void setBrushStyle(Qt::BrushStyle eStyle){m_clBrush.setStyle(eStyle);}	//ブラシ種
	void setSolid(bool bIsSolid){m_bIsSolid = bIsSolid;}					//塗りつぶし
	void setEraserWidth(qreal dWidth){m_clErasePen.setWidth(dWidth);}		//消しゴムサイズ
	void setEraserStyle(Qt::PenCapStyle eStyle){m_clErasePen.setCapStyle(eStyle);}	//消しゴムタイプ設定
	void setCursorType(eCursorType eType, QCursor clCursor){m_pclCursorList[eType] = clCursor;}	//カーソル設定
	void setImage(QImage clImage);											//画像設定
	void cleanPixmap();														//画像消去
	void setCenterImage(QImage clImage);									//特殊状態画像設定
	void adjust(QSize clSize);												//サイズ最適化
	void rotate(qint32 lValue, bool bIsUndo = false);						//回転
	void zoomIn(QPointF clPointCenter = QPointF(-1, -1));					//拡大
	void zoomOut(QPointF clPointCenter = QPointF(-1, -1));					//縮小
	void save(QString clFileName, const char *pcFormat = NULL);				//保存
	QImage getSaveImage();													//保存画像取得
	void procMouseLeave();													//カーソルが領域外へでたときの処理
	QSizeF getScaledRatio();												//描画画像と描画領域の比率取得
	QPixmap getCursorMap(eCursorType eType){return m_pclCursorMapList[eType].clCursorMap;}	//カーソル画像取得
	void setCursorMap(eCursorType eType, const QPixmap clMap);								//カーソル画像設定
	QPoint getCursorPoint(eCursorType eType){return m_pclCursorMapList[eType].clHotPoint;}	//カーソルヒット座標取得
	void setCursorPoint(eCursorType eType, const QPoint clPoint);							//カーソルヒット座標設定
	void undo();														//アンドゥ
	void redo();														//リドゥ
	void clearUndoList(){m_clUndoStack.clear();}						//アンドゥリストクリア
	void setGraphic(qint32 lHue, qint32 lContrast, qint32 lBrightness);	//画像調整
	void setCanvasSize(QSize clSize);									//キャンバスサイズ設定
	void setBackColor(qint32 lVal);										//背景色設定
	void startEditGraphic(qint32 lHue, qint32 lContrast, qint32 lBrightness);	//画像調整開始
	void endEditGraphic(qint32 lHue, qint32 lContrast, qint32 lBrightness);		//画像調整終了
	void showScrollbar();												//スクロールバー表示
	void hideScrollbar();												//スクロールバー非表示
	void clearIsEdit(){m_bIsEdit = false;};								//編集フラグ下ろす
	void procButtonPress(QPointF clPoint);								//左ボタン押下
	void procButtonRelease();											//左押下終了
	void procInput(QPointF clPoint);									//入力処理
	eDrawMode getDrawMode(){return m_eDrawMode;}						//描画モード取得

protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *pclEvent);	//マウス押下イベント
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *pclEvent);	//マウスドラッグイベント
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *pclEvent);	//マウス押下終了イベント
	virtual void drawForeground(QPainter *painter, const QRectF &rect);	//前面レイヤ描画
	virtual void drawBackground(QPainter *painter, const QRectF &rect);	//背面レイヤ描画

	void drawEraserFore(QPointF clNewPoint);			//前面レイヤへの消しゴム描画
	void drawEraser(QPointF clNewPoint);				//消しゴム描画
	void scroll(QGraphicsSceneMouseEvent *pclEvent);	//スクロール

#ifdef USE_QPIXMAP
	QRectF getScaledRect(QPixmap clPixmap, QRectF clRectSource, bool bIsAdjustSource = false);		//スケーリングした領域計算
	QPointF getScaledPoint(QPixmap clPixmap, QPointF clPointSource, bool bAdjustSource = false);	//スケーリングした座標計算
#else
	QRectF getScaledRect(QImage clPixmap, QRectF clRectSource, bool bIsAdjustSource = false);		//スケーリングした領域計算
	QPointF getScaledPoint(QImage clPixmap, QPointF clPointSource, bool bAdjustSource = false);		//スケーリングした座標計算
#endif

signals:
	void sigStartEdit();	//編集開始
	void sigEndEdit();		//編集終了
	void sigZoomMax();		//最大ズーム
	void sigZoomIn();		//拡大
	void sigZoomNone();		//ズーム解除
	void sigZoomOut();		//縮小
	void sigCanUndo();		//アンドゥ可能
	void sigCantUndo();		//アンドゥ不可
	void sigCanRedo();		//リドゥ可能
	void sigCantRedo();		//リドゥ不可
	void sigSetUndoGraphic(qint32, qint32, qint32);	//アンドゥで画像調整された

protected:
	ePaintMode	m_ePaintMode;				//動作モード
	eDrawMode	m_eDrawMode;				//描画モード
	QSize		m_clSize;					//自分のサイズ
	bool		m_bIsAdjust;				//サイズ変更時使用
	bool		m_bIsZoom;					//ズーム中フラグ(背景画像設定なし時に使用)
	bool		m_bIsZoomMax;				//最大ズームフラグ
	bool		m_bIsInArea;				//領域内にいるフラグ(消しゴム時使用)
#ifdef USE_QPIXMAP
	QPixmap		m_clForeGroundMap;			//前面レイヤ画像(生成中オブジェクト描画)
	QPixmap		m_clBackGroundMap;			//背面レイヤ画像(生成済みオブジェクト描画)
	QPixmap		m_clImage;					//背景画像(描画用)
	QPixmap		m_clSrcImage;				//背景画像(元画像/画像編集で使用)
#else
	QImage		m_clForeGroundMap;			//前面レイヤ画像(生成中オブジェクト描画)
	QImage		m_clBackGroundMap;			//背面レイヤ画像(生成済みオブジェクト描画)
	QImage		m_clImage;					//背景画像(描画用)
	QImage		m_clSrcImage;				//背景画像(元画像/画像編集で使用)
	QImage		m_clCenterImage;			//特殊状態画像
#endif
	bool		m_bIsSetImage;				//背景画像設定フラグ
	AbstObject	*m_pclCreatingItem;			//生成中アイテム
	QList<AbstObject*> m_clItemList;		//アイテムリスト(オブジェクトモード時使用)
	QRectF		m_clBeforeRepaintRect;		//前回描画した領域(直線などで使用)
	QPen		m_clPen;					//描画ペン
	QBrush		m_clBrush;					//描画ブラシ
	bool		m_bIsSolid;					//塗りつぶしフラグ
	QPointF		m_clEraserPoint;			//消しゴム用座標
	QPen		m_clErasePen;				//消しゴム用ペン
	QCursor		m_pclCursorList[eCURSOR_TERM];		//カーソルリスト
	stCursorMapInfo	m_pclCursorMapList[eCURSOR_TERM];	//カーソルの画像リスト
	ClUndoStack	m_clUndoStack;				//アンドゥリスト
	const QUndoCommand *m_pclUndoCommand;	//アンドゥコマンド記憶用
	qint32		m_lBackColor;				//背景色
	//編集開始時の設定
	qint32		m_lHue;						//色相
	qint32		m_lContrast;				//コントラスト
	qint32		m_lBrightness;				//明るさ
	//直前の設定
	qint32		m_lBeforeHue;				//色相
	qint32		m_lBeforeContrast;			//コントラスト
	qint32		m_lBeforeBrightness;		//明るさ
	ClGraphicUndo	*m_pclGraphicCommand;	//画像調整アンドゥコマンド
	qint32		m_lShowScrollbarCount;		//表示中スクロールバー数
	bool		m_bIsEdit;					//編集フラグ
};
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@class	ClDrawWidget
	@brief	描画ウィジェット
*/
//--------------------------------------------------------------------------------//
class ClDrawWidget : public QGraphicsView
{
	Q_OBJECT

	//カーソル画像情報プロパティ
	Q_PROPERTY(QPixmap freelineCursorMap READ getFreelineCursorMap WRITE setFreelineCursorMap)
	Q_PROPERTY(QPixmap lineCursorMap READ getLineCursorMap WRITE setLineCursorMap)
	Q_PROPERTY(QPixmap rectCursorMap READ getRectCursorMap WRITE setRectCursorMap)
	Q_PROPERTY(QPixmap circleCursorMap READ getCircleCursorMap WRITE setCircleCursorMap)
	Q_PROPERTY(QPixmap eraserCursorMap READ getEraserCursorMap WRITE setEraserCursorMap)
	Q_PROPERTY(QPixmap handopenCursorMap READ getHandopenCursorMap WRITE setHandopenCursorMap)
	Q_PROPERTY(QPixmap handcloseCursorMap READ getHandcloseCursorMap WRITE setHandcloseCursorMap)
	Q_PROPERTY(QPixmap zoominCursorMap READ getZoominCursorMap WRITE setZoominCursorMap)
	Q_PROPERTY(QPixmap zoomoutCursorMap READ getZoomoutCursorMap WRITE setZoomoutCursorMap)
	Q_PROPERTY(QPixmap selectCursorMap READ getSelectCursorMap WRITE setSelectCursorMap)
	//ヒットポイントプロパティ
	Q_PROPERTY(QPoint freelineCursorPoint READ getFreelineCursorPoint WRITE setFreelineCursorPoint)	
	Q_PROPERTY(QPoint lineCursorPoint READ getLineCursorPoint WRITE setLineCursorPoint)
	Q_PROPERTY(QPoint rectCursorPoint READ getRectCursorPoint WRITE setRectCursorPoint)	
	Q_PROPERTY(QPoint circleCursorPoint READ getCircleCursorPoint WRITE setCircleCursorPoint)
	Q_PROPERTY(QPoint eraserCursorPoint READ getEraserCursorPoint WRITE setEraserCursorPoint)
	Q_PROPERTY(QPoint handopenCursorPoint READ getHandopenCursorPoint WRITE setHandopenCursorPoint)
	Q_PROPERTY(QPoint handcloseCursorPoint READ getHandcloseCursorPoint WRITE setHandcloseCursorPoint)
	Q_PROPERTY(QPoint zoominCursorPoint READ getZoominCursorPoint WRITE setZoominCursorPoint)
	Q_PROPERTY(QPoint zoomoutCursorPoint READ getZoomoutCursorPoint WRITE setZoomoutCursorPoint)
	Q_PROPERTY(QPoint selectCursorPoint READ getSelectCursorPoint WRITE setSelectCursorPoint)

	//カーソルプロパティメソッド
public:
	//自由線
	QPixmap getFreelineCursorMap(){return m_clScene.getCursorMap(eCURSOR_FREELINE);}
	void setFreelineCursorMap(const QPixmap &clMap){m_clScene.setCursorMap(eCURSOR_FREELINE, clMap);}
	QPoint getFreelineCursorPoint(){return m_clScene.getCursorPoint(eCURSOR_FREELINE);}
	void setFreelineCursorPoint(const QPoint clPoint){m_clScene.setCursorPoint(eCURSOR_FREELINE, clPoint);}
	//直線
	QPixmap getLineCursorMap(){return m_clScene.getCursorMap(eCURSOR_LINE);}
	void setLineCursorMap(const QPixmap &clMap){m_clScene.setCursorMap(eCURSOR_LINE, clMap);}
	QPoint getLineCursorPoint(){return m_clScene.getCursorPoint(eCURSOR_LINE);}
	void setLineCursorPoint(const QPoint clPoint){m_clScene.setCursorPoint(eCURSOR_LINE, clPoint);}
	//四角
	QPixmap getRectCursorMap(){return m_clScene.getCursorMap(eCURSOR_RECT);}
	void setRectCursorMap(const QPixmap &clMap){m_clScene.setCursorMap(eCURSOR_RECT, clMap);}
	QPoint getRectCursorPoint(){return m_clScene.getCursorPoint(eCURSOR_RECT);}
	void setRectCursorPoint(const QPoint clPoint){m_clScene.setCursorPoint(eCURSOR_RECT, clPoint);}
	//円
	QPixmap getCircleCursorMap(){return m_clScene.getCursorMap(eCURSOR_CIRCLE);}
	void setCircleCursorMap(const QPixmap &clMap){m_clScene.setCursorMap(eCURSOR_CIRCLE, clMap);}
	QPoint getCircleCursorPoint(){return m_clScene.getCursorPoint(eCURSOR_CIRCLE);}
	void setCircleCursorPoint(const QPoint clPoint){m_clScene.setCursorPoint(eCURSOR_CIRCLE, clPoint);}
	//消しゴム
	QPixmap getEraserCursorMap(){return m_clScene.getCursorMap(eCURSOR_ERASER);}
	void setEraserCursorMap(const QPixmap &clMap){m_clScene.setCursorMap(eCURSOR_ERASER, clMap);}
	QPoint getEraserCursorPoint(){return m_clScene.getCursorPoint(eCURSOR_ERASER);}
	void setEraserCursorPoint(const QPoint clPoint){m_clScene.setCursorPoint(eCURSOR_ERASER, clPoint);}
	//スクロール(非ドラッグ)
	QPixmap getHandopenCursorMap(){return m_clScene.getCursorMap(eCURSOR_HAND_OPEN);}
	void setHandopenCursorMap(const QPixmap &clMap){m_clScene.setCursorMap(eCURSOR_HAND_OPEN, clMap);}
	QPoint getHandopenCursorPoint(){return m_clScene.getCursorPoint(eCURSOR_HAND_OPEN);}
	void setHandopenCursorPoint(const QPoint clPoint){m_clScene.setCursorPoint(eCURSOR_HAND_OPEN, clPoint);}
	//スクロール(ドラッグ)
	QPixmap getHandcloseCursorMap(){return m_clScene.getCursorMap(eCURSOR_HAND_CLOSE);}
	void setHandcloseCursorMap(const QPixmap &clMap){m_clScene.setCursorMap(eCURSOR_HAND_CLOSE, clMap);}
	QPoint getHandcloseCursorPoint(){return m_clScene.getCursorPoint(eCURSOR_HAND_CLOSE);}
	void setHandcloseCursorPoint(const QPoint clPoint){m_clScene.setCursorPoint(eCURSOR_HAND_CLOSE, clPoint);}
	//拡大
	QPixmap getZoominCursorMap(){return m_clScene.getCursorMap(eCURSOR_ZOOMIN);}
	void setZoominCursorMap(const QPixmap &clMap){m_clScene.setCursorMap(eCURSOR_ZOOMIN, clMap);}
	QPoint getZoominCursorPoint(){return m_clScene.getCursorPoint(eCURSOR_ZOOMIN);}
	void setZoominCursorPoint(const QPoint clPoint){m_clScene.setCursorPoint(eCURSOR_ZOOMIN, clPoint);}
	//縮小
	QPixmap getZoomoutCursorMap(){return m_clScene.getCursorMap(eCURSOR_ZOOMOUT);}
	void setZoomoutCursorMap(const QPixmap &clMap){m_clScene.setCursorMap(eCURSOR_ZOOMOUT, clMap);}
	QPoint getZoomoutCursorPoint(){return m_clScene.getCursorPoint(eCURSOR_ZOOMOUT);}
	void setZoomoutCursorPoint(const QPoint clPoint){m_clScene.setCursorPoint(eCURSOR_ZOOMOUT, clPoint);}
	//選択
	QPixmap getSelectCursorMap(){return m_clScene.getCursorMap(eCURSOR_SELECT);}
	void setSelectCursorMap(const QPixmap &clMap){m_clScene.setCursorMap(eCURSOR_SELECT, clMap);}
	QPoint getSelectCursorPoint(){return m_clScene.getCursorPoint(eCURSOR_SELECT);}
	void setSelectCursorPoint(const QPoint clPoint){m_clScene.setCursorPoint(eCURSOR_SELECT, clPoint);}

public:
	ClDrawWidget(QWidget *parent = NULL);		//コンストラクタ
	~ClDrawWidget();							//デストラクタ
	void setCursorType(eCursorType eType, QCursor clCursor){m_clScene.setCursorType(eType, clCursor);}	//モード毎のカーソル設定
	void procMouseMoveScroll(){m_clScene.procMouseLeave();}		//スクロールバーのマウス移動処理
	QSizeF getScaledRatio(){return m_clScene.getScaledRatio();}	//描画画像と描画領域の比率取得
	void procDragEvent(QDropEvent *pclEvent);					//ドラッグ処理
	QString procDropEvent(QDropEvent *pclEvent);				//ドロップ処理
	void setBackColor(qint32 lVal){m_clScene.setBackColor(lVal);}	//背景色設定
	void procShowScrollbar(){m_clScene.showScrollbar();}			//スクロールバー表示
	void procHideScrollbar(){m_clScene.hideScrollbar();}			//スクロールバー非表示
	virtual bool event(QEvent *pclEvent);							//イベント処理
	QImage getSaveImage(){return m_clScene.getSaveImage();}			//保存画像取得
	eDrawMode getDrawMode(){return m_clScene.getDrawMode();}		//描画モード取得

protected:
	virtual void resizeEvent(QResizeEvent *pclEvent);				//サイズ変更イベント
	virtual void leaveEvent(QEvent *){m_clScene.procMouseLeave();}	//マウスが領域外に出た通知
	virtual void dragEnterEvent(QDragEnterEvent *pclEvent){procDragEvent(pclEvent);}	//ドラッグイベント
	virtual void dragMoveEvent(QDragMoveEvent *pclEvent){procDragEvent(pclEvent);}		//ドラッグイベント
	virtual void dropEvent(QDropEvent *pclEvent){procDropEvent(pclEvent);}				//ドロップイベント

protected:
	ClDrawArea			m_clScene;		//描画
	bool				m_bIsStopInput;	//入力処理停止フラグ

public slots:
	void slotSetDrawMode(eDrawMode mode){m_clScene.setDrawMode(mode);}				//描画モード設定
	void slotSetColor(QColor clColor){m_clScene.setColor(clColor);}					//色設定
	void slotSetPenWidth(qreal dWidth){m_clScene.setPenWidth(dWidth);}				//ペン幅設定
	void slotSetPenStyle(Qt::PenStyle eStyle){m_clScene.setPenStyle(eStyle);}		//ペン種設定
	void slotSetBrushStyle(Qt::BrushStyle eStyle){m_clScene.setBrushStyle(eStyle);}	//ブラシ種設定
	void slotSetSolid(bool bIsSolid){m_clScene.setSolid(bIsSolid);}					//塗りつぶし設定
	void slotClear(){m_clScene.areaClear();}										//全消去
	void slotSetEraserWidth(qreal dWidth){m_clScene.setEraserWidth(dWidth);}		//消しゴムサイズ設定
	void slotSetEraserStyle(Qt::PenCapStyle eStyle){m_clScene.setEraserStyle(eStyle);}	//消しゴムタイプ設定
	void slotImage(QImage clImage){m_clScene.setImage(clImage);}					//画像設定
	void slotCleanPixmap(){m_clScene.cleanPixmap();}								//画像消去
	void slotCenterImage(QImage clImage){m_clScene.setCenterImage(clImage);}		//特殊状態画像設定
	void slotAdjust(){m_clScene.adjust(viewport()->size());}						//画像サイズ最適化
	void slotRotate(eRotateValue eValue){m_clScene.rotate(eValue);}					//回転
	void slotZoomIn(QPointF clPointCenter = QPointF(-1, -1)){m_clScene.zoomIn(clPointCenter);}		//拡大
	void slotZoomOut(QPointF clPointCenter = QPointF(-1, -1)){m_clScene.zoomOut(clPointCenter);}	//縮小
	virtual void slotFullScreen();													//フルスクリーン化
	virtual void slotReleaseFullScreen();											//フルスクリーン解除
	void slotSave(QString clFileName, const char *pcFormat = NULL){m_clScene.save(clFileName, pcFormat);}	//保存
	void slotUndo(){m_clScene.undo();}												//アンドゥ
	void slotRedo(){m_clScene.redo();}												//リドゥ
	void slotClearUndoList(){m_clScene.clearUndoList();}							//アンドゥリストクリア
	void slotGraphic(qint32 lHue, qint32 lContrast, qint32 lBrightness)				//画像編集
	{m_clScene.setGraphic(lHue, lContrast, lBrightness);}
	void slotSetCanvasSize(QSize clSize){m_clScene.setCanvasSize(clSize);}			//キャンバスサイズ設定
	void slotStartEditGraphic(qint32 lHue, qint32 lContrast, qint32 lBrightness)
	{m_clScene.startEditGraphic(lHue, lContrast, lBrightness);}						//画像調整開始
	void slotEndEditGraphic(qint32 lHue, qint32 lContrast, qint32 lBrightness)
	{m_clScene.endEditGraphic(lHue, lContrast, lBrightness);}						//画像調整終了
	void slotStopInput(bool bIsStop){m_bIsStopInput = bIsStop;}						//入力停止設定

signals:
	void sigReleaseFullScreen();	//フルスクリーン解除通知
	void sigStartEdit();			//編集開始
	void sigEndEdit();				//編集終了
	void sigZoomMax();				//最大ズーム
	void sigZoomIn();				//拡大
	void sigZoomNone();				//ズーム解除
	void sigZoomOut();				//縮小
	void sigEdit();					//編集開始
	void sigCanUndo();				//アンドゥ可能シグナル
	void sigCantUndo();				//アンドゥ不可シグナル
	void sigCanRedo();				//リドゥ可能シグナル
	void sigCantRedo();				//リドゥ不可シグナル
	void sigSetUndoGraphic(qint32, qint32, qint32);	//アンドゥで画像調整された
};
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@class	ClSendposScrollBar
	@brief	マウス座標を親に通知するようにしたスクロールバー
*/
//--------------------------------------------------------------------------------//
class ClSendposScrollBar : public QScrollBar
{
	Q_OBJECT

public:
	ClSendposScrollBar(ClDrawWidget *pclParent)		//コンストラクタ
	{
		m_pclParent = pclParent;	//親ウィジェット設定
		setMouseTracking(true);		//ドラッグ中でなくてもマウス座標通知する
	}

protected:
	virtual void mouseMoveEvent(QMouseEvent *pclEvent)	//マウス移動イベント
	{
		QScrollBar::mouseMoveEvent(pclEvent);		//デフォ処理
		m_pclParent->procMouseMoveScroll();			//親に通知
	}
	virtual void showEvent(QShowEvent *)	//表示イベント
	{
		m_pclParent->procShowScrollbar();
	}
	virtual void hideEvent(QHideEvent *)	//非表示イベント
	{
		m_pclParent->procHideScrollbar();
	}

signals:
	void sigShow();
	void sigHide();

protected:
	ClDrawWidget *m_pclParent;		//親ウィジェット
};
//--------------------------------------------------------------------------------//



//--------------------------------------------------------------------------------//
/*!
	@class	ClDrawUndo
	@brief	描画アンドゥコマンド
*/
//--------------------------------------------------------------------------------//
class ClDrawUndo : public QUndoCommand
{
public:
	ClDrawUndo(QRectF clRect,				//コンストラクタ
#ifdef USE_QPIXMAP
			   QPixmap *pclSourceDevice);
#else
				QImage *pclSourceDevice);
#endif
	virtual void undo();					//アンドゥ
	virtual void redo();					//リドゥ

protected:
	QRectF	m_clDrawRect;				//描画領域
#ifdef USE_QPIXMAP
	QPixmap	m_clPixmapUndo;				//アンドゥしたときの画像
	QPixmap m_clPixmapRedo;				//リドゥしたときの画像
	QPixmap *m_pclSourcePixmap;			//アンドゥの描画対象
#else
	QImage	m_clPixmapUndo;
	QImage m_clPixmapRedo;
	QImage *m_pclSourcePixmap;
#endif
};
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@class	ClRotateUndo
	@brief	回転アンドゥコマンド
*/
//--------------------------------------------------------------------------------//
class ClRotateUndo : public QUndoCommand
{
public:
	ClRotateUndo(ClDrawArea *pclArea, qint32 lValue);	//コンストラクタ
	virtual void undo();								//アンドゥ
	virtual void redo();								//リドゥ

protected:
	ClDrawArea	*m_pclArea;		//描画エリア
	qint32		m_lValue;		//回転角度
	bool		m_bIsUndo;		//アンドゥ実行フラグ
};
//--------------------------------------------------------------------------------//


//--------------------------------------------------------------------------------//
/*!
	@class	ClGraphicUndo
	@brief	画像調整アンドゥコマンド
*/
//--------------------------------------------------------------------------------//
class ClGraphicUndo : public QUndoCommand
{
public:
	ClGraphicUndo(ClDrawArea *pclArea, qint32 lHue, qint32 lContrast, qint32 lBrightness);		//コンストラクタ
	virtual void undo();													//アンドゥ
	virtual void redo();													//リドゥ
	void setRedoValue(qint32 lHue, qint32 lContrast, qint32 lBrightness)	//リドゥ値設定
	{
		m_stRedoValue.lHue = lHue;
		m_stRedoValue.lContrast = lContrast;
		m_stRedoValue.lBrightness = lBrightness;
	}
	void getUndoValue(qint32 *plHue, qint32 *plContrast, qint32 *plBrightness)	//アンドゥ値取得
	{
		*plHue = m_stUndoValue.lHue;
		*plContrast = m_stUndoValue.lContrast;
		*plBrightness = m_stUndoValue.lBrightness;
	}
	void getRedoValue(qint32 *plHue, qint32 *plContrast, qint32 *plBrightness)	//リドゥ値取得
	{
		*plHue = m_stRedoValue.lHue;
		*plContrast = m_stRedoValue.lContrast;
		*plBrightness = m_stRedoValue.lBrightness;
	}

protected:
	ClDrawArea *m_pclArea;		//描画エリア
	bool m_bIsUndo;				//アンドゥ実行フラグ
	typedef struct
	{
		qint32 lHue;			//色相
		qint32 lContrast;		//コントラスト
		qint32 lBrightness;		//明るさ
	}stValue;

	stValue m_stUndoValue;		//アンドゥ時の値
	stValue m_stRedoValue;		//リドゥ時の値
};
//--------------------------------------------------------------------------------//
#endif
