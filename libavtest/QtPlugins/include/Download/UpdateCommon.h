//--------------------------------------------------------------------------------//
/*!
	@file	UpdateCommon.h
	@brief	アップデートツール共有ヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef UPDATECOMMON_H
#define UPDATECOMMON_H

#include <QString>
#include <QByteArray>

#define MESSAGEBOX_TITLE		QObject::tr("ELMO Update service program")	//!< メッセージボックスタイトル
#define MESSAGE_IS_UPDATE		QObject::tr("New version found.\n you can update the version of your %1.\n Do you want to update now?")	//!< アップデート確認メッセージ
#define MESSAGE_NOTNEED_UPDATE	QObject::tr("Your %1 is the latest version.")	//!< アップデート不必要メッセージ
#define MESSAGE_NETWORK_ERROR	QObject::tr("Failed to obtain update files. Check the network settings on your computer.\n")	//!< 通信エラーメッセージ
#define MESSAGE_SUCCESS			QObject::tr("Your %1 was successfulley update.\n Your %1 is the latest version.")	//!< アップデート成功メッセージ
#define	MESSAGE_UPDATE_FAILED	QObject::tr("Faield to update.")	//!< アップデート失敗メッセージ
#define MESSAGE_PARSE_ERROR		QObject::tr("Failed to read update files.\n If this error continues to occur, consult your dealer.")	//!< XML解析エラーメッセージ
#define MESSAGE_UPDATE_CHECK	QObject::tr("Always check the latest version.")	//!< アップデート通知不要チェックボックスメッセージ
#define MESSAGE_PROCESS_STOP	QObject::tr("%1 is running.\n %1 needs to be closed to perform the update.\n Do you want to close %1 now?")	//!< プロセス終了確認メッセージ

#define MESSAGEBOX_BTN_OK		QObject::tr("OK")	//!< OKボタン
#define MESSAGEBOX_BTN_CANCEL	QObject::tr("Cancel")	//!< キャンセルボタン

#ifdef Q_OS_WIN
#define DOWNLOAD_TOOL_NAME		L"UpdateDownloadTool.exe"	//!< ダウンロードツール名
#endif

#define NETWORK_XML_FILE_NAME	"network.xml"			//!< ネットワーク設定の書かれたXML
#define MAIN_APPLICATION_DEF	"Application"			//!< メインのアップデート対象アプリ名(テキスト出力で使用)
#endif
