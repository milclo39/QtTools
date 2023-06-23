///////////////////////////////////////////////
//	5U87xCtl : Ricoh camera control library
//	2010/10/07	ver.2.9.0.0
///////////////////////////////////////////////
#if	!defined(__5U87xCtl_H__)
#define	__5U87xCtl_H__
#if	defined(_MBCS)
#error	MBCS is not supported.
#endif
#if	!defined(_UNICODE)
//#error	Only the project of unicode is supported.
#endif

#include	<tchar.h>
#include	<windows.h>
#include	<cfgmgr32.h>
#include	<setupapi.h>
#include	<dshow.h>
#include	<ks.h>
#include	<ksmedia.h>
#include	<ksproxy.h>

//	calling conversion
//		functions : R5U87XCTL_API
#define R5U87XCTL_API	__cdecl			//	compiler option = "/Gd"
//		class member functions : default setting (x86 = __thiscall)

//	ROM version of Ricoh camera module
#define	ROMVERS_5U87x_OLD	(0x0000)	//	No ROM version (870 / 871 / 872 / 875ES1 / 875ES2)
#define	ROMVERS_5U875_ESx	(0x5021)	//	875ES2.1 / 875ES3 / 875ES3.1 / 876ES1 / 876ES2 / 877ES1
#define	ROMVERS_5U876_ES3	(0x6030)	//	876ES3
#define	ROMVERS_5U877_ES2	(0x7020)	//	877ES2
#define	ROMVERS_5U877_ES3	(0x7030)	//	877ES3
#define	ROMVERS_5U877_ES3_1	(0x7031)	//	877ES3.1
#define	ROMVERS_5U878_ES1	(0x7810)	//	878ES1 / 878ES2
#define	ROMVERS_5U879_ES1	(0x7910)	//	879ES1


#pragma	warning(push)
#pragma	warning(disable:4200)	//	zero-sized array

enum	USBDEVICETYPE
{
	USBDVCTYPE_NONE,	//	No device
	USBDVCTYPE_HC,		//	Host controller
	USBDVCTYPE_HUB,		//	Root/External hub
	USBDVCTYPE_DEVICE,	//	Single interface device
	USBDVCTYPE_CCGP,	//	Composite device
	USBDVCTYPE_FUNCTION	//	Function in multi interface device
};

// ================================================================ //
/*! -----------------------------------------------------------------

		@brief	USBデバイスを表すクラス

--------------------------------------------------------------------*/
// ================================================================ //
typedef	class	_USBDEVICEDATA
{
public:
	_USBDEVICEDATA*				pParentDevice;
	ULONG						ConnectionIndex;
	USBDEVICETYPE				Type;
	BYTE*						pDeviceDescriptor;			// デバイスディスクリプタ
	BYTE*						pConfigurationDescriptor;
	HDEVINFO					hDevInfoSet;
	SP_DEVICE_INTERFACE_DATA	DeviceInterfaceData;
	SP_DEVINFO_DATA				DevInfoData;
	LPCTSTR						HardwareID;
	LPCTSTR						DriverKey;
	LPCTSTR						DeviceInstance;
	LPCTSTR						SymbolicLinkName;
	LPCTSTR						UsbPath;
	LPCTSTR						FriendlyName;
public:
	_USBDEVICEDATA();
	~_USBDEVICEDATA();
	void	Clear( void );
	bool	Duplicate(_USBDEVICEDATA& src,_USBDEVICEDATA* pParentDevice);
}	USBDEVICEDATA,*PUSBDEVICEDATA;

///////////////////////////////////////////////
//	COM controller
///////////////////////////////////////////////
class	cCOMctl
{
private:
	bool	bInitialized;
public:
	cCOMctl();
	~cCOMctl();	//	CoUninitialize() is called.
	HRESULT	CoInitializeEx(LPVOID pvReserved=NULL,DWORD dwCoInit=COINIT_MULTITHREADED);
};

///////////////////////////////////////////////
//	c5U87xCtl class
///////////////////////////////////////////////
//	Control mode
enum	CTLMODE
{
	CM_NOT_SPECIFIED = 0,		//	not specified
	CM_UNKNOWN = 0,				//	unknown(not initialized)
	CM_CAMERA_DRIVER_XU,		//	use Camera driver for class request by ExtensionUnit
	CM_CAMERA_DRIVER_VR,		//	use Camera driver for vendor request
	CM_UPDATE_DRIVER,			//	use update driver for vendor request
	CM_UPDATE_DRIVER_INSTALL,	//	The update driver is compulsorily installed.(initialized to CM_UPDATE_DRIVER)
	CM_COUNT
};

//	Device memory type
enum	MEMORYTYPE
{
	//	Device that supports only I2C-SROM
	//		5U870/5U871/5U872/5U875/5U876ES1/5U876ES2/5U877ES1/5U877ES2
	//	Device that supports I2C-SROM and SPI-FLASH (*)
	//		5U876ES3/5U877ES3/5U877ES3.1
	//	(*)	When CTLMODE is only CM_CAMERA_DRIVER_XU, it is possible to distinguish.
	//		If CTLMODE is not CM_CAMERA_DRIVER_XU, it is judged as MEMTYPE_UNKNOWN.
	MEMTYPE_UNKNOWN,
	MEMTYPE_I2C_SROM,
	MEMTYPE_SPI_FLASH
};

typedef	union	_RCSROMVERS
{
	DWORD		dw;
	struct	sSROMVERS	{
		DWORD	minute	: 6;
		DWORD	hour	: 5;
		DWORD	day		: 5;
		DWORD	month	: 4;
		DWORD	year	:12;
	}			bm;
}	RCSROMVERS,*PRCSROMVERS;

//	LED mode
enum	LED_MODE
{
	LED_MODE_STANDARD,		//	Movie capture is started = ON / stopped = OFF
	LED_MODE_BLINK,			//	Movie capture is started = BLINK / stopped = OFF
	LED_MODE_FOURCE_ON,		//	Always ON
	LED_MODE_FOURCE_OFF,	//	Always OFF
	LED_MODE_FOURCE_BLINK	//	Always BLINK
};

//	Display direction
enum	RCIMGDIR_DISPDIR
{
	RCIMGDIR_0,				//	  0[deg] = DMDO_DEFAULT
	RCIMGDIR_90,			//	 90[deg] = DMDO_90
	RCIMGDIR_180,			//	180[deg] = DMDO_180
	RCIMGDIR_270,			//	270[deg] = DMDO_270
	RCIMGDIR_DISPDIR_0,		//	desktop display +   0[deg]
	RCIMGDIR_DISPDIR_90,	//	desktop display +  90[deg]
	RCIMGDIR_DISPDIR_180,	//	desktop display + 180[deg]
	RCIMGDIR_DISPDIR_270	//	desktop display + 270[deg]
};

//	Update property flags
typedef	struct	_PROPUPDATE
{
	union
	{
		DWORD	dw;
		struct
		{
			DWORD	Pan							:1;
			DWORD	Tilt						:1;
			DWORD	Roll						:1;
			DWORD	Zoom						:1;
			DWORD	Exposure					:1;
			DWORD	Iris						:1;
			DWORD	Focus						:1;
			DWORD	Privacy						:1;
			DWORD	AutoExposurePriority		:1;
			DWORD	reserved					:23;
		}	flag;
	}	CameraControl;
	union
	{
		DWORD	dw;
		struct
		{
			DWORD	Brightness					:1;
			DWORD	Contrast					:1;
			DWORD	Hue							:1;
			DWORD	Saturation					:1;
			DWORD	Sharpness					:1;
			DWORD	Gamma						:1;
			DWORD	WhiteBalanceTemperature		:1;
			DWORD	BacklightCompensation		:1;
			DWORD	Gain						:1;
			DWORD	PowerLineFrequency			:1;
			DWORD	reserved					:22;
		}	flag;
	}	VideoProcAmp;
	union
	{
		DWORD	dw;
		struct
		{
			DWORD	WhiteBalanceFullComponent:1;
			DWORD	ExposureEx:1;
			DWORD	GainEx:1;
			DWORD	PowerLineFrequencyEx:1;
			DWORD	ColorEnable:1;
			DWORD	Flip:1;
			DWORD	Mirror:1;
			DWORD	LED:1;
			DWORD	Extension1:1;
			DWORD	Extension2:1;
			DWORD	Extension3:1;
			DWORD	Extension4:1;
			DWORD	Extension5:1;
			DWORD	Extension6:1;
			DWORD	Extension7:1;
			DWORD	Extension8:1;
			DWORD	reserved:16;
		}	flag;
	}	Xu5U877;
}	PROPUPDATE,*PPROPUPDATE;

//	Callback function types
typedef	void	(CALLBACK *fINSTALLCALLBACK)(bool bInstall,bool bEnd,bool Result,void* pContext);
typedef	bool	(CALLBACK *fPROGRESSCALLBACK)(bool Success,WORD address,void* pContext);

// ================================================================ //
/*! -----------------------------------------------------------------

		@brief	USBデバイス対する処理いろいろやってくれそうなクラス

--------------------------------------------------------------------*/
// ================================================================ //
class	c5U87xCtl
{
private:
	CTLMODE				CtlMode;
	fINSTALLCALLBACK	CbInstall;
	void*				CtxInstall;
	USBDEVICEDATA		Target;
	LPVOID				Context;
	WORD				RomVers;
	MEMORYTYPE			MemType;
	LPVOID				WriteProtectInfo;
	IMoniker*			pMoniker;
	cCOMctl				ComCtl;
	bool				bInitPrevMode;
public:
	c5U87xCtl();
	c5U87xCtl(DWORD dwCoInit);
	~c5U87xCtl();
private:
	void	ConstSub(DWORD dwCoInit);
	bool	SubWriteSromVR(
		bool bWp,WORD Adrs,LPVOID Buf,size_t Size,fPROGRESSCALLBACK pcb,void* ctx);
	bool	SubWriteSromXU(
		LPVOID pWp,WORD Adrs,LPVOID Buf,size_t Size,fPROGRESSCALLBACK pcb,void* ctx);
	static	bool	CALLBACK VerifyCB(bool Success,WORD address,void* pContext);
public:
	//	Initialize
	bool	Initialize( PUSBDEVICEDATA pUsbDeviceData,
						CTLMODE CtlModeReq=CM_NOT_SPECIFIED,
						fINSTALLCALLBACK InstallCallback=NULL,void* pContext=NULL);
	void	SetInitPrevMode(bool bEnable);
	//	Uninitialize
	void	Uninitialize( void );
	//	Get control mode (CM_UNKNOWN = not initialized)
	CTLMODE	GetControlMode(MEMORYTYPE* pMemType=NULL);
	//	Get ROM version
	bool	GetRomVersion(WORD* pRomVersion);
	//	Get SROM data version (cannot get SROM data version by 5U877ES2)
	bool	GetSromVersion(PRCSROMVERS pSromVersion);
	//	Get address of moniker interface object
	bool	GetMoniker(IMoniker** ppMoniker);
	//	SROM access
	//								5U875/5U876ES2	5U876ES3	5U877ES2	5U877ES3/...
	//		Ricoh camera driver		OK				OK			OK(*)		OK
	//		ExtensionUnit			NG				OK(**)		OK			OK
	//		UpdateDriver			OK				OK			NG			OK
	//		(*)		5U877ES2 doesn't have the vendor request response processing.
	//				ExtensionUnit is udes for the access to SROM in 5U877ES2.
	//		(**)	When descriptor corresponds to 5U877Xu, ExtensionUnit can be used.
	//				5U876ES3 cannot inform host PC of the kink od the interface (I2C or SPI) that device uses.
	bool	ChkSromData(LPVOID Buf,size_t Size);
	bool	CompareSromData(LPVOID Buf1,size_t Size1,LPVOID Buf2,size_t Size2,
							bool bChkSerial=true,bool bChkManufacture=true,bool bChkProp=false);
	DWORD	GetSromDataSize( void );
	bool	GetSromDeviceInfo(LPVOID Buf,size_t Size,WORD* pVID,WORD* pPID,WORD* pRev);
	int		GetManufactureStringA(LPVOID Buf,size_t Size,char* buf,int cc);
	int		GetManufactureStringW(LPVOID Buf,size_t Size,WCHAR* buf,int cc);
	bool	SetManufactureStringA(LPVOID Buf,size_t Size,char* buf);
	bool	SetManufactureStringW(LPVOID Buf,size_t Size,WCHAR* buf);
	int		GetSerialStringA(LPVOID Buf,size_t Size,char* buf,int cc);
	int		GetSerialStringW(LPVOID Buf,size_t Size,WCHAR* buf,int cc);
	bool	SetSerialStringA(LPVOID Buf,size_t Size,char* buf);
	bool	SetSerialStringW(LPVOID Buf,size_t Size,WCHAR* buf);
	bool	SetWriteProtectInfo(LPVOID Buf,size_t Size);
	bool	ReadSrom(LPVOID Buf,size_t Size,
				fPROGRESSCALLBACK ProgressCallback=NULL,void* pContext=NULL);
	bool	WriteSrom(LPVOID Buf,size_t Size,
				fPROGRESSCALLBACK ProgressCallback=NULL,void* pContext=NULL,
				bool EnableAccessEnd=true);
	bool	WriteSromEx(LPVOID Buf,size_t Size,
				fPROGRESSCALLBACK WriteProgressCallback=NULL,
				fPROGRESSCALLBACK VerifyProgressCallback=NULL,
				void* pContext=NULL,bool* pVerifyError=NULL,size_t VerifySize=0);
	bool	WriteSromEx2(LPVOID Buf,size_t Size,
				fPROGRESSCALLBACK WriteProgressCallback=NULL,
				fPROGRESSCALLBACK VerifyProgressCallback=NULL,
				void* pContext=NULL,bool* pVerifyError=NULL,size_t VerifySize=0,
				bool EnableAccessEnd=true);
	bool	EraseSromA(size_t Size,char* WpSetting);	//	support with ExtensionUnit for 5U87x
	bool	EraseSromW(size_t Size,WCHAR* WpSetting);	//	support with ExtensionUnit for 5U87x
	bool	SromAccessEnd( void );
	//	5U87x external data access
	//								5U875/5U876ES2	5U876ES3	5U877ES2	5U877ES3/...
	//		Ricoh camera driver		NG				OK(*)		OK			OK
	//		ExtensionUnit			NG				OK(*)		OK			OK
	//		UpdateDriver			-(**)			-			-			-
	//		(*)		When descriptor corresponds to 5U877Xu, ExtensionUnit can be used.
	//		(**)	UpdateDriver doesn't have the capture function.
	bool	Read5U87xExData(WORD Adrs,BYTE* Buf,DWORD Size,
				fPROGRESSCALLBACK ProgressCallback=NULL,void* pContext=NULL);
	bool	Write5U87xExData(WORD Adrs,BYTE* Buf,DWORD Size,
				fPROGRESSCALLBACK ProgressCallback=NULL,void* pContext=NULL);
	//	5U87x code memory access
	//								5U875/5U876ES2	5U876ES3	5U877ES2	5U877ES3/...
	//		Ricoh camera driver		NG				OK(*)		OK			OK
	//		ExtensionUnit			NG				OK(*)		OK			OK
	//		UpdateDriver			-(**)			-			-			-
	//		(*)		When descriptor corresponds to 5U877Xu, ExtensionUnit can be used.
	//		(**)	UpdateDriver doesn't have the capture function.
	bool	Read5U87xCodeData(WORD Adrs,BYTE* Buf,DWORD Size,
				fPROGRESSCALLBACK ProgressCallback=NULL,void* pContext=NULL);
	bool	Write5U87xCodeData(WORD Adrs,BYTE* Buf,DWORD Size,
				fPROGRESSCALLBACK ProgressCallback=NULL,void* pContext=NULL);
	//	Sensor register access
	//								5U875/5U876ES2	5U876ES3	5U877ES2	5U877ES3/...
	//		Ricoh camera driver		OK(*)			OK(**)		OK			OK
	//		ExtensionUnit			NG				OK(**)		OK			OK
	//		UpdateDriver			-(***)			-			-			-
	//		(*)		When vendor request is supported.
	//		(**)	When descriptor corresponds to 5U877Xu, ExtensionUnit can be used.
	//		(***)	UpdateDriver doesn't have the capture function.
	bool	ReadCameraRegister(WORD Adrs,BYTE AdrsSize,WORD* pData,BYTE DataSize);
	bool	WriteCameraRegister(WORD Adrs,BYTE AdrsSize,WORD Data,BYTE DataSize);
	//	I2C0 device access
	//		(*)		UpdateDriver doesn't have the capture function.
	//		(**)	When descriptor corresponds to 5U877Xu, ExtensionUnit can be used.
	//		(***)	need Extended data access in Xu5U877
	bool	ReadI2C0Device(BYTE SlvAdrsW,BYTE SlvAdrsR,WORD Adrs,BYTE AdrsSize,WORD* pData,BYTE DataSize);
	bool	WriteI2C0Device(BYTE SlvAdrsW,BYTE SlvAdrsR,WORD Adrs,BYTE AdrsSize,WORD Data,BYTE DataSize);
	//	Camera Terminal controls
	bool	GetPan(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetPan(LONG Cur);

	bool	GetTilt(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetTilt(LONG Cur);

	bool	GetRoll(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetRoll(LONG Cur);

	bool	GetZoom(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetZoom(LONG Cur);

	bool	GetExposure(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL,
				bool* pCapAuto=NULL,bool* pAutoCur=NULL,bool* pAutoDef=NULL);
	bool	SetExposure(LONG Cur,bool AutoCur);

	bool	GetIris(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL,
				bool* pCapAuto=NULL,bool* pAutoCur=NULL,bool* pAutoDef=NULL);
	bool	SetIris(LONG Cur,bool AutoCur);

	bool	GetAutoExposurePriority(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetAutoExposurePriority(LONG Cur);

	bool	GetFocus(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL,
				bool* pCapAuto=NULL,bool* pAutoCur=NULL,bool* pAutoDef=NULL);
	bool	SetFocus(LONG Cur,bool AutoCur);

	bool	GetPrivacy(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetPrivacy(LONG Cur);
	//	Video Processing Unit controls
	bool	GetBrightness(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetBrightness(LONG Cur);

	bool	GetContrast(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetContrast(LONG Cur);

	bool	GetHue(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL,
				bool* pCapAuto=NULL,bool* pAutoCur=NULL,bool* pAutoDef=NULL);
	bool	SetHue(LONG Cur,bool AutoCur);

	bool	GetSaturation(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetSaturation(LONG Cur);

	bool	GetSharpness(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetSharpness(LONG Cur);

	bool	GetGamma(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetGamma(LONG Cur);

	bool	GetWhiteBalanceTemperature(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL,
				bool* pCapAuto=NULL,bool* pAutoCur=NULL,bool* pAutoDef=NULL);
	bool	SetWhiteBalanceTemperature(LONG Cur,bool AutoCur);

	bool	GetBacklightCompensation(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetBacklightCompensation(LONG Cur);

	bool	GetGain(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetGain(LONG Cur);

	bool	GetPowerLineFrequency(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetPowerLineFrequency(LONG Cur);
	//	5U877 Extension Unit controls
/*	old to 2009/05/15 ver.1.4.0.0
	bool	GetWhiteBalanceFullComponent(
				LONG* pRedCur,  LONG* pRedMin,  LONG* pRedMax,  LONG* pRedRes,  LONG* pRedDef,
				LONG* pGreenCur,LONG* pGreenMin,LONG* pGreenMax,LONG* pGreenRes,LONG* pGreenDef,
				LONG* pBlueCur, LONG* pBlueMin, LONG* pBlueMax, LONG* pBlueRes, LONG* pBlueDef,
				bool* pCapAuto,bool* pAutoCur,bool* pAutoDef);
*/
/*	new from 2009/05/23 ver.1.5.0.0 */
	bool	GetWhiteBalanceFullComponent(
				LONG* pRedCur,			LONG* pGreenCur,		LONG* pBlueCur,
				LONG* pRedMin=NULL,		LONG* pGreenMin=NULL,	LONG* pBlueMin=NULL,
				LONG* pRedMax=NULL,		LONG* pGreenMax=NULL,	LONG* pBlueMax=NULL,
				LONG* pRedRes=NULL,		LONG* pGreenRes=NULL,	LONG* pBlueRes=NULL,
				LONG* pRedDef=NULL,		LONG* pGreenDef=NULL,	LONG* pBlueDef=NULL,
				bool* pCapAuto=NULL,bool* pAutoCur=NULL,bool* pAutoDef=NULL);
/**/
	bool	SetWhiteBalanceFullComponent(LONG RedCur,LONG GreenCur,LONG BlueCur,bool AutoCur);

	bool	GetExposureEx(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL,
				bool* pCapAuto=NULL,bool* pAutoCur=NULL,bool* pAutoDef=NULL);
	bool	SetExposureEx(LONG Cur,bool AutoCur);

	bool	GetGainEx(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL,
				bool* pCapAuto=NULL,bool* pAutoCur=NULL,bool* pAutoDef=NULL);
	bool	SetGainEx(LONG Cur,bool AutoCur);

	bool	GetPowerLineFrequencyEx(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL,
				bool* pCapAuto=NULL,bool* pAutoCur=NULL,bool* pAutoDef=NULL);
	bool	SetPowerLineFrequencyEx(LONG Cur,bool AutoCur);

	bool	GetColorEnable(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetColorEnable(LONG Cur);

	bool	GetFlip(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetFlip(LONG Cur);

	bool	GetMirror(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetMirror(LONG Cur);

	bool	GetLED(LED_MODE* pMode,LONG* pBlinkOnTime,LONG* pBlinkOffTime);
	bool	SetLED(LED_MODE Mode,LONG BlinkOnTime,LONG BlinkOffTime);

	bool	GetExtension1(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetExtension1(LONG Cur);

	bool	GetExtension2(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetExtension2(LONG Cur);

	bool	GetExtension3(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetExtension3(LONG Cur);

	bool	GetExtension4(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetExtension4(LONG Cur);

	bool	GetExtension5(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetExtension5(LONG Cur);

	bool	GetExtension6(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetExtension6(LONG Cur);

	bool	GetExtension7(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetExtension7(LONG Cur);

	bool	GetExtension8(LONG* pCur,LONG* pMin=NULL,LONG* pMax=NULL,LONG* pRes=NULL,LONG* pDef=NULL);
	bool	SetExtension8(LONG Cur);

	//	Check update property
	bool	ChkUpdateProperty(PPROPUPDATE pPropUpdate);

	//	Ricoh camera driver function
	bool	SetDisplayDir(RCIMGDIR_DISPDIR Direction);	//	Set frame image direction
	bool	SaveFrame(LPCSTR Path);						//	Save video streaming frame image
	bool	SaveImage(LPCSTR Path);						//	Save device frame image
	bool	EnableLowSizeDrop(bool bEnable);			//	Set low size frame is dropped
	bool	GetFrame(LPVOID buf,ULONG* pSize);			//	Get frame data (only stream state is running)
	bool	RegQueryValue(LPCTSTR SubKeyName,LPCTSTR ValueName,ULONG* pSize,LPVOID Buf,ULONG* pType);	//	query software key value
	bool	RegSetValue(LPCTSTR SubKeyName,LPCTSTR ValueName,ULONG Size,LPVOID Buf,ULONG Type);			//	set software key value
};

///////////////////////////////////////////////
//	USB device tree
///////////////////////////////////////////////
class	cUsbDeviceTreeItem
{
public:
	USBDEVICEDATA		DeviceData;
	ULONG				Count;
	cUsbDeviceTreeItem*	Items;
public:
	cUsbDeviceTreeItem();
	~cUsbDeviceTreeItem();
};
class	cUsbDeviceTree
{
public:
	ULONG				Count;
	cUsbDeviceTreeItem*	Items;
private:
	HDEVINFO			hDevInfoSet;
public:
	cUsbDeviceTree();
	~cUsbDeviceTree();
	bool	Build( void );
	void	Free( void );
	bool	Update( void );
	bool	Compare(cUsbDeviceTree& UsbDeviceTree);
	operator	HDEVINFO() const;
private:
	bool	EnumerateHub(LPCTSTR HubName,cUsbDeviceTreeItem& Item);
	bool	EnumerateCcgp(cUsbDeviceTreeItem& Item);
	bool	UpdateItem(cUsbDeviceTreeItem& dstItem,cUsbDeviceTreeItem& srcItem);
	bool	CompareItem(cUsbDeviceTreeItem& Item1,cUsbDeviceTreeItem& Item2);
};

///////////////////////////////////////////////
//	Device enumarator
///////////////////////////////////////////////
#define	NOT_SPECIFIED_VID		(0x0000)
#define	NOT_SPECIFIED_PID		(0x0000)
#define	NOT_SPECIFIED_USBPATH	(NULL)
/*! -----------------------------------------------------------------

		@brief	USBデバイスを探したりデバイス情報とったり

--------------------------------------------------------------------*/
class	cEnumDevices
{
	typedef	bool	(CALLBACK *fEnumDeviceProc)(PUSBDEVICEDATA pUsbDeviceData,void* pContext);
private:
	cUsbDeviceTree	UsbDeviceTree;
	cCOMctl			ComCtl;
public:
	cEnumDevices();
	cEnumDevices(DWORD dwCoInit);
private:
	void	ConstSub(DWORD dwCoInit);
public:
	int	EnumDevices(
		const GUID& gCategoryClass,const GUID& gDeviceClass,
		fEnumDeviceProc Callback,void* pContext,bool BuildUsbDeviceTree=false);

	// デバイスを探索し、デバイスデータを返す
	PUSBDEVICEDATA	Find(const GUID& gCategoryClass,const GUID& gDeviceClass,
		WORD VID = NOT_SPECIFIED_VID,
		WORD PID = NOT_SPECIFIED_PID,
		LPCTSTR UsbPath = NOT_SPECIFIED_USBPATH,bool BuildUsbDeviceTree=false);
	operator	HDEVINFO() const;
private:
	int	EnumUsbDeviceTree(cUsbDeviceTreeItem* pItem,
		const GUID& gCategoryClass,const GUID& gDeviceClass,
		fEnumDeviceProc Callback,void* pContext,bool& bContinue);
};

///////////////////////////////////////////////////
//	Is Ricoh's driver installed?
///////////////////////////////////////////////////
extern	bool	R5U87XCTL_API	IsRicohDriver(PUSBDEVICEDATA pUsbDeviceData);

extern	bool	R5U87XCTL_API	IsRicohDriver(
	HDEVINFO hDevInfoSet,PSP_DEVINFO_DATA DevInfoData);

///////////////////////////////////////////////
//	WOW64 checker
///////////////////////////////////////////////
extern	bool	R5U87XCTL_API	IsWow64( void );

///////////////////////////////////////////////
//	Enable privileges
///////////////////////////////////////////////
extern	bool	R5U87XCTL_API	EnablePrivileges(const LPCTSTR* RequestPrivileges);

///////////////////////////////////////////////
//	Get string descriptor data
///////////////////////////////////////////////
extern	bool	R5U87XCTL_API	GetStringDescData(PUSBDEVICEDATA pDeviceData,int ID,LPWSTR* pString);

///////////////////////////////////////////////
//	Enable/Disable device
///////////////////////////////////////////////
extern	bool	R5U87XCTL_API	EnableUsbDevice(PUSBDEVICEDATA pDeviceData,bool bEnable,bool* pbNeedReboot);

///////////////////////////////////////////////
//	Driver Installer
///////////////////////////////////////////////
extern	bool	R5U87XCTL_API	InstallDriver(
	PUSBDEVICEDATA pUsbDeviceData,
	LPCTSTR InfPath,
	HWND hWndParent,
	bool bBackup,
	bool* pNeedReboot,
	bool bAutoUninstall);

extern	bool	R5U87XCTL_API	InstallDriver(
	LPCTSTR InstanceID,
	LPCTSTR InfPath,
	HWND hWndParent,
	bool bBackup,
	bool* pNeedReboot,
	bool bAutoUninstall);

extern	bool	R5U87XCTL_API	InstallDriver(
	HDEVINFO hDevInfoSet,
	PSP_DEVINFO_DATA DevInfoData,
	LPCTSTR HardwareID,
	LPCTSTR InfPath,
	HWND hWndParent,
	bool bBackup,
	bool* pNeedReboot,
	bool bAutoUninstall);

///////////////////////////////////////////////
//	Rollback driver
///////////////////////////////////////////////
extern	bool	R5U87XCTL_API	RollbackDriver(
	PUSBDEVICEDATA pUsbDeviceData,
	HWND hWndParent,
	bool bNoUI,
	bool* pNeedReboot);

extern	bool	R5U87XCTL_API	RollbackDriver(
	LPCTSTR InstanceID,
	HWND hWndParent,
	bool bNoUI,
	bool* pNeedReboot);

extern	bool	R5U87XCTL_API	RollbackDriver(
	HDEVINFO hDevInfoSet,
	PSP_DEVINFO_DATA DevInfoData,
	HWND hWndParent,
	bool bNoUI,
	bool* pNeedReboot);

///////////////////////////////////////////////
//	Remove device instance
///////////////////////////////////////////////
extern	bool	R5U87XCTL_API	RemoveDeviceInstance(
	PUSBDEVICEDATA pUsbDeviceData,
	bool* pNeedReboot);

extern	bool	R5U87XCTL_API	RemoveDeviceInstance(
	LPCTSTR InstanceID,
	bool* pNeedReboot);

extern	bool	R5U87XCTL_API	RemoveDeviceInstance(
	HDEVINFO hDevInfoSet,
	PSP_DEVINFO_DATA DevInfoData,
	bool* pNeedReboot);

///////////////////////////////////////////////////////////////////////
//	Set device instance ID list for uninstall driver automatically
///////////////////////////////////////////////////////////////////////
extern	void	R5U87XCTL_API	SetUninstallDevice(
	PUSBDEVICEDATA pUsbDeviceData,
	bool bRollback);

extern	void	R5U87XCTL_API	SetUninstallDevice(
	LPCTSTR InstanceID,
	bool bRollback);

extern	void	R5U87XCTL_API	SetUninstallDevice(
	HDEVINFO hDevInfoSet,
	PSP_DEVINFO_DATA DevInfoData,
	bool bRollback);

///////////////////////////////////////////////////////////////////////
//	Delete device instance ID list for uninstall driver automatically
///////////////////////////////////////////////////////////////////////
extern	void	R5U87XCTL_API	DelUninstallDevice(
	PUSBDEVICEDATA pUsbDeviceData);

extern	void	R5U87XCTL_API	DelUninstallDevice(
	LPCTSTR InstanceID);

extern	void	R5U87XCTL_API	DelUninstallDevice(
	HDEVINFO hDevInfoSet,
	PSP_DEVINFO_DATA DevInfoData);

///////////////////////////////////////////////////////////
//	Check drivers installed in this library
///////////////////////////////////////////////////////////
extern	bool	R5U87XCTL_API	IsUninstallDevice( void );

///////////////////////////////////////////////////////////
//	The drivers installed in this library are uninstalled.
///////////////////////////////////////////////////////////
extern	void	R5U87XCTL_API	UninstAll(bool bDeleteUpdateDriver,bool bReenumDevices);

///////////////////////////////////////////////////
//	Re-enumerate device nodes (Rescan PnP devices)
///////////////////////////////////////////////////
extern	bool	R5U87XCTL_API	ReenumerateDeviceNode( void );


///////////////////////////////////////////////////
//	Video preview window class
///////////////////////////////////////////////////
class	PREVWND
{
#define	PREVWND_FIRST_FRAME_DETECT_TIMEOUT	(10000)	//	[ms]=10[sec]
#define	PREVWND_FIRST_FRAME_DETECT_INTERVAL	(100)	//	[ms]
#define	PREVWND_DEF_STYLE					(WS_POPUP|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX)
#define	PREVWND_DEF_EX_STYLE				(0)
#define	GWLP_PREVWND_USERDATA				(sizeof(PREVWND*))	//	data size = sizeof(void*)
	typedef	bool	(CALLBACK *fAddFilterCB)(PREVWND* pPrevWnd,LPVOID context);
//public:
public:
	//	hWnd is created by PREVWND::Create()
	HWND					hWnd;
	//	hParent is set by PREVWND::Create() or PREVWND::SerParent()
	HWND					hParent;
	//	hStatusBar is created by PREVWND::Create()
	HWND					hStatusBar;
	//	Running : CaptureStarted = true / Stopping : CaptureStarted = false
	bool					CaptureStarted;
	//	Interfaces
	IMoniker*				pMoniker;
	IGraphBuilder*			pGraph;
	IBaseFilter*			pBaseFilter;	//	Video input device
	ICaptureGraphBuilder2*	pBuilder;
	IAMStreamConfig*		pStreamConfig;
	IVideoWindow*			pWnd;
	IMediaControl*			pMediaControl;
	//	Video format
	AM_MEDIA_TYPE*			pMediaType;
	//	Window size type
	BYTE					SizeType;	//	0=1/1(default) 1=1/2 2= 1/4
private:
	//	Sink filter for movie capture
	GUID					guidCaptureFormatType;
	TCHAR					CapFile[MAX_PATH+1];
	IFileSinkFilter*		pSink;
	IBaseFilter*			pFileOut;
	//	Graph info for PinSelect
	fAddFilterCB			AddFilterCB;
	LPVOID					ctxAddFilterCB;
	int						GraphCompleted;
	IBaseFilter*			pIntermediate;
	IBaseFilter*			pRenderer;
public:
	PREVWND();
	~PREVWND();
	//	Create/Close parent window
	bool	Create(LPCTSTR Name=NULL,HWND hParent=NULL,
		DWORD Style=PREVWND_DEF_STYLE,DWORD ExStyle=PREVWND_DEF_EX_STYLE,
		int x=CW_USEDEFAULT,int y=0,
		int w=CW_USEDEFAULT,int h=0,
		int nCmdShow=SW_HIDE,WORD IdStatusBar=0);
	bool	Create(IMoniker* pMoniker,
		fAddFilterCB AddFilterCB=NULL,LPVOID ctxAddFilterCB=NULL,
		bool bBuild=true,IBaseFilter* pIntermediate=NULL,IBaseFilter* pRenderer=NULL,
		LPCTSTR Name=NULL,HWND hParent=NULL,
		DWORD Style=PREVWND_DEF_STYLE,DWORD ExStyle=PREVWND_DEF_EX_STYLE,
		int x=CW_USEDEFAULT,int y=0,
		int nCmdShow=SW_HIDE,WORD IdStatusBar=0);
	bool	Create(c5U87xCtl& ctl,
		fAddFilterCB AddFilterCB=NULL,LPVOID ctxAddFilterCB=NULL,
		bool bBuild=true,IBaseFilter* pIntermediate=NULL,IBaseFilter* pRenderer=NULL,
		LPCTSTR Name=NULL,HWND hParent=NULL,
		DWORD Style=PREVWND_DEF_STYLE,DWORD ExStyle=PREVWND_DEF_EX_STYLE,
		int x=CW_USEDEFAULT,int y=0,
		int nCmdShow=SW_HIDE,WORD IdStatusBar=0);
	void	Close( void );
	//	Set parent window
	void	SetParent(HWND hParent);
	//	Show window
	bool	ShowWindow(int nCmdShow);
	//	Resize window
	bool	SetWindowSize(int w=0,int h=0,bool bClientSize=true);
	//	Attach video capture control to device by pMoniker
	bool	Attach(IMoniker* pMoniker,
		fAddFilterCB AddFilterCB=NULL,LPVOID ctxAddFilterCB=NULL,
		bool bBuild=true,IBaseFilter* pIntermediate=NULL,IBaseFilter* pRenderer=NULL);
	bool	Attach(c5U87xCtl& ctl,
		fAddFilterCB AddFilterCB=NULL,LPVOID ctxAddFilterCB=NULL,
		bool bBuild=true,IBaseFilter* pIntermediate=NULL,IBaseFilter* pRenderer=NULL);
	//	Detach and disconnect filter graph
	void	Detach( void );
	//	Utility functions are called from fAddFilterCB
	bool	AddFilter(IBaseFilter* pFilter,LPCWSTR pName);
	bool	ConnectFilter(
		IBaseFilter* pDstFilter,LPCWSTR pDstPinName,
		IBaseFilter* pSrcFilter,LPCWSTR pSrcPinName);
	bool	ConnectPin(IPin* pDstPin,IPin* pSrcPin);
	IPin*	FindPin(IBaseFilter* pFilter,PIN_DIRECTION PinDir,LPCWSTR Id,bool bOnlyNotConnected=true);
	bool	Render(IBaseFilter* pRenderer,LPCWSTR pRenderPinName);
	bool	Render(IPin* pPin);
	//	Build filter graph
	void	SetMovieCaptureFile(LPCTSTR FilePath=NULL,const GUID* pType=NULL);
	bool	GetMovieCaptureFile(LPTSTR FilePath=NULL,GUID* pType=NULL);
	bool	Build(IBaseFilter* pIntermediate=NULL,IBaseFilter* pRenderer=NULL);
	bool	BuildComplete( void );
	//	Properties
	bool	FilterProperty( void );
	bool	PinSelect( void );
	bool	GetVideoFormat(
		GUID* pMediaSubType,
		BITMAPINFOHEADER** ppBmpInfoHdr,
		REFERENCE_TIME* pFrameInterval);
	//	Media controls
	bool	Run(DWORD msWait1stFrame=PREVWND_FIRST_FRAME_DETECT_TIMEOUT,
				DWORD Interval=PREVWND_FIRST_FRAME_DETECT_INTERVAL);
	bool	Stop( void );
private:
	static	LRESULT	CALLBACK	WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
#undef	PREVWND_FIRST_FRAME_DETECT_TIMEOUT
#undef	PREVWND_FIRST_FRAME_DETECT_INTERVAL
};


///////////////////////////////////////////////////
//	ExtensionUnit control class
///////////////////////////////////////////////////
class	UVCXUCTL
{
private:
	GUID		guidXu;
	bool		bUseNode;
	DWORD		NodeID;
	IKsControl*	pKsControl;
public:
	UVCXUCTL();
	~UVCXUCTL();
	bool	FindXu(IBaseFilter* pBaseFilter,const GUID& guid,IKsControl* pKsControl=NULL);
	bool	FindXu(IMoniker* pMoniker,const GUID& guid,IKsControl* pKsControl=NULL);
	bool	FindXu(PREVWND& PrevWnd,const GUID& guid,IKsControl* pKsControl=NULL);
	bool	FindXu(c5U87xCtl& ctl,const GUID& guid,IKsControl* pKsControl=NULL);
	void	Release( void );
	bool	GetParam(BYTE cs,PULONG pLen,LPBYTE pMin,LPBYTE pMax,LPBYTE pRes,LPBYTE pDef);
	bool	GetCur(BYTE cs,LPBYTE pCur,ULONG Len);
	bool	SetCur(BYTE cs,LPBYTE pCur,ULONG Len);
};

#pragma	warning(pop)
#endif
