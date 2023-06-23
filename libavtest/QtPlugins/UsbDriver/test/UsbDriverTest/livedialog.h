#ifndef LIVEDIALOG_H
#define LIVEDIALOG_H

#include <QDialog>

namespace Ui {
    class LiveDialog;
}

class LiveDialog : public QDialog {
    Q_OBJECT
public:
    LiveDialog(QWidget *parent = 0);
    ~LiveDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::LiveDialog *ui;

public slots:
	void slotStreamImg(QByteArray qArray);
};

#endif // LIVEDIALOG_H
