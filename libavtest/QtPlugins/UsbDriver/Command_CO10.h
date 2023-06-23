// CO-10 用 コントロールコマンド
//
//
#ifndef _COMMAND_CO10_H
#define _COMMAND_CO10_H

#include "Winsock2.h"
#include "ws2def.h"

// 機種情報

#define PRESENTER_CO10			(0x0005)			//Digital Visual Presenter CO10


//iIoctlで使用する構造体
/*------------------------------------------------------------------------------*/
/*!
 * @struct	stGETPICTURE_CO10
 * @brief	Picture取得で使用する構造体
 */
/*------------------------------------------------------------------------------*/
struct stGETPICTURE_CO10{
	unsigned int uiSize;							//Jpegサイズ
	unsigned char *pucJpegData;						//Jpegデータ
};

//機種別コマンド

#define CO10_IOC_BASE			'C'
#define CO10_GET_PICTURE		_IOR(CO10_IOC_BASE,1,struct stGETPICTURE_CO10)		//Picture取得
#define CO10_PICTURE_RELEASE	_IOW(CO10_IOC_BASE,2,struct stGETPICTURE_CO10)		//Picture取得で確保した領域の解放
#define CO10_PICTURE_RECT		_IOR(CO10_IOC_BASE,3,struct RECT)				//Pictureの縦横長さ取得

#define CO10_ZOOM_WIDE			_IO(CO10_IOC_BASE,4)							//カメラへのズーム操作（WIDE側）
#define CO10_ZOOM_TELE			_IO(CO10_IOC_BASE,5)							//カメラへのズーム操作（TELE側）
#define CO10_ZOOM_STOP			_IO(CO10_IOC_BASE,6)							//カメラへのズーム操作（STOP）

#define CO10_IRIS_OPEN			_IO(CO10_IOC_BASE,7)							//カメラへのIRIS操作（OPEN側）
#define CO10_IRIS_CLOSE			_IO(CO10_IOC_BASE,8)							//カメラへのIRIS操作（CLOSE側）
#define CO10_IRIS_NORMAL		_IO(CO10_IOC_BASE,9)							//カメラへのIRIS操作（NORMAL）
#define CO10_IRIS_STOP			_IO(CO10_IOC_BASE,10)							//カメラへのIRIS操作（STOP）

#define CO10_FOCUS_FAR			_IO(CO10_IOC_BASE,11)							//カメラへのFOCUS操作（FAR側）
#define CO10_FOCUS_NEAR			_IO(CO10_IOC_BASE,12)							//カメラへのFOCUS操作（NEAR側）
#define CO10_FOCUS_STOP			_IO(CO10_IOC_BASE,13)							//カメラへのFOCUS操作（STOP側）
#define CO10_FOCUS_AF			_IO(CO10_IOC_BASE,14)							//カメラへのFOCUS操作（AutoFocus）

#define CO10_GET_RAW			_IOR(CO10_IOC_BASE,15,struct stGETPICTURE_CO10)		//Raw取得
#define CO10_RAW_RELEASE		_IOW(CO10_IOC_BASE,16,struct stGETPICTURE_CO10)		//Raw取得で確保した領域の解放


#define CO10_G_CAMERA_NAME		_IOR(CO10_IOC_BASE,17,struct stGETCAMERANAME)	//機種名取得要求

#define CO10_IOC_MAXNR			18
//コマンドに必要なパラメータ構造体

#endif
