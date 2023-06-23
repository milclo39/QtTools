//--------------------------------------------------------------------------------//
/*!
	@file	ClModelVersionCtrl.h
	@brief	QBiC系モデル&バージョン管理
	@author	星島
	@author 熊崎
*/
//--------------------------------------------------------------------------------//
#ifndef CLMODELVERSIONCTRL_H
#define CLMODELVERSIONCTRL_H

#include <QObject>
#include <QUuid>
#include <QList>




//--------------------------------------------------------------------------------//
/*!
	@brief		バージョン管理クラス
*/
//--------------------------------------------------------------------------------//
class ClModelVersionCtrl
{
public:
	ClModelVersionCtrl();											// コンストラクタ
	static QList<QUuid> getList(qint32 lType, QString strVersion);	// リスト取得
};

//--------------------------------------------------------------------------------//
/*!
	@brief		List
*/
//--------------------------------------------------------------------------------//
class AbstListGen
{
protected:

	/*!
		@brief Neo version コマンド実装/未実装
	*/
	typedef struct{
		bool bIsNeoV01;		//!< version 1.03.023~
		bool bIsNeoV02;		//!< version 1.03.025~
	}NeoVer;

	/*!
		@brief S version コマンド実装/未実装
	*/
	typedef struct{
		bool bIsSecV01;		//!< version 1.00.011~
		bool bIsSecV02;		//!< version 1.00.07~
		bool bIsSecV03;		//!< version 1.00.08~
		bool bIsSecV04;		//!< version 1.00.09~
	}SecVer;

	/*!
		@brief D version コマンド実装/未実装
	*/
	typedef struct{
		bool bIsDV01;		//!< version 0.00.001~(仮
	}DVer;

	/*!
		@brief コマンド実装/未実装テーブル要素　構造体
	*/
	typedef struct{
		QUuid	quid;		//!< コマンドUUID
		NeoVer	stNeo;		//!< Neoのコマンド実装を表す構造体
		SecVer	stSec;		//!< Sの		;
		DVer	stDoc;		//!< Dの		;
	}stList;

	typedef qint32 CmdVersion;	//!< コマンドバージョン

public:
	AbstListGen();											// コンストラクタ
	virtual QList<QUuid> genList( QString strVersion ) = 0;	//!< リスト取得

protected:

	virtual bool	isContain( QString strFwVersion, CmdVersion clCmdVersion );	// バージョン比較関数
	virtual QString getCompliantVer( CmdVersion clCmdVersion ) = 0;				//!< 対応バージョン取得

protected:

	static const stList stIDList[];		//!< UUID対応リスト
	static const qint32	m_lListSize;	//!< UUID対応リストの要素数
};

//--------------------------------------------------------------------------------//
/*!
	@brief	 Neo List
*/
//--------------------------------------------------------------------------------//
class ClNeoListGen : public AbstListGen
{
public:
	ClNeoListGen();											// コンストラクタ
	virtual QList<QUuid> genList( QString strVersion );		// リスト取得

protected:

	virtual QString getCompliantVer( CmdVersion clCmdVersion );		// 対応バージョン取得

protected:

	static const QString m_strCompliantVerTbl[];	//!< バージョン対応テーブル
};

//--------------------------------------------------------------------------------//
/*!
	@brief	 Sec List
*/
//--------------------------------------------------------------------------------//
class ClSecListGen : public AbstListGen
{
public:
	ClSecListGen();											// コンストラクタ
	virtual QList<QUuid> genList( QString strVersion );		// リスト生成

protected:

	virtual QString getCompliantVer( CmdVersion clCmdVersion );		// 対応バージョン取得

protected:

	static const QString m_strCompliantVerTbl[];	//!< バージョン対応テーブル

};

//--------------------------------------------------------------------------------//
/*!
	@brief	 Doc List
*/
//--------------------------------------------------------------------------------//
class ClDocListGen : public AbstListGen
{
public:
	ClDocListGen();											// コンストラクタ
	virtual QList<QUuid> genList( QString strVersion );		// リスト生成

protected:

	virtual QString getCompliantVer( CmdVersion clCmdVersion );		// 対応バージョン取得

protected:

	static const QString m_strCompliantVerTbl[];	//!< バージョン対応テーブル

};
#endif // CLMODELVERSIONCTRL_H
