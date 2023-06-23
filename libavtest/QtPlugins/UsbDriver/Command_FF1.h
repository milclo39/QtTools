// FF1 用 コントロールコマンド
//
//
#ifndef _COMMAND_FF1_H
#define _COMMAND_FF1_H

#include "Winsock2.h"
#include "ws2def.h"

// 機種情報

#define PRESENTER_FF1			(0x0009)			//Digital Visual Presenter FF1/TT02


//iIoctlで使用する構造体
/*------------------------------------------------------------------------------*/
/*!
 * @struct	stGETPICTURE_FF1
 * @brief	Picture取得で使用する構造体
 */
/*------------------------------------------------------------------------------*/
struct stGETPICTURE_FF1{
	unsigned int uiSize;							//Jpegサイズ
	unsigned char *pucJpegData;						//Jpegデータ
};


//機種別コマンド

#define FF1_IOC_BASE			'L'
#define FF1_GET_PICTURE			_IOR(FF1_IOC_BASE,1,struct stGETPICTURE_FF1)		//Picture取得
#define FF1_PICTURE_RELEASE		_IOW(FF1_IOC_BASE,2,struct stGETPICTURE_FF1)		//Picture取得で確保した領域の解放

#define FF1_ZOOM_WIDE			_IO(FF1_IOC_BASE,3)								//カメラへのズーム操作（WIDE側）
#define FF1_ZOOM_TELE			_IO(FF1_IOC_BASE,4)								//カメラへのズーム操作（TELE側）
#define FF1_ZOOM_STOP			_IO(FF1_IOC_BASE,5)								//カメラへのズーム操作（STOP）

#define FF1_IRIS_OPEN			_IO(FF1_IOC_BASE,6)								//カメラへのIRIS操作（OPEN側）
#define FF1_IRIS_CLOSE			_IO(FF1_IOC_BASE,7)								//カメラへのIRIS操作（CLOSE側）
#define FF1_IRIS_NORMAL			_IO(FF1_IOC_BASE,8)								//カメラへのIRIS操作（NORMAL）
#define FF1_IRIS_STOP			_IO(FF1_IOC_BASE,9)								//カメラへのIRIS操作（STOP）

#define FF1_FOCUS_FAR			_IO(FF1_IOC_BASE,10)							//カメラへのFOCUS操作（FAR側）
#define FF1_FOCUS_NEAR			_IO(FF1_IOC_BASE,11)							//カメラへのFOCUS操作（NEAR側）
#define FF1_FOCUS_STOP			_IO(FF1_IOC_BASE,12)							//カメラへのFOCUS操作（STOP側）
#define FF1_FOCUS_AF			_IO(FF1_IOC_BASE,13)							//カメラへのFOCUS操作（AutoFocus）

#define FF1_G_CAMERA_NAME		_IOR(FF1_IOC_BASE,14,struct stGETCAMERANAME)	//機種名取得要求

#define FF1_IOC_MAXNR			15
//コマンドに必要なパラメータ構造体

#endif
