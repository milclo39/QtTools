#ifndef CLWIDGETDRAWIMAGE_H
#define CLWIDGETDRAWIMAGE_H
#include <QWidget>
#include <QImage>

//--------------------------------------------------------------------------------//
/*!
	@brief	画像描画ウィジェット
*/
//--------------------------------------------------------------------------------//
class ClWidgetDrawImage : public QWidget
{
public:
	ClWidgetDrawImage();	//コンストラクタ

	void updateImage(QImage img);	//画像更新
	QImage getImage(){return m_imgSource;}	//!<　画像取得
	void setRectSize(QRect cRect);

protected:
	void paintEvent(QPaintEvent *);	//描画イベント

private:
	QImage m_imgSource;	//!< 元画像
	QImage m_imgDraw;	//!< 描画画像
	bool m_bIsRectLine;
	QRect m_iRectSize;
};
//--------------------------------------------------------------------------------//
#endif
