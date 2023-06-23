/*--------------------------------------------------------------------------------*/
/*!
    @file	ClMainView.h
    @brief	
*/
/*--------------------------------------------------------------------------------*/
#ifndef CLMAINVIEW_H
#define CLMAINVIEW_H
#include <QQuickView>

class ClMainView : public QQuickView
{
	Q_OBJECT

public:
    explicit ClMainView(QWindow *parent = nullptr);	//コンストラクタ
    ~ClMainView();	//デストラクタ

public slots:

signals:

protected:

private slots:

private:

};
#endif	//CLMAINVIEW_H
