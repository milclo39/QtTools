//--------------------------------------------------------------------------------//
/*!
	@file	ClUvcDriver.h
	@brief	UVCドライバヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLUVCDRIVER_H
#define CLUVCDRIVER_H
#include <QAxObject>
#include <QPair>
#include <QUuid>
#include <QList>
#include <QImage>
#include <QTimer>
#include <QLabel>
#include <QFile>

#if 0 //平均フレームレート測定
#define MEASURE_AVGFRANMERATE
#endif

struct IMoniker;
struct IKsTopologyInfo;
struct IPin;
struct IBaseFilter;
struct IGraphBuilder;
struct ISampleGrabber;
struct IAMVideoProcAmp;
struct IAMCameraControl;
struct IVMRWindowlessControl9;
struct ISpecifyPropertyPages;
#if defined(MEASURE_AVGFRANMERATE)
struct IQualProp;
#endif

class ClCaptureCallBack;

//#define ENABLE_STILL				//Still機能を有効にする

#define UVCDRIVER_GRAPH_VIDEONAME	(L"Video Source")
#define UVCDRIVER_GRAPH_GRABBERNAME	(L"Grabber")
#define UVCDRIVER_GRAPH_RENDERER	(L"Renderer")
#define UVCDRIVER_GRAPH_SMARTTEE	(L"SmartTee")
#if 0 //標準装備のMJPEG Decompressor
#define UVCDRIVER_GRAPH_DECODER		(L"MJPEG Deconmpressor")
#else //MainConcept
#define UVCDRIVER_GRAPH_DECODER		(L"MainConcept MJPEG Video Decoder")
#endif

#ifdef ENABLE_STILL
//Still用フィルタ名
#define UVCDRIVER_STILL_GRABBERNAME	(L"Still Grabber")
#define UVCDRIVER_STILL_RENDERER	(L"Still Renderer")
#define UVCDRIVER_STILL_SMARTTEE	(L"Still SmartTee")
#define UVCDRIVER_STILL_DECODER		(L"Still MainConcept MJPEG Video Decoder")
#endif

//--------------------------------------------------------------------------------//
/*!
	@brief	UVCドライバ
*/
//--------------------------------------------------------------------------------//
typedef enum{
	EL_MEDIATYPE_NULL,
	EL_MEDIATYPE_YUY2,
	EL_MEDIATYPE_YUYV,
	EL_MEDIATYPE_MJPG
}eMEDIATYPE;

typedef struct{
	eMEDIATYPE iMediaType;
	qint32 iHeight;
	qint32 iWidth;
	qint64 llMinFramerate;
	qint64 llMaxFramerate;
}stMEDIACONFIG;

typedef enum{
	EL_INTERFACE_VIDEOPROCAMP,
	EL_INTERFACE_CAMERACONTROL
}eINTERFACE;

typedef enum{
	//VideProcAmp
	EL_COMMANDTYPE_BRIGHTNESS,
	EL_COMMANDTYPE_CONTRAST,
	EL_COMMANDTYPE_HUE,
	EL_COMMANDTYPE_SATURATION,
	EL_COMMANDTYPE_SHARPNESS,
	EL_COMMANDTYPE_GAMMA,
	EL_COMMANDTYPE_WB,
	EL_COMMANDTYPE_BC,
	EL_COMMANDTYPE_GAIN,

	//CameraControl
	EL_COMMANDTYPE_ROTATION,
	EL_COMMANDTYPE_ZOOM,
	EL_COMMANDTYPE_EXPOSURE,
	EL_COMMANDTYPE_FOCUS
}eCOMMANDTYPE;

typedef struct{
	long lMin;
	long lMax;
	long lStep;
	long lDef;
	long lFlg;
}stCOMMANDCONFIG;

class ClUvcDriver : public QObject
{
	Q_OBJECT

public:

    ClUvcDriver(quint16 usVendor, quint16 usProduct, QObject *pParent = nullptr);	//コンストラクタ
	virtual ~ClUvcDriver();			//デストラクタ

	virtual bool searchDevice();	//デバイス検索

	virtual bool startCapture();	//キャプチャ開始
	virtual void stopCapture();		//キャプチャ停止
	virtual void pauseCapture();	//キャプチャ一時停止
	virtual void restartCapture();	//キャプチャ再開
	virtual bool startView(QLabel *label, QRect rect);	//ビュー開始
	virtual void stopView();		//ビュー停止
	virtual void stillCapture(bool pause = false);	//スティルキャプチャ
	virtual void setVideoWindowRect(QRect rect);
	virtual void showProperty(QWidget *widget);	//プロパティ
	virtual void setDevicePath(QString devicePath);		//接続対象となるDevicePath
	virtual QList<stMEDIACONFIG> getStreamConfigList() const;	//MediaTypeに応じたコンフィグを返す

	/*!
	 * \brief カメラのMediaType、解像度、フレームレートを設定する
	 * \param type				：	メディアタイプ
	 * \param resolution		：	変更する解像度
	 *								カメラがサポートする解像度でないと変更できない
	 *								QSize(0,0)の場合は現在値を使う
	 * \param llFramerate		：	フレームレート
	 *								サポート範囲内でないと変更できない
	 *								0の場合は現在値を使う
	 * \return	true：成功
	 */
	virtual bool setStreamConfig(eMEDIATYPE type, QSize resolution = QSize(0,0), qint64 llFramerate = 0);

	virtual void getCameraControllConfig();
    virtual bool commandPresenter(qint32 CmdID, long *lValue = nullptr, bool *bAuto = nullptr, stCOMMANDCONFIG *stConfig = nullptr);

	virtual quint16 getVendor(){ return m_usVendor; }
	virtual quint16 getProduct(){ return m_usProduct; }

	virtual eMEDIATYPE getStreamType(){ return m_ConfMediaType; }
	virtual QSize getStreamResolution(){ return m_ConfResolution; }
	virtual qint64 getStreamFramerate(){ return m_ConfFramerate; }

#if defined(MEASURE_AVGFRANMERATE)
	virtual int getAvgFramerate();
#endif

    virtual void repairPowerlineFrequency();

protected:
	virtual bool checkDevice(IMoniker *pMoniker);	//デバイス確認
	virtual bool createBaseFilter(IMoniker *pMoniker);	//ベースフィルタ生成
	virtual bool initialize(){return true;}	//!< 初期化
	virtual bool initializeCapture();	//キャプチャ初期化
	virtual bool initializeSampleGrabber(ISampleGrabber *pGrabber);	//サンプルグラバ初期化
#ifndef ENABLE_STILL
	virtual bool initializeStreamConfig();	//コンフィグ設定
#else
	virtual bool initializeStreamConfig(IPin *pSrcPinOut, eMEDIATYPE confMediaType, QSize confResolution, qint64 confFramerate);	//コンフィグ設定
#endif

	virtual bool getStreamConfig();		//カメラの持っている解像度の取得
	virtual bool getNodeNumber(IKsTopologyInfo *pKsTopology, QUuid idSearch, quint32 *pulNumber);	//KsTopologyからノード検索
	virtual bool getPin(IBaseFilter *pFilter, IPin **ppPin, int iDirection);	//ピン取得
	virtual bool getPinByName(IBaseFilter *pFilter, IPin **ppPin, QString clName); //名前によるピン取得
	virtual bool getFilterByName(IBaseFilter **ppFilter, QString name);

	/*!
		@brief		拡張ノードGUID取得
		@param[in]	ulNodeNumber : ノード番号
		@retval		対応するGUID
	*/
	virtual GUID getExtensionNodeGuid(quint32 ulNodeNumber){Q_UNUSED(ulNodeNumber);return QUuid();}
	virtual bool getExtensionProperty(quint32 ulId, quint32 ulNodeNumber, void *pBuffer, quint32 *pulDataSize);	//拡張プロパティ取得
	virtual bool setExtensionProperty(quint32 ulId, quint32 ulNodeNumber, void *pData, quint32 ulDataSize);	//拡張プロパティ設定

	virtual bool getParameterRange(eINTERFACE eInterface, long lProperty, long *plMin, long *plMax, long *plSteppingDelta, long *plDefault, long *plCapsFlags);
	virtual bool getParam(eINTERFACE eInterface, long lProperty, long *lValue, bool *bAuto);
	virtual bool setParam(eINTERFACE eInterface, long lProperty, long lValue, bool bAuto);
	virtual bool getParamPowerlineFrequency(long *lValue, bool *bAuto);
	virtual bool getParamLowLightCompensation(long *lValue, bool *bAuto);
	virtual bool setParamLowLightCompensation(long lValue, bool bAuto);
	virtual bool setParamPowerlineFrequency(long lValue, bool bAuto);
	virtual long getVideoProcAmpPropertyAutoFlag(bool bAuto);
	virtual long getCameraPropertyAutoFlag(bool bAuto);

	virtual bool getConfig(eINTERFACE eInterface, long lProperty, int *iInterval, stCOMMANDCONFIG *stConfig);

protected slots:
	virtual bool slotProcUp(eINTERFACE eInterface, long lProperty);
	virtual bool slotProcDown(eINTERFACE eInterface, long lProperty);
	virtual bool slotProcAuto(eINTERFACE eInterface, long lProperty);
	virtual bool slotProcDefault(eINTERFACE eInterface, long lProperty);
	virtual bool slotProcStop();
	virtual void slotTimeout();

	//VideoProcAmp
	virtual bool slotProcBrightnessUp();
	virtual bool slotProcBrightnessDown();

	//CameraControl
	virtual bool slotProcRotation();
	virtual bool slotProcZoomUp();
	virtual bool slotProcZoomDown();
	virtual bool slotProcExposureUp();
	virtual bool slotProcExposureDown();
	virtual bool slotProcFocusUp();
	virtual bool slotProcFocusDown();

signals:
	void sigImage(QImage img, qreal pts);		//画像更新
	void sigRotate(long lCur, bool bAuto);		//画像回転

	void sigError(HRESULT error);

protected:
	quint16 m_usVendor;		//!< 対象ベンダID
	quint16 m_usProduct;	//!< 対象プロダクトID

	eMEDIATYPE	m_ConfMediaType;		//!< 外部設定メディアタイプ
	QSize		m_ConfResolution;		//!< 外部設定解像度
	qint64		m_ConfFramerate;		//!< 外部設定フレームレート

	IBaseFilter *m_pBaseFilter;			//!< ベースフィルタ
	IGraphBuilder *m_pGraphBuilder;		//!< キャプチャフィルタグラフ

	IVMRWindowlessControl9 *m_pWindowslessControl9;

#if defined(MEASURE_AVGFRANMERATE)
	IQualProp *m_pQualProp;
#endif

	ClCaptureCallBack *m_pCaptuerCallBack;	//!< キャプチャコールバッククラス

	QList<stMEDIACONFIG> m_clConfigList;	//!< コンフィグリスト
	QImage m_imgCapture;	//!< キャプチャ用画像
	QString m_DevicePath;	//!< 接続対象のDevicePath保存用
	bool m_bIsCapture;		//!< キャプチャ中フラグ

	QTimer *m_pclTimerCommand;
	qint32 m_lCmdID;
	QMap<eCOMMANDTYPE, stCOMMANDCONFIG> m_clListCamCtrl;

#ifdef QT_DEBUG
	QFile	m_clLogFile;
#endif

private:
    bool IsOx1() const;

    long m_lPowerlineFrequency{};       // フリッカ補正値　1:50Hz, 2:60Hz
};
//--------------------------------------------------------------------------------//
Q_DECLARE_METATYPE(eMEDIATYPE)
Q_DECLARE_METATYPE(stMEDIACONFIG)

#endif //CLUVCDRIVER_H
