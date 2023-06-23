#ifndef CMNDEFINE_H
#define CMNDEFINE_H
//**************************************************************************//
/*!
	@file		CmnDefine.h

	@brief		Image Mate 全体で共有する定義

	@author		株式会社エルモ社　映像技術２部

	Confidential

	Copyright(c) Elmo Industrial Co.,Ltd.		http://www.elmo.co.jp/
*/
//**************************************************************************//

//**************************************************************************//
/* !
 *	@brief		Qtで標準的に必要なヘッダ定義
*/
//**************************************************************************//
#include <QtGlobal>
#include <QApplication>
#include <QObject>
#include <QString>
#include <QSettings>
#include <QTranslator>
#include <QUuid>
#include <QDebug>
#include <QMessageBox>
#include <QSysInfo>

#include "qtdxuttimer.h"

#if defined(Q_OS_LINUX) || defined(Q_OS_MACX)

#elif defined(Q_OS_WIN)
#include <windows.h>
#include <stdarg.h>

#ifdef TRUE
#undef TRUE
#define TRUE	(1)
#endif
#ifdef FALSE
#undef FALSE
#define FALSE	(0)
#endif
#endif

//--------------------------------------------------------------------
/* !
 *	@brief	DEBUG表示用プラグマメッセージヘルパーマクロ
 */
//--------------------------------------------------------------------

#ifdef __DEBUG
#if defined(Q_OS_LINUX) || defined(Q_OS_MACX)
#define chSTR2(x)       #x
#define chSTR(x)        chSTR2(x)
#define DBGFL(fmt, desc...)		qDebug("%s ( %s ):" fmt, __FILE__, chSTR(__LINE__), ##desc)
#elif defined(Q_OS_WIN)
inline void DBGFL(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	qDebug() << QString().vsprintf(fmt, ap);
	va_end(ap);
}
#endif

#define DBG(desc)				qDebug() << desc

#define MEASU_BEGIN \
{\
	 QtDXUTTimer clTimer;\
	 double time1 = clTimer.getAbsoluteTime();

#define MEASU_END \
	double time2 = clTimer.getAbsoluteTime();\
	qDebug() << time2 - time1;\
}

#else
#if defined(Q_OS_LINUX) || defined(Q_OS_MACX)
#define DBGFL(fmt, desc...)
#elif defined(Q_OS_WIN)
inline void DBGFL(const char *, ...){}
#endif
#define DBG(desc)
#define MEASU_BEGIN
#define MEASU_END
#endif

//--------------------------------------------------------------------
/* !
 *	@brief	CLEAR macro
 *			Macro for clearing structures
 */
//--------------------------------------------------------------------
#define CLEAR(x) memset (&(x), 0, sizeof (x))

//--------------------------------------------------------------------
/* !
 *	@brief	chINRANGE マクロ
 *			このマクロは数値が他の２つの数値の範囲内にある場合はTRUEを返す
 */
//--------------------------------------------------------------------

#define chINRANGE(low, Num, High) (((low) <= (Num)) && ((Num) <= (High)))

//--------------------------------------------------------------------
/* !
 *	@brief	chSIZEOFSTRING マクロ
 *			このマクロは文字列に必要なバイト数を評価する
 */
//--------------------------------------------------------------------

#define chSIZEOFSTRING(psz)   ((lstrlen(psz) + 1) * sizeof(TCHAR))
#if 0
//--------------------------------------------------------------------
/*!
 *	@brief	chROUNDDOWN & chROUNDUP インライン関数
 */
//--------------------------------------------------------------------
//! @brief	このインライン関数は最も近い倍数に切り下げる
template <class TV, class TM>
inline TV chROUNDDOWN(TV Value, TM Multiple) {
	return((Value / Multiple) * Multiple);
}

//! @brief	このインライン関数は最も近い倍数に切り上げる
template <class TV, class TM>
inline TV chROUNDUP(TV Value, TM Multiple) {
	return(chROUNDDOWN(Value, Multiple) + (((Value % Multiple) > 0) ? Multiple : 0));
}
#endif

#endif	//CMNDEFINE_H
