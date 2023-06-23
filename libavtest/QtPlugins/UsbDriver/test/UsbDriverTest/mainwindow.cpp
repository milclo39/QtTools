#include "mainwindow.h"
#include "ui_mainwindow.h"

//--------------------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	m_lProductId = 0;

    ui->setupUi(this);

	usb = new ClUsbDriver(this);


	//matsubara add. for V2 command ----------------------------------------------------
	connect(ui->pushCaptureRawV2, SIGNAL(clicked()), this, SLOT(slotPushCaptureRawV2()));
	connect(ui->pushCaptureJpegV2, SIGNAL(clicked()), this, SLOT(slotPushCaptureJpegV2()));
	connect(ui->pushStartStreamRawV2, SIGNAL(clicked()), this, SLOT(slotPushStartStreamRawV2()));
	connect(ui->pushStopStreamRawV2, SIGNAL(clicked()), this, SLOT(slotPushStopStreamRawV2()));
	connect(ui->pushStartStreamJpegV2, SIGNAL(clicked()), this, SLOT(slotPushStartStreamJpegV2()));
	connect(ui->pushStopStreamJpegV2, SIGNAL(clicked()), this, SLOT(slotPushStopStreamJpegV2()));
	connect(ui->pushRefUploadFileV2, SIGNAL(clicked()), this, SLOT(slotRefUploadFileV2()));
	connect(ui->pushSendFileInfoV2, SIGNAL(clicked()), this, SLOT(slotPushSendFileInfoV2()));
	connect(ui->pushUploadJpgV2, SIGNAL(clicked()), this, SLOT(slotPushUploadJpgV2()));
	connect(this, SIGNAL(sigSendUploadFileInfoV2(qint32,qint32,QVariant&)), usb, SLOT(slotSendImgFileInfoV2(qint32,qint32,QVariant&)));
	connect(this, SIGNAL(sigUploadJpgFileV2(qint32,qint32,QVariant&)), usb, SLOT(slotUploadFileV2(qint32,qint32,QVariant&)));

	connect(ui->pushButton_myCmdV1, SIGNAL(clicked()), this, SLOT(slotPushMyCmdV1()));
	connect(ui->pushButton_myCmdV2, SIGNAL(clicked()), this, SLOT(slotPushMyCmdV2()));
	connect(this, SIGNAL(sigExternalInputCmd(qint32,QVariant,QVariant&)), usb, SLOT(slotExternalInputCmd(qint32,QVariant,QVariant&)));
	connect(this, SIGNAL(sigExternalInputCmdV2(qint32,QVariant,QVariant&)), usb, SLOT(slotExternalInputCmdV2(qint32,QVariant,QVariant&)));
	//----------------------------------------------------------------------------------


	//button connect
	//line 1
	connect(ui->pushStartStreamRaw, SIGNAL(clicked()), this, SLOT(slotPushStartStreamRaw()));
	connect(ui->pushStopStreamRaw, SIGNAL(clicked()), this, SLOT(slotPushStopStreamRaw()));
	connect(ui->pushStartStreamJpeg, SIGNAL(clicked()), this, SLOT(slotPushStartStreamJpeg()));
	connect(ui->pushStopStreamJpeg, SIGNAL(clicked()), this, SLOT(slotPushStopStreamJpeg()));
	connect(ui->pushGetFRateRaw, SIGNAL(clicked()), this, SLOT(slotPushGetFrameRateRaw()));
	connect(ui->pushSetFRateRaw, SIGNAL(clicked()), this, SLOT(slotPushSetFrameRateRaw()));
	connect(ui->pushGetFRateJpeg, SIGNAL(clicked()), this, SLOT(slotPushGetFrameRateJpeg()));
	connect(ui->pushSetFRateJpeg, SIGNAL(clicked()), this, SLOT(slotPushSetFrameRateJpeg()));
	connect(ui->pushCaptureRaw, SIGNAL(clicked()), this, SLOT(slotPushCaptureRaw()));
	connect(ui->pushCaptureJpeg, SIGNAL(clicked()), this, SLOT(slotPushCaptureJpeg()));

	//line 2
	connect(ui->pushModelName, SIGNAL(clicked()), this, SLOT(slotPushModelName()));
	connect(ui->pushModelversion, SIGNAL(clicked()), this, SLOT(slotPushModelVersion()));
	connect(ui->pushGetPresenterList, SIGNAL(clicked()), this, SLOT(slotPushGetPresenterList()));
	connect(ui->pushGetRect, SIGNAL(clicked()), this, SLOT(slotPushGetRect()));
	connect(ui->pushGetSize, SIGNAL(clicked()), this, SLOT(slotPushGetSize()));
	connect(ui->pushSetRect, SIGNAL(clicked()), this, SLOT(slotPushSetRect()));

	//line 3
	connect(ui->pushZoomTele, SIGNAL(clicked()), this, SLOT(slotPushZoomTele()));
	connect(ui->pushZoomWide, SIGNAL(clicked()), this, SLOT(slotPushZoomWide()));
	connect(ui->pushZoomStop, SIGNAL(clicked()), this, SLOT(slotPushZoomStop()));
	connect(ui->pushIrisOpen, SIGNAL(clicked()), this, SLOT(slotPushIrisBright()));
	connect(ui->pushIrisClose, SIGNAL(clicked()), this, SLOT(slotPushIrisDark()));
	connect(ui->pushIrisStop, SIGNAL(clicked()), this, SLOT(slotPushIrisStop()));
	connect(ui->pushIrisNormal, SIGNAL(clicked()), this, SLOT(slotPushIrisNormal()));
	connect(ui->pushFocusNear, SIGNAL(clicked()), this, SLOT(slotPushFocusNear()));
	connect(ui->pushFocusFar, SIGNAL(clicked()), this, SLOT(slotPushFocusFar()));
	connect(ui->pushFocusStop, SIGNAL(clicked()), this, SLOT(slotPushFocusStop()));
	connect(ui->pushFocusAuto, SIGNAL(clicked()), this, SLOT(slotPushFocusAuto()));

	//line 4
	connect(ui->pushRotateOn, SIGNAL(clicked()), this, SLOT(slotPushRotateOn()));
	connect(ui->pushRotateOff, SIGNAL(clicked()), this, SLOT(slotPushRotateOff()));
	connect(ui->pushPosi, SIGNAL(clicked()), this, SLOT(slotPushPosi()));
	connect(ui->pushNega, SIGNAL(clicked()), this, SLOT(slotPushNega()));
	connect(ui->pushColor, SIGNAL(clicked()), this, SLOT(slotPushColor()));
	connect(ui->pushBw, SIGNAL(clicked()), this, SLOT(slotPushBw()));
	connect(ui->pushSHOn, SIGNAL(clicked()), this, SLOT(slotPushSHOn()));
	connect(ui->pushSHOff, SIGNAL(clicked()), this, SLOT(slotPushSHOff()));
	connect(ui->pushImageMode, SIGNAL(clicked()), this, SLOT(slotPushImageMode()));
	connect(ui->pushMSOn, SIGNAL(clicked()), this, SLOT(slotPushMSOn()));
	connect(ui->pushMSOff, SIGNAL(clicked()), this, SLOT(slotPushMSOff()));

	//open
	connect(usb,SIGNAL(sigOpenDevice(qint32)),this,SLOT(slotOpenDevice(qint32)),Qt::QueuedConnection);
	//close
	connect(usb,SIGNAL(sigCloseDevice(qint32)),this,SLOT(slotCloseDevice(qint32)),Qt::QueuedConnection);

	//stream
	//start stream
	connect(this, SIGNAL(sigStartStream(qint32,eKINDSTREAM,qint32,qint32)), usb, SLOT(slotStartStream(qint32,eKINDSTREAM,qint32,qint32)));
	//stop stream
	connect(this, SIGNAL(sigStopStream(qint32,eKINDSTREAM)), usb, SLOT(slotStopStream(qint32,eKINDSTREAM)));
	//stream image
	connect(usb,SIGNAL(sigRawImage(qint32, qint32, QImage)),this,SLOT(slotRawImage(qint32, qint32, QImage)));
	connect(usb,SIGNAL(sigJpegImage(qint32, qint32, QByteArray)),this,SLOT(slotJpegImage(qint32, qint32, QByteArray)));
	//get framerate
	connect(this, SIGNAL(sigGetFrameRate(qint32,eKINDSTREAM,qint32&)), usb, SLOT(slotGetFRate(qint32,eKINDSTREAM,qint32&)));
	//set framerate
	connect(this, SIGNAL(sigSetFrameRate(qint32,eKINDSTREAM,qint32)), usb, SLOT(slotSetFRate(qint32,eKINDSTREAM,qint32)));
	//capture
	connect(this, SIGNAL(sigCapture(qint32,eKINDSTREAM,qint32&,QVariant&,eKINDRECTID)), usb, SLOT(slotCapture(qint32,eKINDSTREAM,qint32&,QVariant&,eKINDRECTID)));

	//info
	//get presenter name
	connect(this, SIGNAL(sigPresenterName(qint32,QString&)), usb, SLOT(slotGetPresenterName(qint32,QString&)));
	//get presenter version
	connect(this, SIGNAL(sigPresenterVersion(qint32,QStringList&)), usb, SLOT(slotGetPresenterVersion(qint32,QStringList&)));
	//get list of presenter
	connect(this,SIGNAL(sigPresenterList(QVariantList&)), usb, SLOT(slotGetPresenterList(QVariantList&)));
	//get height and width of picture
	connect(this, SIGNAL(sigGetRect(qint32,QSize&)), usb, SLOT(slotGetPictureRect(qint32,QSize&)));
	//get size of picture
	connect(this, SIGNAL(sigGetSize(qint32,qint32&)), usb, SLOT(slotGetPictureSize(qint32,qint32&)));
	//get RectID of picture
	connect(this, SIGNAL(sigGetRectId(qint32,eKINDRECTID&)), usb, SLOT(slotGetRectId(qint32,eKINDRECTID&)));
	//set rectID of picture
	connect(this, SIGNAL(sigSetRect(qint32,eKINDRECTID)), usb, SLOT(slotSetRectId(qint32,eKINDRECTID)));

	//camera control
	connect(this, SIGNAL(sigZoom(qint32, qint32)), usb, SLOT(slotZoom(qint32,qint32)));
	connect(this, SIGNAL(sigIris(qint32,qint32)), usb, SLOT(slotIris(qint32,qint32)));
	connect(this, SIGNAL(sigFocus(qint32,qint32)), usb, SLOT(slotFocus(qint32,qint32)));
	connect(this, SIGNAL(sigRotate(qint32,qint32)), usb, SLOT(slotRotate(qint32,qint32)));
	connect(this, SIGNAL(sigPosiNega(qint32, qint32)), usb, SLOT(slotPosiNega(qint32,qint32)));
	connect(this, SIGNAL(sigColorBw(qint32,qint32)), usb, SLOT(slotColorBw(qint32,qint32)));
	connect(this, SIGNAL(sigSlowShutter(qint32,qint32)), usb, SLOT(slotSlowShutter(qint32,qint32)));
	connect(this, SIGNAL(sigImageMode(qint32,qint32)), usb, SLOT(slotImageMode(qint32,qint32)));
	connect(this, SIGNAL(sigMicroscopeMode(qint32,qint32)), usb, SLOT(slotMicroscopeMode(qint32,qint32)));

	connect(ui->pushPrintAllDevice, SIGNAL(clicked()),usb,SLOT(slotChkAllDevice()));

	connect(ui->pushButton_setSysTime, SIGNAL(clicked()), this, SLOT(slotPushSetSysTime()));
	connect(this, SIGNAL(sigSetSystemTimeV2(qint32,QDateTime&)), usb, SLOT(slotSetSystemTimeV2(qint32,QDateTime&)));

	connect(ui->pushGetV2, SIGNAL(clicked()), this, SLOT(slotPushGetV2()));

	connect(ui->pushButton_clear, SIGNAL(clicked()), this, SLOT(slotPushClearBtn()));

	QComboBox *pclCombo = ui->comboCommand;
	pclCombo->addItem("zoom");
	pclCombo->addItem("focus");
	pclCombo->addItem("afmode");
	pclCombo->addItem("iris");
	pclCombo->addItem("brtmode");
	pclCombo->addItem("ressw");
	pclCombo->addItem("imout");
	pclCombo->addItem("cappush");
	pclCombo->addItem("white");

	usb->initialize();
#if 0
	bRes = usb->isInitialize();
	qDebug() << "isInitialized : " << bRes;
#endif

	//FPS測定用
	m_clTimerStreamRaw.start();
	m_clTimerStreamJpg.start();
	m_lCnt = 0;

	//音声確認用
	m_pclAudioOutput	= NULL;
	m_pclAudioStream	= NULL;
	QAudioFormat clFormat;
	clFormat.setFrequency(44100);						//サンプリング周波数
	clFormat.setChannels(2);							//チャネル数
	clFormat.setSampleSize(16);							//bps
	clFormat.setCodec(AUDIO_INFO_FORMAT_QT);			//フォーマット
	clFormat.setSampleType(QAudioFormat::SignedInt);

	//デフォルト音声出力デバイス情報取得
	QAudioDeviceInfo clInfo = QAudioDeviceInfo::defaultOutputDevice();
	if(clInfo.isNull() == true ||						//取得失敗
	   clInfo.isFormatSupported(clFormat) == false)		//フォーマット未対応
	{
		qDebug("Audio --- Device Format Not Support");
		//return false;									//再生不可能
	}
	m_pclAudioOutput	= new QAudioOutput(clFormat);

	//Live
	m_pclLiveDialog = new LiveDialog(this);
//	connect(this, SIGNAL(sigStreamImg(QByteArray)), m_pclLiveDialog, SLOT(slotStreamImg(QByteArray)));
//	m_pclLiveDialog->show();

}
//--------------------------------------------------------------------------------------

MainWindow::~MainWindow()
{
	if(usb != NULL)
	{
		delete usb;
		usb = NULL;
	}

	if(m_pclAudioStream != NULL)
	{
		delete m_pclAudioStream;
		m_pclAudioStream	= NULL;
	}
	if(m_pclAudioStream != NULL)
	{
		m_pclAudioOutput->stop();
		delete m_pclAudioOutput;
		m_pclAudioOutput	= NULL;
	}

	if(m_pclLiveDialog != NULL)
	{
		delete m_pclLiveDialog;
		m_pclLiveDialog = NULL;
	}

    delete ui;
}
//--------------------------------------------------------------------------------------
void MainWindow::slotPushClearBtn()
{
	this->ui->textEdit_log->clear();
}
//--------------------------------------------------------------------------------------
void MainWindow::slotRawImage(qint32 lProductId, qint32 lStatus, QImage qImage)
{
#if 1
	if(lStatus == IM_ERROR_SUCCESS){
//		qDebug() << lProductId << " : get raw Picture : " << qImage.byteCount();

		//ログの表示（FPS)
		m_lCnt++;
		QString clStrLog = QString("No.%1 \t Stream Raw elpased time :: %2 [msec]").arg(m_lCnt).arg(m_clTimerStreamRaw.elapsed());
		ui->textEdit_log->append(clStrLog);
		m_clTimerStreamRaw.restart();
	}else if(lStatus == IM_ERROR_CANNOT_OUTPUT_IMAGE){
		qDebug() << lProductId << " :  get raw Picture : SD MODE NOW!";
	}
#else
	QString qStrLog = QString("stream raw --- status %1, img size (%2,%3)").arg(lStatus).arg(qImage.size().width()).arg(qImage.size().height());
	this->ui->textEdit_log->append(qStrLog);
//	qDebug() << "get stream --- " << qImage.size();
#endif
}

//--------------------------------------------------------------------------------------
void MainWindow::slotJpegImage(qint32 lProductId, qint32 lStatus, QByteArray Ary)
{
#if 1
	if(lStatus == IM_ERROR_SUCCESS){
		//matsubara add.
		char cJudge[2];
		if(!Ary.isEmpty())
		{
			memcpy(cJudge, Ary.constData(), 2);
		}
		else
		{
			qDebug("get jpg image is empty --- size : %d",Ary.length());
			return;
		}
		QString clStrLog;
		if((unsigned char)cJudge[0] == 0xFF && (unsigned char)cJudge[1] == 0xD8)				//jpgデータであるか判定
		{
			//LiveDialogへ送る
//			emit sigStreamImg(Ary);

			//ログ
			clStrLog = QString("No.%1 \t Stream (jpeg ) elpased time :: %2 [msec], length : %3").arg(m_lCnt).arg(m_clTimerStreamJpg.elapsed()).arg(Ary.length());
			qDebug() << "count : " << m_lCnt << ", ProductID : " << lProductId << ", data length : " << Ary.length() << "--- jpg";
		}
		else																					//音声の場合
		{
			//音声確認用
			if(m_pclAudioStream == NULL){
				m_pclAudioStream = m_pclAudioOutput->start();
			}
			if(m_pclAudioStream != NULL){
				m_pclAudioStream->write(Ary);
			}

			//音声データ保存用
			m_qArraySound.append(Ary);
			//ログ
			clStrLog = QString("No.%1 \t Stream (sound) elpased time :: %2 [msec], length : %3").arg(m_lCnt).arg(m_clTimerStreamJpg.elapsed()).arg(Ary.length());
//			qDebug() << "count : " << m_lCnt << ", ProductID : " << lProductId << ", data length : " << Ary.length() << "--- sound";
		}
		//ログの表示（FPS)
		m_lCnt++;
		ui->textEdit_log->append(clStrLog);
		m_clTimerStreamJpg.restart();
	}else if(lStatus == IM_ERROR_CANNOT_OUTPUT_IMAGE){
		ui->textEdit_log->append("SD MOCE NOW!");
		qDebug() << "count : " << m_lCnt << ", ProductID : " << lProductId << ", data length : SD MODE NOW!";
	}

	QByteArray qArrayImg;
	QFile file("test.jpg");
	if(file.open(QFile::ReadOnly)){
		qArrayImg = file.readAll();
		file.close();
	}
	//LiveDialogへ送る
	emit sigStreamImg(qArrayImg);
#else
	QString qStrLog = QString("stream raw --- status %1, img length %2").arg(lStatus).arg(Ary.length());
	this->ui->textEdit_log->append(qStrLog);
#endif

}

//--------------------------------------------------------------------------------------
void MainWindow::slotOpenDevice(qint32 lProductId)
{
	QVariantList clVListProduct;
	emit sigPresenterList(clVListProduct);
	ui->textEdit_log->append("product list ");
	for(qint32 i = 0; i < clVListProduct.count(); i++)
	{
		ui->textEdit_log->append(clVListProduct.at(i).toString());
	}

	qDebug("slotOpenDevice 0x%x",lProductId);
	QString clStrLog = QString("slotOpenDevice %1").arg(lProductId);
	ui->textEdit_log->append(clStrLog);

	m_lProductId = lProductId;

//	slotPushSetRect();

//	slotPushCaptureRaw();
//	slotPushCaptureJpeg();

//	slotPushStartStreamJpeg();
//	slotPushStartStreamRaw();
}

//--------------------------------------------------------------------------------------
void MainWindow::slotCloseDevice(qint32 lProductId)
{
	QVariantList clVListProduct;
	emit sigPresenterList(clVListProduct);
	ui->textEdit_log->append("product list ");
	for(qint32 i = 0; i < clVListProduct.count(); i++)
	{
		ui->textEdit_log->append(clVListProduct.at(i).toString());
	}

	qDebug("slotCloseDevice 0x%x",lProductId);
	QString clStrLog = QString("slotCloseDevice %1").arg(lProductId);
	ui->textEdit_log->append(clStrLog);
	if(lProductId == m_lProductId)
	{
		m_lProductId = 0;
	}
#if 0
	usb->closeDevice();
#endif

//	slotPushStopStreamJpeg();
//	slotPushStopStreamRaw();

}
//--------------------------------------------------------------------------------------
void MainWindow::slotPushSetRect()
{
	eKINDRECTID lRectId = static_cast<eKINDRECTID>(ui->comboSize->currentIndex() + 1);
	emit sigSetRect(m_lProductId, lRectId);
}
//--------------------------------------------------------------------------------------
void MainWindow::slotPushImageMode()
{
	emit sigImageMode(m_lProductId, ui->comboSize->currentIndex() + 1);
}


//--------------------------------------------------------------------------------------
void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


//--------------------------------------------------------------------------------------
/*
										V2 コマンド
*/
//--------------------------------------------------------------------------------------
void MainWindow::slotPushCaptureRawV2()
{
	QVariant qParam;
	qint32 lStatus = 0;
	emit sigCapture(m_lProductId, RAW_STREAM_V2, lStatus, qParam, RECTID_PICTURE_DEFAULT);
	if(lStatus == IM_ERROR_SUCCESS){
		if(qParam.type() == QVariant::Image){
			qDebug() << "get to the Raw picture V2";
		}else{
			qDebug() << "sigCapture RAW error V2";
		}
	}else if(lStatus == IM_ERROR_CANNOT_OUTPUT_IMAGE){
		qDebug() << "sigCapture SD MODE NOW!";
	}
}
//--------------------------------------------------------------------------------------
void MainWindow::slotPushCaptureJpegV2()
{
	QVariant qParam;
	qint32 lStatus = 0;
	emit sigCapture(m_lProductId, JPEG_STREAM_V2, lStatus, qParam, RECTID_PICTURE_DEFAULT);
	if(lStatus == IM_ERROR_SUCCESS){
		if(qParam.type() == QVariant::ByteArray){
			qDebug() << "get to the Jpeg picture V2";

			//save jpg capture image
			QByteArray qArrayJpg(qParam.toByteArray().constData(), qParam.toByteArray().length());
			QFile file("testV2.jpg");
			if(file.open(QFile::WriteOnly))
			{
				file.write(qArrayJpg);
				file.close();
			}
		}else{
			qDebug() << "sigCapture Jpeg error V2";
		}
	}else if(lStatus == IM_ERROR_CANNOT_OUTPUT_IMAGE){
		qDebug() << "sigCapture SD MODE NOW!";
	}
}
//--------------------------------------------------------------------------------------
void MainWindow::slotPushStartStreamRawV2()
{
	m_lCnt = 0;
	m_clTimerStreamRaw.restart();
	emit sigStartStream(m_lProductId, RAW_STREAM_V2, 30, 80);
}
//--------------------------------------------------------------------------------------
void MainWindow::slotPushStopStreamRawV2()
{
	emit sigStopStream(m_lProductId, RAW_STREAM_V2);
}
//--------------------------------------------------------------------------------------
void MainWindow::slotPushStartStreamJpegV2()
{
	qDebug("slotPushStartStreamJpeg");

	//FPS測定用処理
	m_lCnt = 0;
	m_clTimerStreamJpg.restart();

	//SIGNAL送信
	emit sigStartStream(m_lProductId, JPEG_STREAM, 30, 80);								//ストリームスレッド1 開始シグナル
	emit sigStartStream(m_lProductId, JPEG_STREAM_V2, 30, 80);							//ストリームスレッド2 開始シグナル
}
//--------------------------------------------------------------------------------------
void MainWindow::slotPushStopStreamJpegV2(){
	qDebug("slotPushStopStreamJpeg");

	//音声保存
	QFile file("sound");																//ファイル保存
	if(file.open(QFile::ReadWrite))
	{
		file.write(m_qArraySound);
		file.close();
	}
	m_qArraySound.clear();

	//SIGNAL送信
	emit sigStopStream(m_lProductId, JPEG_STREAM);										//ストリームスレッド1 終了シグナル
	emit sigStopStream(m_lProductId, JPEG_STREAM_V2);									//ストリームスレッド2 終了シグナル
}
//--------------------------------------------------------------------------------------
void MainWindow::slotRefUploadFileV2()																		//matsubara add. for V2 command
{
	QString fileName = QFileDialog::getOpenFileName(NULL,
								tr("SingleTest open file dialog"),
								".",
								tr("Jpeg Files (*.jpg)"));
	if (!fileName.isEmpty()){
		ui->lineEditUploadFilePathV2->setText(fileName);
	}
	return;
}
//--------------------------------------------------------------------------------------
void MainWindow::slotPushSendFileInfoV2()																		//matsubara add. for V2 command
{
	//指定したファイルのパスを取得
	QString qWork;
	qWork = ui->lineEditUploadFilePathV2->text();																//参照したファイルパスを取得
	//保存先デバイスタイプを設定
	QString qDeviceType = QString("%1").arg(ui->comboDevTypeV2->currentIndex());
	//リストに登録
	QStringList lStrList;
	lStrList.append(qWork);
	lStrList.append(qDeviceType);
	qDebug() << "send image file info : \n" << lStrList;
	//送信できる形式に変換
	QVariant qVariant;
	qVariant = lStrList;
	emit sigSendUploadFileInfoV2(m_lProductId, SEND_UPIM_INFO_V2, qVariant);
	return;
}

void MainWindow::slotPushUploadJpgV2()																		//matsubara add. for V2 command
{
	//指定したファイルのパスを取得
	QVariant qVariant;
	emit sigUploadJpgFileV2(m_lProductId, UPLOAD_FILE_V2, qVariant);
	return;
}

//--------------------------------------------------------------------------------------
/*
									外部入力 コマンド
*/
//--------------------------------------------------------------------------------------
void MainWindow::slotPushMyCmdV1()
{
	QByteArray qArrayCmd;
	QDataStream qWork(&qArrayCmd, QIODevice::WriteOnly);
	qWork.setByteOrder(QDataStream::LittleEndian);
	//バージョン取得
	qWork	<< qint16(0x0000)								//PacketType
			<< qint16(0x0000)								//TransID
			<< qint32(0x00000018)							//DataSize
			<< qint16(0x8b11)								//Command
			<< qint16(0x0000)								//Dummy
			<< qint32(0x00000000)							//Param1
			<< qint32(0x00000000)							//Param2
			<< qint32(0x00000000)							//Param3
			<< qint32(0x00000000)							//Param4
			<< qint32(0x00000000);							//Param5
#if 0
	//Zoom
	qWork	<< qint16(0x0000)								//PacketType
			<< qint16(0x0000)								//TransID
			<< qint32(0x00000018)							//DataSize
			<< qint16(0x00e0)								//Command
			<< qint16(0x0000)								//Dummy
			<< qint32(0x00000001)							//Param1
			<< qint32(0x00000000)							//Param2
			<< qint32(0x00000000)							//Param3
			<< qint32(0x00000000)							//Param4
			<< qint32(0x00000000);							//Param5
#endif
	QVariant qVariantReq(qArrayCmd);
	QVariant qVariantRes;
	emit sigExternalInputCmd(m_lProductId, qVariantReq, qVariantRes);

	PELMO_PACKET  sReturn	= (PELMO_PACKET)qVariantRes.toByteArray().constData();
	PELMO_COMMAND pRetPacket	= (PELMO_COMMAND)sReturn->Data;
	QString clStr;
	QTextStream clTS(&clStr);
	clTS
	<< "res command v1 \n"
	<< " getHeaderVer\t"	<< sReturn->Header.PacketType	<< "\n"
	<< " getTransId\t"	<< sReturn->Header.TransID			<< "\n"
	<< " getDataSize\t"	<< sReturn->Header.DataSize			<< "\n"
	<< " getCode\t"		<< pRetPacket->OpCode				<< "\n"
	<< " getParam1\t"	<< pRetPacket->Param1				<< "\n"
	<< " getParam2\t"	<< pRetPacket->Param2				<< "\n"
	<< " getParam3\t"	<< pRetPacket->Param3				<< "\n"
	<< " getParam4\t"	<< pRetPacket->Param4				<< "\n"
	<< " getParam5\t"	<< pRetPacket->Param5				<< "\n";
	ui->textEdit_log->append(clStr);
}
//--------------------------------------------------------------------------------------
void MainWindow::slotPushMyCmdV2()
{
	QByteArray qArrayCmd;
	QDataStream qWork(&qArrayCmd, QIODevice::WriteOnly);
	qWork	<< qint8(0x10)									//HeaderVersion
			<< qint8(0x00)									//TransID
			<< qint16(0x0000)								//Command
			<< qint32(0x00000000)							//DataSize
			<< qint32(0x7fffffff)							//Param0
			<< qint32(0x80000000)							//Param1
			<< qint32(0x00000000)							//Param2
			<< qint32(0x00000000)							//Param3
			<< qint32(0x00000000)							//Param4
			<< qint32(0x00000000);							//Param5
	QVariant qVariantReq(qArrayCmd);
	QVariant qVariantRes;

	PELMO_PACKET_V2  sReturn1	= (PELMO_PACKET_V2)qVariantReq.toByteArray().constData();
	PELMO_COMMAND_V2 pRetPacket1	= (PELMO_COMMAND_V2)sReturn1->Data;
	qDebug() << "request command external Input";
	qDebug() << "getHeaderVer\t"	<< sReturn1->Header.HeaderVer;
	qDebug() << "getTransId\t"		<< sReturn1->Header.TransID;
	qDebug() << "getCommand\t"		<< sReturn1->Header.OpCode;
	qDebug() << "getDataSize\t"		<< sReturn1->Header.DataSize;
	qDebug() << "getParam0\t"		<< pRetPacket1->Param0;
	qDebug() << "getParam1\t"		<< pRetPacket1->Param1;
	qDebug() << "getParam2\t"		<< pRetPacket1->Param2;
	qDebug() << "getParam3\t"		<< pRetPacket1->Param3;
	qDebug() << "getParam4\t"		<< pRetPacket1->Param4;
	qDebug() << "getParam5\t"		<< pRetPacket1->Param5;

	emit sigExternalInputCmdV2(m_lProductId, qVariantReq, qVariantRes);

	PELMO_PACKET_V2  sReturn	= (PELMO_PACKET_V2)qVariantRes.toByteArray().constData();
	PELMO_COMMAND_V2 pRetPacket	= (PELMO_COMMAND_V2)sReturn->Data;
	qDebug() << "responce command external Input";
	qDebug() << "getHeaderVer\t"	<< sReturn->Header.HeaderVer;
	qDebug() << "getTransId\t"		<< sReturn->Header.TransID;
	qDebug() << "getCommand\t"		<< sReturn->Header.OpCode;
	qDebug() << "getDataSize\t"		<< sReturn->Header.DataSize;
	qDebug() << "getParam0\t"		<< pRetPacket->Param0;
	qDebug() << "getParam1\t"		<< pRetPacket->Param1;
	qDebug() << "getParam2\t"		<< pRetPacket->Param2;
	qDebug() << "getParam3\t"		<< pRetPacket->Param3;
	qDebug() << "getParam4\t"		<< pRetPacket->Param4;
	qDebug() << "getParam5\t"		<< pRetPacket->Param5;

	QString clStr;
	QTextStream clTS(&clStr);
	clTS
	<< "res command v2 \n"
	<< " getHeaderVer\t"<< sReturn->Header.DataSize		<< "\n"
	<< " getTransId\t"	<< sReturn->Header.TransID		<< "\n"
	<< " getCommand\t"	<< sReturn->Header.OpCode		<< "\n"
	<< " getDataSize\t"	<< sReturn->Header.DataSize		<< "\n"
	<< " getParam0\t"	<< pRetPacket->Param0			<< "\n"
	<< " getParam1\t"	<< pRetPacket->Param1			<< "\n"
	<< " getParam2\t"	<< pRetPacket->Param2			<< "\n"
	<< " getParam3\t"	<< pRetPacket->Param3			<< "\n"
	<< " getParam4\t"	<< pRetPacket->Param4			<< "\n"
	<< " getParam5\t"	<< pRetPacket->Param5			<< "\n";
	ui->textEdit_log->append(clStr);
}

void MainWindow::slotPushSetSysTime()
{
	QDateTime qDateTime;
	qDateTime = QDateTime::currentDateTime();
	qDebug() << qDateTime;

	emit sigSetSystemTimeV2(m_lProductId, qDateTime);
}

void MainWindow::slotPushGetV2()
{
	QString strCommand = ui->comboCommand->currentText();

	if(strCommand == "zoom")
	{
		connect(this, SIGNAL(sigGetV2(qint32,qint32&)), usb, SLOT(slotGetZoomStatusV2(qint32,qint32&)));
	}
	else if(strCommand == "focus")
	{
	}
	else if(strCommand =="afmode")
	{
	}
	else if(strCommand == "iris")
	{
	}
	else if(strCommand == "brtmode")
	{
	}
	else if(strCommand == "ressw")
	{
	}
	else if(strCommand == "imout")
	{
	}
	else if(strCommand == "cappush")
	{
	}
	else if(strCommand == "white")
	{
	}

	qint32 lParam;
	emit sigGetV2(m_lProductId, lParam);

	QString strText = strCommand;
	strText += ":" + QString::number(lParam);
	ui->textEdit_log->append(strText);

	disconnect(this, SIGNAL(sigGetV2(qint32,qint32&)));
}
