// L1n 用 コントロールコマンド
//
//
#ifndef _COMMAND_L1N_H
#define _COMMAND_L1N_H

#include "Winsock2.h"
#include "ws2def.h"

// 機種情報

#define PRESENTER_L1N			(0x0001)			//Digital Visual Presenter L1n/TT02


//iIoctlで使用する構造体
/*------------------------------------------------------------------------------*/
/*!
 * @struct	stGETPICTURE_L1N
 * @brief	Picture取得で使用する構造体
 */
/*------------------------------------------------------------------------------*/
struct stGETPICTURE_L1N{
	unsigned int uiSize;							//Jpegサイズ
	unsigned char *pucJpegData;						//Jpegデータ
};


//機種別コマンド

#define L1N_IOC_BASE			'L'
#define L1N_GET_PICTURE			_IOR(L1N_IOC_BASE,1,struct stGETPICTURE_L1N)		//Picture取得
#define L1N_PICTURE_RELEASE		_IOW(L1N_IOC_BASE,2,struct stGETPICTURE_L1N)		//Picture取得で確保した領域の解放

#define L1N_ZOOM_WIDE			_IO(L1N_IOC_BASE,3)								//カメラへのズーム操作（WIDE側）
#define L1N_ZOOM_TELE			_IO(L1N_IOC_BASE,4)								//カメラへのズーム操作（TELE側）
#define L1N_ZOOM_STOP			_IO(L1N_IOC_BASE,5)								//カメラへのズーム操作（STOP）

#define L1N_IRIS_OPEN			_IO(L1N_IOC_BASE,6)								//カメラへのIRIS操作（OPEN側）
#define L1N_IRIS_CLOSE			_IO(L1N_IOC_BASE,7)								//カメラへのIRIS操作（CLOSE側）
#define L1N_IRIS_NORMAL			_IO(L1N_IOC_BASE,8)								//カメラへのIRIS操作（NORMAL）
#define L1N_IRIS_STOP			_IO(L1N_IOC_BASE,9)								//カメラへのIRIS操作（STOP）

#define L1N_FOCUS_FAR			_IO(L1N_IOC_BASE,10)							//カメラへのFOCUS操作（FAR側）
#define L1N_FOCUS_NEAR			_IO(L1N_IOC_BASE,11)							//カメラへのFOCUS操作（NEAR側）
#define L1N_FOCUS_STOP			_IO(L1N_IOC_BASE,12)							//カメラへのFOCUS操作（STOP側）
#define L1N_FOCUS_AF			_IO(L1N_IOC_BASE,13)							//カメラへのFOCUS操作（AutoFocus）

#define L1N_G_CAMERA_NAME		_IOR(L1N_IOC_BASE,14,struct stGETCAMERANAME)	//機種名取得要求

#define L1N_IOC_MAXNR			15
//コマンドに必要なパラメータ構造体

#endif
