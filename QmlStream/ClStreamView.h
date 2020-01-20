#ifndef CLSTREAMVIEW_H
#define CLSTREAMVIEW_H
#include <QQuickPaintedItem>
#include <QPainter>
#include <QRect>
#include <QImage>

#include "./AvLib/StreamDecoder/ClStreamDecoder.h"

class ClStreamView : public QQuickPaintedItem{
	Q_OBJECT
	Q_DISABLE_COPY(ClStreamView)

public:
	explicit ClStreamView(QQuickItem *parent = 0);
	~ClStreamView();

signals:

protected slots:
	Q_INVOKABLE void slotStreamStart(QString strUrl);	//!< ストリーム開始
	Q_INVOKABLE void slotStreamStop();	//!< 停止
	Q_INVOKABLE void slotStreamEnd();	//!< 終了

	void slotImage(ClAvImage img);	//!< デコーダから映像をもらう

protected:
	void paint(QPainter *painter);

private:
	ClStreamDecoder *m_pDecoder;	//!< デコーダ
	QImage m_imgSource;	//!< 元画像
	QImage m_imgDraw;	//!< 描画画像
};
#endif	//CLSTREAMVIEW_H
