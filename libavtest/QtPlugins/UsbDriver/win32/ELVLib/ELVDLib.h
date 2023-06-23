/* ========================================================================
*  PROJECT: DirectShow Video Processing Library (ELVDLib)
*  Version: 0.0.8 (05/13/2005)
*  ========================================================================
*  Author:  Thomas Pintaric, Vienna University of Technology
*  Contact: pintaric@ims.tuwien.ac.at http://ims.tuwien.ac.at/~thomas
*  =======================================================================
* 
*  Copyright (C) 2005  Vienna University of Technology
* 
*  This library is free software; you can redistribute it and/or
*  modify it under the terms of the GNU General Public License
*  as published by the Free Software Foundation; either version 2
*  of the License, or (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*  For further information please contact Thomas Pintaric under
*  <pintaric@ims.tuwien.ac.at> or write to Thomas Pintaric,
*  Vienna University of Technology, Favoritenstr. 9-11/E188/2, A-1040
*  Vienna, Austria.
* ========================================================================*/

#ifndef	__ELVDLIB_H__	// for include once
#define __ELVDLIB_H__

#define ELVDLib_VERSION 0x00000802 // 0.0.8b

#ifdef ELVDLIB_EXPORTS
#define ELVDLib_API __declspec(dllexport)
#else
#define ELVDLib_API __declspec(dllimport)
#endif

#include <Windows.h>
#include "ELVDLib_PixelFormatTypes.h"
#include <strmif.h>

#define VideoProcAmp_PowerLineFrequency (VideoProcAmp_Gain + 1)


// ------------------------------------------------------------
#ifndef __MEMORY_BUFFER_HANDLE__
#define __MEMORY_BUFFER_HANDLE__

#define MIN_ALLOCATOR_BUFFERS_PER_CLIENT  3
#define DEF_CONCURRENT_CLIENTS            3
struct MemoryBufferHandle
{
	unsigned long  n; // sample number
	LONGLONG t; // timestamp
};

#endif

/*! --------------------------------------------- //
//
//	@brief	UVCの種類
//
//  --------------------------------------------- */
typedef enum _eCAMERA_CONTROL_TYPE
{
	CC_CameraControl	= 0, // IAMCameraControl
	CC_VideoProcAmp		= 1  // IAMVideoProcAmp
} eCAMERA_CONTROL_TYPE;

// 解像度
struct stRESOLUTION
{
	stRESOLUTION(
			int iWidth	= 0,
			int iHeight = 0
		)
		:iWidth( iWidth )
		,iHeight( iHeight )
	{}
	LONG iWidth;
	LONG iHeight;
};

// 解像度ズ
struct stRESOLUTIONS
{
	stRESOLUTIONS(
			int iCount = 0,
			stRESOLUTION* stResolutions = NULL
		)
		:iCount(iCount)
		,stResolutions(stResolutions)
	{}

	int	iCount;
	stRESOLUTION* stResolutions;
};

// フレームレート
struct stFRAMERATE
{
	stFRAMERATE(
			double dFrameRate = 0
		)
		:dFrameRate( dFrameRate )
	{}
	DOUBLE dFrameRate;
};

// フレームレートズ
struct stFRAMERATES
{
	stFRAMERATES(
			int iCount = 0,
			stFRAMERATE* stFrameRates = NULL
		)
		:iCount(iCount)
		,stFrameRates(stFrameRates)
	{}

	int	iCount;
	stFRAMERATE* stFrameRates;
};

#define _INVALID_TIMESTAMP 0;
// ------------------------------------------------------------

class ELVDLib_API ELVDLib_VideoSource
{
public:
	ELVDLib_VideoSource();
	~ELVDLib_VideoSource();

	HRESULT buildString(char* xml_string = NULL);
	HRESULT buildFile(char* xml_filename = NULL);
	HRESULT release();

	HRESULT bufferEnable(unsigned int _maxConcurrentClients = DEF_CONCURRENT_CLIENTS,
                         unsigned int _allocatorBuffersPerClient = MIN_ALLOCATOR_BUFFERS_PER_CLIENT);
	HRESULT bufferDisable();

	bool isInitialized();
	DWORD waitSample(long dwMilliseconds = INFINITE); 

	HRESULT bufferCheckout(MemoryBufferHandle* pHandle, 
		BYTE** Buffer,
		unsigned int *Width = NULL,
		unsigned int *Height = NULL,
		PIXELFORMAT* PixelFormat = NULL,
		LONGLONG* Timestamp = NULL);

	HRESULT bufferCheckin(MemoryBufferHandle Handle, bool ForceRelease = false); 

	HRESULT getFormat(LONG* frame_width,
                                  LONG *frame_height,
								  double* frames_per_second,
								  PIXELFORMAT* pixel_format);
	LONGLONG getTimestamp();

	// media flow control
	HRESULT run();
	HRESULT pause();
	HRESULT stop(bool forcedStop = false);

public:
	// valid types for (long property): CameraControlProperty, VideoProcAmpProperty
	HRESULT getParamRange(	eCAMERA_CONTROL_TYPE interface_type,
							long property,
							long *pMin,
							long *pMax,
							long *pSteppingDelta,
							long *pDefault,
							long *pCapsFlags);

	HRESULT getParam( eCAMERA_CONTROL_TYPE interface_type, long Property, long *lValue, bool *bAuto);
	HRESULT setParam( eCAMERA_CONTROL_TYPE interface_type, long Property, long lValue, bool bAuto);

public:
	// applies settings to a normalized the parameter range [0..1]
	HRESULT getParams( eCAMERA_CONTROL_TYPE interface_type, long Property, double *dValue);
	HRESULT setParams( eCAMERA_CONTROL_TYPE interface_type, long Property, double dValue); // dValue will be clamped to [0..1]

	HRESULT setDefParam( eCAMERA_CONTROL_TYPE interface_type, long Property, bool bAuto = TRUE); 
	// bAuto: indicates if the property should be controlled automatically

	HRESULT resetParam(bool bAuto = TRUE); // reset all parameters to their defaults

//	DWORD getTemp();
public:
	/* --------------------------------
	//
	//		@brief	解像度系
	//
	// -------------------------------*/

	HRESULT getSupportedResolutions( stRESOLUTIONS& stResolutions );
	HRESULT getCurrentResolution( stRESOLUTION& stResolution );
	HRESULT setCurrentResolution( stRESOLUTION stResolution );

	/* --------------------------------
	//
	//		@brief	フレームレート系
	//
	// -------------------------------*/

	HRESULT getSupportedFrameRates( stRESOLUTION stResolution, stFRAMERATES& stFrameRates );
	HRESULT getCurrentFrameRate( stFRAMERATE& stFrameRate );
	HRESULT setCurrentFrameRate( stFRAMERATE stFrameRate );

	/* --------------------------------
	//
	//		@brief	音声系
	//
	// -------------------------------*/

	HRESULT getCurrentVolume( DOUBLE &dVol );
	HRESULT setCurrentVolume( DOUBLE dVol );

	HRESULT getAudioEnable( BOOL &bIsEnable );
	HRESULT setAudioEnable( BOOL bIsEnable );

public:

	// Ks　準備
	HRESULT initKsNode( GUID guidExtension );		// エクステンションユニット固有のGUID

	/* --------------------------------
	//
	//		@brief	ExtensionUnit
	//
	// ------------------------------- */


	// エクステンションユニット　ゲッタ
	HRESULT getExtensionValues(	ULONG	ulProrerty,			// 識別番号
								ULONG*	pulDestLength,		// 値代入用変数データ長
								void*	pvCur,				// 値代入用変数
								void*	pvMin,				//		;
								void*	pvMax,				//		;
								void*	pvRes,				//		;
								void*	pvDef);				//		;

	// エクステンションユニット　セッタ
	HRESULT setExtensionValue(	ULONG	ulProperty,			// 識別番号
							ULONG	ulValueLength,		// データ長
							void*	pvValue);			// パラメータ



private:
	void*	p_graphManager;
	bool	bIsBufferCheckOuted;
};

#endif	// for include once