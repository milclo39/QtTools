/*------------------------------------------------------------------------------*/
/*!
	@file		IDeviceInfo.h

	@brief		デバイス固有情報取得用インターフェイス

	@author		株式会社エルモ社  映像技術第二部

	Confidential

	Copyright(c) Elmo Industrial Co.,Ltd.		http://www.elmo.co.jp/
*/
/*------------------------------------------------------------------------------*/
#pragma once

class IDeviceInfo
{
public:
	virtual ~IDeviceInfo(){}
	virtual BOOL isOpen() = 0;
	virtual DWORD getLastError() = 0;

};