//--------------------------------------------------------------------------------//
/*!
	@file	Command.h
	@brief	MassStorageコマンド
	@author	星島
*/
//--------------------------------------------------------------------------------//

#ifndef COMMAND_H
#define COMMAND_H

#include <QString>
#include <QUuid>

#define MASSSTORAGE_CMD_USBCMD	QUuid("{B6BF5A24-9313-4f4c-AE2E-BB1FE48BCB7F}")

//エラー値
#define USB_ERROR_SUCCESS		0		//成功

//--------------------------------------------------------------------------------//
/*!
	@brief	デバイスオープン通知
	@details
			EL_CMD_KEY_PARAM1	void* オープンしたデバイス操作クラスのポインタ
			EL_CMD_KEY_PARAM2	qint32 QBIC_VALUE_TYPE_* カメラ種別
			EL_CMD_KEY_PARAM3	quint16 カメラ種別取得エラー値
*/
//--------------------------------------------------------------------------------//
#define MASSSTORAGE_CMD_DEVICE_OPEN QUuid("{0E75B74E-7B61-4f5f-95D8-B8BF81A35136}")

//--------------------------------------------------------------------------------//
/*!
	@brief	デバイスクローズ通知
	@details
			EL_CMD_KEY_PARAM1	void* クローズしたデバイス操作クラスのポインタ
*/
//--------------------------------------------------------------------------------//
#define MASSSTORAGE_CMD_DEVICE_CLOSE QUuid("{5DD9208C-6FD6-4b70-98B1-F2FE2C4FD671}")


#define EL_CMD_KEY_METHOD		"METHOD"
#define EL_CMD_KEY_PARAMCOUNT	"PARAMCOUNT"
#define EL_CMD_KEY_RESULT		"RESULT"
#define EL_CMD_KEY_JSON			"JSON"


#endif // COMMAND_H

