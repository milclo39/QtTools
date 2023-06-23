#ifndef CLPRESENTERMO_H
#define CLPRESENTERMO_H

#include <QLibrary>
#include <QTime>

#include "ClUsbDriver/ClUsbDriver_global.h"
#include "win32/ClDeviceInfo_win.h"
#include "win32/ELVLib/ELVDLib.h"
#include "win32/jpeg/jpeglib.h"
#include "win32/jpeg/jerror.h"

#include "ClPresenterUVC.h"

#if 1
//MO-1/MO-1wのExtension Unitを使用する場合は有効にする
#define EU_MO1
#endif

// デバイス
#define VENDOR_ID_MO1	(0x09A1)	//!< ベンダID
#define PRODUCT_ID_MO1	(0x001F)	//!< プロダクトID

/*! --------------------------------------------- //
//
//		@brief	リコーUSBXu識別番号
//
//  --------------------------------------------- */
enum	eKSPROPERTY_877XU
{
	eKSPROPERTY_877XU_EXTENSION_INFO,
	eKSPROPERTY_877XU_DATA_VERSION,
	eKSPROPERTY_877XU_DATA_ACCESS,
	eKSPROPERTY_877XU_LED_CONTROL,
	eKSPROPERTY_877XU_WHITEBALANCE_FULL_COMPONENT,
	eKSPROPERTY_877XU_EXPOSURE_VALUE,
	eKSPROPERTY_877XU_GAIN,
	eKSPROPERTY_877XU_POWER_LINE_FREQUENCY,
	eKSPROPERTY_877XU_SWITCH,
	eKSPROPERTY_877XU_EXTENSION1,
	eKSPROPERTY_877XU_EXTENSION2,
	eKSPROPERTY_877XU_EXTENSION3,
	eKSPROPERTY_877XU_EXTENSION4,
	eKSPROPERTY_877XU_EXTENSION5,
	eKSPROPERTY_877XU_EXTENSION6,
	eKSPROPERTY_877XU_EXTENSION7,
	eKSPROPERTY_877XU_EXTENSION8,
	eKSPROPERTY_877XU_COUNT
};

/*! --------------------------------------------- //
//
//		@brief	HICS UVCミドル Extension Unit識別番号
//
//  --------------------------------------------- */
enum	eEXTUNIT_HICS_UVCMID
{
	eEXTUNIT_HICS_UVCMID_1 = 1,				//11 bytes
	eEXTUNIT_HICS_UVCMID_2,					//11 bytes
	eEXTUNIT_HICS_UVCMID_3,					//11 bytes
	eEXTUNIT_HICS_UVCMID_4,					// 6 bytes
	eEXTUNIT_HICS_UVCMID_5,					// 4 bytes
	eEXTUNIT_HICS_UVCMID_6,					// 2 bytes
	eEXTUNIT_HICS_UVCMID_7,					// 1 bytes
	eEXTUNIT_HICS_UVCMID_8,					//11 bytes
	eEXTUNIT_HICS_UVCMID_9,					// 2 bytes
	eEXTUNIT_HICS_UVCMID_10,				// 1 bytes
	eEXTUNIT_HICS_UVCMID_11,				// 1 bytes
	eEXTUNIT_HICS_UVCMID_12,				// 1 bytes
	eEXTUNIT_HICS_UVCMID_13,				// 1 bytes
	eEXTUNIT_HICS_UVCMID_14,				// 1 bytes
	eEXTUNIT_HICS_UVCMID_15,				// 1 bytes
	eEXTUNIT_HICS_UVCMID_16					// 1 bytes
};

/*! --------------------------------------------- //
//
//		@brief	LEDパラメータ
//
//  --------------------------------------------- */
enum eMO1_LED_PARAM
{
	eMO1_LED_OFF	=	0,		//!< LEDオフ
	eMO1_LED_ON		=	1,		//!< LEDオン
	eMO1_LED_PARAM_INVALID		//!< 無効な値
};

/*! --------------------------------------------- //
//
//		@brief	映像回転パラメータ
//
//  --------------------------------------------- */
enum eMO1_ROTATE_PARAM
{
	eMO1_ROTATE_OFF	=	0,		//!< 回転オフ
	eMO1_ROTATE_ON	=	1,		//!< 回転オン
	eMO1_ROTATE_PARAM_INVALID	//!< 無効な値
};

/*! --------------------------------------------- //
//
//		@brief	フリッカ補正値
//
//  --------------------------------------------- */
enum eMO1_FLICKER_PARAM
{
	eMO1_FLICKER_OFF	=	0,		//!< 回転オフ
	eMO1_FLICKER_50		=	1,		//!< 回転オン
	eMO1_FLICKER_60		=	2,		//!< 回転オン
	eMO1_FLICKER_PARAM_INVALID		//!< 無効な値
};


// UIとLEDの関連
#define MO_BUTTON_STATE_TO_LED_STATE_ON			(true)	//!< UIのボタン true  で　LED ON
#define MO_BUTTON_STATE_TO_LED_STATE_OFF		(false)	//!< UIのボタン false で LED OFF


/*! --------------------------------------------- //
//
//	@brief	リコーUSBXu操作用構造体
//
//	LED
//
//  --------------------------------------------- */
struct	KSPROPERTY_877XU_LED_CONTROL_S
{
	BYTE	Mode;
	WORD	BlinkOnTime;
	WORD	BlinkOffTime;
	BYTE	reserved[6];
};

/*! --------------------------------------------- //
//
//	@brief	リコーUSBXu操作用構造体
//
//	White Balance Full Component
//		: KSPROPERTY_877XU_WHITEBALANCE_FULL_COMPONENT
//	[0]=Red.L   [1]=Red.H
//	[2]=Green.L [3]=Green.H
//	[4]=Blue.L  [5]=Blue.H
//
//  --------------------------------------------- */
struct	KSPROPERTY_877XU_WHITEBALANCE_FULL_COMPONENT_S
{
	WORD	Red;
	WORD	Green;
	WORD	Blue;
	BYTE	reserved[5];
};

/*! --------------------------------------------- //
//
//	@brief	リコーUSBXu操作用構造体
//
//	Exposure Vaue
//		: KSPROPERTY_877XU_EXPOSURE_VALUE
//	[0]=Exposure.LL
//	[1]=Exposure.LH
//	[2]=Exposure.HL
//	[3]=Exposure.HH
//
//  --------------------------------------------- */
struct	KSPROPERTY_877XU_EXPOSURE_VALUE_S
{
	DWORD	Value;
	BYTE	reserved[7];
};

/*! --------------------------------------------- //
//
//	@brief	リコーUSBXu操作用構造体
//
//	Gain - ProcessingUnit(VideoProcAmp)
//		: KSPROPERTY_877XU_GAIN
//	[0]=Gain.L [1]=Gain.H
//
//  --------------------------------------------- */
struct	KSPROPERTY_877XU_GAIN_S
{
	WORD	Value;
	BYTE	reserved[9];
};

/*! --------------------------------------------- //
//
//	@brief	リコーUSBXu操作用構造体
//
//	PowerLineFrequency - ProcessingUnit(VideoProcAmp)
//		: KSPROPERTY_877XU_POWER_LINE_FREQUENCY
//	[0]=PowerLineFrequency
//
//  --------------------------------------------- */
struct	KSPROPERTY_877XU_POWER_LINE_FREQUENCY_S
{
	BYTE	Value;
	BYTE	reserved[10];
};

/*! --------------------------------------------- //
//
//	@brief	リコーUSBXu操作用構造体
//
//	Switch : KSPROPERTY_877XU_SWITCH
//	[0]=Auto
//		Auto.b0=White Balance Full Component
//		Auto.b1=Exposure Value
//		Auto.b2=Gain
//		Auto.b3=Black Level
//		Auto.b4=Flicker Detection
//		Auto.b5=(reserved)
//		Auto.b6=(reserved)
//		Auto.b7=(reserved)
//	[1]=b1
//		b1.b0=ColorEnable
//		b1.b1=Flip
//		b1.b2=Mirror
//		b1.b3=(reserved)
//		b1.b4=(reserved)
//		b1.b5=(reserved)
//		b1.b6=(reserved)
//		b1.b7=(reserved)
//	[2..10]=reserved
//
//  --------------------------------------------- */
struct	KSPROPERTY_877XU_SWITCH_S
{
	struct
	{
		BYTE	WhiteBalanceFullComponent				:1;
		BYTE	ExposureValue							:1;
		BYTE	Gain									:1;
		BYTE	BlackLevel								:1;
		BYTE	FlickerDetection						:1;
		BYTE	reserved								:3;
	}	Auto;
	struct
	{
		BYTE	ColorEnable								:1;
		BYTE	Flip									:1;
		BYTE	Mirror									:1;
		BYTE	reserved								:5;
	}	b1;
	BYTE	reserved[8];
	struct
	{
		BYTE	CameraControl_Exposure					:1;
		BYTE	CameraControl_Iris						:1;
		BYTE	CameraControl_Focus						:1;
		BYTE	VideoProcAmp_Hue						:1;
		BYTE	VideoProcAmp_WhiteBalanceTemperature	:1;
		BYTE	VideoProcAmp_WhiteBalanceComponent		:1;
		BYTE	reserved								:2;
	}	DefaultAutoProp;
};

/*! --------------------------------------------- //
//
//		@brief	HICS UVCミドル Extension Unit構造体
//
//  --------------------------------------------- */
typedef struct {
	eEXTUNIT_HICS_UVCMID	id;
	ULONG					length;
} stEXTUNIT_HICS;

/*! --------------------------------------------- //
//
//		@brief	HICS UVCミドル Extension Unit識別番号
//
//  --------------------------------------------- */
const stEXTUNIT_HICS stEXTUNIT_TBL[16] =
{
	{eEXTUNIT_HICS_UVCMID_1,	11},
	{eEXTUNIT_HICS_UVCMID_2,	11},
	{eEXTUNIT_HICS_UVCMID_3,	11},
	{eEXTUNIT_HICS_UVCMID_4,	6},
	{eEXTUNIT_HICS_UVCMID_5,	4},
	{eEXTUNIT_HICS_UVCMID_6,	2},
	{eEXTUNIT_HICS_UVCMID_7,	1},
	{eEXTUNIT_HICS_UVCMID_8,	11},
	{eEXTUNIT_HICS_UVCMID_9,	2},
	{eEXTUNIT_HICS_UVCMID_10,	1},
	{eEXTUNIT_HICS_UVCMID_11,	1},
	{eEXTUNIT_HICS_UVCMID_12,	1},
	{eEXTUNIT_HICS_UVCMID_13,	1},
	{eEXTUNIT_HICS_UVCMID_14,	1},
	{eEXTUNIT_HICS_UVCMID_15,	1},
	{eEXTUNIT_HICS_UVCMID_16,	1}
};


//--------------------------------------------------------------------------------------

// ～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～ //
/*! ------------------------------------------------------------------------------------- //

			@brief クラス　ELVDLibを使ったリコーUSBカメラとの通信

//  ------------------------------------------------------------------------------------- */
// ～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～*～ //
class ClPresenterMO : public ClPresenterUVC
{
	Q_OBJECT

public:
	ClPresenterMO(qint32 lProductId = PRODUCT_NONE, QString strDevPath = QString(), QObject *parent = 0);
	virtual ~ClPresenterMO();

signals:

	void sigSetTextRemoconFlicker(QString);

protected:
	virtual HRESULT	buildGraphMgr( void );
	virtual qint32 getModelName(QVariant &qParam);				//get model name from device and output to QVariant

	// make remocon
	virtual void createRemoconWidget();

	// ex
	virtual qint32 setFlicker(long lParam);
	virtual qint32 getFlicker(long &lParam);
	virtual qint32 setEx1(long lParam);
	virtual qint32 getEx1(long &lParam);
	virtual qint32 setEx2(long lParam);
	virtual qint32 getEx2(long &lParam);

	// 3～8は現状仕様上未使用
#ifdef _USE_ALL_Xu_
	virtual qint32 setEx3(qint32 lParam);
	virtual qint32 getEx3(qint32 &lParam);
	virtual qint32 setEx4(qint32 lParam);
	virtual qint32 getEx4(qint32 &lParam);
	virtual qint32 setEx5(qint32 lParam);
	virtual qint32 getEx5(qint32 &lParam);
	virtual qint32 setEx6(qint32 lParam);
	virtual qint32 getEx6(qint32 &lParam);
	virtual qint32 setEx7(qint32 lParam);
	virtual qint32 getEx7(qint32 &lParam);
	virtual qint32 setEx8(qint32 lParam);
	virtual qint32 getEx8(qint32 &lParam);
#endif

	virtual void setTranslator();

protected slots:

	void slotProcChangeFlicker(QString strItem);	// フリッカ補正値の変更
	void slotProcToggleLED(bool bState);			// LEDのON・OFF
	void slotProcRotate();							// 映像90度回転

protected: // -- Variables

	QPushButton*	m_pclLED;
	QPushButton*	m_pclRotate;
	QString			m_strDevPath;					//!<	デバイスパス
#ifndef EU_MO1
	stEXTUNIT_HICS	m_stEU1;						//Extension Unit情報
	stEXTUNIT_HICS	m_stEU2;						//Extension Unit情報

protected:
	virtual void getEUInf();						//Extension Unit情報読み出し関数
#endif
};
//--------------------------------------------------------------------------------------

#endif // CLPRESENTERMO_H
