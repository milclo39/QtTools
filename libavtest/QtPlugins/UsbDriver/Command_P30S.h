// P30S 用 コントロールコマンド
//
//
#ifndef _COMMAND_P30S_H
#define _COMMAND_P30S_H

#include "Winsock2.h"
#include "ws2def.h"

// 機種情報

#define PRESENTER_P30S			(0x0004)			//Digital Visual Presenter P30S


//iIoctlで使用する構造体
/*------------------------------------------------------------------------------*/
/*!
 * @struct	stGETPICTURE_P30S
 * @brief	Picture取得で使用する構造体
 */
/*------------------------------------------------------------------------------*/
struct stGETPICTURE_P30S{
	unsigned int uiSize;							//Jpegサイズ
	unsigned char *pucJpegData;						//Jpegデータ
};


//機種別コマンド

#define P30S_IOC_BASE			'L'
#define P30S_GET_PICTURE			_IOR(P30S_IOC_BASE,1,struct stGETPICTURE_P30S)		//Picture取得
#define P30S_PICTURE_RELEASE		_IOW(P30S_IOC_BASE,2,struct stGETPICTURE_P30S)		//Picture取得で確保した領域の解放

#define P30S_ZOOM_WIDE			_IO(P30S_IOC_BASE,3)								//カメラへのズーム操作（WIDE側）
#define P30S_ZOOM_TELE			_IO(P30S_IOC_BASE,4)								//カメラへのズーム操作（TELE側）
#define P30S_ZOOM_STOP			_IO(P30S_IOC_BASE,5)								//カメラへのズーム操作（STOP）

#define P30S_IRIS_OPEN			_IO(P30S_IOC_BASE,6)								//カメラへのIRIS操作（OPEN側）
#define P30S_IRIS_CLOSE			_IO(P30S_IOC_BASE,7)								//カメラへのIRIS操作（CLOSE側）
#define P30S_IRIS_NORMAL			_IO(P30S_IOC_BASE,8)								//カメラへのIRIS操作（NORMAL）
#define P30S_IRIS_STOP			_IO(P30S_IOC_BASE,9)								//カメラへのIRIS操作（STOP）

#define P30S_FOCUS_FAR			_IO(P30S_IOC_BASE,10)							//カメラへのFOCUS操作（FAR側）
#define P30S_FOCUS_NEAR			_IO(P30S_IOC_BASE,11)							//カメラへのFOCUS操作（NEAR側）
#define P30S_FOCUS_STOP			_IO(P30S_IOC_BASE,12)							//カメラへのFOCUS操作（STOP側）
#define P30S_FOCUS_AF			_IO(P30S_IOC_BASE,13)							//カメラへのFOCUS操作（AutoFocus）

#define P30S_G_CAMERA_NAME		_IOR(P30S_IOC_BASE,14,struct stGETCAMERANAME)	//機種名取得要求

#define P30S_IOC_MAXNR			15
//コマンドに必要なパラメータ構造体

#endif
