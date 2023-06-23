#ifndef CMNDEFINE_H
#define CMNDEFINE_H
//**************************************************************************//
/*!
	@file		CmnDefine.h

	@brief		全体で共有する定義

	@author		株式会社エルモ社　ソフトウェア技術１部

	Confidential

	Copyright(c) Elmo Industrial Co.,Ltd.		http://www.elmo.co.jp/
*/
//**************************************************************************//

//**************************************************************************//
/* !
 *	@brief		Qtで標準的に必要なヘッダ定義
 *	@attention	QGuiに依存するIncludeをしないこと
*/
//**************************************************************************//
#include <QtGlobal>
#include <QObject>
#include <QString>
#include <QSettings>
#include <QTranslator>
#include <QUuid>
#include <QDebug>
#include <QSysInfo>
#include <QMap>
#include <QVariant>


//QT -= guiの時に下のincludeが出来ないのでコメントアウト
//#include <QApplication>
//#include <QMessageBox>

//#if defined(Q_OS_LINUX) || defined(Q_OS_MACX)
#if defined(Q_OS_LINUX) || defined(Q_OS_MACX) || defined(Q_OS_IOS)

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
//#if defined(Q_OS_LINUX) || defined(Q_OS_MACX)
#if defined(Q_OS_LINUX) || defined(Q_OS_MACX) || defined(Q_OS_IOS)
#define chSTR2(x)		#x
#define chSTR(x)		chSTR2(x)
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
//#if defined(Q_OS_LINUX) || defined(Q_OS_MACX)
#if defined(Q_OS_LINUX) || defined(Q_OS_MACX) || defined(Q_OS_IOS)
#define DBGFL(fmt, desc...)
#elif defined(Q_OS_WIN)
inline void DBGFL(const char *, ...){}
#endif
#define DBG(desc)
#define MEASU_BEGIN
#define MEASU_END
#endif

//--------------------------------------------------------------------
/*
	Qtバージョン切り分け用マクロ
*/
//--------------------------------------------------------------------
#define QT_VERSION_4 ((QT_VERSION & 0xff0000) == 0x040000)
#define QT_VERSION_5 ((QT_VERSION & 0xff0000) == 0x050000)

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

#define chSIZEOFSTRING(psz) ((lstrlen(psz) + 1) * sizeof(TCHAR))


namespace EL_MSG{
//--------------------------------------------------------------------
/* !
 *	@brief	EL_CMD
 *			メッセージ通信に使用するクラス
 */
//--------------------------------------------------------------------

typedef		QMap<QString, QVariant>			EL_CMD;
typedef		QMapIterator<QString, QVariant>	EL_CMD_ITE;

#define		EL_CMD_KEY_CMD		"CMD"		//!<	コマンドIDキー
#define		EL_CMD_KEY_ERR		"ERROR"		//!<	エラーキー
#define		EL_CMD_KEY_PARAM1	"PARAM1"	//!<	パラメータ1キー
#define		EL_CMD_KEY_PARAM2	"PARAM2"	//!<	パラメータ2キー
#define		EL_CMD_KEY_PARAM3	"PARAM3"	//!<	パラメータ3キー
#define		EL_CMD_KEY_PARAM4	"PARAM4"	//!<	パラメータ4キー
#define		EL_CMD_KEY_PARAM5	"PARAM5"	//!<	パラメータ5キー
#define		EL_CMD_KEY_PARAM6	"PARAM6"	//!<	パラメータ6キー
#define		EL_CMD_KEY_PARAM7	"PARAM7"	//!<	パラメータ7キー
#define		EL_CMD_KEY_PARAM8	"PARAM8"	//!<	パラメータ8キー


//--------------------------------------------------------------------
/* !
 *	@brief	コマンドエラー定義
 *			ここでは一般的なコマンドの定義を行う
 *			細かいエラー定義は各プロジェクトで行うこと
 */
//--------------------------------------------------------------------
typedef enum {
	CMD_ERR_SUCCESS		= 0,				//!< コマンド成功
	CMD_ERR_FAILURE		= -1,				//!< コマンド失敗
	CMD_ERR_FATAL		= -2,				//!< 致命的な失敗
	CMD_ERR_MEMORY		= -3,				//!< メモリ
	CMD_ERR_PARAM		= -4,				//!< パラメータ
	CMD_ERR_TRANSACTION	= -5,				//!< トランザクションの失敗
	CMD_ERR_COMMIT		= -6,				//!< コミットの失敗
	CMD_ERR_BUSY		= -7,				//!< 急がしい
	CMD_ERR_STATE		= -8,				//!< 状態ずれ
	CMD_ERR_TIMEOUT		= -9				//!< タイムアウト
} eCMD_ERR;

//--------------------------------------------------------------------
/* !
 *	@brief	EL_GET_CMD
 *			メッセージクラスからCMDを取得するマクロ
 *	@param[in]	cmd		EL_CMDクラス
 */
//--------------------------------------------------------------------

#define		EL_GET_CMD(cmd)			(cmd).value(EL_CMD_KEY_CMD).value<QUuid>()

//--------------------------------------------------------------------
/* !
 *	@brief	EL_SET_CMD_VAL
 *			メッセージクラスに値を設定するマクロ
 *	@param[in]	cmd		EL_CMDクラス
 *	@param[in]	key		キー
 *	@param[in]	val		設定する値
 */
//--------------------------------------------------------------------

#define		EL_SET_CMD_VAL(cmd,key,val)		{QVariant clVal;\
											clVal.setValue(val);\
											(cmd).insert((key),clVal);}

//--------------------------------------------------------------------
/* !
 *	@brief	EL_GET_CMD_VAL
 *			メッセージクラスから値をQVariantで取得するマクロ
 *	@param[in]	cmd		EL_CMDクラス
 *	@param[in]	key		キー
 */
//--------------------------------------------------------------------

#define		EL_GET_CMD_VAL(cmd,key)			(cmd).value((key))

//--------------------------------------------------------------------
/* !
 *	@brief	EL_GET_CMD_VAL2
 *			メッセージクラスから値を指定した型で取得するマクロ
 *	@param[in]	cmd		EL_CMDクラス
 *	@param[in]	key		キー
 *	@param[in]	type	取得する型
 */
//--------------------------------------------------------------------

#define		EL_GET_CMD_VAL2(cmd,key,type)	EL_GET_CMD_VAL(cmd,key).value<type>()

//--------------------------------------------------------------------
/* !
 *	@brief	EL_HAS_CMD_VAL
 *			メッセージクラスが対応するキーの値を持っているかを確認するマクロ
 *	@param[in]	cmd		EL_CMDクラス
 *	@param[in]	key		キー
 */
//--------------------------------------------------------------------

#define		EL_HAS_CMD_VAL(cmd,key)			(cmd).contains((key))

//--------------------------------------------------------------------
/* !
 *	@brief	EL_CONNECT_CMD
 *			メッセージコネクト用マクロ
 */
//--------------------------------------------------------------------
#define		EL_CONNECT_CMD(src, dst) \
	QObject::connect(src,SIGNAL(sigSendCmd(EL_CMD)),dst,SLOT(slotRecvCmd(EL_CMD)))

//--------------------------------------------------------------------
/* !
 *	@brief	EL_CONNECT_CMD2
 *			メッセージコネクト用マクロ
 */
//--------------------------------------------------------------------
#define		EL_CONNECT_CMD2(src, dst, opt) \
	QObject::connect(src,SIGNAL(sigSendCmd(EL_CMD)),dst,SLOT(slotRecvCmd(EL_CMD)),opt)

//--------------------------------------------------------------------
/* !
 *	@brief	EL_DISCONNECT_CMD
 *			メッセージディスコネクト用マクロ
 */
//--------------------------------------------------------------------
#define		EL_DISCONNECT_CMD(src, dst) \
	QObject::disconnect(src,SIGNAL(sigSendCmd(EL_CMD)),dst,SLOT(slotRecvCmd(EL_CMD)))
}

using namespace EL_MSG;
Q_DECLARE_METATYPE(EL_CMD)
Q_DECLARE_METATYPE(QUuid)
Q_DECLARE_METATYPE(eCMD_ERR)

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
