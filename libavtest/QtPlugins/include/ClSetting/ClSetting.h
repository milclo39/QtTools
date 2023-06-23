//***************************************************************************//
//***************************************************************************//
/*!
	@file		ClSetting.h

	@brief		設定

	@author		株式会社エルモ社  映像技術第二部

	Confidential

	Copyright(c) Elmo Industrial Co.,Ltd.		http://www.elmo.co.jp/
*/
//***************************************************************************//
//***************************************************************************//
#ifndef CLSETTING_H
#define CLSETTING_H


#include	<QSettings>
#include	<QStringList>


//***************************************************************************//
/*!
	@class	ClSetting
	@brief	設定書き込み・読み出しクラス
*/
//***************************************************************************//
class ClSetting
{
	////////////////////////////////////////////////////////////////////
	//	生成・破棄
	////////////////////////////////////////////////////////////////////
public:
	ClSetting( void );				//	コンストラクタ
	virtual ~ClSetting( void );		//	デストラクタ


	////////////////////////////////////////////////////////////////////
	//	インターフェース
	////////////////////////////////////////////////////////////////////
public:
	virtual void		setValue(QString strKey, QVariant clValue){setValue(strKey, clValue, m_eScope);}	//設定値書き込み
	virtual void		setValue( QString  strKey, QVariant  clValue,
									QSettings::Scope  eScope );		//	設定値書き込み
	virtual QVariant	getValue(QString strKey, QVariant clDefValue = QVariant()){return getValue(strKey, m_eScope, clDefValue);}	//設定値読み出し
	virtual QVariant	getValue( QString  strKey,
								  QSettings::Scope  eScope,
									QVariant  clDefValue = QVariant());		//	設定値読み出し
	virtual QVariant	getSetValue(QString strKey, QVariant clDefValue = QVariant()){return getSetValue(strKey, m_eScope, clDefValue);}	//設定値読み出し
	virtual QVariant	getSetValue( QString  strKey,
									 QSettings::Scope  eScope,
									QVariant  clDefValue = QVariant());		//	設定値を読み出し、なければ書き込む
	virtual QStringList getChildGroupList(QString strKey){return getChildGroupList(strKey, m_eScope);}	//すべての子グループを取得
	virtual QStringList	getChildGroupList( QString  strKey,
										QSettings::Scope  eScope);			//	すべての子供グループを取得
	virtual QStringList getChildKeyList(QString strKey){return getChildKeyList(strKey, m_eScope);}	//すべての子キー取得
	virtual QStringList	getChildKeyList( QString  strKey,
										QSettings::Scope  eScope);			//	すべての子供キーを取得
	virtual void		setFormat( QSettings::Format  eFormat );			//	フォーマット設定
	virtual void		setScope( QSettings::Scope  eScope );				//	スコープ設定

	virtual void		remove(QString strKey){remove(strKey, m_eScope);}	//削除
	virtual void		remove(QString strKey, QSettings::Scope eScope);	//削除

	////////////////////////////////////////////////////////////////////
	//	メンバー変数
	////////////////////////////////////////////////////////////////////
protected:
	QSettings::Format	m_eFormat;				//	設定値フォーマット
	QSettings::Scope	m_eScope;				//	設定値スコープ
};

//--------------------------------------------------------------------------------//
/*!
	@brief	ClSetting設定クラス
*/
//--------------------------------------------------------------------------------//
class ClSettingSetting
{
private:
	ClSettingSetting();		//コンストラクタ

public:
	static ClSettingSetting *getInstance();	//インスタンス取得

	/*!
		@brief		ルートパス設定
		@param[in]	strOrg : ルートパス
	*/
	void setOrgName(QString strOrg){m_strOrg = strOrg;}
	/*!
		@brief		ルートパス取得
		@retval		ルートパス
	*/
	QString getOrgName(){return m_strOrg;}
	/*!
		@brief		アプリパス設定
		@param[in]	strApp : アプリパス
	*/
	void setAppName(QString strApp){m_strApp = strApp;}	//アプリパス設定
	/*!
		@brief		アプリパス取得
		@retval		アプリパス
	*/
	QString getAppName(){return m_strApp;}
	/*!
		@brief		レジストリファイル設定
		@param[in]	strPath : レジストリファイルパス
	*/
	void setPfile(QString strPath, bool bIsAbsolute = false);	//レジストリファイル設定
	/*!
		@brief		レジストリファイルパス取得
		@retval		レジストリファイルパス
	*/
	QString getPfile(){return m_strPfile;}

private:
	QString m_strOrg;	//!< ルートパス
	QString m_strApp;	//!< アプリパス設定
	QString m_strPfile;	//!< レジストリファイル設定
};

#endif // CLSETTING_H
