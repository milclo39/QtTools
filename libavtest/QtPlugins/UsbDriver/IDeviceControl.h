/*------------------------------------------------------------------------------*/
/*!
	@file		IDeviceControl.h

	@brief		デバイスを操作するためのインターフェイス

	@author		株式会社エルモ社  映像技術第二部

	Confidential

	Copyright(c) Elmo Industrial Co.,Ltd.		http://www.elmo.co.jp/
*/
/*------------------------------------------------------------------------------*/
#pragma once

/*------------------------------------------------------------------------------*/
/*!
 * @struct	stVERSION
 * @brief	ドライバコントロールバージョンを取得する構造体
 * @note	現在のDLLのバージョンを返す
 */
/*------------------------------------------------------------------------------*/
struct stVERSION{
	unsigned long Major;
	unsigned long Minor;
	unsigned long Build;
};
/*------------------------------------------------------------------------------*/
/*!
 * @class	IDeviceControl
 * @brief	デバイスを操作するためのインターフェイスクラス
 * @brief	クライアント側はこのインターフェイスで操作を行う
 */
/*------------------------------------------------------------------------------*/
class IElmoDevice;

class IDeviceControl
{
public:
	virtual ~IDeviceControl(){}
	virtual LONG lOpenDev(WORD ModelId) = 0;						//任意のデバイスを開いて待ちうけ状態にする
	virtual LONG lCloseDev(WORD ModelId) = 0;						//任意のデバイスを閉じる
	virtual LONG lOpenDevList() = 0;								//全デバイスを開いて待ちうけ状態にする
	virtual LONG lCheckDevList() = 0;								//全デバイスのOpen状態を再確認して、Openされているデバイスは待ちうけ状態にする
	virtual LONG lCloseDevList() = 0;								//全デバイスを閉じて待ちうけ状態を解除する
	virtual LONG lGetDevList(IElmoDevice ***list) = 0;				//全デバイスを列挙したリストを返す
	virtual LONG lReleaseDevList(IElmoDevice **list) = 0;			//デバイスを列挙したリストを解放する
	virtual void getVersion(stVERSION *stVersion) = 0;				//ドライバーのバージョンを取得する
	virtual BOOL isSupportDev(WORD ModelId) = 0;					//デバイスをサポートしているか調べる

	virtual DWORD getDeviceCount() = 0;								//現在のデバイス数を返す

	virtual IElmoDevice *getElmoDevice(WORD) = 0;					//デバイスIDを指定して任意のデバイスを取得する
	virtual IElmoDevice *getDeviceWithNumber(WORD wNumber) = 0;		//Numberを指定して任意のデバイスを取得する

};

//===========================================================================
// エクスポート関数の型の定義
//===========================================================================
typedef BOOL (WINAPI *CREATEDEVICECONTROLLER_FNC)(IDeviceControl **);
typedef void (WINAPI *RELEASEDEVICECONTROLLER_FNC)(IDeviceControl *);

//===========================================================================
// エクスポート関数の名前
//===========================================================================
#define	FNC_CREATEDEVICECONTROLLER	"CreateDeviceController"
#define	FNC_RELEASEDEVICECONTROLLER	"ReleaseDeviceController"

