/*--------------------------------------------------------------------------------*/
/*!
	@file	ClDiskSpaceGetThread.h
	@brief	一定時間ごとにディスク使用率等を監視するスレッド
 */
/*--------------------------------------------------------------------------------*/
#ifndef CLDISKSPACEGETTHREAD_H
#define CLDISKSPACEGETTHREAD_H


#include <QThread>				//継承元
#include "ClDiskSpaceManager.h"	//スレッド中にディスク使用容量とかを取得するため




/*================================================================================*/
/*--------------------------------------------------------------------------------*/
/*!
	@class	ClDiskSpaceGetThread
	@brief	一定時間ごとにディスク使用率等を監視するスレッド
 */
/*--------------------------------------------------------------------------------*/
class ClDiskSpaceGetThread : public QThread
{
	Q_OBJECT


	/*----------------------------------------*/
	/*!
		@property	targetDrivePath
		@brief		監視対象のドライブ名を含むパス
	*/
	/*----------------------------------------*/
	Q_PROPERTY(QString m_qstrTargetDrivePath READ targetDrivePath WRITE setTargetDrivePath)
private:
	QString	m_qstrTargetDrivePath;
public:
	QString	targetDrivePath(){ return m_qstrTargetDrivePath; }
	void	setTargetDrivePath(QString in_qstrTargetDrivePath){ m_qstrTargetDrivePath = in_qstrTargetDrivePath; }

	/*----------------------------------------*/
	/*!
		@property	checkInterval
		@brief		取得間隔、単位はミリ秒
	*/
	/*----------------------------------------*/
	Q_PROPERTY(unsigned long m_ulCheckInterval READ checkInterval WRITE setCheckInterval)
private:
	unsigned long	m_ulCheckInterval;
public:
	unsigned long	checkInterval(){ return m_ulCheckInterval; }
	void			setCheckInterval(unsigned long in_ulCheckInterval){ m_ulCheckInterval = in_ulCheckInterval; }

	/*----------------------------------------*/
	/*!
		@property	isEnd
		@brief		スレッド終了フラグ
		@note		run開始からexecに入る前にスレッド終了要求を受けた場合の対応用
	*/
	/*----------------------------------------*/
	Q_PROPERTY(bool m_bIsEnd READ isEnd WRITE setIsEnd)
private:
	bool	m_bIsEnd;
public:
	bool	isEnd(){ return m_bIsEnd; }
	void	setIsEnd(bool in_bIsEnd){ m_bIsEnd = in_bIsEnd; }


	/*----------------------------------------*/
	/*
		メンバー変数定義
	*/
	/*----------------------------------------*/
private:
	ClDiskSpaceManager	m_clDiskSpaceManager;	//!< HDD容量等取得クラス


	/*----------------------------------------*/
	/*
		メンバー関数定義
	*/
	/*----------------------------------------*/
public:
	ClDiskSpaceGetThread();				//コンストラクタ
	virtual ~ClDiskSpaceGetThread();	//デストラクタ

protected:
	virtual void	run();				//オーバーライド：スレッドの中身

	/*----------------------------------------*/
	/*
		slot
	*/
	/*----------------------------------------*/
private slots:
	void	slotTimerFired();			//タイマーイベント発火処理

	/*----------------------------------------*/
	/*
		signal
	*/
	/*----------------------------------------*/
signals:
	void	sigUsedRatio(char ucUsedRatio);		//!< ドライブの使用率を通知する。取得に失敗した場合は…どうしよう？とりあえず-1を返す。
};
/*================================================================================*/

#endif // CLDISKSPACEGETTHREAD_H
