#ifndef CLPRESENTERL12_WIN32_H
#define CLPRESENTERL12_WIN32_H
#include "mac/ClDeviceInfo_mac.h"
#include "ClUsbDriver/ClUsbDriver_global.h"

//--------------------------------------------------------------------------------------
class ClPresenterL12 : public ClDeviceInfo
{
	Q_OBJECT
public:
	ClPresenterL12(qint32 lProductId = PRODUCT_SESAME, QObject *parent = 0);
	virtual ~ClPresenterL12();

	virtual qint32	open();														//open for a this device
	virtual qint32	CommandPresenter(qint32 CmdID, QVariant& qParameter);

protected:
	virtual qint32	setupEndpoint();											//Override function

	//V1 Command
	virtual qint32	getPicture(QVariant &qParam);								//get picture from presenter
	virtual qint32	getModelName(QVariant &qParam);								//get model name from device and output to QVariant
	virtual qint32	getVersion(QVariant &qParam);								//get version from device and output to QVariant
	virtual qint32	execCaptureInit();											//CaptureFlag
	virtual qint32	getCaptureStatus(QVariant &Param);							//function of get CaptureFlag

	//V2 Command
	virtual qint32	getPictureV2(QVariant &qParam);								//get picture from presenter command v2 version
	virtual qint32	startStreamCmdV2();											//start stream cmd
	virtual qint32	getStreamV2(QVariant &qParam);								//get stream from presenter command v2 version
	virtual qint32	stopStreamCmdV2();											//stop stream cmd
	virtual qint32	sendImgFileInfoCmdV2(QVariant &qParam);						//send upload imaeg file info
	virtual qint32	sendFwFileInfoCmdV2(QVariant &qParam);						//send upload fw file info
	virtual qint32	uploadFileCmdV2(QVariant &qParam);							//upload file
	virtual qint32	getModelNameV2(QVariant &qParam);							//get model name from device and output to QVariant
	virtual qint32	getVersionV2(QVariant &qParam);								//get model version from device and output to QVariant
	virtual qint32	getSHVersionV2(QVariant &qParam);							//get sh version from device and output to QVariant
	virtual qint32	getLensVersionV2(QVariant &qParam);							//get lens version from device and output to QVariant
	virtual qint32	getFPGAVersionV2(QVariant &qParam);							//get fpga version from device and output to QVariant
	virtual qint32	getProductCodeV2(QVariant &qParam);
	virtual qint32	getCurrentModeV2(QVariant &qParam);
	virtual qint32	setSystemTimeV2(QVariant &qParam);
	virtual long	isLeapYear(unsigned long ulYear);
	virtual qint32 setModelNameV2(QVariant &qParam);
	virtual qint32 setProductCodeV2(QVariant &qParam);

	virtual qint32	externalInputCmd(QVariant &qParam);
	virtual qint32	externalInputCmdV2(QVariant &qParam);

	virtual qint64	readData ( char * data, qint64 maxSize );					//
	virtual qint64	writeData ( const char * data, qint64 maxSize );			//
	virtual qint32	reqPicture(eENDPOINT	*peOutEndp, QVariant &param);
	virtual qint32	resPicture(eENDPOINT	*peInEndp, QVariant &param);
	virtual qint32	reqCommand(eENDPOINT	*peOutEndp, QVariant &param);
	virtual qint32	resCommand(eENDPOINT	*peInEndp, QVariant &param);
	virtual bool	analyJpegHeader(const QByteArray qJpgData, QSize &qSize);
	virtual qint32	execCommand(ClUsbCmd &clCmd);

	//V2 Command
	virtual qint32	reqPictureV2(eENDPOINT	*pOutEndp, QVariant &param);		//
	virtual qint32	resPictureV2(eENDPOINT	*pInEndp, QVariant &qParam);		//
	virtual qint32	reqStreamV2(eENDPOINT	*pOutEndp, QVariant &param);		//
	virtual qint32	resStreamV2(eENDPOINT	*pInEndp, QVariant &qParam);		//
	virtual qint32	execCommandV2(ClUsbCmdV2 &clCmd);
	virtual qint32	execGetStatusCommand(ClUsbCmdV2 &clCmd, QVariant &clParam);

signals:
	void sigFwUpdateProgress(qint32 lParam, qint32 lStatus);	//ファームアップデート進捗状態

private:
	qint32			m_lCmdID;
	qint32			m_lJpgSize;
	qint32			m_lJudgeCmdVer;

	qint32			m_lImgDataSize;												//1画像分のJpgサイズ
	qint32			m_lFileSize;												//作成する画像ファイルのサイズ
	qint32			m_lSessionID;												//SessionID格納用
	qint32			m_lFileType;												//ファイルの種別
	qint32			m_lTimeStamp;												//タイムスタンプ

	QString			m_clStrFilePath;											//アップロード ファイルパス
	qint32			m_lUpFileSize;												//アップロード ファイルサイズ
	qint32			m_lUploadCmd;												//アップロード コマンド

	QByteArray		m_qArrayBackStream;											//未送信ストリーム

	qint32			m_lWroteSize;
	qint32			m_lProgressPer;
	qint32			m_lStatus;
};
//--------------------------------------------------------------------------------------


#endif // CLPRESENTERL12_WIN32_H
