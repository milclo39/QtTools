#ifndef CLPRESENTERL12_WIN32_H
#define CLPRESENTERL12_WIN32_H
#include "ClUsbDriver/ClUsbDriver_global.h"
#include "win32/ClDeviceInfo_win.h"

//Q_DECLARE_METATYPE(eKINDSTREAM);
//Q_DECLARE_METATYPE(eKINDRECTID);

//--------------------------------------------------------------------------------------
class ClPresenterL12 : public ClDeviceInfo
{
	Q_OBJECT
public:
	ClPresenterL12(qint32 lProductId = PRODUCT_SESAME, QObject *parent = 0);
	virtual ~ClPresenterL12();

	virtual qint32	open();														//open for a this device
	virtual qint32	CommandPresenter(qint32 CmdID, QVariant& qParameter);

signals:
	void sigFwUpdateProgress(qint32 lParam, qint32 lStatus);					//Fwアップデートの進捗状態を伝えるシグナル

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
	virtual qint32	getZoomStatusV2(QVariant &qParam);
	virtual qint32	getFocusStatusV2(QVariant &qParam);
	virtual qint32	getAFModeStatusV2(QVariant &qParam);
	virtual qint32	getIrisStatusV2(QVariant &qParam);
	virtual qint32	getBRTModeStatusV2(QVariant &qParam);
	virtual qint32	startStreamCmdV2(QVariant &qParam);							//start stream cmd
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
	virtual qint32	getResSwV2(QVariant &qParam);
	virtual qint32	getImgOutputSwV2(QVariant &qParam);
	virtual qint32	getCapPushFlgV2(QVariant &qParam);
	virtual qint32	setSystemTimeV2(QVariant &qParam);
	virtual long	isLeapYear(unsigned long ulYear);
	virtual qint32	setModelNameV2(QVariant &qParam);
	virtual qint32	setProductCodeV2(QVariant &qParam);
	virtual qint32	getWhiteStatusV2(QVariant &qParam);
	virtual qint32	getFlickerStatusV2(QVariant &qParam);
	virtual qint32	getImageModeStatusV2(QVariant &qParam);
	virtual qint32	getEdgeStatusV2(QVariant &qParam);
	virtual qint32	getGammaStatusV2(QVariant &qParam);
	virtual qint32	getFreezeStatusV2(QVariant &qParam);
	virtual qint32	getRotationStatusV2(QVariant &qParam);
	virtual qint32	getNegaPosiStatusV2(QVariant &qParam);
	virtual qint32	getColorMonoStatusV2(QVariant &qParam);
	virtual qint32	getScopeStatusV2(QVariant &qParam);
	virtual qint32	setSpeakerStatusV2(QVariant &qParam);
	virtual qint32	getSpeakerStatusV2(QVariant &qParam);
	virtual qint32	getVideoStatusV2(QVariant &qParam);
	virtual qint32	getLanguageStatusV2(QVariant &qParam);
	virtual qint32	getGuidStatusV2(QVariant &qParam);
	virtual qint32	saveSettingV2(QVariant &qParam);
	virtual qint32	loadSettingV2(QVariant &qParam);

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

private:
	qint32			m_lCmdID;

	qint32			m_lSessionID;												//SessionID格納用
	qint32			m_lFileType;												//ファイルの種別
	qint32			m_lTimeStamp;												//タイムスタンプ

	QString			m_qStrFilePath;												//アップロード ファイルパス
	qint32			m_lUpFileSize;												//アップロード ファイルサイズ
	qint32			m_lUploadCmd;												//アップロード コマンド

	qint32			m_lWroteSize;
	qint32			m_lProgressPer;
	qint32			m_lStatus;

	bool			m_bIsFinishedStream;										//ストリームが終わったかを確認するフラグ
};

//--------------------------------------------------------------------------------------


#endif // CLPRESENTERL12_WIN32_H
