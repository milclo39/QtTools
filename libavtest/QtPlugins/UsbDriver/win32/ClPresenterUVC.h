#ifndef CLPRESENTERUVC_H
#define CLPRESENTERUVC_H

#include <QLibrary>
#include <QTime>
#include <QObject>
#include <QVBoxLayout>
#include <QTimer>

#include "ClUsbDriver/ClUsbDriver_global.h"
#include "win32/ClDeviceInfo_win.h"
#include "win32/ELVLib/ELVDLib.h"
#include "win32/jpeg/jpeglib.h"
#include "win32/jpeg/jerror.h"
#include "RectWidget/ClRectWidget.h"

#define _AUDIO_MUTE_SET_BY_VOLUME_


class QCheckBox;
class ClRollOverToolBtn;


// ～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～ //
/*! ------------------------------------------------------------------------------------- //

			@brief クラス　ELVDLibを使ったUCV・UAC向けカメラとの通信

//  ------------------------------------------------------------------------------------- */
// ～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～ //
class ClPresenterUVC : public ClDeviceInfo
{
	Q_OBJECT

public:
	ClPresenterUVC(qint32 lProductId = PRODUCT_NONE, QString strDevPath = QString(), QObject *parent = 0);
	virtual ~ClPresenterUVC();

	virtual qint32 open();										//open for a this device
	virtual void close();										//close for a this device

	virtual void reopen();										//

	virtual qint32 CommandPresenter(qint32 CmdID, QVariant& qParameter);

	virtual QWidget* getRemoconWidget();

	// 解像度のゲッタ・セッタ
	QStringList	getSupportedResolutions();
	QString		getCurrentResolution();
	qint32		setCurrentResolution(QString strResolution);

	// フレームレートのゲッタ・セッタ
	QString getSupportedFrameRates(stFRAMERATES& stFrameRates);
	double getCurrentFrameRate();
	qint32 setCurrentFrameRate(bool bIsFrameRateMin);

	//初期パラメータ設定
	void setDefaultParams();

signals:	// for private

	void sigSetCheckedAutoWB(bool);

	// リモコンの文言セッタ
	void sigSetTextRemoconExpose( QString strText );		// 露出
	void sigSetTextRemoconContrast( QString strText );		// コントラスト
	void sigSetTextRemoconHue( QString strText );			// 色相
	void sigSetTextRemoconSatuation( QString strText );		// 彩度
	void sigSetTextRemoconSharpness( QString strText );		// 鮮明度
	void sigSetTextRemoconGamma( QString strText );			// ガンマ
	void sigSetTextRemoconWB( QString strText );			// ホワイトバランス
	void sigSetTextRemoconBacklight( QString strText );		// 逆光補正
	void sigSetTextRemoconGain( QString strText );			// ゲイン
	void sigSetTextRemoconVolume( QString strText );		// ボリューム

protected:
	virtual HRESULT	buildGraphMgr( void );

	// 読込/書込
	virtual qint64	readData ( char * data, qint64 maxSize );
	virtual qint64	writeData ( const char * data, qint64 maxSize );

	// get picture from presenter
	virtual qint32 getPicture(QVariant &qParam);

	// IM用のリモコンウィジェットを作成する
	virtual void createRemoconWidget();

	// ストリームの開始と停止
	virtual	void startStream();
	virtual	void stopStream();

	// Go/Stopコマンド制御
	virtual void	startGoStop( qint32  lCmd );
	virtual void	stopGoStop( void );

protected slots:
	virtual void	slotTimeoutGoStop( void );

	// ------------------------------------------ //
	//
	//		UVC Camera control
	//
	// ------------------------------------------ //

	// focus
	virtual qint32 slotProcFocusNear();
	virtual qint32 slotProcFocusFar();
	virtual qint32 slotProcFocusStop();
	virtual qint32 slotProcFocusAuto();

	// zoom
	virtual qint32 slotProcZoomTele();
	virtual qint32 slotProcZoomWide();
	virtual qint32 slotProcZoomStop();

	// iris
	virtual qint32 slotProcBrightnessOpen();
	virtual qint32 slotProcBrightnessClose();
	virtual qint32 slotProcBrightnessStop();
	virtual qint32 slotProcBrightnessNomal();

	// expose
	virtual qint32 slotSetExpose(int lParam);

	// expose auto
	virtual qint32 slotSetExposeAuto();

	// ------------------------------------------ //
	//
	//		UVC Video amp
	//
	// ------------------------------------------ //

	// contrast
	virtual qint32 slotSetContrast(int lParam);

	// hue
	virtual qint32 slotSetHue(int lParam);

	// satuation
	virtual qint32 slotSetSatuation(int lParam);

	// sharpness
	virtual qint32 slotSetSharoness(int lParam);

	// gamma
	virtual qint32 slotSetGamma(int lParam);

	// WB
	virtual qint32 slotSetWB(int lParam);

	// WB auto
	virtual qint32 slotSetWBAuto(bool bEnable);

	// backlight
	virtual qint32 slotSetBacklight(int lParam);

	// gain
	virtual qint32 slotSetGain(int lParam);

	// ------------------------------------------ //
	//
	//		UAC
	//
	// ------------------------------------------ //

	// volume(0~100)
	virtual qint32 slotSetVolume(int iParam);

	// mute
	virtual qint32 slotSetMute(bool bIsEnable);

	// ------------------------------------------ //

	virtual qint32 execCommand(ClUsbCmd &clCmd);				//execute some command with device
	virtual qint32 getModelName(QVariant &qParam);				//get model name from device and output to QVariant
	virtual qint32 getVersion(QVariant &qParam);				//get version from device and output to QVariant

	virtual void setTranslator();

protected:	// -- Variables

	ELVDLib_VideoSource*	m_pclGraphManager;					//グラフマネージャー
	MemoryBufferHandle		m_BufHandle;						//バッファハンドル
	BOOL					bufferCheckedOut;					//バッファ取得フラグ
	qint32					m_lCmdGoStop;						//Go/Stopコマンド
	QTimer					m_clTimerGoStop;					//Go/Stopコマンド用タイマー

	long			frame_width;								//フォーマット情報(幅)
	long			frame_height;								//フォーマット情報(高)
	__int64			m_Timestamp;								//フレーム取得のタイムスタンプ
	long			frame_timeout_ms;							//タイムアウトまでの時間

	#define			MAIN_FPS		(15)						//フレーム速度(fps)
	#define			MAIN_FPS_SLEEP	(int)(1000/MAIN_FPS)		//MAIN_FPSを実現するために、一回のループで使用する時間(msec)

	void convertBGRtoRGB( const unsigned char* bgr,
										 unsigned char* rgb, qint32 widht, qint32 height );

	QTime		m_qTimeSleep;
	qint32		m_lWaitTime;
	qint32		m_lNextTime;

	unsigned char *m_ucBuffer;				//ワークバッファ
	unsigned char *m_ucJpgBuffer;			//取得データ格納


	QWidget*		m_pclRemoconWidget;		//!< リモコン（隙があったらクラスに修正したかった
	QCheckBox*		m_pclWBAuto;			//!< ホワイトバランス自動化のチェックボックス（setTextがスロットじゃなく言語変換の時に困ったのでメンバに保有
	QCheckBox*		m_pclMute;				//!< ミュートのチェックボックス（setTextがスロットじゃなく言語変換の時に困ったのでメンバに保有

	ClRollOverToolBtn* m_pclZoomTeleBtn;	//!< テレボタン
	ClRollOverToolBtn* m_pclZoomWideBtn;	//!< ワイドボタン
	ClRollOverToolBtn* m_pclFocusNearBtn;	//!< ニアボタン
	ClRollOverToolBtn* m_pclFocusFarBtn;	//!< ファーボタン
	ClRollOverToolBtn* m_pclFocusAutoBtn;	//!< AFボタン
	ClRollOverToolBtn* m_pclBrightOpenBtn;	//!< 明るくボタン
	ClRollOverToolBtn* m_pclBrightCloseBtn;	//!< 暗くボタン
	ClRollOverToolBtn* m_pclBrightDefBtn;	//!< 明るさでふぉ

	QVBoxLayout*	m_pclMainLayout;		//!< リモコンウィジェットのメインレイアウト

	QString			m_ProductName;			//!< 怪しげなXmlに入れるプロダクト名

	QList<QWidget*>	m_pclSettingWidgetList;	//!< 設定ウィジェットたち

	stRESOLUTIONS	m_stResolution;			//!< 対応解像度保持用

	QWidget			m_clParent;				//!< リモコンとか設定ウィジェットの親用

	qint32			m_iZoomInterval;		//!< ズーム	設定値間隔
	qint32			m_iFocusInterval;		//!< フォーカス	設定値間隔
	qint32			m_iBrightnessInterval;	//!< 明度	設定値間隔

	QString			m_strDevPath;			//!< デバイスパス

#ifdef  _AUDIO_MUTE_SET_BY_VOLUME_
signals:
	void sigSetMuteCheck(bool);
protected:
	DOUBLE			m_iLastVolume;			//!< ボリューム保持用
#endif

};
//--------------------------------------------------------------------------------------

#endif // CLPRESENTERUVC_H
