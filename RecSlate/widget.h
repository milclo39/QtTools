#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include "cltabletcanvas.h"
#include "ClComponentRec.h"

class Widget : public QWidget
{
	Q_OBJECT

public:
	Widget(QWidget *parent = 0);
	~Widget();

protected:
	QPixmap capture(QSize clSize, bool bIsDesktop = false);

private:
	clTabletCanvas *m_pclTabletcanvas;
	ClComponentRec *m_pclRec;
	QPushButton *m_clBtnRecord;
	VARIANT m_clData;

public slots:
	void slotRecvCmd(EL_CMD clCommand);

signals:
	void sigSendCmd(EL_CMD);

private slots:
	void slotBtn1Clicked();
	void slotBtn2Clicked();
	void slotBtnStillClicked();
	void slotBtnRecordClicked();
	void slotCmbWidthClicked(int);
	void slotCmbColorClicked(int);
	void slotCmbDepthClicked(int);
	void slotCmbModeClicked(int);
};

#endif // WIDGET_H
