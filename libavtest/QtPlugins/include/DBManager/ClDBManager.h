//--------------------------------------------------------------------------------//
/*!
	@file	ClDBManager.h
	@brief	DataBase操作
	@author	星島
*/
//--------------------------------------------------------------------------------//
#ifndef CLDBMANAGER_H
#define CLDBMANAGER_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QMap>
#include <QList>
#include <QStringList>

typedef QMap< QString, QString >		ClFieldData;		//!< フィールド名(Key),値(Value)

typedef enum
{
	eNoLock = 0,
	eAccessShare,
	eRowShare,
	eRowExclusive,
	eShareUpdateExclusive,
	eShare,
	eShareRowExclusive,
	eExclusive,
	eAccessExcusive
}eLockMode;

//--------------------------------------------------------------------------------//
/*!
	@brief	DB
*/
//--------------------------------------------------------------------------------//
class ClDBManager : public QObject
{
	Q_OBJECT
public:
	explicit ClDBManager(QObject *parent = NULL);		//コンストラクタ

	/*!
		@brief		データベースオープン
		@param[in]	strDriverName	: ドライバー名
		@param[in]	strDBName		: DBName
		@param[in]	strHostName		: 接続先
		@param[in]	strPassword		: パスワード
		@param[in]	strPort			: ポート番号
		@retval		データベースオープン
	*/
	virtual bool open(QString strDriverName,
					  QString strDBName,
					  QString strHostName,
					  QString strUserName,
					  QString strPassword,
					  QString strPort);

	/*!
		@brief		データベースクローズ
	*/
	virtual void close();

	/*!
		@brief		データベースの開閉
		@retval		開閉状態(true:開,false:閉)
	*/
	virtual bool bIsOpenDatabase(){return m_clSqlDB.isOpen();}


	/*!
		@brief		トランザクション開始
		@param[in]	eMode:ロックモード
		@param[in]	strTbName:テーブル名
		@retval		成否(true:成,false:否)
	*/
	virtual bool startTransaction(eLockMode eMode, QString strTbName);

	/*!
		@brief		トランザクション終了
		@param[in]	bIsSuccess :
		@retval		成否(true:成,false:否)
	*/
	virtual bool endTransaction(bool bIsSuccess);


	/*!
		@brief		ロック開始
		@param[in]	eMode:ロックモード
		@param[in]	strTbName:テーブル名
		@retval		成否(true:成,false:否)
	*/
	virtual bool setLockMode(eLockMode eMode, QString strTbName);


	/*!
		@brief		テーブル名一覧取得
		@retval		テーブル名一覧
	*/
	virtual QStringList getTableList(){return m_clSqlDB.tables();}

	/*!
		@brief		フィールド名一覧取得
		@param[in]	strDBName	: DB名
		@param[in]	strTbName	:　Table名
		@retval		フィールド名一覧
	*/
	virtual QSqlQuery getFiledList(QString strDBName, QString strTbName);

	/*!
		@brief		レコードの追加
		@param[in]	strTable : テーブル名
		@param[in]	clFieldData : (フィールド名(Key),値(Value))
		@retval		成否
	*/
	virtual bool insertRecord(QString strTable, QList<ClFieldData> clFieldDataList);


	/*!
		@brief		特定条件のフィールドデータ取得
		@param[in]	strFieldList	: 取得したいフィールド名リスト
		@param[in]	strTable		: テーブル名
		@param[in]	clFieldData		: (探索先フィールド名(Key),探索値(Value))
		@retval		存在した場合レコードデータ取得、無ければ空？
	*/
	virtual QSqlQuery getConditionField(QStringList strFieldList, QString strTable,
										 ClFieldData clFieldData);

	/*!
		@brief		フィールドデータ取得
		@param[in]	strFieldList	: 取得したいフィールド名リスト
		@param[in]	strTable		: テーブル名
		@param[in]	strOrder		: 並び順(未指定可)
		@retval		存在した場合レコードデータ取得、無ければ空？
	*/
	virtual QSqlQuery getFieldData(QStringList strFieldList, QString strTable, QString strOrder = QString());

	/*!
		@brief		レコードの更新
		@param[in]	strTable		: テーブル名
		@param[in]	clFieldData		: 設定したい(フィールド名(Key),値(Value))
		@param[in]	clFieldInfo		: 追加条件(フィールド名(Key),値(Value))
		@retval		成否
	*/
	virtual bool updateRecord(QString strTable, ClFieldData clFieldData, ClFieldData clFieldInfo);


	/*!
		@brief		最後に起きたエラー取得
		@retval		エラー内容
	*/
	virtual QString getLastError();

	/*!
		@brief		最後に送ったSQL文
		@retval		SQL文
	*/
	virtual QString getLastSql();
private:
	QSqlQuery getQuery(QString strQuery);

private:
	QSqlDatabase	m_clSqlDB;
	QSqlQuery		m_clSqlQuery;

};
#endif // CLDBMANAGER_H
