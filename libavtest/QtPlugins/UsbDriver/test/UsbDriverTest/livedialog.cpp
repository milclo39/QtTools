#include "livedialog.h"
#include "ui_livedialog.h"
#include <QPainter>

LiveDialog::LiveDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LiveDialog)
{
    ui->setupUi(this);
}

LiveDialog::~LiveDialog()
{
    delete ui;
}

void LiveDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void LiveDialog::slotStreamImg(QByteArray qArray)
{
	QImage qImg((unsigned char*)qArray.constData(),1280,1024,QImage::Format_RGB32);
	QPainter painter;
	painter.begin(ui->myWidget);
	painter.drawImage(this->rect(), qImg);
	painter.end();
	this->repaint();
	return;
}
