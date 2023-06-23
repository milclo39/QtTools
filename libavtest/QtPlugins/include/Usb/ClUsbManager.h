//--------------------------------------------------------------------------------//
/*!
	@file	ClUsbManager.h
	@brief	USB通信クラスヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLUSBMANAGER_H
#define CLUSBMANAGER_H
#include "qglobal.h"
#include "Command.h"

#ifdef Q_OS_WIN
#include "ClUsbManagerWin.h"
#else
#include "ClUsbManagerMac.h"
#endif

#endif	//CLUSBMANAGER_H
