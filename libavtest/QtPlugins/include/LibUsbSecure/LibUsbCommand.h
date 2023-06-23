//--------------------------------------------------------------------------------//
/*!
	@file	LibUsbCommand.h
	@brief	LIBUSBコマンド定義ヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef LIBUSBCOMMAND_H
#define LIBUSBCOMMAND_H
#include <QUuid>

//USBマネージャエラー値
#define LIBUSB_ERROR_SUCCESS		0		//成功
#define LIBUSB_ERROR_NODEVICE		-1000	//デバイスがない
#define LIBUSB_ERROR_BADPARAM		-1001	//パラメータエラー
#define LIBUSB_ERROR_BUSY			-1002	//デバイスビジー
#define LIBUSB_ERROR_TIMEOUT		-1003	//タイムアウト
#define LIBUSB_ERROR_RECEIVEDATA	-1004	//受信データ不正
#define LIBUSB_ERROR_UNKNOWN		-1005	//不明エラー
#define LIBUSB_ERROR_NOTINITIALIZED	-1006	//未初期化

//コマンドキー
#define EL_CMD_KEY_CONTROLLER	("Controller")	//操作クラス設定

//コマンド
//--------------------------------------------------------------------------------//
/*
	デバイスオープン
	EL_CMD_KEY_PARAM1	ClLibUsbController* : オープンしたデバイス操作クラスのポインタ
*/
//--------------------------------------------------------------------------------//
#define LIBUSB_CMD_DEVICE_OPEN	QUuid("{131dea99-f4a3-470a-9ab7-50ed83499efd}")

//--------------------------------------------------------------------------------//
/*
	デバイスクローズ
	EL_CMD_KEY_PARAM1	ClLibUsbController* : クローズしたデバイス操作クラスのポインタ
*/
//--------------------------------------------------------------------------------//
#define LIBUSB_CMD_DEVICE_CLOSE	QUuid("{d6ae8279-71b2-4246-bf75-be1d3981d820}")

//--------------------------------------------------------------------------------//
/*
	コマンドキャンセル
	EL_CMD_KEY_PARAM1	ClLibUsbController* : キャンセルしたいデバイス操作クラスのポインタ,NULL指定で全コマンドキャンセル
*/
//--------------------------------------------------------------------------------//
#define LIBUSB_CMD_COMMAND_CANCEL	QUuid("{36ad9d61-8471-4039-88b8-3ecce468384c}")
#endif //LIBUSBCOMMAND_H
