#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include "ClTabletPaint.h"
#include "ClComponentRec.h"

class Widget : public QWidget
{
	Q_OBJECT

public:
	Widget(QWidget *parent = 0);
	~Widget();

protected:
	QPixmap capture(QSize clSize, bool bIsDesktop = false);
	bool setSource(QString in_qstrSource, qint32 nDelay = 0);

private:
	ClTabletPaint *m_pclTabletcanvas;
	ClComponentRec *m_pclRec;
	QPushButton *m_clBtnRecord;
	VARIANT m_clData;

public slots:
	void slotRecvCmd(EL_CMD clCommand);

signals:
	void sigSendCmd(EL_CMD);

private slots:
	void slotCmbWidthClicked(int);
	void slotCmbColorClicked(int);
	void slotCmbDepthClicked(int);
	void slotCmbModeClicked(int);
	void slotBtnStillClicked();
	void slotBtnRecordClicked();
	void slotBtnSaveClicked();
	void slotBtnLoadClicked();
};

#endif // WIDGET_H
