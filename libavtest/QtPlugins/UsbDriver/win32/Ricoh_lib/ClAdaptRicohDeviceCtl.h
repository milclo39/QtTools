// --------------------------------------------------------------------------------------------- //
  /*! =========================================================================================

		@file   ClAdaptRicohDeviceCtl.h

		@author （Ｓ技１）熊崎

		@brief  リコーのデバイスコントロールクラスの仲介クラス定義

	==========================================================================================　*/
// --------------------------------------------------------------------------------------------- //

#ifndef CLADAPTRICOHDEVICECTL_H
#define CLADAPTRICOHDEVICECTL_H

// #define DEVICE_DESCRIPTORPOINTEROUTPUT
// #define DEVICE_DESCRIPTOR_DUMP


#include "5U87xCtl.h"

#include <QObject>
#include <QMetaType>

#include	<tchar.h>
#include	<stdio.h>
#include	<conio.h>

#include	<initguid.h>	//	include for generate GUID object
#include	<devguid.h>		//	include for GUID_DEVCLASS_IMAGE

#define		_CLADAPTRICOHDEVICECTL_OUTPUT_POINTER_BASE			(16)	//!< ポインタ出力の進数
#define		_CLADAPTRICOHDEVICECTL_OUTPUT_POINTER_FIELDWIDTH	(0)		//!< ポインタ出力の表示桁数

/*! ----------------------------------------
//
//		@brief	UVC系のパラメータ
//
// --------------------------------------- */
struct stUVC_STD_PARAM
{
	stUVC_STD_PARAM( qlonglong	qlCur = 0,
				 qlonglong	qlMin = 0,
				 qlonglong	qlMax = 0,
				 qlonglong	qlRes = 0,
				 qlonglong	qlDef = 0)
		: qlCur( qlCur )
		, qlMin( qlMin )
		, qlMax( qlMax )
		, qlRes( qlRes )
		, qlDef( qlDef )
	{}

	qlonglong	qlCur;
	qlonglong	qlMin;
	qlonglong	qlMax;
	qlonglong	qlRes;
	qlonglong	qlDef;
};
Q_DECLARE_METATYPE(stUVC_STD_PARAM)

/*! ----------------------------------------
//
//		@brief	UVC系のAuto系パラメータ
//
// --------------------------------------- */
struct stUVC_AUTO_PARAM
{
	stUVC_AUTO_PARAM( bool	qlCap = 0,
					  bool	qlCur = 0,
					  bool	qlDef = 0)
		: qlCap( qlCap )
		, qlCur( qlCur )
		, qlDef( qlDef )
	{}

	bool	qlCap;
	bool	qlCur;
	bool	qlDef;
};
Q_DECLARE_METATYPE(stUVC_AUTO_PARAM)

// ---------------------------------------------------------------------------------------- //
  /*! ====================================================================================

			@ingroup    grpAdaptRicohLib

			@brief		リコーのデバイスコントロールクラスの仲介クラス

	=====================================================================================　*/
// ---------------------------------------------------------------------------------------- //
class ClAdaptRicohDeviceCtl : public QObject
{

	Q_OBJECT

public:



public:

	// コンストラクタ
	explicit ClAdaptRicohDeviceCtl( QObject *pclParent = 0 );

	// ----------------- //
	//		ゲッター
	// ----------------- //

	// Camera Terminal controls
	bool	getZoom	( stUVC_STD_PARAM* pstRetVal );
	bool	getExposure	( stUVC_STD_PARAM*	pstRetVal, stUVC_AUTO_PARAM*	pstRetAutoVal );
	bool	getAutoExposurePriority( stUVC_STD_PARAM* pstRetVal );
	bool	getFocus	( stUVC_STD_PARAM*	pstRetVal, stUVC_AUTO_PARAM* pstRetAutoVal );

	// Video Processing Unit controls
	bool	getBrightness	( stUVC_STD_PARAM* pstRetVal );
	bool	getContrast		( stUVC_STD_PARAM* pstRetVal );
	bool	getHue( stUVC_STD_PARAM*	pstRetVal, stUVC_AUTO_PARAM*	pstRetAutoVal );
	bool	getSaturation	( stUVC_STD_PARAM* pstRetVal );
	bool	getSharpness	( stUVC_STD_PARAM* pstRetVal );
	bool	getGamma		( stUVC_STD_PARAM* pstRetVal );
	bool	getWhiteBalanceTemperature( stUVC_STD_PARAM*	pstRetVal , stUVC_AUTO_PARAM*	pstRetAutoVal );
	bool	getBacklightCompensation( stUVC_STD_PARAM* pstRetVal );
	bool	getGain					( stUVC_STD_PARAM* pstRetVal );
	bool	getPowerLineFrequency	( stUVC_STD_PARAM* pstRetVal );
	bool	getWhiteBalanceFullComponent( stUVC_STD_PARAM*	pstRed,
										  stUVC_STD_PARAM*	pstGreen,
										  stUVC_STD_PARAM*	pstBlue,
										  stUVC_AUTO_PARAM*	pstRetAutoVal );
	bool	getExposureEx	( stUVC_STD_PARAM*	pstRetVal, stUVC_AUTO_PARAM*	pstRetAutoVal );
	bool	getGainEx		( stUVC_STD_PARAM*	pstRetVal, stUVC_AUTO_PARAM*	pstRetAutoVal );
	bool	getPowerLineFrequencyEx( stUVC_STD_PARAM*	pstRetVal, stUVC_AUTO_PARAM*	pstRetAutoVal );
	bool	getExtension1	( stUVC_STD_PARAM* pstRetVal );
	bool	getExtension2	( stUVC_STD_PARAM* pstRetVal );
	bool	getExtension3	( stUVC_STD_PARAM* pstRetVal );
	bool	getExtension4	( stUVC_STD_PARAM* pstRetVal );
	bool	getExtension5	( stUVC_STD_PARAM* pstRetVal );
	bool	getExtension6	( stUVC_STD_PARAM* pstRetVal );
	bool	getExtension7	( stUVC_STD_PARAM* pstRetVal );
	bool	getExtension8	( stUVC_STD_PARAM* pstRetVal );

signals:

public slots:

	// ----------------- //
	//		セッター
	// ----------------- //

	// Camera Terminal controls
	bool	slotSetZoom( qlonglong qlParam );
	bool	slotSetExposure( qlonglong qlParam, bool bAuto);
	bool	slotSetAutoExposurePriority( qlonglong qlParam );
	bool	slotSetFocus( qlonglong qlParam, bool bAuto);

	// Video Processing Unit controls
	bool	slotSetBrightness	( qlonglong qlParam );
	bool	slotSetContrast		( qlonglong qlParam );
	bool	slotSetHue			( qlonglong qlParam, bool bAutoPAram);
	bool	slotSetSaturation	( qlonglong qlParam );
	bool	slotSetSharpness	( qlonglong qlParam );
	bool	slotSetGamma		( qlonglong qlParam );
	bool	slotSetWhiteBalanceTemperature	( qlonglong qlParam, bool bAutoPAram);
	bool	slotSetBacklightCompensation	( qlonglong qlParam );
	bool	slotSetGain						( qlonglong qlParam );
	bool	slotSetPowerLineFrequency		( qlonglong qlParam );
	bool	slotSetWhiteBalanceFullComponent( qlonglong qlRed, qlonglong qlGreen, qlonglong qlBlue, bool bAutoPAram);
	bool	slotSetExposureEx			( qlonglong qlParam, bool bAutoPAram);
	bool	slotSetGainEx				( qlonglong qlParam, bool bAutoPAram);
	bool	slotSetPowerLineFrequencyEx	( qlonglong qlParam, bool bAutoPAram);
	bool	slotSetExtension1( qlonglong qlParam );
	bool	slotSetExtension2( qlonglong qlParam );
	bool	slotSetExtension3( qlonglong qlParam );
	bool	slotSetExtension4( qlonglong qlParam );
	bool	slotSetExtension5( qlonglong qlParam );
	bool	slotSetExtension6( qlonglong qlParam );
	bool	slotSetExtension7( qlonglong qlParam );
	bool	slotSetExtension8( qlonglong qlParam );

protected:

	USBDEVICEDATA*	m_pclUsbDeviceData;		//!< デバイスデータそのもの
	cEnumDevices*	m_pclEnumDevice;		//!< デバイスを見つけてくれたりするいいやつ
	c5U87xCtl		m_clRicohDeviceCtl;		//!< デバイスにいろいろしてくれるいいやつ

};




#endif // CLADAPTRICOHDEVICECTL_H
