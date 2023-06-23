//--------------------------------------------------------------------------------//
/*!
	@file		UpdateCmnDefine.h

	@brief		アップデートで共有する定義

	@author		松原
*/
//--------------------------------------------------------------------------------//
#ifndef UPDATECMNDEFINE_H
#define UPDATECMNDEFINE_H

#include <QtCore>
#ifdef Q_OS_WIN
#include <Windows.h>
#include <WinBase.h>
#endif
//--------------------------------------------------------------------------------//
/*!
	@brief	network.xmlのデフォルト設定値
*/
//--------------------------------------------------------------------------------//
#define	NETWORKXML_DEFAULT_PROXY_USE		(false)									//プロキシ設定の有無
#define	NETWORKXML_DEFAULT_PROXY_HOST		("")									//ホスト
#define	NETWORKXML_DEFAULT_PROXY_PORT		("3128")								//ポート番号
#define	NETWORKXML_DEFAULT_PROXY_TYPE		("Http")								//プロキシタイプ

#define	NETWORKXML_DEFAULT_AUTH_USE			(false)									//認証の有無
#define	NETWORKXML_DEFAULT_AUTH_USERNAME	("")									//認証のユーザー名
#define	NETWORKXML_DEFAULT_AUTH_PASSWORD	("")									//認証のパスワード

#define	NETWORKXML_DEFAULT_URL				\
("https://elmoupdatecenter.blob.core.windows.net/")									//参照先URL

#define	NETWORKXML_DEFAULT_AUTO_UPDATE_USE	(false)									//自動アップデートの有無


//--------------------------------------------------------------------------------//
/*!
	@brief	xmlファイル作成に使用する情報定義
*/
//--------------------------------------------------------------------------------//
#define	NETWORKXML_FOLDER_NAME				("ELMO")								//network.xmlの保存フォルダ名
#define NETWPRKXML_NAME						("network.xml")							//xmlファイル名

//--------------------------------------------------------------------------------//
/*!
	@brief	UpdateVersionCheckToolに渡す引数定義
*/
//--------------------------------------------------------------------------------//
#define UPDATE_VERCHECK_TOOL_NAME			("UpdateVersionCheckTool.exe")			//アップデートツール ファイル名

#define CONNECT_URL_ARCHITECTURE_X86		("86")									//network.xml生成用アーキテクチャ
#define CONNECT_URL_ARCHITECTURE_X64		("64")									//network.xml生成用アーキテクチャ

#define CONNECT_URL_IM						("imagemate3/update.xml")				//参照先URL生成に使用(IM3)
#define CONNECT_URL_IMA4SRS					("imaforsrs/update.xml")				//参照先URL生成に使用(IMA4SRS)
#define CONNECT_URL_IMA2wSRS				("ima2wsrs/update.xml")                 //参照先URL生成に使用(IMA2wSRS　※IMA4SRS改)
#define CONNECT_URL_IMA2					("ima2/update.xml")						//参照先URL生成に使用(IMA2 ※IMA4SRSのSRS削除版)
#define CONNECT_URL_EIT						("eit/update.xml")						//参照先URL生成に使用(EIT)

#define APPNAME_IM							("Image Mate")							//アプリケーション名(IM用)
#define APPNAME_IMA4SRS						("Image Mate Accent for SRS")			//アプリケーション名(IMA4SRS)
#define APPNAME_IMA2wSRS					("Image Mate Accent 2")					//アプリケーション名(IMA2wSRS　※IMA4SRS改)
#define APPNAME_IMA2						("Image Mate Accent 2")					//アプリケーション名(IMA2用 ※IMA4SRSのSRS削除版)
#define APPNAME_EIT							("ELMO Interactive Toolbox")			//アプリケーション名(EIT用)

#define CMDLINE_AUTORUN						"-a"									//!< 自動アップデート設定
#define CMDLINE_URL							"-u"									//!< URL設定(ネットワーク設定XMLのURLへの追加パス)
#define CMDLINE_NETWARK_FILE				"-nf"									//!< ネットワーク設定ファイル設定
#define CMDLINE_UPDATE_MAIN					"-app"									//!< アップデート対象メインアプリ名
#endif	//UPDATECMNDEFINE_H
