//--------------------------------------------------------------------------------//
/*!
	@file	ClOwnUpdate.h
	@brief	自分アップデータヘッダ
	@author	大橋
*/
//--------------------------------------------------------------------------------//
#ifndef CLOWNUPDATE_H
#define CLOWNUPDATE_H
#include <QThread>
#include <QObject>
#include <QStringList>
#include <QMap>
#include <QWaitCondition>

//--------------------------------------------------------------------------------//
/*!
	@brief	自分アップデータ
*/
//--------------------------------------------------------------------------------//
class ClOwnUpdate : public QObject
{
public:
	ClOwnUpdate();	//コンストラクタ

	void exec(int argc, char **argv);	//処理実行
	virtual void exec(QStringList clArguments);	//処理実行

protected:
	virtual bool copy();	//コピー

protected:
	QMap<QString, QString> m_clFileInfoList;	//!< ファイル情報リスト(key:コピー元パス,value:コピー先パス)
	QWaitCondition m_clWait;	//!< スレッド終了待ち用クラス
};
//--------------------------------------------------------------------------------//
#endif //ABSTOWNUPDATE_H
