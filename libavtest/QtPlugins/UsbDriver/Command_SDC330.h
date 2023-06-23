// L-1ex 用 コントロールコマンド
//
//
#ifndef _COMMAND_SDC330_H
#define _COMMAND_SDC330_H

#include "Winsock2.h"
#include "ws2def.h"

// 機種情報

#define PRESENTER_SDC330			(0x0007)			//Digital Visual Presenter SDC330

/*------------------------------------------------------------------------------*/
/*!
 * @enum	emPictureSize_SDC330
 * @brief	Picture取得で画像サイズが指定できる機種で使用
 */
/*------------------------------------------------------------------------------*/
enum emPictureSize_SDC330{
	SDC330_SIZE_HIGH = 1,									//サイズ大
	SDC330_SIZE_MID,										//サイズ中
	SDC330_SIZE_LOW,										//サイズ小
};

//iIoctlで使用する構造体
/*------------------------------------------------------------------------------*/
/*!
 * @struct	stGETPICTURE_SDC330
 * @brief	Picture取得で使用する構造体
 */
/*------------------------------------------------------------------------------*/
struct stGETPICTURE_SDC330{
	unsigned int uiSize;							//Jpegサイズ
	unsigned char *pucJpegData;						//Jpegデータ
	emPictureSize_SDC330 eKind;						//画像種別
};

//機種別コマンド

#define SDC330_IOC_BASE			'C'
#define SDC330_GET_PICTURE		_IOR(SDC330_IOC_BASE,1,struct stGETPICTURE_SDC330)	//Picture取得
#define SDC330_PICTURE_RELEASE	_IOW(SDC330_IOC_BASE,2,struct stGETPICTURE_SDC330)	//Picture取得で確保した領域の解放
#define SDC330_PICTURE_RECT		_IOR(SDC330_IOC_BASE,3,struct RECT)				//Pictureの縦横長さ取得

#define SDC330_ZOOM_WIDE		_IO(SDC330_IOC_BASE,4)							//カメラへのズーム操作（WIDE側）
#define SDC330_ZOOM_TELE		_IO(SDC330_IOC_BASE,5)							//カメラへのズーム操作（TELE側）
#define SDC330_ZOOM_STOP		_IO(SDC330_IOC_BASE,6)							//カメラへのズーム操作（STOP）

#define SDC330_IRIS_OPEN		_IO(SDC330_IOC_BASE,7)							//カメラへのIRIS操作（OPEN側）
#define SDC330_IRIS_CLOSE		_IO(SDC330_IOC_BASE,8)							//カメラへのIRIS操作（CLOSE側）
#define SDC330_IRIS_NORMAL		_IO(SDC330_IOC_BASE,9)							//カメラへのIRIS操作（NORMAL）
#define SDC330_IRIS_STOP		_IO(SDC330_IOC_BASE,10)							//カメラへのIRIS操作（STOP）

#define SDC330_FOCUS_FAR		_IO(SDC330_IOC_BASE,11)							//カメラへのFOCUS操作（FAR側）
#define SDC330_FOCUS_NEAR		_IO(SDC330_IOC_BASE,12)							//カメラへのFOCUS操作（NEAR側）
#define SDC330_FOCUS_STOP		_IO(SDC330_IOC_BASE,13)							//カメラへのFOCUS操作（STOP側）
#define SDC330_FOCUS_AF			_IO(SDC330_IOC_BASE,14)							//カメラへのFOCUS操作（AutoFocus）

#define SDC330_G_CAMERA_NAME	_IOR(SDC330_IOC_BASE,15,struct stGETCAMERANAME)	//機種名取得要求

#define SDC330_CAPTURE_INIT		_IO(SDC330_IOC_BASE,16)							//Capture Initialize
#define SDC330_CAPTURE_STATUS	_IOR(SDC330_IOC_BASE,17,char *)					//Capture Status
#define SDC330_ROTATION			_IOW(SDC330_IOC_BASE,18,char)						//Image Rotation
#define SDC330_POSI_NEGA		_IOW(SDC330_IOC_BASE,19,char)						//Posi Nega
#define SDC330_COLOR_BW			_IOW(SDC330_IOC_BASE,20,char)						//Color BW
#define SDC330_SLOW_SHUTTER		_IOW(SDC330_IOC_BASE,21,char)						//Slow Shutter
#define SDC330_IMAGE_MODE		_IOW(SDC330_IOC_BASE,22,char)						//Image Mode
#define SDC330_MICROSCOPE_MODE	_IOW(SDC330_IOC_BASE,23,char)						//MicroScope Mode


#define SDC330_IOC_MAXNR			24
//コマンドに必要なパラメータ構造体

#endif
