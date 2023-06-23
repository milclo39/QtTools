#ifndef CLPRESENTERCO10_H
#define CLPRESENTERCO10_H

#include <QLibrary>
#include <QTime>

#include "ClUsbDriver/ClUsbDriver_global.h"
#include "win32/ClDeviceInfo_win.h"
#include "win32/ELVLib/ELVDLib.h"
#include "win32/jpeg/jpeglib.h"
#include "win32/jpeg/jerror.h"

//--------------------------------------------------------------------------------------

class ClPresenterCO10 : public ClDeviceInfo
{
public:
	ClPresenterCO10(qint32 lProductId = PRODUCT_NONE, QObject *parent = 0);
	virtual ~ClPresenterCO10();

	virtual qint32 open();										//open for a this device
	virtual void close();										//close for a this device

	virtual qint32 CommandPresenter(qint32 CmdID, QVariant& qParameter);

	char			JpegData[1280*1024*3];						//
	unsigned long	JpegSize;									//

protected:
	virtual qint64	readData ( char * data, qint64 maxSize );
	virtual qint64	writeData ( const char * data, qint64 maxSize );

	virtual qint32 getPicture(QVariant &qParam);				//get picture from presenter

	virtual qint32 execZoom(qint32 lParam);
	virtual qint32 execIris(qint32 lParam);
	virtual qint32 execAF();
	virtual qint32 execFocus(qint32 lParam);

	virtual qint32 execCommand(ClUsbCmd &clCmd);				//execute some command with device
	virtual qint32 getModelName(QVariant &qParam);				//get model name from device and output to QVariant
	virtual qint32 getVersion(QVariant &qParam);				//get version from device and output to QVariant

	ELVDLib_VideoSource *m_pclGraphManager;						//グラフィックマネージャー
	MemoryBufferHandle m_BufHandle;								//バッファハンドル
	BOOL bufferCheckedOut;										//バッファ取得フラグ

	HMODULE			hLumens;									//ルーメンス提供SDK
	long			frame_width;
	long			frame_height;
	__int64			m_Timestamp;								//フレーム取得のタイムスタンプ
	long			frame_timeout_ms;							//タイムアウトまでの時間

	#define			NAME_LUMENS_DLL	("PrsenterV2_SDK.dll")
	#define			MAIN_FPS		(15)						//フレーム速度(fps)
	#define			MAIN_FPS_SLEEP	(int)(1000/MAIN_FPS)		//MAIN_FPSを実現するために、一回のループで使用する時間(msec)

	void convertBGRtoRGB( const unsigned char* bgr,
										 unsigned char* rgb, qint32 widht, qint32 height );

	QLibrary	m_qLib;
	QTime		m_qTimeSleep;
	qint32		m_lWaitTime;
	qint32		m_lNextTime;

	unsigned char *m_ucBuffer;									//ワークバッファ
	unsigned char *m_ucJpgBuffer;								//取得データ格納


};
//--------------------------------------------------------------------------------------

#endif // CLPRESENTERCO10_H
