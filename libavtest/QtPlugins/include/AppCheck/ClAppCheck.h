//***************************************************************************//
//***************************************************************************//
/*!
	@file		ClAppCheck.h
	@author		(S技1)高井
	@author		大橋
	@brief		アプリケーションの多重起動チェックを行うクラスの定義
*/
//***************************************************************************//
//***************************************************************************//
#ifndef CLAPPCHECK_H
#define CLAPPCHECK_H

#include	<QObject>
#include	<QString>

#ifdef Q_OS_WIN
#include	<windows.h>
#else
#include	<QSharedMemory>
#endif

//--------------------------------------------------//
/*!
	@brief		アプリケーションの多重起動チェックを行うクラス
*/
//--------------------------------------------------//
class ClAppCheck : public QObject
{
Q_OBJECT

	/*!
		@brief		チェック結果
	*/
public:
	enum emEXIST {
		DOIEXIST_NOTEXIST = 0,
		DOIEXIST_EXIST_SELF = 1,
		DOIEXIST_EXIST_OTHER = 2,
		DOIEXIST_ERROR = 3
	};

	//-------------------------------------------
	//	生成、初期化
	//-------------------------------------------
public:
	ClAppCheck( QString strApplication = "", QObject  *pclParent = NULL );	//	引数つきコンストラクタ
	virtual ~ClAppCheck( void );				//	デストラクタ

	//-------------------------------------------
	//	インターフェース
	//-------------------------------------------
public:
	virtual emEXIST		doIExist( QString strMutexName );		//	多重起動チェック
	virtual emEXIST		doIExistVHM( void );		//	VHM用多重起動チェック

	QString getErrorStringDef(emEXIST eValue);	//エラー文字列取得

	//-------------------------------------------
	//	メンバー変数
	//-------------------------------------------
protected:
	QString m_strApplication;	//!< エラー文字列に使用するアプリ名
#ifdef Q_OS_WIN
	HANDLE		m_hHandle;		//!<	VHM用ミューテックスハンドル
#else
	QSharedMemory *m_pclMemory;	//!< 共有メモリ
#endif
};

#endif // CLAPPCHECK_H
