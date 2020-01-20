#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QDropEvent>
#include <QDragMoveEvent>
#include "Parts/ClProcessWidgetBase.h"

class Widget : public ClProcessWidgetBase
{
	Q_OBJECT

public:
	Widget(QWidget *parent = 0);
	~Widget();

public slots:

protected:
	void dragEnterEvent(QDragEnterEvent *e);
	void dropEvent(QDropEvent *e);
	void dragMoveEvent(QDragMoveEvent *e);

	QWidget *createStartWidget();

private:
	QLineEdit *m_pclEdtReadFile;
	QLineEdit *m_pclEdtWriteFile;
	QLineEdit *m_pclEdtKey;
	QPushButton *m_pclBtnOpen;
	QPushButton *m_pclBtnSend;
	QComboBox *m_pclCmbMethod;

private slots:
	void slotOpenClicked();
	void slotSendClicked();
	void slotConvert();
};

#endif // WIDGET_H
