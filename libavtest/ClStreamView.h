/*--------------------------------------------------------------------------------*/
/*!
    @file	ClStreamView.h
    @brief	RTSPストリーミング/USBストリーミング処理
*/
/*--------------------------------------------------------------------------------*/
#ifndef CLSTREAMVIEW_H
#define CLSTREAMVIEW_H
#include <QQuickPaintedItem>
#include <QPainter>
#include <QRect>
#include <QImage>

#include "ClL12Presenter.h"
#include "StreamDecoder/ClStreamDecoder.h"
//#include "../component/Setting/ClDocElusbSetting.h"

class ClStreamView : public QQuickPaintedItem{
	Q_OBJECT
	Q_DISABLE_COPY(ClStreamView)
	Q_PROPERTY(bool rotate READ rotate WRITE setRotate)
	Q_PROPERTY(bool freeze READ freeze WRITE setFreeze)

public:
	explicit ClStreamView(QQuickItem *parent = 0);
	~ClStreamView();
	bool rotate(){return m_bMirror;}
	void setRotate(bool val){m_bMirror = val;}
	bool freeze(){return m_bFreeze;}
	void setFreeze(bool val){m_bFreeze = val;}

signals:
	void sigRawStream(QImage  clImg, qint32  lType);

protected slots:
	Q_INVOKABLE void slotStreamStart(QString strUrl);	//!< ストリーム開始
	Q_INVOKABLE void slotStreamEnd();	//!< 終了
	void slotImage(ClAvImage img);	//!< デコーダから映像をもらう
	void slotError();
	void slotChangeResolution(QSize);

	Q_INVOKABLE void startLive();	//!< ストリーム開始
	Q_INVOKABLE void stopLive();	//!< 終了
    Q_INVOKABLE void selectCam();	//!< カメラ選択
    Q_INVOKABLE void execAf();		//!< AF実行
    Q_INVOKABLE void zoomTele();	//!< zoom(tele)
    Q_INVOKABLE void zoomWide();	//!< zoom(wide)
    Q_INVOKABLE void irisOpen();	//!< iris(open)
    Q_INVOKABLE void irisClose();	//!< iris(close)
    Q_INVOKABLE void stopCtrl();	//!< 制御停止
    void slotConvAvImage(QImage img, qreal pts);
	void slotGetStreamRaw( qint32  lProductId, qint32  lState, QImage  clImg );
	void slotDriverError(qint32 err);
	void slotStreamError(qint32, eKINDSTREAM kind);

protected:
	void paint(QPainter *painter);

	void setup();
    void connectCam(ClCamDevice*);

private:
	ClStreamDecoder *m_pDecoder;	//!< デコーダ
	QImage m_imgSource;	//!< 元画像
	QImage m_imgDraw;	//!< 描画画像

    ClUsbDriverForIM4 *m_pUsbDriver;
    qint32	m_iDuration;						//!< Live時のDuration記憶用
	qint64  m_llPts;							//!< Live時のPTS記憶用
	qreal	m_dFramTime;						//!< Frame単位の時間保存用
	qint32	m_uiProductId;						//プロダクトID
	qint32	m_lImgType;							//!<	画像種別
	bool	m_bMirror;							//!< ミラー反転
	bool	m_bFreeze;

    qint32 m_nProduct;							//!< pid+vidの数値化
};
#endif	//CLSTREAMVIEW_H
