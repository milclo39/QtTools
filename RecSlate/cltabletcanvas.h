#ifndef CLTABLETCANVAS_H
#define CLTABLETCANVAS_H

#include <QDebug>
#include <QWidget>
#include <QTabletEvent>
#include <QColor>
#include <QPoint>
#include <QStylePainter>
#include <QPen>
#include <QBrush>
#include <QPixmap>
#include <QMouseEvent>

#include <QUuid>
#include <QAxBase>
#include <QAxObject>
#include <QAxWidget>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <assert.h>
#include <QActionGroup>

#include <msinkaut.h>

//--------------------------------------------------------------------------------------
class clTabletCanvas : public QAxWidget
{
public:
	explicit clTabletCanvas(QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~clTabletCanvas();

	void setPenColor(COLORREF color);							//色の変更
	void setPenWidth(int width);							//
	void setPenTransparency(int value);
	void setPenMode(InkOverlayEditingMode mode);			//ペンモード
	void setSaveData(VARIANT*);
	void setLoadData(VARIANT);

protected:
	IInkPicture *m_pInkPicture;
	IInkCollector *m_pInkCollector;
#if 0	// 実験用
	IInk *m_pInk;
	IInkOverlay *m_pInkOverlay;
	IInkStrokes *m_pInkStrokes;
	IInkDrawingAttributes *m;
	IInkTablets *m_pInkTablets;
	IInkRecognizerContext *m_pInkRecognizerContext;
	IInkStrokeDisp *m_pInkStrokeDisp;
	IInkDisp *m_pInkDisp;
	IInkRectangle *m_pInkRectangle;
#endif
};
//--------------------------------------------------------------------------------------

#endif // CLTABLETCANVAS_H
