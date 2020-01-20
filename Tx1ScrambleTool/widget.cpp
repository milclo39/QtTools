#include "widget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QLabel>
#include <QSettings>
#include <QApplication>
#include <QMimeData>
#include <QEventLoop>
#include <QTimer>

#include "keydefine.h"
#define SETTING_FILENAME	"\\setting.ini"
static const char DEFAULT_AESKEY[] = {
	 1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16,
	17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
};
Widget::Widget(QWidget *parent) : ClProcessWidgetBase(parent)
{
	this->resize(QSize(480, 320));
	this->setMinimumSize(QSize(100, 100));
	this->setFont(QFont("MS UI Gothic", 12, QFont::Normal));
	this->setWindowFlags(windowFlags() &= ~Qt::WindowMaximizeButtonHint);
	this->setAcceptDrops(true);
}

Widget::~Widget()
{
	QSettings settings(qApp->applicationDirPath() + SETTING_FILENAME, QSettings::IniFormat);
	settings.setValue("PATH", m_pclEdtReadFile->text());
	settings.setValue("WPATH", m_pclEdtWriteFile->text());
	settings.setValue("TYPE", m_pclCmbMethod->currentIndex());
}

QWidget *Widget::createStartWidget()
{
	QWidget *pclStartWidget = new QWidget();

	QSettings settings(qApp->applicationDirPath() + SETTING_FILENAME, QSettings::IniFormat);
	m_pclEdtReadFile = new QLineEdit(settings.value("PATH", "").toString());
	m_pclEdtWriteFile = new QLineEdit(settings.value("WPATH", "").toString());

	QString str;
	QString strKey;
	for(int i = 0; i < sizeof(DEFAULT_AESKEY); i++){
		strKey.append(str.sprintf("%02x", DEFAULT_AESKEY[i]));
	}
	m_pclEdtKey = new QLineEdit(strKey);
	m_pclEdtKey->setFont(QFont("", 9, QFont::Normal));

	m_pclBtnOpen = new QPushButton("Refer");
	connect(m_pclBtnOpen, SIGNAL(clicked()), this, SLOT(slotOpenClicked()));
	m_pclBtnSend = new QPushButton("Convert");
	m_pclBtnSend->setFont(QFont("", 20, QFont::Normal));
	connect(m_pclBtnSend, SIGNAL(clicked()), this, SLOT(slotSendClicked()));

	m_pclCmbMethod = new QComboBox();
	m_pclCmbMethod->addItem("AES Encode");
	m_pclCmbMethod->addItem("AES Decode");
//	m_pclCmbMethod->addItem("RSA Encode");
//	m_pclCmbMethod->addItem("RSA Decode");
	m_pclCmbMethod->setCurrentIndex(settings.value("TYPE", "").toInt());

	QVBoxLayout *layout = new QVBoxLayout;
	{
		QHBoxLayout *layout1 = new QHBoxLayout;
		QLabel *input = new QLabel("Input");
		layout1->addWidget(input);
		layout1->addWidget(m_pclEdtReadFile);
		layout1->addWidget(m_pclBtnOpen);
		layout->addLayout(layout1);
	}
	{
		QHBoxLayout *layout2 = new QHBoxLayout;
		QLabel *aeskey = new QLabel("Key");
		layout2->addWidget(aeskey);
		layout2->addWidget(m_pclEdtKey);
		layout->addLayout(layout2);
	}
	{
		QHBoxLayout *layout2 = new QHBoxLayout;
		QLabel *label = new QLabel("Convert Type");
		layout2->addWidget(label);
		layout2->addWidget(m_pclCmbMethod);
		layout->addLayout(layout2);
	}
	{
		QHBoxLayout *layout1 = new QHBoxLayout;
		QLabel *label = new QLabel("Output");
		layout1->addWidget(label);
		layout1->addWidget(m_pclEdtWriteFile);
		layout->addLayout(layout1);
	}
	{
		QHBoxLayout *layout3 = new QHBoxLayout;
		layout3->addWidget(m_pclBtnSend);
		layout->addLayout(layout3);
	}
	pclStartWidget->setLayout(layout);

	return pclStartWidget;
}

void Widget::dragEnterEvent(QDragEnterEvent *e)
{
	if(e->mimeData()->hasFormat("text/uri-list"))
	{
		e->acceptProposedAction();
	}
}

void Widget::dragMoveEvent(QDragMoveEvent *e)
{
	if (!e->mimeData()->hasUrls())
		e->setDropAction(Qt::IgnoreAction);
	else if (e->keyboardModifiers() & Qt::ShiftModifier)
		e->setDropAction(Qt::MoveAction);
	else if (e->keyboardModifiers() & Qt::ControlModifier)
		e->setDropAction(Qt::CopyAction);
	else
		e->acceptProposedAction();
	e->accept();
}

void Widget::dropEvent(QDropEvent *e)
{
	QString filePath = e->mimeData()->urls().first().toLocalFile();
	m_pclEdtReadFile->setText(filePath);
	if(filePath.right(3) == "bin"){
		m_pclEdtWriteFile->setText(filePath.replace("bin", "aes"));
		m_pclCmbMethod->setCurrentIndex(0);
	}
	else if(filePath.right(3) == "aes"){
		m_pclEdtWriteFile->setText(filePath.replace("aes", "bin"));
		m_pclCmbMethod->setCurrentIndex(1);
	}
}

void Widget::slotOpenClicked()
{
	QFileDialog::Options options;
	QString filePath = QFileDialog::getOpenFileName(this, tr("open file"), m_pclEdtReadFile->text()
										, "BIN (*.bin);;AES (*.aes);;ALL (*.*)", NULL, options);
	if(filePath == ""){
		return;	//cancel
	}
	m_pclEdtReadFile->setText(filePath);
	if(filePath.right(3) == "bin"){
		m_pclEdtWriteFile->setText(filePath.replace("bin", "aes"));
		m_pclCmbMethod->setCurrentIndex(0);
	}
	else if(filePath.right(3) == "aes"){
		m_pclEdtWriteFile->setText(filePath.replace("aes", "bin"));
		m_pclCmbMethod->setCurrentIndex(1);
	}
	return;
}

void Widget::slotSendClicked()
{
	QFileDialog::Options options;
	QString filePath = QFileDialog::getSaveFileName(this, tr("save file"), m_pclEdtWriteFile->text()
										, "AES (*.aes);;BIN (*.bin);;ALL (*.*)" , NULL, options);
	if(filePath == ""){
		return;	//cancel
	}
	m_pclEdtWriteFile->setText(filePath);

	setOverlayIndicator(false);
//	QEventLoop loop;	QTimer::singleShot(1000, &loop, SLOT(quit()));	loop.exec();
	QTimer::singleShot(100, this, SLOT(slotConvert()));
}

void Widget::slotConvert()
{
	QFile rfile(m_pclEdtReadFile->text());
	if(!rfile.open(QIODevice::ReadOnly)){
		QMessageBox clMsgBox( QMessageBox::Critical, NULL, tr("read file error"), QMessageBox::Ok );
		clMsgBox.exec();
		return;//error
	}

	QByteArray data = rfile.readAll();
#if 0
	m_AESKey = QByteArray::fromRawData(DEFAULT_AESKEY, sizeof(DEFAULT_AESKEY));
#else
	for(int i = 0; i < 32; i++){
		m_AESKey[i] = m_pclEdtKey->text().mid(i * 2, 2).toInt(0, 16);
	}
#endif
	QByteArray encdata;
	switch(m_pclCmbMethod->currentIndex()){
	case 0:	// AES Enc
		encdata = encrypt(data);
		break;
	case 1:	// AES Dec
		encdata = decrypt(data);
		break;
	case 2:	// RSA Enc
		encdata = RSA_encrypt(data);
		break;
	case 3:	// RSA Dec
		encdata = RSA_decrypt(data);
		break;
	}
	if(encdata.isEmpty()){
		setOverlayIndicator(true);
		QMessageBox clMsgBox( QMessageBox::Critical, NULL, tr("convert error"), QMessageBox::Ok );
		clMsgBox.exec();
		return;//error
	}

	QFile wfile(m_pclEdtWriteFile->text());
	if(!wfile.open(QIODevice::WriteOnly)){
		setOverlayIndicator(true);
		QMessageBox clMsgBox( QMessageBox::Critical, NULL, tr("save file error"), QMessageBox::Ok );
		clMsgBox.exec();
		return;//error
	}
	wfile.write(encdata);
	wfile.flush();
	setOverlayIndicator(true);
	QMessageBox clMsgBox( QMessageBox::Information, NULL, tr("Complete"), QMessageBox::Ok );
	clMsgBox.exec();
}
