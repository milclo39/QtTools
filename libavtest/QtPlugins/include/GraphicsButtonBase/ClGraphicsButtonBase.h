//***************************************************************************//
/*!
 @file		ClGraphicsButtonBase.h
 @author		(S技1)須田
 @brief		QGraphicsItemを利用したボタンライブラリ
*/
//***************************************************************************//
#ifndef CLGRAPHICSBUTTONBASE_H
#define CLGRAPHICSBUTTONBASE_H

#include <QtGui>
#include <QtCore/qstate.h>
#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>

#if 0 //一時保留

//--------------------------------------------------//
/*!
	@ingroup	grpGraphics
	@brief		GraphicsItem上で扱うボタンを描画するクラス
				主に数字を表示することが目的
*/
//--------------------------------------------------//
class ClGraphicsNumberBase : public QObject, public QGraphicsPixmapItem
{
	Q_OBJECT
public:
	explicit ClGraphicsNumberBase(QGraphicsItem *parent = 0);
	explicit ClGraphicsNumberBase(qint32 number, QGraphicsItem *parent = 0);
	virtual ~ClGraphicsNumberBase();
	virtual initialize();

	virtual void setAspectMode(Qt::AspectRatioMode mode){ m_eAspectMode = mode; }
	virtual Qt::AspectRatioMode getAspectMode(){ return m_eAspectMode; }
	virtual void setTransMode(Qt::TransformationMode mode){ m_eTransMode = mode; }
	virtual Qt::TransformationMode getTransMode(){ return m_eTransMode; }

	virtual bool isDigit();												//設定されたTextが数値か判定
	virtual void setZeroSuppress(bool sw){ m_bZeroSuppress = sw; }		//先頭のゼロを抑制するかどうか
	virtual bool getZeroSuppress(){ return m_bZeroSuppress; }

	virtual void addState(QState *state){ m_qMachine.addState(state); }
	virtual void addTransition(QAbstractTransition *transition){ m_qMachine.addTransition(transition); }

	virtual QStateMachine getStateMachine() const { return m_qMachine; }

protected:
	Qt::AspectRatioMode m_eAspectMode;		//Scale時のオプション指定
	Qt::TransformationMode m_eTransMode;	//Scale時のオプション指定
	bool m_bInit;							//初期化済みフラグ
	bool m_bZeroSuppress;					//ゼロ抑制フラグ

	qint32 m_Number;						//表示する数字

	QVector qDigitVector;					//桁制御用Vector
	QGraphicsSimpleTextItem *m_pTextItem;	//数字の表現
	QStateMachine	m_qMachine;				//ステートマシーン



};
#endif	//一時保留
//--------------------------------------------------//

/*!
	@brief SXGA基準で作成されたボタンの標準サイズ
	@{
*/

#define	BUTTON_WIDTH_DEF	106				//!<　Width標準
#define	BUTTON_HEIGHT_DEF	106				//!<  Heigh標準
/*!
@}
*/

/*!
	@brief Selected状態を表すENUM
*/
typedef enum{
	SEL_NORMAL = 0,								//通常状態
	SEL_SELECTION = 1							//選択状態
}eSELECTED_STATE;


//--------------------------------------------------//
/*!
	@ingroup	grpGraphics
	@brief		GraphicsItem上で扱うボタンを描画するクラス
*/
//--------------------------------------------------//
class ClGraphicsButtonBase : public QObject, public QGraphicsPixmapItem
{
	Q_OBJECT
	Q_PROPERTY( QString hintText READ getHintText WRITE setHintText )		//	表示する文字列
	Q_PROPERTY(QTransform transform READ transform WRITE setTransform)
	Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)
	Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)
	Q_PROPERTY(qreal scale READ scale WRITE setScale)
	Q_PROPERTY(bool visible READ isVisible WRITE setVisible)
	Q_PROPERTY(QPointF pos READ pos WRITE setPos)
	Q_PROPERTY(QSize size READ getSize WRITE setSize)
	Q_PROPERTY(bool rollover READ getRolloverEnabled WRITE setRolloverEnabled)
	Q_PROPERTY(bool hit READ getHitEnabled WRITE setHitEnabled)
	Q_PROPERTY(bool selected READ getSelectedEnabled WRITE setSelectedEnabled)
	Q_PROPERTY(bool enable READ getBtnEnabled WRITE setBtnEnabled)
	Q_PROPERTY(eSELECTED_STATE selectedstate READ getSelState WRITE setSelState)

public:
	//プロパティ群の登録

	explicit ClGraphicsButtonBase(QGraphicsItem *parent = 0);	//バックグラウンドクラス
	explicit ClGraphicsButtonBase(QString NormalFileName, QGraphicsItem *parent = 0);	//バックグラウンドクラス
	virtual ~ClGraphicsButtonBase(){}
	virtual bool initialize();															//初期化

	virtual void setHintText(QString text){ setToolTip(text); }				//Hint用文字列設定
	virtual QString getHintText() const{ return toolTip(); }				//Hint用文字列取得

	virtual void setSize(QSize size);
	virtual QSize getSize(){ return m_qSize; }

	virtual void setBtnEnabled(bool sw){ this->setEnabled(sw); update(); }
	virtual bool getBtnEnabled(){ return this->isEnabled(); }
	virtual void setRolloverEnabled(bool sw){ m_bRolloverEnabled = sw; }
	virtual bool getRolloverEnabled(){ return m_bRolloverEnabled; }
	virtual void setHitEnabled(bool sw){ m_bHitEnable = sw; }
	virtual bool getHitEnabled(){ return m_bHitEnable; }
	virtual void setSelectedEnabled(bool sw, bool bIsAutoSelectChange = true){ m_bSelEnable = sw; m_bIsAutoSelectChange = bIsAutoSelectChange; }
	virtual bool getSelectedEnabled(){ return m_bSelEnable; }
	virtual void setAntialiasing(bool sw){ m_bAliasing = sw; }
	virtual bool getAntialiasing(){ return m_bAliasing; }

	virtual void setSelState(eSELECTED_STATE sw){ m_eSelState = sw; update();}
	virtual eSELECTED_STATE getSelState(){ return m_eSelState; }

	virtual void setAspectMode(Qt::AspectRatioMode mode){ m_eAspectMode = mode; }
	virtual Qt::AspectRatioMode getAspectMode(){ return m_eAspectMode; }
	virtual void setTransMode(Qt::TransformationMode mode){ m_eTransMode = mode; }
	virtual Qt::TransformationMode getTransMode(){ return m_eTransMode; }

	virtual bool loadNormalPicture(QString fileName);
	virtual bool loadNormalPictureJustFit(QString fileName);
	virtual bool loadRolloverPicture(QString fileName);
	virtual bool loadHitPicture(QString fileName);
	virtual bool loadDisablePicture(QString fileName);
	virtual bool loadSelectedPicture(QString fikeName);

	virtual bool setNormalPicture(QPixmap clPixmap);
	virtual bool setNormalPictureJustFit(QPixmap clPixmap);
	virtual bool setRolloverPicture(QPixmap clPixmap);
	virtual bool setHitPicture(QPixmap clPixmap);
	virtual bool setDisablePicture(QPixmap clPixmap);
	virtual bool setSelectedPicture(QPixmap clPixmap);

signals:
	void sigBtnPress();						//ボタンが押された時のイベント
	void sigBtnReleased();					//ボタンが離された時のイベント
	void sigBtnEntered();					//カーソルが領域に侵入したときのイベント
	void sigBtnLeaved();					//カーソルが領域から離れた時のイベント

protected:
	Qt::AspectRatioMode m_eAspectMode;		//Scale時のオプション指定
	Qt::TransformationMode m_eTransMode;	//Scale時のオプション指定

	bool m_bRolloverEnabled;				//Rollover可能フラグ
	bool m_bHitEnable;						//Hit可能フラグ
	bool m_bSelEnable;						//Selected可能フラグ
	bool m_bIsAutoSelectChange;				//自動で選択状態切替フラグ

	QString m_strNolFilename;				//通常ボタンのファイル名
	QString m_strRollFilename;				//Rolloverボタンのファイル名
	QString m_strHitFilename;				//Hitボタンのファイル名
	QString m_strDisFilename;				//Disableボタンのファイル名
	QString m_strSelFilename;				//Selectedボタンのファイル名

	QPixmap m_qNormal;						//通常ボタン
	QPixmap m_qRollover;					//ロールオーバー時のボタン
	QPixmap m_qHit;							//ヒット中ボタン
	QPixmap m_qDisble;						//Disable中ボタン
	QPixmap m_qSelected;					//Selectedボタン（交換用）

	eSELECTED_STATE m_eSelState;			//Selected状態時の状態フラグ

	bool m_bInit;							//初期化済みフラグ
	bool m_bAliasing;						//アンチエイリアススイッチ
	QSize m_qSize;							//このオブジェクトのサイズ

	virtual void buildBtn(QPixmap &pixmap, QColor color = Qt::gray);	//標準のボタンを作成
	virtual bool loadNormal(QString filename);	//m_qNormalにロード

//	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	virtual void paint ( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

	//ボタン描画イベント
	virtual qint32 OnDrawNormalBtn(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );				//通常ボタン描画
	virtual qint32 OnDrawRolloverBtn(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );			//ロールオーバーボタン描画
	virtual qint32 OnDrawHitBtn(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );				//ヒット時のボタン描画
	virtual qint32 OnDrawDisableBtn(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );			//Disable時のボタン描画
	virtual qint32 OnDrawSelectedBtn(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0 );			//Disable時のボタン描画

	virtual QPixmap	resizePixmap( QString  strPath );
	virtual QPixmap	resizePixmap( QPixmap  clPixmap );
};


#endif // CLGRAPHICSBUTTONBASE_H
