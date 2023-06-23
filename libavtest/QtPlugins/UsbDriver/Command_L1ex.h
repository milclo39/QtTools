// L-1ex 用 コントロールコマンド
//
//
#ifndef _COMMAND_L1EX_H
#define _COMMAND_L1EX_H

#include "Winsock2.h"
#include "ws2def.h"

// 機種情報

#define PRESENTER_L1EX			(0x0002)			//Digital Visual Presenter L1ex

/*------------------------------------------------------------------------------*/
/*!
 * @enum	emPictureSize
 * @brief	Picture取得で画像サイズが指定できる機種で使用
 */
/*------------------------------------------------------------------------------*/
enum emPictureSize{
	SIZE_HIGH = 1,									//サイズ大
	SIZE_MID,										//サイズ中
	SIZE_LOW,										//サイズ小
};

//iIoctlで使用する構造体
/*------------------------------------------------------------------------------*/
/*!
 * @struct	stGETPICTURE_L1EX
 * @brief	Picture取得で使用する構造体
 */
/*------------------------------------------------------------------------------*/
struct stGETPICTURE_L1EX{
	unsigned int uiSize;							//Jpegサイズ
	unsigned char *pucJpegData;						//Jpegデータ
	emPictureSize eKind;							//画像種別
};

//機種別コマンド

#define L1EX_IOC_BASE			'M'
#define L1EX_GET_PICTURE		_IOR(L1EX_IOC_BASE,1,struct stGETPICTURE_L1EX)	//Picture取得
#define L1EX_PICTURE_RELEASE	_IOW(L1EX_IOC_BASE,2,struct stGETPICTURE_L1EX)	//Picture取得で確保した領域の解放
#define L1EX_PICTURE_RECT		_IOR(L1EX_IOC_BASE,3,struct RECT)				//Pictureの縦横長さ取得

#define L1EX_ZOOM_WIDE			_IO(L1EX_IOC_BASE,4)							//カメラへのズーム操作（WIDE側）
#define L1EX_ZOOM_TELE			_IO(L1EX_IOC_BASE,5)							//カメラへのズーム操作（TELE側）
#define L1EX_ZOOM_STOP			_IO(L1EX_IOC_BASE,6)							//カメラへのズーム操作（STOP）

#define L1EX_IRIS_OPEN			_IO(L1EX_IOC_BASE,7)							//カメラへのIRIS操作（OPEN側）
#define L1EX_IRIS_CLOSE			_IO(L1EX_IOC_BASE,8)							//カメラへのIRIS操作（CLOSE側）
#define L1EX_IRIS_NORMAL		_IO(L1EX_IOC_BASE,9)							//カメラへのIRIS操作（NORMAL）
#define L1EX_IRIS_STOP			_IO(L1EX_IOC_BASE,10)							//カメラへのIRIS操作（STOP）

#define L1EX_FOCUS_FAR			_IO(L1EX_IOC_BASE,11)							//カメラへのFOCUS操作（FAR側）
#define L1EX_FOCUS_NEAR			_IO(L1EX_IOC_BASE,12)							//カメラへのFOCUS操作（NEAR側）
#define L1EX_FOCUS_STOP			_IO(L1EX_IOC_BASE,13)							//カメラへのFOCUS操作（STOP側）
#define L1EX_FOCUS_AF			_IO(L1EX_IOC_BASE,14)							//カメラへのFOCUS操作（AutoFocus）

#define L1EX_G_CAMERA_NAME		_IOR(L1EX_IOC_BASE,15,struct stGETCAMERANAME)	//機種名取得要求

#define L1EX_CAPTURE_INIT		_IO(L1EX_IOC_BASE,16)							//Capture Initialize
#define L1EX_CAPTURE_STATUS		_IOR(L1EX_IOC_BASE,17,char *)					//Capture Status
#define L1EX_ROTATION			_IOW(L1EX_IOC_BASE,18,char)						//Image Rotation
#define L1EX_POSI_NEGA			_IOW(L1EX_IOC_BASE,19,char)						//Posi Nega
#define L1EX_COLOR_BW			_IOW(L1EX_IOC_BASE,20,char)						//Color BW
#define L1EX_SLOW_SHUTTER		_IOW(L1EX_IOC_BASE,21,char)						//Slow Shutter
#define L1EX_IMAGE_MODE			_IOW(L1EX_IOC_BASE,22,char)						//Image Mode
#define L1EX_MICROSCOPE_MODE	_IOW(L1EX_IOC_BASE,23,char)						//MicroScope Mode


#define L1EX_IOC_MAXNR			24
//コマンドに必要なパラメータ構造体

#endif
