// P10ex 用 コントロールコマンド
//
//
#ifndef _COMMAND_P10EX_H
#define _COMMAND_P10EX_H

#include "Winsock2.h"
#include "ws2def.h"

// 機種情報

#define PRESENTER_P10EX			(0x0006)			//Digital Visual Presenter P10EX

/*------------------------------------------------------------------------------*/
/*!
 * @enum	emPictureSize_P10EX
 * @brief	Picture取得で画像サイズが指定できる機種で使用
 */
/*------------------------------------------------------------------------------*/
enum emPictureSize_P10EX{
	SIZE_HIGH_P10EX = 1,								//サイズ大
	SIZE_MID_P10EX,										//サイズ中
	SIZE_LOW_P10EX,										//サイズ小
};

//iIoctlで使用する構造体
/*------------------------------------------------------------------------------*/
/*!
 * @struct	stGETPICTURE_P10EX
 * @brief	Picture取得で使用する構造体
 */
/*------------------------------------------------------------------------------*/
struct stGETPICTURE_P10EX{
	unsigned int uiSize;							//Jpegサイズ
	unsigned char *pucJpegData;						//Jpegデータ
	emPictureSize_P10EX eKind;							//画像種別
};

//機種別コマンド

#define P10EX_IOC_BASE			'1'
#define P10EX_GET_PICTURE		_IOR(P10EX_IOC_BASE,1,struct stGETPICTURE_P10EX)//Picture取得
#define P10EX_PICTURE_RELEASE	_IOW(P10EX_IOC_BASE,2,struct stGETPICTURE_P10EX)//Picture取得で確保した領域の解放
#define P10EX_PICTURE_RECT		_IOR(P10EX_IOC_BASE,3,struct RECT)				//Pictureの縦横長さ取得

#define P10EX_ZOOM_WIDE			_IO(P10EX_IOC_BASE,4)							//カメラへのズーム操作（WIDE側）
#define P10EX_ZOOM_TELE			_IO(P10EX_IOC_BASE,5)							//カメラへのズーム操作（TELE側）
#define P10EX_ZOOM_STOP			_IO(P10EX_IOC_BASE,6)							//カメラへのズーム操作（STOP）

#define P10EX_IRIS_OPEN			_IO(P10EX_IOC_BASE,7)							//カメラへのIRIS操作（OPEN側）
#define P10EX_IRIS_CLOSE		_IO(P10EX_IOC_BASE,8)							//カメラへのIRIS操作（CLOSE側）
#define P10EX_IRIS_NORMAL		_IO(P10EX_IOC_BASE,9)							//カメラへのIRIS操作（NORMAL）
#define P10EX_IRIS_STOP			_IO(P10EX_IOC_BASE,10)							//カメラへのIRIS操作（STOP）

#define P10EX_FOCUS_FAR			_IO(P10EX_IOC_BASE,11)							//カメラへのFOCUS操作（FAR側）
#define P10EX_FOCUS_NEAR		_IO(P10EX_IOC_BASE,12)							//カメラへのFOCUS操作（NEAR側）
#define P10EX_FOCUS_STOP		_IO(P10EX_IOC_BASE,13)							//カメラへのFOCUS操作（STOP側）
#define P10EX_FOCUS_AF			_IO(P10EX_IOC_BASE,14)							//カメラへのFOCUS操作（AutoFocus）


#define P10EX_G_CAMERA_NAME		_IOR(P10EX_IOC_BASE,15,struct stGETCAMERANAME)	//機種名取得要求

#define P10EX_IOC_MAXNR			16
//コマンドに必要なパラメータ構造体

#endif
