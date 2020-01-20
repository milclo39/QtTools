//------------------------------------------------------------------------
// Copyright(C) Infinitegra, INC. All Right Reserved.
//
// Confidential:
//  The contents of this material are confidential and proprietary to
//  Infinitegra, INC. and may not be reproduced, published or disclosed
//  to others without company authorization.
//------------------------------------------------------------------------

#pragma once

#include <Windows.h>

#ifdef XUCONTROLLIB_EXPORTS
#define XUCONTROLLIB_API __declspec(dllexport)
#else
#define XUCONTROLLIB_API __declspec(dllimport)
#endif

//--------------------------------
// Types
//

struct _XuCtrl;
typedef struct _XuCtrl *XUCTRL;


typedef enum
{
	XUCTRL_ST_OK = 0,
	XUCTRL_ST_NULL_POINTER,
	XUCTRL_ST_INVALID_VALUE,
	XUCTRL_ST_NO_MEMORY,
	XUCTRL_ST_HOST_ERROR,
	XUCTRL_ST_DEVICE_NOT_FOUND,
	XUCTRL_ST_DEVICE_NOT_SUPPORTED,
	XUCTRL_ST_DEVICE_IN_PROGRESS,
	XUCTRL_ST_DEVICE_ERROR,
} XuCtrlStatus_t;


//--------------------------------
// Functions
//

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

////// Alloc/Release

XUCONTROLLIB_API XuCtrlStatus_t
XuCtrl_GetCtrlByCaptureFilter(void *capFilter, XUCTRL *ctrlp, HRESULT *hrp);

XUCONTROLLIB_API void
XuCtrl_PutCtrl(XUCTRL ctrl);

////// Properties

XUCONTROLLIB_API UINT32
XuCtrl_GetLibVersion(void);

XUCONTROLLIB_API UINT32
XuCtrl_GetFWVersion(XUCTRL ctrl);

////// Basic Read/Write

#define XUCTRL_DATA_LEN_MAX	254

XUCONTROLLIB_API XuCtrlStatus_t
XuCtrl_Request(XUCTRL ctrl, UCHAR *datap, INT len, HRESULT *hrp);

XUCONTROLLIB_API XuCtrlStatus_t
XuCtrl_Result(XUCTRL ctrl, UCHAR *datap, INT *lenp, HRESULT *hrp);

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus
