// 書画カメラ 用 共通コントロールコマンド
//
//
#ifndef _COMMAND_COMMON_H
#define _COMMAND_COMMON_H

#include "Winsock2.h"
#include "ws2def.h"

//iIoctlで使用する構造体
/*------------------------------------------------------------------------------*/
/*!
 * @struct	stGETCAMERANAME
 * @brief	カメラ名、IDを取得する構造体
 */
/*------------------------------------------------------------------------------*/
struct stGETCAMERANAME{
	unsigned short ModelID;							//[out]カメラID格納
	unsigned char pCamName[100];					//[out]カメラ名格納
};
/*------------------------------------------------------------------------------*/
/*!
 * @struct	stGETCAMERAVER
 * @brief	カメラバージョンを取得する構造体
 * @note	カメラにインデックスを与えるとインデックスに応じたカメラユニットの
 *			バージョンを得ることができる
 *			インデックスに対応するカメラユニットが無い場合、pCamInfoに[none]が返る
 *			機種によってバージョンを返すユニット数は様々なので、[none]が返るまで
 *			取得する必要がある
 */
/*------------------------------------------------------------------------------*/
struct stGETCAMERAVER{
	unsigned short sIndex;							//[in 1-*]インデックス
	unsigned char pCamInfo[100];					//[out]ユニット名格納
	unsigned char pCamVersion[100];					//[out]カメラ名格納
};
/*------------------------------------------------------------------------------*/
/*!
 * @struct	stGETDRVCTLVER
 * @brief	ドライバコントロールバージョンを取得する構造体
 * @note	現在のDLLのバージョンを返す
 */
/*------------------------------------------------------------------------------*/
struct stGETDRVCTLVER{
	unsigned long Major;
	unsigned long Minor;
	unsigned long Build;
};

//機種別コマンド

#define COMM_IOC_BASE			'K'

#define COMM_IS_DEVICE_OPEN		_IOR(COMM_IOC_BASE,1,unsigned long)				//カメラデバイスのオープン状態
#define COMM_S_RESET_USB		_IO(COMM_IOC_BASE,2)							//USBリセット要求
#define COMM_G_CAMERA_NAME		_IOR(COMM_IOC_BASE,3,struct stGETCAMERANAME)	//機種名取得要求
#define COMM_G_CAMERA_VER		_IOR(COMM_IOC_BASE,4,struct stGETCAMERAVER)		//バージョン取得要求
#define COMM_G_DRVCTL_VER		_IOR(COMM_IOC_BASE,5,struct stGETDRVCTLVER)		//ドライババージョン取得要求

#define COMM_IOC_MAXNR			6

#endif
