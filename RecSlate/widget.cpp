#include "widget.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QLayout>
#include <QDebug>

Widget::Widget(QWidget *parent) : QWidget(parent)
{
	this->setMinimumSize(640, 480);
	m_pclTabletcanvas = new clTabletCanvas();
	QHBoxLayout *layoutBtn = new QHBoxLayout;
	{	// ペンの太さ
		QComboBox *clCombo = new QComboBox();
		clCombo->addItem("2 dot");
		clCombo->addItem("4 dot");
		clCombo->addItem("8 dot");
		clCombo->addItem("12 dot");
		clCombo->addItem("16 dot");
		connect(clCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotCmbWidthClicked(int)));
		layoutBtn->addWidget(clCombo);
	}
	{	// ペンの色
		QComboBox *clCombo = new QComboBox();
		clCombo->addItem("Black");
		clCombo->addItem("Red");
		clCombo->addItem("Blue");
		clCombo->addItem("Yellow");
		clCombo->addItem("Green");
		connect(clCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotCmbColorClicked(int)));
		layoutBtn->addWidget(clCombo);
	}
	{	// ペンの濃さ
		QComboBox *clCombo = new QComboBox();
		clCombo->addItem("100%");
		clCombo->addItem("80%");
		clCombo->addItem("60%");
		clCombo->addItem("40%");
		clCombo->addItem("20%");
		connect(clCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotCmbDepthClicked(int)));
		layoutBtn->addWidget(clCombo);
	}
	{	// ペンの種類
		QComboBox *clCombo = new QComboBox();
		clCombo->addItem("Pen");
		clCombo->addItem("Del");
		clCombo->addItem("Sel");
		connect(clCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(slotCmbModeClicked(int)));
		layoutBtn->addWidget(clCombo);
	}
	{	// 静止画保存
		QPushButton *btn = new QPushButton("Still");
		connect(btn, SIGNAL(clicked()), this, SLOT(slotBtnStillClicked()));
		layoutBtn->addWidget(btn);
	}
	{	// 動画保存
		m_clBtnRecord = new QPushButton("Rec");
		connect(m_clBtnRecord, SIGNAL(clicked()), this, SLOT(slotBtnRecordClicked()));
		layoutBtn->addWidget(m_clBtnRecord);
	}
	{
		QPushButton *btn = new QPushButton("test1");
		connect(btn, SIGNAL(clicked()), this, SLOT(slotBtn1Clicked()));
		layoutBtn->addWidget(btn);
	}
	{
		QPushButton *btn = new QPushButton("test2");
		connect(btn, SIGNAL(clicked()), this, SLOT(slotBtn2Clicked()));
		layoutBtn->addWidget(btn);
	}
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(m_pclTabletcanvas);
	layout->addLayout(layoutBtn);

	this->setLayout(layout);

	m_pclRec = new ClComponentRec(NULL);
	QObject::connect( this, SIGNAL(sigSendCmd(EL_CMD) ),
					m_pclRec, SLOT(slotRecvCmd(EL_CMD) ) );
	QObject::connect(m_pclRec, SIGNAL(sigSendCmd(EL_CMD)),
					 this, SLOT(slotRecvCmd(EL_CMD)));
	m_pclRec->setRecSize(this->size());

//	setAttribute(Qt::WA_TranslucentBackground, true);
//	setPalette(QColor(255, 255, 255, 128));
//	setAutoFillBackground(true);
}

Widget::~Widget()
{
}

QRect getUseMonitorInfo( QDesktopWidget* pclDesktop )
{
	return pclDesktop->screenGeometry(pclDesktop->primaryScreen());
}

QPixmap Widget::capture(QSize clSize, bool bIsDesktop)
{
	QPixmap clPixmap(clSize);
	clPixmap.fill(Qt::white);
	if(bIsDesktop == true)
	{	//デスクトップキャプチャ
		QRect clRectScreen = getUseMonitorInfo( qApp->desktop());
		QPixmap clSourcePixmap = QPixmap::grabWindow( qApp->desktop()->winId(),
			clRectScreen.x(), clRectScreen.y(), clRectScreen.width(), clRectScreen.height() );

		if(clSourcePixmap.size() == clSize)
		{	//サイズ変更なし
			clPixmap = clSourcePixmap;
		}
		else
		{	//サイズ変更あり
			if(false == clSourcePixmap.isNull())
			{
				clSourcePixmap = clSourcePixmap.scaled(clSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			}

			QPainter clPainter(&clPixmap);
			clPainter.drawPixmap((clSize.width() - clSourcePixmap.width()) / 2,
								 (clSize.height() - clSourcePixmap.height()) / 2,
								 clSourcePixmap);
		}
	}
	else
	{	//そのままキャプチャ
		QPainter clPainter(&clPixmap);
		clPainter.setRenderHint(QPainter::SmoothPixmapTransform);
		QSize clSizeBase = this->size();
		clSizeBase.scale(clSize, Qt::KeepAspectRatio);
		this->render(&clPainter, QPoint(), QRegion((clSize.width() - clSizeBase.width()) / 2,
			(clSize.height() - clSizeBase.height()) / 2, clSize.width(), clSize.height()));
	}
	return clPixmap;
}
//--------------------------------------------------------------------------------//

void Widget::slotRecvCmd(EL_CMD clCommand)
{	// 録画スレッドからの通知
	QPixmap l_clPixmap = capture(this->size());
	if(false == l_clPixmap.isNull())
	{	// 録画スレッドへ成功通知
		EL_CMD	l_clCmd = clCommand;
		EL_SET_CMD_VAL(l_clCmd, EL_CMD_KEY_PARAM2, l_clPixmap);
		EL_SET_CMD_VAL(l_clCmd, EL_CMD_KEY_ERR, CMD_ERR_SUCCESS);
		emit sigSendCmd(l_clCmd);
	}
}

void Widget::slotCmbWidthClicked(int sel)
{	// ペンの太さ
	const int value[] = {2, 4, 8, 12, 16};
	m_pclTabletcanvas->setPenWidth(value[sel]);
}
void Widget::slotCmbColorClicked(int sel)
{	// ペンの色
	const int value[] = {0x00000000, 0x000000ff, 0x00ff0000, 0x0000ffff, 0x0000ff00};
	m_pclTabletcanvas->setPenColor(value[sel]);
}
void Widget::slotCmbDepthClicked(int sel)
{	// ペンの濃さ
	const int value[] = {0, 60, 120, 180, 240};
	m_pclTabletcanvas->setPenTransparency(value[sel]);
}
void Widget::slotCmbModeClicked(int sel)
{	// ペンの種類
	switch(sel){
	case 0:
		m_pclTabletcanvas->setPenMode(IOEM_Ink);
		break;
	case 1:
		m_pclTabletcanvas->setPenMode(IOEM_Delete);
		break;
	case 2:
		m_pclTabletcanvas->setPenMode(IOEM_Select);
		break;
	}
}

void Widget::slotBtnStillClicked()
{	// 静止画保存
	QString strPath = QApplication::applicationDirPath() + "/"
		+ QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + ".jpg";
	QPixmap clPixmap = capture(this->size());
	//clPixmap = clPixmap.copy(this->rect());		//指定領域を切り出して保存
	if(true == clPixmap.save(strPath, "jpg", 80))
	{
		QFileInfo clFileInf(strPath);
		if( 0 == clFileInf.size() )
		{
			QFile::remove(strPath);
		}
	}
}

void Widget::slotBtnRecordClicked()
{	// 動画保存
	EL_CMD clCmdSend;
	if("Rec" == m_clBtnRecord->text()){
		EL_SET_CMD_VAL(clCmdSend, EL_CMD_KEY_CMD, IMA_CMD_REQ_REC_START);
		m_clBtnRecord->setText("Stop");
	}
	else{
		EL_SET_CMD_VAL(clCmdSend, EL_CMD_KEY_CMD, IMA_CMD_REQ_REC_STOP);
		m_clBtnRecord->setText("Rec");
	}
	emit sigSendCmd(clCmdSend);
}

void Widget::slotBtn1Clicked()
{
	m_pclTabletcanvas->setSaveData(&m_clData);
}

void Widget::slotBtn2Clicked()
{
	m_pclTabletcanvas->setLoadData(m_clData);
}
